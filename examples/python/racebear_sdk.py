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
from typing import Any, Dict, List, Optional, Union


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

    def initialize(self, app_data_name: str, frequency_hz: int = 100) -> None:
        """设置独立应用目录、初始化 SDK 并启动内部计算循环。

        app_data_name 必须是目录名而不是完整路径。任一步失败都会抛出
        RaceBearError；计算循环启动失败时会先自动关闭已初始化的 SDK。
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
        # 后端计算由 SDK 的 MultimediaTimer 驱动，Python 只负责读取状态。
        rc = self._dll.RB_Runtime_StartLoop(frequency_hz)
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
