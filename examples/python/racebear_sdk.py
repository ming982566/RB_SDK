"""RaceBear SDK 的零依赖 Python ctypes 封装。

本文件只映射公开 RaceBearSDK.h，不依赖 SDK 私有源码。ctypes 结构体字段顺序、
数组长度和基础类型必须与同一版本头文件完全一致，否则 DLL 写入状态时会发生字段错位。
"""

from __future__ import annotations

import ctypes
import json
import os
import struct
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple, Union


# 通用 RB_Result。普通 SDK 接口返回这些值；串口读写接口成功时返回字节数。
RB_OK = 0
RB_ERROR = -1
RB_INVALID_ARGUMENT = -2
RB_BUFFER_TOO_SMALL = -3

# 固定数组上限直接对应 RaceBearSDK.h 中的 RB_Limits。
RB_TEXT_SMALL = 64
RB_TEXT_MEDIUM = 128
RB_TEXT_LARGE = 260
RB_DOF_COUNT = 6
RB_WIND_CHANNEL_COUNT = 4
RB_DYNAMIC_DOF_COUNT = 8
RB_DYNAMIC_MAX_INPUTS = 8
RB_MOTION_EFFECT_COUNT = 5
RB_MOTION_ROUTE_COUNT = 6
RB_HAPTIC_EFFECT_COUNT = 7
RB_MAX_OUTPUT_INSTANCES = 32
RB_MAX_OUTPUT_KEYS = 128
RB_MAX_EFFECTS = 32
RB_MAX_PLUGINS = 64
RB_MAX_CAN_MOTORS = 16
RB_MAX_TELEMETRY_VALUES = 512
RB_CUSTOM_TRANSPORT_MMF = 1
RB_CUSTOM_TRANSPORT_UDP = 2
RB_CUSTOM_INPUT_RAW = 1
RB_CUSTOM_INPUT_6DOF = 2
RB_CUSTOM_PACKET_MAGIC = 0x4D544252
RB_CUSTOM_PROTOCOL_VERSION = 1

# 不要给这些结构体添加 _pack_ = 1。SDK 由 VS2019 按默认对齐方式构建，
# 当前 x64 ABI 下 RB_RuntimeState=4776 字节、RB_SerialPortInfo=2048 字节。
# 如果升级 SDK 后尺寸变化，必须按新 RaceBearSDK.h 同步下面全部字段。

class RB_LicenseSnapshot(ctypes.Structure):
    """授权状态快照；RemainingSeconds 用于限时授权和本地试用倒计时。"""

    _fields_ = [
        ("State", ctypes.c_int),
        ("Problem", ctypes.c_int),
        ("OutputAllowed", ctypes.c_int),
        ("RemainingSeconds", ctypes.c_longlong),
        ("ActivationCode", ctypes.c_char * RB_TEXT_MEDIUM),
    ]


class RB_GameState(ctypes.Structure):
    """当前选中游戏和进程运行状态。"""

    _fields_ = [
        ("CatalogIndex", ctypes.c_int),
        ("SteamAppID", ctypes.c_int),
        ("GameKey", ctypes.c_char * RB_TEXT_SMALL),
        ("GameName", ctypes.c_char * RB_TEXT_MEDIUM),
        ("ImageName", ctypes.c_char * RB_TEXT_MEDIUM),
        ("ProcessNames", ctypes.c_char * RB_TEXT_LARGE),
        ("CanCreateSource", ctypes.c_int),
        ("NeedsTelemetryConfig", ctypes.c_int),
        ("ProcessRunning", ctypes.c_int),
    ]


class RB_SourceState(ctypes.Structure):
    """遥测源状态以及前端常用的最终遥测摘要。"""

    _fields_ = [
        ("Count", ctypes.c_int),
        ("ActiveIndex", ctypes.c_int),
        ("Connected", ctypes.c_int),
        ("Ready", ctypes.c_int),
        ("TelemetryActive", ctypes.c_int),
        ("NeedsTelemetryConfig", ctypes.c_int),
        ("StateCode", ctypes.c_int),
        ("TransitionPercent", ctypes.c_double),
        ("Name", ctypes.c_char * RB_TEXT_MEDIUM),
        ("StatusText", ctypes.c_char * RB_TEXT_MEDIUM),
        ("ErrorText", ctypes.c_char * RB_TEXT_LARGE),
        ("FinalSpeed", ctypes.c_double),
        ("FinalRPM", ctypes.c_double),
        ("FinalGear", ctypes.c_double),
        ("FinalWheelSlip", ctypes.c_double),
        ("FinalWheelLock", ctypes.c_double),
        ("FinalWheelSpin", ctypes.c_double),
        ("FinalTractionLoss", ctypes.c_double),
        ("FinalABSPulse", ctypes.c_double),
        ("FinalTCPulse", ctypes.c_double),
    ]


class RB_Motion6DOFState(ctypes.Structure):
    """6DOF 分层数据；所有数组顺序固定为 Sway/Surge/Heave/Yaw/Roll/Pitch。"""

    _fields_ = [
        ("RawInput", ctypes.c_double * RB_DOF_COUNT),
        ("BaseFiltered", ctypes.c_double * RB_DOF_COUNT),
        ("MixDelta", ctypes.c_double * RB_DOF_COUNT),
        ("EffectDelta", ctypes.c_double * RB_DOF_COUNT),
        ("Unclipped", ctypes.c_double * RB_DOF_COUNT),
        ("Cropped", ctypes.c_double * RB_DOF_COUNT),
        ("RigFiltered", ctypes.c_double * RB_DOF_COUNT),
        ("PoseLimit", ctypes.c_double * RB_DOF_COUNT),
        ("RigLimit", ctypes.c_double * RB_DOF_COUNT),
        ("PosePossible", ctypes.c_int * RB_DOF_COUNT),
    ]


class RB_RigState(ctypes.Structure):
    """当前平台信息和完整 6DOF 运动状态。"""

    _fields_ = [
        ("Count", ctypes.c_int),
        ("ActiveIndex", ctypes.c_int),
        ("RigName", ctypes.c_char * RB_TEXT_MEDIUM),
        ("RigType", ctypes.c_int),
        ("NumberOfAxes", ctypes.c_int),
        ("MotionState", RB_Motion6DOFState),
    ]


class RB_FunctionState(ctypes.Structure):
    """风感、安全带和震动等功能容器的运行摘要。"""

    _fields_ = [
        ("Count", ctypes.c_int),
        ("ActiveIndex", ctypes.c_int),
        ("FunctionName", ctypes.c_char * RB_TEXT_MEDIUM),
        ("Enabled", ctypes.c_int),
        ("OutputCount", ctypes.c_int),
        # 关键 ABI 规则：Windows 的 C long 在 x64 进程中仍为 32 位。
        ("FirstOutputValue", ctypes.c_long),
        ("HapticEffectCount", ctypes.c_int),
    ]


class RB_OutputState(ctypes.Structure):
    """输出实例连接状态和发送周期。"""

    _fields_ = [
        ("Count", ctypes.c_int),
        ("ActiveIndex", ctypes.c_int),
        ("OutName", ctypes.c_char * RB_TEXT_MEDIUM),
        ("OutType", ctypes.c_int),
        ("Connected", ctypes.c_int),
        ("TransitionPercent", ctypes.c_double),
        ("SendTimeInterval", ctypes.c_double),
        ("AnyRuntimeActive", ctypes.c_int),
        ("AllDisconnected", ctypes.c_int),
    ]


class RB_CanMotorState(ctypes.Structure):
    """单个 CANopen 电机节点状态。"""

    _fields_ = [
        ("NodeId", ctypes.c_int),
        ("AxisKey", ctypes.c_char * RB_TEXT_SMALL),
        ("Online", ctypes.c_int),
        ("Homed", ctypes.c_int),
        ("Ready", ctypes.c_int),
        ("Fault", ctypes.c_int),
        ("StatusWord", ctypes.c_int),
        ("ErrorCode", ctypes.c_int),
        ("Temperature", ctypes.c_int),
        ("OperationMode", ctypes.c_int),
    ]


class RB_CanState(ctypes.Structure):
    """CAN 控制器、固件升级和最多 16 个电机的状态。"""

    _fields_ = [
        ("StatusText", ctypes.c_char * RB_TEXT_MEDIUM),
        ("MotorCount", ctypes.c_int),
        ("FirmwareUpdateRunning", ctypes.c_int),
        ("FirmwareUpdateComplete", ctypes.c_int),
        ("FirmwareUpdateSuccess", ctypes.c_int),
        ("FirmwareUpdateProgress", ctypes.c_int),
        ("FirmwareUpdateErrorCode", ctypes.c_int),
        ("FirmwareUpdateStatusText", ctypes.c_char * RB_TEXT_MEDIUM),
        ("Motors", RB_CanMotorState * RB_MAX_CAN_MOTORS),
    ]


class RB_WindState(ctypes.Structure):
    """风感模拟输入、归一化结果和四通道最终输出。"""

    _fields_ = [
        ("Enabled", ctypes.c_int),
        ("CurrentInputKmh", ctypes.c_double),
        ("CurrentNormalizedPercent", ctypes.c_double),
        ("ChannelNormalizedPercent", ctypes.c_double * RB_WIND_CHANNEL_COUNT),
        ("ChannelOutput", ctypes.c_long * RB_WIND_CHANNEL_COUNT),
        ("TestOutputEnabled", ctypes.c_int),
        ("TestOutputPercent", ctypes.c_double),
    ]


class RB_SeatbeltState(ctypes.Structure):
    """安全带左右张紧、碰撞保持和测试输出状态。"""

    _fields_ = [
        ("Enabled", ctypes.c_int),
        ("InputMode", ctypes.c_int),
        ("CurrentSpeedKmh", ctypes.c_double),
        ("BrakeTensionPercent", ctypes.c_double),
        ("LeftLateralTensionPercent", ctypes.c_double),
        ("RightLateralTensionPercent", ctypes.c_double),
        ("CrashActive", ctypes.c_int),
        ("CrashHoldRemainingMs", ctypes.c_double),
        ("LeftOutputPercent", ctypes.c_double),
        ("RightOutputPercent", ctypes.c_double),
        ("LeftOutput", ctypes.c_long),
        ("RightOutput", ctypes.c_long),
        ("TestOutputEnabled", ctypes.c_int),
        ("TestLeftPercent", ctypes.c_double),
        ("TestRightPercent", ctypes.c_double),
    ]


class RB_RuntimeState(ctypes.Structure):
    """RB_State_Read 一次性填充的完整高频运行状态。"""

    _fields_ = [
        ("Size", ctypes.c_int),
        ("Version", ctypes.c_int),
        ("License", RB_LicenseSnapshot),
        ("Game", RB_GameState),
        ("Source", RB_SourceState),
        ("Rig", RB_RigState),
        ("Function", RB_FunctionState),
        ("Output", RB_OutputState),
        ("Can", RB_CanState),
        ("Wind", RB_WindState),
        ("Seatbelt", RB_SeatbeltState),
        ("LastAction", ctypes.c_char * RB_TEXT_MEDIUM),
        ("LastError", ctypes.c_char * RB_TEXT_LARGE),
        ("LastRequestSuccess", ctypes.c_int),
    ]


class RB_SerialPortInfo(ctypes.Structure):
    """系统串口枚举信息；每个文本字段都是 NUL 结尾的 UTF-8 固定数组。"""

    _fields_ = [
        ("PortName", ctypes.c_char * 256),
        ("FriendlyName", ctypes.c_char * 256),
        ("Description", ctypes.c_char * 256),
        ("Manufacturer", ctypes.c_char * 256),
        ("Model", ctypes.c_char * 256),
        ("BusReportedDeviceDesc", ctypes.c_char * 256),
        ("HardwareId", ctypes.c_char * 512),
    ]


class RB_OutputConfig(ctypes.Structure):
    """单个 Serial/UDP/MMF/CAN 输出实例的完整配置。"""

    _fields_ = [
        ("Size", ctypes.c_int),
        ("Version", ctypes.c_int),
        ("CANindex", ctypes.c_int),
        ("ConnectionSpeed", ctypes.c_int),
        ("ParkPositionPercent", ctypes.c_int),
        ("AutoConnect", ctypes.c_int),
        ("OutBit", ctypes.c_int),
        ("OutputKind", ctypes.c_char * RB_TEXT_SMALL),
        ("Type", ctypes.c_int),
        ("BaudRate", ctypes.c_int),
        ("Port", ctypes.c_char * RB_TEXT_MEDIUM),
        ("IpAddrs", ctypes.c_char * RB_TEXT_MEDIUM),
        ("UdpPort", ctypes.c_int),
        ("UdpLPort", ctypes.c_int),
        ("MMFName", ctypes.c_char * RB_TEXT_MEDIUM),
        ("StartString", ctypes.c_char * RB_TEXT_LARGE),
        ("StartTime", ctypes.c_char * RB_TEXT_SMALL),
        ("OutPutString", ctypes.c_char * RB_TEXT_LARGE),
        ("OutPutTime", ctypes.c_char * RB_TEXT_SMALL),
        ("EndString", ctypes.c_char * RB_TEXT_LARGE),
        ("EndTime", ctypes.c_char * RB_TEXT_SMALL),
    ]


class RB_EffectCatalogItem(ctypes.Structure):
    _fields_ = [
        ("Index", ctypes.c_int),
        ("Name", ctypes.c_char * RB_TEXT_MEDIUM),
        ("Description", ctypes.c_char * RB_TEXT_LARGE),
        ("OutputText", ctypes.c_char * RB_TEXT_MEDIUM),
    ]


class RB_EffectCatalog(ctypes.Structure):
    _fields_ = [
        ("Size", ctypes.c_int),
        ("Version", ctypes.c_int),
        ("Count", ctypes.c_int),
        ("Items", RB_EffectCatalogItem * RB_MAX_EFFECTS),
    ]


class RB_PluginInfo(ctypes.Structure):
    _fields_ = [
        ("Index", ctypes.c_int),
        ("Name", ctypes.c_char * RB_TEXT_MEDIUM),
        ("Version", ctypes.c_char * RB_TEXT_SMALL),
        ("StatusText", ctypes.c_char * RB_TEXT_MEDIUM),
        ("IconPath", ctypes.c_char * RB_TEXT_LARGE),
        ("Launchable", ctypes.c_int),
    ]


class RB_PluginCatalog(ctypes.Structure):
    _fields_ = [
        ("Size", ctypes.c_int),
        ("Version", ctypes.c_int),
        ("Count", ctypes.c_int),
        ("Items", RB_PluginInfo * RB_MAX_PLUGINS),
    ]


class RB_GameInstallInfo(ctypes.Structure):
    _fields_ = [
        ("GameKey", ctypes.c_char * RB_TEXT_SMALL),
        ("SteamAppID", ctypes.c_int),
        ("GameName", ctypes.c_char * RB_TEXT_MEDIUM),
        ("ProcessName", ctypes.c_char * RB_TEXT_MEDIUM),
        ("ProcessId", ctypes.c_int),
        ("InstallPath", ctypes.c_char * RB_TEXT_LARGE),
        ("ExePath", ctypes.c_char * RB_TEXT_LARGE),
        ("Source", ctypes.c_char * RB_TEXT_MEDIUM),
        ("Found", ctypes.c_int),
        ("Running", ctypes.c_int),
        ("Ambiguous", ctypes.c_int),
    ]


class RB_GameConfig(ctypes.Structure):
    _fields_ = [
        ("Size", ctypes.c_int),
        ("Version", ctypes.c_int),
        ("SourcePort", ctypes.c_int),
        ("SourceIP", ctypes.c_char * RB_TEXT_MEDIUM),
        ("RemotehostIP", ctypes.c_char * RB_TEXT_MEDIUM),
        ("ForwardingPort", ctypes.c_int),
        ("ForwardingIP", ctypes.c_char * RB_TEXT_MEDIUM),
        ("Forwarding", ctypes.c_int),
        ("DirectlyConnected", ctypes.c_int),
        ("SerialPort", ctypes.c_char * RB_TEXT_MEDIUM),
        ("SerialBaudRate", ctypes.c_int),
        ("Configuration", ctypes.c_char * RB_TEXT_MEDIUM),
        ("GamePath", ctypes.c_char * RB_TEXT_LARGE),
    ]


class RB_PlatformConfig(ctypes.Structure):
    _fields_ = [
        ("Size", ctypes.c_int),
        ("Version", ctypes.c_int),
        ("Type", ctypes.c_int),
        ("Index", ctypes.c_int),
        ("ConfigName", ctypes.c_char * RB_TEXT_MEDIUM),
        ("Name", ctypes.c_char * RB_TEXT_MEDIUM),
        ("L1", ctypes.c_int), ("L2", ctypes.c_int), ("L3", ctypes.c_int),
        ("L4", ctypes.c_int), ("L5", ctypes.c_int), ("L6", ctypes.c_int),
        ("L7", ctypes.c_int), ("L8", ctypes.c_int), ("L9", ctypes.c_int),
        ("L10", ctypes.c_int),
        ("OutBit", ctypes.c_int),
        ("DefaultPos", ctypes.c_int),
        ("OutGain", ctypes.c_int),
        ("OutSmoothing", ctypes.c_int),
        ("Lateral", ctypes.c_int),
        ("Longitudinal", ctypes.c_int),
        ("Vertical", ctypes.c_int),
        ("DecelerationSpeed", ctypes.c_int),
        ("Percent", ctypes.c_int),
        ("OutKey", ctypes.c_char * RB_TEXT_MEDIUM),
    ]


class RB_FilterEffect(ctypes.Structure):
    """一个动态输入通道的完整滤波链参数。"""

    _fields_ = [
        ("Enabled", ctypes.c_int),
        ("Dof", ctypes.c_int),
        ("InMapping", ctypes.c_int),
        ("InGain", ctypes.c_double),
        ("OutScaling", ctypes.c_int),
        ("Proportion", ctypes.c_int),
        ("Strength", ctypes.c_int),
        ("Smoothing", ctypes.c_int),
        ("DeadZone", ctypes.c_int),
        ("Washout", ctypes.c_int),
        ("Sensitivity", ctypes.c_int),
        ("SensitivityStrength", ctypes.c_int),
    ]


class RB_DynamicInputEffect(ctypes.Structure):
    _fields_ = [
        ("Enabled", ctypes.c_int),
        ("InputType", ctypes.c_int),
        ("TelemetryIndex", ctypes.c_int),
        ("Key", ctypes.c_char * RB_TEXT_SMALL),
        ("Filter", RB_FilterEffect),
    ]


class RB_DynamicDofEffect(ctypes.Structure):
    _fields_ = [
        ("Enabled", ctypes.c_int),
        ("InputCount", ctypes.c_int),
        ("Inputs", RB_DynamicInputEffect * RB_DYNAMIC_MAX_INPUTS),
    ]


class RB_DynamicV2Profile(ctypes.Structure):
    """八个动态槽位及每槽最多八个独立输入通道。"""

    _fields_ = [
        ("Size", ctypes.c_int),
        ("Version", ctypes.c_int),
        ("Dofs", RB_DynamicDofEffect * RB_DYNAMIC_DOF_COUNT),
    ]


class RB_MotionEffectOutputRoute(ctypes.Structure):
    _fields_ = [
        ("Enabled", ctypes.c_int),
        ("Dof", ctypes.c_int),
        ("OutputRatio", ctypes.c_double),
        ("Direction", ctypes.c_double),
        ("Threshold", ctypes.c_double),
        ("Frequency", ctypes.c_double),
        ("Duration", ctypes.c_double),
    ]


class RB_MotionEffectConfig(ctypes.Structure):
    _fields_ = [
        ("Enabled", ctypes.c_int),
        ("EffectType", ctypes.c_int),
        ("InputIndex", ctypes.c_int),
        ("SecondaryInputIndex", ctypes.c_int),
        ("Gain", ctypes.c_double),
        ("RouteCount", ctypes.c_int),
        ("Routes", RB_MotionEffectOutputRoute * RB_MOTION_ROUTE_COUNT),
        ("Threshold", ctypes.c_double),
        ("Limit", ctypes.c_double),
        ("Frequency", ctypes.c_double),
        ("Decay", ctypes.c_double),
        ("OutputDof", ctypes.c_int),
        ("SecondaryOutputDof", ctypes.c_int),
        ("SecondaryGain", ctypes.c_double),
        ("Direction", ctypes.c_double),
    ]


class RB_MotionEffectProfile(ctypes.Structure):
    _fields_ = [
        ("Size", ctypes.c_int),
        ("Version", ctypes.c_int),
        ("Effects", RB_MotionEffectConfig * RB_MOTION_EFFECT_COUNT),
    ]


class RB_HapticEffectConfig(ctypes.Structure):
    _fields_ = [
        ("Enabled", ctypes.c_int),
        ("EffectType", ctypes.c_int),
        ("InputIndex", ctypes.c_int),
        ("SecondaryInputIndex", ctypes.c_int),
        ("OutputChannel", ctypes.c_int),
        ("Gain", ctypes.c_double),
        ("Threshold", ctypes.c_double),
        ("MinOutput", ctypes.c_double),
        ("MaxOutput", ctypes.c_double),
        ("Frequency", ctypes.c_double),
        ("Direction", ctypes.c_double),
    ]


class RB_HapticEffectProfile(ctypes.Structure):
    _fields_ = [
        ("Size", ctypes.c_int),
        ("Version", ctypes.c_int),
        ("Effects", RB_HapticEffectConfig * RB_HAPTIC_EFFECT_COUNT),
    ]


class RB_WindEffectConfig(ctypes.Structure):
    _fields_ = [
        ("Size", ctypes.c_int),
        ("Version", ctypes.c_int),
        ("Enabled", ctypes.c_int),
        ("InputMinKmh", ctypes.c_double),
        ("InputMaxKmh", ctypes.c_double),
        ("OutputMinWorkPercent", ctypes.c_double),
        ("Gamma", ctypes.c_double),
        ("MasterGainPercent", ctypes.c_double),
        ("ChannelGainPercent", ctypes.c_double * RB_WIND_CHANNEL_COUNT),
        ("ChannelOffsetPercent", ctypes.c_double * RB_WIND_CHANNEL_COUNT),
    ]


class RB_SeatbeltEffectConfig(ctypes.Structure):
    _fields_ = [
        ("Size", ctypes.c_int),
        ("Version", ctypes.c_int),
        ("Enabled", ctypes.c_int),
        ("InputMode", ctypes.c_int),
        ("BasePreloadPercent", ctypes.c_double),
        ("StartSpeedKmh", ctypes.c_double),
        ("FullSpeedKmh", ctypes.c_double),
        ("MasterGainPercent", ctypes.c_double),
        ("ReleaseSpeedPercent", ctypes.c_double),
        ("BrakeEnabled", ctypes.c_int),
        ("BrakePedalGainPercent", ctypes.c_double),
        ("BrakeDecelGainPercent", ctypes.c_double),
        ("LateralEnabled", ctypes.c_int),
        ("LateralGainPercent", ctypes.c_double),
        ("LateralDeadzoneG", ctypes.c_double),
        ("CrashEnabled", ctypes.c_int),
        ("CrashThresholdPercent", ctypes.c_double),
        ("CrashHoldMs", ctypes.c_double),
        ("CrashOutputPercent", ctypes.c_double),
        ("LeftEnabled", ctypes.c_int),
        ("RightEnabled", ctypes.c_int),
        ("LeftOutputRatioPercent", ctypes.c_double),
        ("RightOutputRatioPercent", ctypes.c_double),
        ("LeftReverse", ctypes.c_int),
        ("RightReverse", ctypes.c_int),
    ]


class RB_ExternalTelemetryValue(ctypes.Structure):
    """外部游戏单个遥测值；Index由稳定key在启动时解析。"""

    _fields_ = [("Index", ctypes.c_int), ("Value", ctypes.c_double)]


class RB_ExternalSourceDesc(ctypes.Structure):
    """外部游戏源名称、断流超时和淡入淡出时间。"""

    _fields_ = [
        ("Size", ctypes.c_int),
        ("Version", ctypes.c_int),
        ("SourceKey", ctypes.c_char * RB_TEXT_SMALL),
        ("SourceName", ctypes.c_char * RB_TEXT_MEDIUM),
        ("DataTimeoutMs", ctypes.c_int),
        ("TransitionMs", ctypes.c_int),
    ]


class RB_ExternalTelemetryFrame(ctypes.Structure):
    """一帧完整遥测数据；未写入Values的槽位由SDK清零。"""

    _fields_ = [
        ("Size", ctypes.c_int),
        ("Version", ctypes.c_int),
        ("Sequence", ctypes.c_ulonglong),
        ("TimestampUs", ctypes.c_ulonglong),
        ("ValueCount", ctypes.c_int),
        ("Values", RB_ExternalTelemetryValue * RB_MAX_TELEMETRY_VALUES),
    ]


class RB_ExternalSourceState(ctypes.Structure):
    """外部游戏源连接、Ready和帧计数状态。"""

    _fields_ = [
        ("Size", ctypes.c_int),
        ("Version", ctypes.c_int),
        ("Open", ctypes.c_int),
        ("Connected", ctypes.c_int),
        ("Ready", ctypes.c_int),
        ("LastSequence", ctypes.c_ulonglong),
        ("SubmittedFrameCount", ctypes.c_ulonglong),
    ]


class RB_CustomGameDesc(ctypes.Structure):
    """正式自定义游戏的目录、进程识别和输入模式。"""

    _fields_ = [
        ("Size", ctypes.c_int), ("Version", ctypes.c_int),
        ("GameKey", ctypes.c_char * RB_TEXT_SMALL),
        ("GameName", ctypes.c_char * RB_TEXT_MEDIUM),
        ("ProcessNames", ctypes.c_char * RB_TEXT_LARGE),
        ("ImageName", ctypes.c_char * RB_TEXT_MEDIUM),
        ("SteamAppID", ctypes.c_int), ("Transport", ctypes.c_int),
        ("InputMode", ctypes.c_int), ("DataTimeoutMs", ctypes.c_int),
        ("TransitionMs", ctypes.c_int), ("DofOptions", ctypes.c_int),
    ]


class RB_CustomGameMmfConfig(ctypes.Structure):
    """游戏创建、SDK只读打开的Windows命名共享内存配置。"""

    _fields_ = [("MappingName", ctypes.c_char * RB_TEXT_MEDIUM), ("MappingSize", ctypes.c_int)]


class RB_CustomGameUdpConfig(ctypes.Structure):
    """SDK UDP监听地址、端口和可选发送端限制。"""

    _fields_ = [
        ("ListenAddress", ctypes.c_char * RB_TEXT_SMALL),
        ("ListenPort", ctypes.c_int),
        ("AllowedSenderAddress", ctypes.c_char * RB_TEXT_SMALL),
    ]


class RB_CustomGameRegistration(ctypes.Structure):
    """RB_Game_RegisterCustom一次接收的完整注册配置。"""

    _fields_ = [
        ("Size", ctypes.c_int), ("Version", ctypes.c_int),
        ("Game", RB_CustomGameDesc), ("Mmf", RB_CustomGameMmfConfig),
        ("Udp", RB_CustomGameUdpConfig),
    ]


def build_custom_raw_packet(
    sequence: int, game_time_us: int, values: List[tuple[int, float]]
) -> bytes:
    """按V1小端协议构造完整UDP/MMF原始遥测包。"""
    header_size = struct.calcsize("<IHHIQQII")
    payload = b"".join(struct.pack("<Id", field_id, value) for field_id, value in values)
    packet_size = header_size + len(payload)
    header = struct.pack(
        "<IHHIQQII", RB_CUSTOM_PACKET_MAGIC, RB_CUSTOM_PROTOCOL_VERSION,
        header_size, packet_size, sequence, game_time_us,
        RB_CUSTOM_INPUT_RAW, len(values),
    )
    return header + payload


def decode_utf8(value: Union[bytes, ctypes.Array[Any]]) -> str:
    """把 C 固定 char 数组按第一个 NUL 截断并解码为 Python 字符串。"""
    raw = bytes(value)
    return raw.split(b"\0", 1)[0].decode("utf-8", errors="replace")


class RaceBearError(RuntimeError):
    """保留原始 SDK 返回码，便于前端按 RB_Result 分类显示错误。"""

    def __init__(self, operation: str, code: int):
        super().__init__(f"{operation} failed with RaceBear SDK code {code}")
        self.operation = operation
        self.code = code


class RaceBearSDK:
    """管理 DLL、SDK 生命周期及常用状态/JSON/Command 接口。"""

    def __init__(self, dll_path: Optional[Union[str, Path]] = None):
        """加载 x64 DLL，但此时尚未初始化后端运行态。

        dll_path 为空时，从当前 SDK 示例目录自动定位 bin/x64/Release。
        """
        if os.name != "nt":
            raise OSError("RaceBear SDK is only available on Windows.")
        if struct.calcsize("P") != 8:
            raise OSError("RaceBear SDK requires 64-bit Python.")

        # 默认路径只适用于 SDK 源码包；发布应用应显式传入 EXE 附近的 DLL 路径。
        if dll_path is None:
            sdk_root = Path(__file__).resolve().parents[2]
            dll_path = sdk_root / "bin" / "x64" / "Release" / "RaceBearSDK.dll"
        self.dll_path = Path(dll_path).resolve()
        if not self.dll_path.is_file():
            raise FileNotFoundError(self.dll_path)

        # Python 3.8+ 收紧了 Windows DLL 搜索路径，显式加入目录才能找到依赖库。
        self._dll_directory = None
        if hasattr(os, "add_dll_directory"):
            self._dll_directory = os.add_dll_directory(str(self.dll_path.parent))

        # 公开导出使用 extern "C" C ABI；x64 Python 必须使用相同位数加载。
        self._dll = ctypes.CDLL(str(self.dll_path))
        self._initialized = False
        self._bind_functions()

    def _bind_functions(self) -> None:
        """为 ctypes 声明准确参数和返回类型。

        ctypes 默认把未声明返回值当作 c_int，指针或 64 位值会因此被截断，
        所以每个被示例使用的函数都必须明确设置 argtypes/restype。
        """
        # 版本和生命周期接口。
        self._dll.RB_Runtime_GetVersion.argtypes = []
        self._dll.RB_Runtime_GetVersion.restype = ctypes.c_char_p

        self._dll.RB_Runtime_SetAppDataName.argtypes = [ctypes.c_char_p]
        self._dll.RB_Runtime_SetAppDataName.restype = ctypes.c_int
        self._dll.RB_Runtime_Initialize.argtypes = []
        self._dll.RB_Runtime_Initialize.restype = ctypes.c_int
        self._dll.RB_Runtime_StartLoop.argtypes = [ctypes.c_int]
        self._dll.RB_Runtime_StartLoop.restype = ctypes.c_int
        self._dll.RB_Runtime_Shutdown.argtypes = []
        self._dll.RB_Runtime_Shutdown.restype = None

        # 高频状态和低频 JSON/Command/日志边界。
        self._dll.RB_State_Read.argtypes = [ctypes.POINTER(RB_RuntimeState)]
        self._dll.RB_State_Read.restype = ctypes.c_int
        self._dll.RB_Data_ReadJson.argtypes = [
            ctypes.c_char_p,
            ctypes.POINTER(ctypes.c_char),
            ctypes.c_int,
        ]
        self._dll.RB_Data_ReadJson.restype = ctypes.c_int
        self._dll.RB_Data_WriteJson.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
        self._dll.RB_Data_WriteJson.restype = ctypes.c_int
        self._dll.RB_Command_Run.argtypes = [
            ctypes.c_char_p,
            ctypes.c_char_p,
            ctypes.POINTER(ctypes.c_char),
            ctypes.c_int,
        ]
        self._dll.RB_Command_Run.restype = ctypes.c_int
        self._dll.RB_Log_Read.argtypes = [ctypes.POINTER(ctypes.c_char), ctypes.c_int]
        self._dll.RB_Log_Read.restype = ctypes.c_int

        # 常规前端配置接口。Python 也直接传结构体，不要求客户编辑 JSON。
        self._dll.RB_Output_GetInstanceCount.argtypes = []
        self._dll.RB_Output_GetInstanceCount.restype = ctypes.c_int
        self._dll.RB_Output_GetKindCount.argtypes = []
        self._dll.RB_Output_GetKindCount.restype = ctypes.c_int
        self._dll.RB_Output_GetKind.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_char), ctypes.c_int]
        self._dll.RB_Output_GetKind.restype = ctypes.c_int
        self._dll.RB_Output_AddInstance.argtypes = [ctypes.c_char_p]
        self._dll.RB_Output_AddInstance.restype = ctypes.c_int
        self._dll.RB_Output_DeleteInstanceByIndex.argtypes = [ctypes.c_int]
        self._dll.RB_Output_DeleteInstanceByIndex.restype = ctypes.c_int
        self._dll.RB_Output_ReadInstanceConfigByIndex.argtypes = [ctypes.c_int, ctypes.POINTER(RB_OutputConfig)]
        self._dll.RB_Output_ReadInstanceConfigByIndex.restype = ctypes.c_int
        self._dll.RB_Output_SaveInstanceConfigByIndex.argtypes = [ctypes.c_int, ctypes.POINTER(RB_OutputConfig)]
        self._dll.RB_Output_SaveInstanceConfigByIndex.restype = ctypes.c_int
        self._dll.RB_Output_ApplyInstanceConfigByIndex.argtypes = [ctypes.c_int, ctypes.POINTER(RB_OutputConfig)]
        self._dll.RB_Output_ApplyInstanceConfigByIndex.restype = ctypes.c_int
        self._dll.RB_Output_ConnectInstanceByIndex.argtypes = [ctypes.c_int]
        self._dll.RB_Output_ConnectInstanceByIndex.restype = ctypes.c_int
        self._dll.RB_Output_DisconnectInstanceByIndex.argtypes = [ctypes.c_int]
        self._dll.RB_Output_DisconnectInstanceByIndex.restype = ctypes.c_int

        self._dll.RB_Dynamic_GetProfileCount.argtypes = []
        self._dll.RB_Dynamic_GetProfileCount.restype = ctypes.c_int
        self._dll.RB_Dynamic_GetProfileName.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_char), ctypes.c_int]
        self._dll.RB_Dynamic_GetProfileName.restype = ctypes.c_int
        self._dll.RB_Dynamic_ReadProfileByIndex.argtypes = [ctypes.c_int, ctypes.POINTER(RB_DynamicV2Profile)]
        self._dll.RB_Dynamic_ReadProfileByIndex.restype = ctypes.c_int
        self._dll.RB_Dynamic_SaveProfileByIndex.argtypes = [ctypes.c_int, ctypes.POINTER(RB_DynamicV2Profile)]
        self._dll.RB_Dynamic_SaveProfileByIndex.restype = ctypes.c_int
        self._dll.RB_Dynamic_ApplyProfileToCurrentRig.argtypes = [ctypes.POINTER(RB_DynamicV2Profile)]
        self._dll.RB_Dynamic_ApplyProfileToCurrentRig.restype = ctypes.c_int

        self._dll.RB_MotionEffect_GetCatalog.argtypes = [ctypes.POINTER(RB_EffectCatalog)]
        self._dll.RB_MotionEffect_GetCatalog.restype = ctypes.c_int
        self._dll.RB_MotionEffect_ReadProfileByIndex.argtypes = [ctypes.c_int, ctypes.POINTER(RB_MotionEffectProfile)]
        self._dll.RB_MotionEffect_ReadProfileByIndex.restype = ctypes.c_int
        self._dll.RB_MotionEffect_SaveProfileByIndex.argtypes = [ctypes.c_int, ctypes.POINTER(RB_MotionEffectProfile)]
        self._dll.RB_MotionEffect_SaveProfileByIndex.restype = ctypes.c_int
        self._dll.RB_MotionEffect_ApplyProfileToCurrentRig.argtypes = [ctypes.POINTER(RB_MotionEffectProfile)]
        self._dll.RB_MotionEffect_ApplyProfileToCurrentRig.restype = ctypes.c_int

        self._dll.RB_Haptic_GetCatalog.argtypes = [ctypes.POINTER(RB_EffectCatalog)]
        self._dll.RB_Haptic_GetCatalog.restype = ctypes.c_int
        self._dll.RB_Haptic_ReadProfileByIndex.argtypes = [ctypes.c_int, ctypes.POINTER(RB_HapticEffectProfile)]
        self._dll.RB_Haptic_ReadProfileByIndex.restype = ctypes.c_int
        self._dll.RB_Haptic_SaveProfileByIndex.argtypes = [ctypes.c_int, ctypes.POINTER(RB_HapticEffectProfile)]
        self._dll.RB_Haptic_SaveProfileByIndex.restype = ctypes.c_int
        self._dll.RB_Haptic_ApplyProfileToCurrentFunction.argtypes = [ctypes.POINTER(RB_HapticEffectProfile)]
        self._dll.RB_Haptic_ApplyProfileToCurrentFunction.restype = ctypes.c_int

        self._dll.RB_Wind_ReadConfig.argtypes = [ctypes.POINTER(RB_WindEffectConfig)]
        self._dll.RB_Wind_ReadConfig.restype = ctypes.c_int
        self._dll.RB_Wind_SaveConfig.argtypes = [ctypes.POINTER(RB_WindEffectConfig)]
        self._dll.RB_Wind_SaveConfig.restype = ctypes.c_int
        self._dll.RB_Wind_ApplyConfigToCurrentFunction.argtypes = [ctypes.POINTER(RB_WindEffectConfig)]
        self._dll.RB_Wind_ApplyConfigToCurrentFunction.restype = ctypes.c_int
        self._dll.RB_Wind_SetTestOutput.argtypes = [ctypes.c_int, ctypes.c_double]
        self._dll.RB_Wind_SetTestOutput.restype = ctypes.c_int

        self._dll.RB_Seatbelt_ReadConfig.argtypes = [ctypes.POINTER(RB_SeatbeltEffectConfig)]
        self._dll.RB_Seatbelt_ReadConfig.restype = ctypes.c_int
        self._dll.RB_Seatbelt_SaveConfig.argtypes = [ctypes.POINTER(RB_SeatbeltEffectConfig)]
        self._dll.RB_Seatbelt_SaveConfig.restype = ctypes.c_int
        self._dll.RB_Seatbelt_ApplyConfigToCurrentFunction.argtypes = [ctypes.POINTER(RB_SeatbeltEffectConfig)]
        self._dll.RB_Seatbelt_ApplyConfigToCurrentFunction.restype = ctypes.c_int
        self._dll.RB_Seatbelt_SetTestOutput.argtypes = [ctypes.c_int, ctypes.c_double, ctypes.c_double]
        self._dll.RB_Seatbelt_SetTestOutput.restype = ctypes.c_int

        # 游戏安装、遥测源配置和功能插件目录。
        self._dll.RB_Game_RefreshInstallations.argtypes = []
        self._dll.RB_Game_RefreshInstallations.restype = ctypes.c_int
        self._dll.RB_Game_GetInstallCount.argtypes = []
        self._dll.RB_Game_GetInstallCount.restype = ctypes.c_int
        self._dll.RB_Game_GetInstallInfo.argtypes = [ctypes.c_int, ctypes.POINTER(RB_GameInstallInfo)]
        self._dll.RB_Game_GetInstallInfo.restype = ctypes.c_int
        self._dll.RB_Game_ReadConfig.argtypes = [ctypes.c_char_p, ctypes.POINTER(RB_GameConfig)]
        self._dll.RB_Game_ReadConfig.restype = ctypes.c_int
        self._dll.RB_Game_SaveConfig.argtypes = [ctypes.c_char_p, ctypes.POINTER(RB_GameConfig)]
        self._dll.RB_Game_SaveConfig.restype = ctypes.c_int
        self._dll.RB_Game_AutoConnect.argtypes = [ctypes.c_int, ctypes.POINTER(RB_GameConfig)]
        self._dll.RB_Game_AutoConnect.restype = ctypes.c_int
        self._dll.RB_Game_AutoConfigureCurrent.argtypes = []
        self._dll.RB_Game_AutoConfigureCurrent.restype = ctypes.c_int
        self._dll.RB_Game_InstallCurrentPlugin.argtypes = [ctypes.c_char_p]
        self._dll.RB_Game_InstallCurrentPlugin.restype = ctypes.c_int
        self._dll.RB_Game_CheckCurrentSideConfig.argtypes = [ctypes.POINTER(RB_GameConfig)]
        self._dll.RB_Game_CheckCurrentSideConfig.restype = ctypes.c_int

        self._dll.RB_Plugin_Refresh.argtypes = []
        self._dll.RB_Plugin_Refresh.restype = ctypes.c_int
        self._dll.RB_Plugin_GetCatalog.argtypes = [ctypes.POINTER(RB_PluginCatalog)]
        self._dll.RB_Plugin_GetCatalog.restype = ctypes.c_int
        self._dll.RB_Plugin_Launch.argtypes = [ctypes.c_int]
        self._dll.RB_Plugin_Launch.restype = ctypes.c_int
        self._dll.RB_Plugin_StartRuntimes.argtypes = []
        self._dll.RB_Plugin_StartRuntimes.restype = ctypes.c_int

        # 平台选择、数值诊断和手动测试，不包含任何UI绘制依赖。
        self._dll.RB_Platform_ReadSelectedIndex.argtypes = []
        self._dll.RB_Platform_ReadSelectedIndex.restype = ctypes.c_int
        self._dll.RB_Platform_SaveSelectedIndex.argtypes = [ctypes.c_int]
        self._dll.RB_Platform_SaveSelectedIndex.restype = ctypes.c_int
        self._dll.RB_Platform_ReadConfigByIndex.argtypes = [ctypes.c_int, ctypes.POINTER(RB_PlatformConfig)]
        self._dll.RB_Platform_ReadConfigByIndex.restype = ctypes.c_int
        self._dll.RB_Platform_GetPreviewPoseLimit.argtypes = [ctypes.c_int, ctypes.POINTER(RB_PlatformConfig), ctypes.c_int]
        self._dll.RB_Platform_GetPreviewPoseLimit.restype = ctypes.c_double
        self._dll.RB_Platform_IsPreviewPosePossible.argtypes = [ctypes.c_int, ctypes.POINTER(RB_PlatformConfig), ctypes.c_int]
        self._dll.RB_Platform_IsPreviewPosePossible.restype = ctypes.c_int
        self._dll.RB_ManualPose_SetTestEnabled.argtypes = [ctypes.c_int]
        self._dll.RB_ManualPose_SetTestEnabled.restype = ctypes.c_int
        self._dll.RB_ManualPose_SetDrive.argtypes = [ctypes.c_int, ctypes.c_double]
        self._dll.RB_ManualPose_SetDrive.restype = ctypes.c_int
        self._dll.RB_ManualPose_ResetDrive.argtypes = []
        self._dll.RB_ManualPose_ResetDrive.restype = ctypes.c_int

        # 外部游戏源使用稳定key解析索引，再按固定结构体提交完整遥测帧。
        self._dll.RB_Telemetry_FindIndexByKey.argtypes = [ctypes.c_char_p]
        self._dll.RB_Telemetry_FindIndexByKey.restype = ctypes.c_int
        self._dll.RB_ExternalSource_Open.argtypes = [ctypes.POINTER(RB_ExternalSourceDesc)]
        self._dll.RB_ExternalSource_Open.restype = ctypes.c_int
        self._dll.RB_ExternalSource_SubmitFrame.argtypes = [ctypes.POINTER(RB_ExternalTelemetryFrame)]
        self._dll.RB_ExternalSource_SubmitFrame.restype = ctypes.c_int
        self._dll.RB_ExternalSource_GetState.argtypes = [ctypes.POINTER(RB_ExternalSourceState)]
        self._dll.RB_ExternalSource_GetState.restype = ctypes.c_int
        self._dll.RB_ExternalSource_Close.argtypes = []
        self._dll.RB_ExternalSource_Close.restype = None

        # 正式自定义游戏注册和稳定线协议FieldId。
        self._dll.RB_Telemetry_FindFieldIdByKey.argtypes = [ctypes.c_char_p]
        self._dll.RB_Telemetry_FindFieldIdByKey.restype = ctypes.c_int
        self._dll.RB_Game_RegisterCustom.argtypes = [ctypes.POINTER(RB_CustomGameRegistration)]
        self._dll.RB_Game_RegisterCustom.restype = ctypes.c_int
        self._dll.RB_Game_UnregisterCustom.argtypes = [ctypes.c_char_p]
        self._dll.RB_Game_UnregisterCustom.restype = ctypes.c_int

        # 示例只绑定串口枚举；完整串口读写函数可按头文件用同样方式继续声明。
        self._dll.RB_Serial_GetAvailablePortCount.argtypes = []
        self._dll.RB_Serial_GetAvailablePortCount.restype = ctypes.c_int
        self._dll.RB_Serial_GetAvailablePortInfo.argtypes = [
            ctypes.c_int,
            ctypes.POINTER(RB_SerialPortInfo),
        ]
        self._dll.RB_Serial_GetAvailablePortInfo.restype = ctypes.c_int

    @property
    def version(self) -> str:
        """读取 DLL 自带版本；该接口不要求先 initialize。"""
        value = self._dll.RB_Runtime_GetVersion()
        return value.decode("utf-8", errors="replace") if value else ""

    def initialize(self, app_data_name: str, interval_ms: int = 2) -> None:
        """设置独立应用目录、初始化 SDK 并启动内部计算循环。

        app_data_name 必须是目录名而不是完整路径。任一步失败都会抛出
        RaceBearError；计算循环启动失败时会先自动关闭已初始化的 SDK。
        interval_ms 单位为毫秒；默认值2表示目标周期约2ms。
        """
        if self._initialized:
            raise RuntimeError("RaceBear SDK is already initialized.")

        # 应用目录名必须先于 Initialize 设置，初始化后不允许切换。
        rc = self._dll.RB_Runtime_SetAppDataName(app_data_name.encode("utf-8"))
        if rc != RB_OK:
            raise RaceBearError("RB_Runtime_SetAppDataName", rc)

        rc = self._dll.RB_Runtime_Initialize()
        if rc != RB_OK:
            raise RaceBearError("RB_Runtime_Initialize", rc)

        self._initialized = True
        # 后端计算由SDK的MultimediaTimer驱动，Python只负责读取状态。
        rc = self._dll.RB_Runtime_StartLoop(interval_ms)
        if rc != RB_OK:
            self.close()
            raise RaceBearError("RB_Runtime_StartLoop", rc)

    def close(self) -> None:
        """关闭 SDK；调用前应先停止 Python/Qt 定时器和其它调用线程。"""
        if self._initialized:
            self._dll.RB_Runtime_Shutdown()
            self._initialized = False

    def read_state(self) -> RB_RuntimeState:
        """读取完整状态结构体；适合放在 30-100 Hz 的 UI 定时器中。"""
        state = RB_RuntimeState()
        rc = self._dll.RB_State_Read(ctypes.byref(state))
        if rc != RB_OK:
            raise RaceBearError("RB_State_Read", rc)
        return state

    def dynamic_profile_names(self) -> List[str]:
        """返回动态配置名；同一索引也用于运动增强和震动配置。"""
        count = self._dll.RB_Dynamic_GetProfileCount()
        if count < 0:
            raise RaceBearError("RB_Dynamic_GetProfileCount", count)
        names: List[str] = []
        for index in range(count):
            buffer = ctypes.create_string_buffer(RB_TEXT_MEDIUM)
            rc = self._dll.RB_Dynamic_GetProfileName(index, buffer, len(buffer))
            if rc != RB_OK:
                raise RaceBearError("RB_Dynamic_GetProfileName", rc)
            names.append(buffer.value.decode("utf-8", errors="replace"))
        return names

    def read_dynamic_profile(self, profile_index: int) -> RB_DynamicV2Profile:
        """读取完整动态配置；修改 Dofs/Inputs/Filter 后传给 save_dynamic_profile。"""
        profile = RB_DynamicV2Profile()
        rc = self._dll.RB_Dynamic_ReadProfileByIndex(profile_index, ctypes.byref(profile))
        if rc != RB_OK:
            raise RaceBearError("RB_Dynamic_ReadProfileByIndex", rc)
        return profile

    def save_dynamic_profile(self, profile_index: int, profile: RB_DynamicV2Profile, apply: bool = True) -> None:
        """保存完整动态配置，并按需立即应用到当前平台。"""
        rc = self._dll.RB_Dynamic_SaveProfileByIndex(profile_index, ctypes.byref(profile))
        if rc != RB_OK:
            raise RaceBearError("RB_Dynamic_SaveProfileByIndex", rc)
        if apply:
            rc = self._dll.RB_Dynamic_ApplyProfileToCurrentRig(ctypes.byref(profile))
            if rc != RB_OK:
                raise RaceBearError("RB_Dynamic_ApplyProfileToCurrentRig", rc)

    def read_output(self, instance_index: int) -> RB_OutputConfig:
        """读取单个输出实例的完整配置。"""
        config = RB_OutputConfig()
        rc = self._dll.RB_Output_ReadInstanceConfigByIndex(instance_index, ctypes.byref(config))
        if rc != RB_OK:
            raise RaceBearError("RB_Output_ReadInstanceConfigByIndex", rc)
        return config

    def output_kinds(self) -> List[str]:
        """返回可传给 add_output() 的输出类型 key，前端不应硬编码此列表。"""
        result: List[str] = []
        for index in range(self._dll.RB_Output_GetKindCount()):
            buffer = ctypes.create_string_buffer(RB_TEXT_SMALL)
            rc = self._dll.RB_Output_GetKind(index, buffer, len(buffer))
            if rc != RB_OK:
                raise RaceBearError("RB_Output_GetKind", rc)
            result.append(buffer.value.decode("utf-8", errors="replace"))
        return result

    def game_installations(self, refresh: bool = True) -> List[RB_GameInstallInfo]:
        """读取游戏安装和运行状态；refresh=True 时先执行一次低频扫描。"""
        if refresh:
            count = self._dll.RB_Game_RefreshInstallations()
        else:
            count = self._dll.RB_Game_GetInstallCount()
        if count < 0:
            raise RaceBearError("RB_Game_RefreshInstallations", count)
        result: List[RB_GameInstallInfo] = []
        for index in range(count):
            info = RB_GameInstallInfo()
            rc = self._dll.RB_Game_GetInstallInfo(index, ctypes.byref(info))
            if rc != RB_OK:
                raise RaceBearError("RB_Game_GetInstallInfo", rc)
            result.append(info)
        return result

    def read_game_config(self, game_key: str) -> RB_GameConfig:
        """按稳定gameKey读取完整遥测配置。"""
        config = RB_GameConfig()
        rc = self._dll.RB_Game_ReadConfig(game_key.encode("utf-8"), ctypes.byref(config))
        if rc != RB_OK:
            raise RaceBearError("RB_Game_ReadConfig", rc)
        return config

    def save_game_config(self, game_key: str, config: RB_GameConfig) -> None:
        """保存完整遥测配置；SDK会拒绝非法端口、布尔值或未终止字符串。"""
        rc = self._dll.RB_Game_SaveConfig(game_key.encode("utf-8"), ctypes.byref(config))
        if rc != RB_OK:
            raise RaceBearError("RB_Game_SaveConfig", rc)

    def connect_game_source(self, catalog_index: int, config: RB_GameConfig) -> None:
        """使用已读取并修改的配置创建或重新连接游戏遥测Source。"""
        rc = self._dll.RB_Game_AutoConnect(catalog_index, ctypes.byref(config))
        if rc != RB_OK:
            raise RaceBearError("RB_Game_AutoConnect", rc)

    def auto_configure_current_game(self) -> None:
        """执行当前游戏内置的一键遥测配置；不支持时抛出RaceBearError。"""
        rc = self._dll.RB_Game_AutoConfigureCurrent()
        if rc != RB_OK:
            raise RaceBearError("RB_Game_AutoConfigureCurrent", rc)

    def install_current_game_plugin(self, game_path: str) -> None:
        """把当前游戏所需遥测插件安装到扫描得到的游戏目录。"""
        rc = self._dll.RB_Game_InstallCurrentPlugin(game_path.encode("utf-8"))
        if rc != RB_OK:
            raise RaceBearError("RB_Game_InstallCurrentPlugin", rc)

    def plugin_catalog(self, refresh: bool = True) -> RB_PluginCatalog:
        """读取功能插件目录；刷新后旧索引不再使用。"""
        refresh_rc = self._dll.RB_Plugin_Refresh() if refresh else 0
        if refresh_rc < 0:
            raise RaceBearError("RB_Plugin_Refresh", refresh_rc)
        catalog = RB_PluginCatalog()
        rc = self._dll.RB_Plugin_GetCatalog(ctypes.byref(catalog))
        if rc != RB_OK:
            raise RaceBearError("RB_Plugin_GetCatalog", rc)
        return catalog

    def launch_plugin(self, plugin_index: int) -> None:
        """启动目录中标记为Launchable的插件前端。"""
        rc = self._dll.RB_Plugin_Launch(plugin_index)
        if rc != RB_OK:
            raise RaceBearError("RB_Plugin_Launch", rc)

    def read_selected_platform(self) -> Tuple[int, RB_PlatformConfig]:
        """读取当前平台索引和完整平台配置。"""
        index = self._dll.RB_Platform_ReadSelectedIndex()
        if index < 0:
            raise RaceBearError("RB_Platform_ReadSelectedIndex", index)
        config = RB_PlatformConfig()
        rc = self._dll.RB_Platform_ReadConfigByIndex(index, ctypes.byref(config))
        if rc != RB_OK:
            raise RaceBearError("RB_Platform_ReadConfigByIndex", rc)
        return index, config

    def set_manual_pose(self, dof_index: int, value: float) -> None:
        """设置手动姿态值；线性DOF单位mm，旋转DOF单位度。"""
        rc = self._dll.RB_ManualPose_SetDrive(dof_index, value)
        if rc != RB_OK:
            raise RaceBearError("RB_ManualPose_SetDrive", rc)

    def enable_manual_pose(self, enabled: bool) -> None:
        """启用或关闭手动测试；关闭前会复位全部手动输入。"""
        if not enabled:
            rc = self._dll.RB_ManualPose_ResetDrive()
            if rc != RB_OK:
                raise RaceBearError("RB_ManualPose_ResetDrive", rc)
        rc = self._dll.RB_ManualPose_SetTestEnabled(int(enabled))
        if rc != RB_OK:
            raise RaceBearError("RB_ManualPose_SetTestEnabled", rc)

    def save_output(self, instance_index: int, config: RB_OutputConfig, apply: bool = True) -> None:
        """保存输出配置；连接参数应在该实例断开后修改。"""
        rc = self._dll.RB_Output_SaveInstanceConfigByIndex(instance_index, ctypes.byref(config))
        if rc != RB_OK:
            raise RaceBearError("RB_Output_SaveInstanceConfigByIndex", rc)
        if apply:
            rc = self._dll.RB_Output_ApplyInstanceConfigByIndex(instance_index, ctypes.byref(config))
            if rc != RB_OK:
                raise RaceBearError("RB_Output_ApplyInstanceConfigByIndex", rc)

    def add_output(self, output_kind: str) -> int:
        """创建 Serial、UDP、MMF 或 CAN 输出实例并返回新索引。"""
        index = self._dll.RB_Output_AddInstance(output_kind.encode("utf-8"))
        if index < 0:
            raise RaceBearError("RB_Output_AddInstance", index)
        return index

    def read_motion_effects(self, profile_index: int) -> RB_MotionEffectProfile:
        """读取与动态配置同索引的运动增强配置。"""
        profile = RB_MotionEffectProfile()
        rc = self._dll.RB_MotionEffect_ReadProfileByIndex(profile_index, ctypes.byref(profile))
        if rc != RB_OK:
            raise RaceBearError("RB_MotionEffect_ReadProfileByIndex", rc)
        return profile

    def save_motion_effects(self, profile_index: int, profile: RB_MotionEffectProfile, apply: bool = True) -> None:
        """保存运动增强配置，并按需立即应用到当前平台。"""
        rc = self._dll.RB_MotionEffect_SaveProfileByIndex(profile_index, ctypes.byref(profile))
        if rc != RB_OK:
            raise RaceBearError("RB_MotionEffect_SaveProfileByIndex", rc)
        if apply:
            rc = self._dll.RB_MotionEffect_ApplyProfileToCurrentRig(ctypes.byref(profile))
            if rc != RB_OK:
                raise RaceBearError("RB_MotionEffect_ApplyProfileToCurrentRig", rc)

    def read_haptic_effects(self, profile_index: int) -> RB_HapticEffectProfile:
        """读取与动态配置同索引的七路震动效果配置。"""
        profile = RB_HapticEffectProfile()
        rc = self._dll.RB_Haptic_ReadProfileByIndex(profile_index, ctypes.byref(profile))
        if rc != RB_OK:
            raise RaceBearError("RB_Haptic_ReadProfileByIndex", rc)
        return profile

    def save_haptic_effects(self, profile_index: int, profile: RB_HapticEffectProfile, apply: bool = True) -> None:
        """保存震动效果配置，并按需立即应用到 Function 运行态。"""
        rc = self._dll.RB_Haptic_SaveProfileByIndex(profile_index, ctypes.byref(profile))
        if rc != RB_OK:
            raise RaceBearError("RB_Haptic_SaveProfileByIndex", rc)
        if apply:
            rc = self._dll.RB_Haptic_ApplyProfileToCurrentFunction(ctypes.byref(profile))
            if rc != RB_OK:
                raise RaceBearError("RB_Haptic_ApplyProfileToCurrentFunction", rc)

    def read_wind(self) -> RB_WindEffectConfig:
        """读取当前风感配置。"""
        config = RB_WindEffectConfig()
        rc = self._dll.RB_Wind_ReadConfig(ctypes.byref(config))
        if rc != RB_OK:
            raise RaceBearError("RB_Wind_ReadConfig", rc)
        return config

    def save_wind(self, config: RB_WindEffectConfig, apply: bool = True) -> None:
        """保存风感配置，并按需立即应用。"""
        rc = self._dll.RB_Wind_SaveConfig(ctypes.byref(config))
        if rc != RB_OK:
            raise RaceBearError("RB_Wind_SaveConfig", rc)
        if apply:
            rc = self._dll.RB_Wind_ApplyConfigToCurrentFunction(ctypes.byref(config))
            if rc != RB_OK:
                raise RaceBearError("RB_Wind_ApplyConfigToCurrentFunction", rc)

    def set_wind_test(self, enabled: bool, output_percent: float = 0.0) -> None:
        """设置四路统一风感测试；离开页面和退出前必须关闭。"""
        rc = self._dll.RB_Wind_SetTestOutput(int(enabled), output_percent)
        if rc != RB_OK:
            raise RaceBearError("RB_Wind_SetTestOutput", rc)

    def read_seatbelt(self) -> RB_SeatbeltEffectConfig:
        """读取当前安全带配置。"""
        config = RB_SeatbeltEffectConfig()
        rc = self._dll.RB_Seatbelt_ReadConfig(ctypes.byref(config))
        if rc != RB_OK:
            raise RaceBearError("RB_Seatbelt_ReadConfig", rc)
        return config

    def save_seatbelt(self, config: RB_SeatbeltEffectConfig, apply: bool = True) -> None:
        """保存安全带配置，并按需立即应用。"""
        rc = self._dll.RB_Seatbelt_SaveConfig(ctypes.byref(config))
        if rc != RB_OK:
            raise RaceBearError("RB_Seatbelt_SaveConfig", rc)
        if apply:
            rc = self._dll.RB_Seatbelt_ApplyConfigToCurrentFunction(ctypes.byref(config))
            if rc != RB_OK:
                raise RaceBearError("RB_Seatbelt_ApplyConfigToCurrentFunction", rc)

    def set_seatbelt_test(self, enabled: bool, left_percent: float = 0.0, right_percent: float = 0.0) -> None:
        """设置左右安全带测试；离开页面和退出前必须关闭。"""
        rc = self._dll.RB_Seatbelt_SetTestOutput(int(enabled), left_percent, right_percent)
        if rc != RB_OK:
            raise RaceBearError("RB_Seatbelt_SetTestOutput", rc)

    def read_json(self, key: str) -> Any:
        """读取并解析低频 JSON 数据包，缓冲区不足时自动翻倍重试。"""
        # 从 64 KiB 开始，避免目录较小时一开始就分配大块内存。
        size = 64 * 1024
        while size <= 16 * 1024 * 1024:
            buffer = ctypes.create_string_buffer(size)
            rc = self._dll.RB_Data_ReadJson(
                key.encode("utf-8"), buffer, size
            )
            if rc == RB_OK:
                return json.loads(buffer.value.decode("utf-8"))
            if rc != RB_BUFFER_TOO_SMALL:
                raise RaceBearError(f"RB_Data_ReadJson({key})", rc)
            # SDK 不返回所需长度，因此只能扩大缓冲区后重新调用。
            size *= 2
        raise MemoryError(f"JSON package {key!r} is larger than 16 MiB")

    def write_json(self, key: str, document: Any) -> None:
        """把 Python 对象序列化为完整配置 JSON 并保存。

        调用方应先 read_json，再修改已知字段并保留未知字段。保存后通常还要
        执行 runtime.reloadConfig 命令，运行态才会重新加载配置。
        """
        text = json.dumps(document, ensure_ascii=False, separators=(",", ":"))
        rc = self._dll.RB_Data_WriteJson(
            key.encode("utf-8"), text.encode("utf-8")
        )
        if rc != RB_OK:
            raise RaceBearError(f"RB_Data_WriteJson({key})", rc)

    def command(
        self,
        command: str,
        args: Optional[Union[str, Dict[str, Any]]] = None,
    ) -> Dict[str, Any]:
        """执行统一动作命令并返回解析后的结果对象。

        args 可以是 Python dict 或已经序列化的 JSON 字符串；None 自动转换为
        空对象 {}。SDK 返回非 RB_OK 时抛出 RaceBearError。
        """
        if args is None:
            args_text = "{}"
        elif isinstance(args, str):
            args_text = args
        else:
            args_text = json.dumps(args, ensure_ascii=False, separators=(",", ":"))

        # 命令结果通常很小，仍处理 RB_BUFFER_TOO_SMALL 以保证接口完整。
        size = 4096
        while size <= 1024 * 1024:
            buffer = ctypes.create_string_buffer(size)
            rc = self._dll.RB_Command_Run(
                command.encode("utf-8"),
                args_text.encode("utf-8"),
                buffer,
                size,
            )
            if rc == RB_BUFFER_TOO_SMALL:
                size *= 2
                continue
            result_text = buffer.value.decode("utf-8") or "{}"
            result = json.loads(result_text)
            if rc != RB_OK:
                raise RaceBearError(f"RB_Command_Run({command})", rc)
            return result
        raise MemoryError(f"Command result for {command!r} is larger than 1 MiB")

    def read_log(self) -> Optional[str]:
        """弹出一条运行日志；队列为空时返回 None。"""
        buffer = ctypes.create_string_buffer(4096)
        rc = self._dll.RB_Log_Read(buffer, len(buffer))
        if rc != RB_OK:
            raise RaceBearError("RB_Log_Read", rc)
        if not buffer.value:
            return None
        return buffer.value.decode("utf-8", errors="replace")

    def telemetry_index(self, key: str) -> int:
        """按RaceBear稳定key解析当前进程索引；调用方不得持久化返回值。"""
        index = self._dll.RB_Telemetry_FindIndexByKey(key.encode("utf-8"))
        if index < 0:
            raise RaceBearError(f"RB_Telemetry_FindIndexByKey({key})", index)
        return index

    def open_external_source(
        self,
        source_key: str,
        source_name: str,
        timeout_ms: int = 500,
        transition_ms: int = 2000,
    ) -> None:
        """创建外部游戏Source；该操作会替换当前活动的内置游戏Source。"""
        desc = RB_ExternalSourceDesc()
        desc.Size = ctypes.sizeof(desc)
        desc.Version = 1
        desc.SourceKey = source_key.encode("utf-8")
        desc.SourceName = source_name.encode("utf-8")
        desc.DataTimeoutMs = timeout_ms
        desc.TransitionMs = transition_ms
        rc = self._dll.RB_ExternalSource_Open(ctypes.byref(desc))
        if rc != RB_OK:
            raise RaceBearError("RB_ExternalSource_Open", rc)

    def submit_external_frame(
        self,
        sequence: int,
        values: Dict[str, float],
        timestamp_us: int = 0,
    ) -> None:
        """提交完整帧；values的键必须来自RaceBear遥测目录。"""
        if len(values) > RB_MAX_TELEMETRY_VALUES:
            raise ValueError("external telemetry frame contains too many values")
        frame = RB_ExternalTelemetryFrame()
        frame.Size = ctypes.sizeof(frame)
        frame.Version = 1
        frame.Sequence = sequence
        frame.TimestampUs = timestamp_us
        frame.ValueCount = len(values)
        for position, (key, value) in enumerate(values.items()):
            frame.Values[position].Index = self.telemetry_index(key)
            frame.Values[position].Value = float(value)
        rc = self._dll.RB_ExternalSource_SubmitFrame(ctypes.byref(frame))
        if rc != RB_OK:
            raise RaceBearError("RB_ExternalSource_SubmitFrame", rc)

    def close_external_source(self) -> None:
        """关闭当前外部游戏源；SDK整体运行时继续工作。"""
        self._dll.RB_ExternalSource_Close()

    def telemetry_field_id(self, key: str) -> int:
        """取得可写入客户MMF/UDP协议的稳定FieldId。"""
        field_id = self._dll.RB_Telemetry_FindFieldIdByKey(key.encode("utf-8"))
        if field_id < 0:
            raise RaceBearError(f"RB_Telemetry_FindFieldIdByKey({key})", field_id)
        return field_id

    def register_custom_udp_game(
        self, game_key: str, game_name: str, process_names: str,
        listen_port: int, image_name: str = "", input_mode: int = RB_CUSTOM_INPUT_RAW,
    ) -> None:
        """注册并持久化一个由SDK监听本机UDP的正式自定义游戏。"""
        registration = RB_CustomGameRegistration()
        registration.Size = ctypes.sizeof(registration)
        registration.Version = 1
        registration.Game.Size = ctypes.sizeof(RB_CustomGameDesc)
        registration.Game.Version = 1
        registration.Game.GameKey = game_key.encode("utf-8")
        registration.Game.GameName = game_name.encode("utf-8")
        registration.Game.ProcessNames = process_names.encode("utf-8")
        registration.Game.ImageName = image_name.encode("utf-8")
        registration.Game.Transport = RB_CUSTOM_TRANSPORT_UDP
        registration.Game.InputMode = input_mode
        registration.Game.DataTimeoutMs = 500
        registration.Game.TransitionMs = 2000
        registration.Udp.ListenAddress = b"127.0.0.1"
        registration.Udp.ListenPort = listen_port
        registration.Udp.AllowedSenderAddress = b"127.0.0.1"
        rc = self._dll.RB_Game_RegisterCustom(ctypes.byref(registration))
        if rc != RB_OK:
            raise RaceBearError("RB_Game_RegisterCustom", rc)

    def serial_ports(self) -> List[Dict[str, str]]:
        """枚举当前系统串口，并把 C 固定数组转换为普通 Python 字典。"""
        count = self._dll.RB_Serial_GetAvailablePortCount()
        if count < 0:
            raise RaceBearError("RB_Serial_GetAvailablePortCount", count)

        result: List[Dict[str, str]] = []
        for index in range(count):
            info = RB_SerialPortInfo()
            rc = self._dll.RB_Serial_GetAvailablePortInfo(
                index, ctypes.byref(info)
            )
            if rc == RB_OK:
                result.append(
                    {
                        "port_name": decode_utf8(info.PortName),
                        "friendly_name": decode_utf8(info.FriendlyName),
                        "description": decode_utf8(info.Description),
                        "manufacturer": decode_utf8(info.Manufacturer),
                    }
                )
        return result

    def __enter__(self) -> "RaceBearSDK":
        """支持 with 语法；进入上下文只返回对象，不会隐式初始化。"""
        return self

    def __exit__(self, exc_type: Any, exc_value: Any, traceback: Any) -> None:
        """无论 with 内是否抛出异常，都按正确顺序关闭 SDK。"""
        self.close()
