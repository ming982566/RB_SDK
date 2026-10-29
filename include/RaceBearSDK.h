#pragma once

// RaceBear SDK public C ABI.
// 公开接口只使用 C 基础类型和固定长度字符数组，避免 std::string/vector/bool 跨 DLL 边界导致 ABI 不稳定。
//
// Developer contract:
// 1. Include this header and link RaceBearSDK.lib, then ship RaceBearSDK.dll with the host app.
// 2. Call RB_Runtime_SetAppDataName() first if the host app does not want to use the default SimRacebear config directory.
// 3. Call RB_Runtime_Initialize() once before using runtime/config/output APIs.
// 4. Call RB_Runtime_StartLoop() to let the SDK drive backend calculations internally.
// 5. Call RB_Runtime_Shutdown() once before process exit. Shutdown automatically stops the runtime loop.
// 6. All char buffers are UTF-8. wchar_t* parameters are Windows UTF-16.
// 7. For full integration steps, examples, return-value rules, and field units, read RaceBearSDK_API.md.
// 8. When License.OutputAllowed is 0, protected calculated fields are zeroed and runtime Apply/test/connect APIs return RB_LICENSE_REQUIRED.
//    Configuration, catalogs, raw telemetry, license actions, serial utilities, disconnect, and reset APIs remain available.
#ifdef RACEBEARSDK_EXPORTS
#define RB_API extern "C" __declspec(dllexport)
#else
#define RB_API extern "C" __declspec(dllimport)
#endif

#include <stdint.h>

// 通用返回值。0 表示成功，负数表示失败。
enum RB_Result
{
	RB_OK = 0,                    // 成功。
	RB_ERROR = -1,                // 通用失败。
	RB_INVALID_ARGUMENT = -2,     // 参数为空、类型无效或索引越界。
	RB_BUFFER_TOO_SMALL = -3,     // 调用方传入的文本缓冲区太小。
	RB_LICENSE_REQUIRED = -4      // 当前授权无效或试用已到期，受保护运行功能不可用。
};

// SDK 授权状态。授权无效时 SDK 不退出宿主程序，但停止专有计算并锁定运行态 Apply、测试和输出连接。
enum RB_LicenseState
{
	RB_LICENSE_UNKNOWN = 0,        // 尚未完成授权检查。
	RB_LICENSE_CHECKING = 1,       // 正在检查授权。
	RB_LICENSE_OUTPUT_LOCKED = 2,  // 授权无效，输出被锁定。
	RB_LICENSE_PERPETUAL = 3,      // 永久授权。
	RB_LICENSE_TIMED = 4,          // 限时授权。
	RB_LICENSE_TRIAL = 5           // 本地试用倒计时，倒计时结束后锁定输出。
};

// 授权异常原因，用于前端显示具体问题。
enum RB_LicenseProblem
{
	RB_LICENSE_PROBLEM_NONE = 0,                // 无异常。
	RB_LICENSE_PROBLEM_NO_LICENSE = 1,          // 未找到本地授权。
	RB_LICENSE_PROBLEM_LOCAL_FILE_INVALID = 2,  // 本地授权文件无效。
	RB_LICENSE_PROBLEM_HARDWARE_MISMATCH = 3,   // 授权机器码不匹配。
	RB_LICENSE_PROBLEM_NETWORK_ERROR = 4,       // 联网检查失败。
	RB_LICENSE_PROBLEM_SERVER_ERROR = 5,        // 授权服务器返回错误。
	RB_LICENSE_PROBLEM_EXPIRED = 6,             // 授权过期。
	RB_LICENSE_PROBLEM_DISABLED = 7,            // 授权被禁用。
	RB_LICENSE_PROBLEM_REVOKED = 8,             // 授权被撤销。
	RB_LICENSE_PROBLEM_PARSE_FAILED = 9         // 授权内容解析失败。
};

// 内置配置文件类型。配置读写走 JSON，便于宿主前端保存完整参数。
enum RB_ConfigType
{
	RB_CONFIG_TELEMETRY = 1,    // 游戏遥测配置。
	RB_CONFIG_DYNAMIC_V2 = 2,   // 动态滤波配置；运动增强和震动使用独立的类型化接口。
	RB_CONFIG_PLATFORM = 3,     // 平台结构和运行参数配置。
	RB_CONFIG_OUTPUT = 4        // 输出实例配置。
};

// 固定数组上限。第三方可直接按这些常量定义 UI 缓冲。
enum RB_Limits
{
	RB_TEXT_SMALL = 64,              // 短 key、轴名等文本长度。
	RB_TEXT_MEDIUM = 128,            // 名称、状态文本长度。
	RB_TEXT_LARGE = 260,             // 路径、错误、进程列表等文本长度。
	RB_DOF_COUNT = 6,                // Sway/Surge/Heave/Yaw/Roll/Pitch 固定 6DOF。
	RB_WIND_CHANNEL_COUNT = 4,       // 风感模拟输出通道数。
	RB_MAX_GAMES = 512,              // 游戏目录最大返回数量。
	RB_MAX_TELEMETRY_VALUES = 512,   // 遥测值最大返回数量。
	RB_MAX_OUTPUT_INSTANCES = 32,    // 输出实例最大返回数量。
	RB_MAX_OUTPUT_KEYS = 128,        // 输出 key 最大返回数量。
	RB_MAX_CAN_MOTORS = 16,          // CAN 电机最大返回数量。
	RB_MAX_EFFECTS = 32,             // 运动/震动效果最大返回数量。
	RB_MAX_PLUGINS = 64,             // 功能插件目录最大返回数量。
	RB_CUSTOM_PACKET_MAX_SIZE = 8192,// 自定义游戏MMF/UDP单帧最大字节数。
	RB_DYNAMIC_DOF_COUNT = 8,        // 动态配置 DOF 槽位数。
	RB_DYNAMIC_MAX_INPUTS = 8,       // 每个 DOF 最大输入数。
	RB_DYNAMIC_MAX_CANDIDATES = 8,   // 单个 DOF 可添加的输入效果候选上限。
	RB_MOTION_EFFECT_COUNT = 5,      // 运动增强效果数量。
	RB_MOTION_ROUTE_COUNT = 6,       // 每个运动增强效果最大输出路由数。
	RB_HAPTIC_EFFECT_COUNT = 7,      // 震动效果数量。
	RB_SEATBELT_OUTPUT_COUNT = 2,    // 安全带输出通道数。
	RB_RGB_OUTPUT_COUNT = 4,         // RGBAPP 独立 WS2812B 输出路数。
	RB_RGB_EFFECT_LAYER_COUNT = 16,  // 单个 RGB 配置最大效果层数。
	RB_RGB_EFFECT_INPUT_COUNT = 12,  // 单个 RGB 效果最多返回的语义输入数量。
	RB_MAX_DASHBOARD_PACKAGES = 128 // 客户仪表目录最大返回数量。
};

enum RB_RgbEffectType
{
	RB_RGB_EFFECT_STATIC = 0,
	RB_RGB_EFFECT_RPM_BAR = 1,
	RB_RGB_EFFECT_SHIFT_FLASH = 2,
	RB_RGB_EFFECT_TELEMETRY_THRESHOLD = 3,
	RB_RGB_EFFECT_PULSE = 4,
	RB_RGB_EFFECT_STATUS_COLOR = 5,
	RB_RGB_EFFECT_TELEMETRY_BLINK = 6, // 任意遥测值达到阈值后闪烁。
	RB_RGB_EFFECT_AUDIO_VOLUME = 7 // 按电脑当前播放声音的音量驱动亮度和点亮数量。
};

// RGB效果输入值的语义类型，供前端选择合适的参数控件和默认范围。
enum RB_RgbEffectInputValueType
{
	RB_RGB_INPUT_CONTINUOUS = 0, // 连续数值或归一化派生强度。
	RB_RGB_INPUT_BOOLEAN = 1,    // 0/1 状态值。
	RB_RGB_INPUT_PERCENT = 2,    // 0-100 百分比。
	RB_RGB_INPUT_EVENT = 3       // 0-1 短时事件或脉冲强度。
};

// RGB效果输入标志。
enum RB_RgbEffectInputFlags
{
	RB_RGB_INPUT_FLAG_FIXED = 1 << 0,       // 效果语义固定，前端不允许改为其它输入。
	RB_RGB_INPUT_FLAG_RECOMMENDED = 1 << 1, // 当前效果的首选输入。
	RB_RGB_INPUT_FLAG_DERIVED = 1 << 2      // SDK统一计算的派生效果值。
};

// RGB效果显示目标。安全带联动目标依赖已就绪的联动控制器。
enum RB_RgbEffectDisplayMode
{
	RB_RGB_DISPLAY_INDEPENDENT = 0,
	RB_RGB_DISPLAY_SEATBELT_LINK = 1,
	RB_RGB_DISPLAY_BOTH = 2
};

enum RB_RgbFillMode
{
	RB_RGB_FILL_FORWARD = 0,    // 从区段起点向终点填充。
	RB_RGB_FILL_REVERSE = 1,    // 从区段终点向起点填充。
	RB_RGB_FILL_CENTER_OUT = 2  // 从区段中心向两侧填充。
};

enum RB_RgbTestPattern
{
	RB_RGB_TEST_OFF = 0,
	RB_RGB_TEST_SOLID = 1,
	RB_RGB_TEST_COLOR_WIPE = 2,
	RB_RGB_TEST_RAINBOW = 3,
	RB_RGB_TEST_OUTPUT_IDENTIFY = 4
};

// 自定义游戏遥测传输方式。MMF仅支持同机Windows进程，UDP支持跨进程和跨主机。
enum RB_CustomGameTransport
{
	RB_CUSTOM_TRANSPORT_MMF = 1,     // Windows命名共享内存。
	RB_CUSTOM_TRANSPORT_UDP = 2      // 无连接UDP数据报。
};

// 自定义游戏数据模式。注册后固定，运行过程中不允许混用两种包体。
enum RB_CustomGameInputMode
{
	RB_CUSTOM_INPUT_RAW = 1,         // 原始遥测字段，进入完整Source和动态计算链。
	RB_CUSTOM_INPUT_6DOF = 2         // 已计算的归一化6DOF，绕过动态映射后进入平台姿态层。
};

// 安全带输入模式。当前遥测算法始终可用；HID 和混合模式要求宿主提供对应 HID 输入源。
enum RB_SeatbeltInputMode
{
	RB_SEATBELT_INPUT_TELEMETRY = 0,
	RB_SEATBELT_INPUT_HID = 1,
	RB_SEATBELT_INPUT_MIXED = 2
};

// 直接6DOF选项，可按位组合。
enum RB_CustomGameDofOptions
{
	RB_CUSTOM_DOF_ADD_MOTION_EFFECTS = 1 // 在客户6DOF上继续叠加RaceBear运动增强效果。
};

// 授权快照。包含前端显示授权状态需要的全部信息。
struct RB_LicenseSnapshot
{
	int State;                                   // RB_LicenseState。
	int Problem;                                 // RB_LicenseProblem。
	int OutputAllowed;                           // 是否允许真实输出，1=允许，0=锁定。
	long long RemainingSeconds;                  // 限时授权或本地试用剩余秒数；永久授权为0。
	char ActivationCode[RB_TEXT_MEDIUM];         // 当前激活码，未激活时为空。
};

// 当前游戏状态。用于首页、启动/停止按钮、当前游戏标题等 UI。
struct RB_GameState
{
	int CatalogIndex;                            // 游戏目录索引，未选择时为 -1。
	int SteamAppID;                              // Steam AppID，非 Steam 游戏为 0。
	char GameKey[RB_TEXT_SMALL];                 // 稳定游戏 key，用于配置和命令。
	char GameName[RB_TEXT_MEDIUM];               // 显示名称。
	char ImageName[RB_TEXT_MEDIUM];              // 游戏封面图片名。
	char ProcessNames[RB_TEXT_LARGE];            // 进程匹配列表，多个进程由内部约定分隔。
	int CanCreateSource;                         // 是否支持创建遥测源。
	int NeedsTelemetryConfig;                    // 当前游戏是否需要遥测配置入口。
	int ProcessRunning;                          // 游戏进程是否正在运行。
};

// 当前遥测源状态。用于显示连接中、等待数据、运行、错误信息等。
struct RB_SourceState
{
	int Count;                                   // 当前 Source 容器数量。
	int ActiveIndex;                             // 当前 Source 索引，未激活时为 -1。
	int Connected;                               // 是否已连接。
	int Ready;                                   // 是否就绪并可输出。
	int TelemetryActive;                         // 是否已进入稳定遥测运行态。
	int NeedsTelemetryConfig;                    // 当前源是否需要配置入口。
	int StateCode;                               // Source 内部状态码。
	double TransitionPercent;                    // 淡入/淡出过渡进度，0-1。
	char Name[RB_TEXT_MEDIUM];                   // Source 名称。
	char StatusText[RB_TEXT_MEDIUM];             // 当前状态文本。
	char ErrorText[RB_TEXT_LARGE];               // 最近错误文本。
	double FinalSpeed;                           // 最终速度摘要。
	double FinalRPM;                             // 最终转速摘要。
	double FinalGear;                            // 最终档位摘要。
	double FinalWheelSlip;                       // 派生轮滑强度，0-1。
	double FinalWheelLock;                       // 派生锁胎强度，0-1。
	double FinalWheelSpin;                       // 派生驱动轮空转强度，0-1。
	double FinalTractionLoss;                    // 派生循迹丢失强度，0-1。
	double FinalABSPulse;                        // ABS 介入强度，0-1。
	double FinalTCPulse;                         // TC 介入强度，0-1。
};

// 6DOF 分层状态。数组顺序固定为 Sway/Surge/Heave/Yaw/Roll/Pitch。
struct RB_Motion6DOFState
{
	double RawInput[RB_DOF_COUNT];               // Source 映射到 6DOF 后的原始输入。
	double BaseFiltered[RB_DOF_COUNT];           // 基础滤波后的 6DOF。
	double MixDelta[RB_DOF_COUNT];               // 基础混合增量。
	double EffectDelta[RB_DOF_COUNT];            // 运动增强效果增量。
	double Unclipped[RB_DOF_COUNT];              // 裁剪前合成输出。
	double Cropped[RB_DOF_COUNT];                // 姿态范围裁剪后的输出。
	double RigFiltered[RB_DOF_COUNT];            // 平台层最终滤波输出。
	double PoseLimit[RB_DOF_COUNT];              // 姿态层限制。
	double RigLimit[RB_DOF_COUNT];               // 平台几何限制。
	int PosePossible[RB_DOF_COUNT];              // 当前平台是否可达该 DOF。
};

// 当前动感平台状态。
struct RB_RigState
{
	int Count;                                   // 当前 Rig 容器数量。
	int ActiveIndex;                             // 当前 Rig 索引。
	char RigName[RB_TEXT_MEDIUM];                // 平台名称。
	int RigType;                                 // 平台类型编号。
	int NumberOfAxes;                            // 执行器轴数量。
	RB_Motion6DOFState MotionState;              // 6DOF 分层输出状态。
};

// 当前功能模块状态，覆盖风感/安全带/震动等 Function 容器摘要。
struct RB_FunctionState
{
	int Count;                                   // Function 数量。
	int ActiveIndex;                             // 当前 Function 索引。
	char FunctionName[RB_TEXT_MEDIUM];           // 当前 Function 名称。
	int Enabled;                                 // 是否启用。
	int OutputCount;                             // 输出通道数量。
	long FirstOutputValue;                       // 第一个输出通道的位值摘要。
	int HapticEffectCount;                       // 震动效果数量。
};

// 当前输出层状态。用于输出连接按钮、自动连接状态、发送间隔显示。
struct RB_OutputState
{
	int Count;                                   // 输出实例数量。
	int ActiveIndex;                             // 当前输出实例索引。
	char OutName[RB_TEXT_MEDIUM];                // 当前输出名称。
	int OutType;                                 // 输出类型编号。
	int Connected;                               // 当前输出是否已连接。
	double TransitionPercent;                    // 输出连接/断开过渡进度。
	double SendTimeInterval;                     // 发送间隔，单位毫秒。
	int AnyRuntimeActive;                        // 是否有任意输出运行态激活。
	int AllDisconnected;                         // 是否全部输出已断开。
};

// 单个 CAN 电机状态。用于 CAN 设备页显示节点状态。
struct RB_CanMotorState
{
	int NodeId;                                  // CAN 节点 ID。
	char AxisKey[RB_TEXT_SMALL];                 // 轴号，例如 A1/A2。
	int Online;                                  // 是否在线。
	int Homed;                                   // 是否已回零。
	int Ready;                                   // 是否已进入可运行状态。
	int Fault;                                   // 是否故障。
	int StatusWord;                              // CiA402 状态字。
	int ErrorCode;                               // 驱动器错误码。
	int Temperature;                             // 温度值，单位由设备协议决定。
	int OperationMode;                           // 当前运行模式。
};

// CAN 总状态。包含控制器状态、电机列表和固件升级状态。
struct RB_CanState
{
	char StatusText[RB_TEXT_MEDIUM];             // CAN 控制器状态文本。
	int MotorCount;                              // Motors 中有效电机数量。
	int FirmwareUpdateRunning;                   // 固件升级是否运行中。
	int FirmwareUpdateComplete;                  // 固件升级是否已结束。
	int FirmwareUpdateSuccess;                   // 固件升级是否成功。
	int FirmwareUpdateProgress;                  // 固件升级进度，0-100。
	int FirmwareUpdateErrorCode;                 // 固件升级错误码。
	char FirmwareUpdateStatusText[RB_TEXT_MEDIUM]; // 固件升级状态文本。
	RB_CanMotorState Motors[RB_MAX_CAN_MOTORS];  // CAN 电机状态数组。
};

// RB_CAN 输出与物理卡的稳定绑定状态。新结构通过 Size 字段独立版本化，不改变旧结构布局。
enum RB_CanBindingStatus
{
	RB_CAN_BINDING_UNBOUND = 0,                  // 尚未绑定 UID 和逻辑索引。
	RB_CAN_BINDING_READY = 1,                    // UID、逻辑索引和协议均匹配。
	RB_CAN_BINDING_OFFLINE = 2,                  // 已有稳定绑定，但物理卡当前离线。
	RB_CAN_BINDING_CONFLICT = 3,                 // UID 或逻辑索引发生重复/不匹配。
	RB_CAN_BINDING_IAP = 4,                      // 物理卡当前处于 IAP 模式。
	RB_CAN_BINDING_ERROR = 5                     // 身份协议或底层访问失败。
};

// 一张当前枚举到的 RB_CAN 卡身份快照。
struct RB_CanDeviceInfo
{
	unsigned int Size;                           // 调用前设为 sizeof(RB_CanDeviceInfo)。
	int CatalogIndex;                            // 本次目录索引，仅用于继续读取目录。
	int EnumerationIndex;                        // 本次目录内唯一运行时访问索引，仅用于当前快照。
	int LogicalIndex;                            // 卡内持久逻辑索引，0 表示未配置。
	int ProtocolType;                            // 固件协议身份：1=RB_CAN，2=FM_CAN，3=FC_CAN/RDS_CAN物理卡身份。
	int IapMode;                                 // 1 表示当前处于 IAP。
	int IdentityValid;                           // 1 表示身份报文有效。
	int DuplicateLogicalIndex;                   // 1 表示检测到重复逻辑索引。
	unsigned int FirmwareVersion;                // APP 或 IAP 固件版本。
	unsigned int Capabilities;                   // 固件能力位。
	unsigned int ConfigGeneration;               // 索引持久记录代数。
	char Uid[RB_TEXT_SMALL];                     // 24 位大写十六进制 MCU UID。
	char StatusText[RB_TEXT_MEDIUM];             // 设备身份或冲突状态文本。
};

// 一个 OUT_n 与物理 RB_CAN 卡的运行时绑定快照。
struct RB_CanOutputBindingState
{
	unsigned int Size;                           // 调用前设为 sizeof(RB_CanOutputBindingState)。
	int RuntimeInstanceIndex;                    // 对应 Output_List/OUT_n。
	int BindingStatus;                           // RB_CanBindingStatus 中的值。
	int EnumerationIndex;                        // 当前实际打开句柄的运行时访问索引。
	int LogicalIndex;                            // 当前绑定的持久逻辑索引。
	int ProtocolType;                            // 当前绑定卡协议类型。
	unsigned int FirmwareVersion;                // 当前绑定卡固件版本。
	unsigned int Capabilities;                   // 当前绑定卡能力位。
	int DeviceOpened;                            // 1 表示 WinUSB 设备已打开。
	int Ready;                                   // 1 表示该卡和所属电机可接收位置输出。
	int EStopLatched;                            // 1 表示全局急停仍锁存。
	int EStopConfirmed;                          // 1 表示本卡已确认最近一次急停命令。
	int EStopCommandStatus;                      // 固件急停状态或底层 WUC 错误码。
	char Uid[RB_TEXT_SMALL];                     // 当前绑定卡 MCU UID。
	char StatusText[RB_TEXT_MEDIUM];             // 绑定或运行状态文本。
};

enum RB_CanRuntimeStatus
{
	RB_CAN_RUNTIME_STARTING = 0,
	RB_CAN_RUNTIME_READY = 1,
	RB_CAN_RUNTIME_RUNNING = 2,
	RB_CAN_RUNTIME_DEGRADED = 3,
	RB_CAN_RUNTIME_RECOVERING = 4,
	RB_CAN_RUNTIME_SAFE_LOCKED = 5,
	RB_CAN_RUNTIME_MAINTENANCE = 6
};

enum RB_CanFaultReason
{
	RB_CAN_FAULT_NONE = 0,
	RB_CAN_FAULT_DEVICE_OFFLINE = 1,
	RB_CAN_FAULT_IDENTITY_CONFLICT = 2,
	RB_CAN_FAULT_USB_TRANSPORT = 3,
	RB_CAN_FAULT_BUS_OFF = 4,
	RB_CAN_FAULT_NODE_OFFLINE = 5,
	RB_CAN_FAULT_DRIVE = 6,
	RB_CAN_FAULT_ESTOP = 7,
	RB_CAN_FAULT_RECOVERY_BUDGET = 8,
	RB_CAN_FAULT_FIRMWARE_MAINTENANCE = 9,
	RB_CAN_FAULT_CONFIGURATION = 10
};

// 无人值守宿主使用的版本化健康快照；新增字段时通过 Size 建立新版本。
struct RB_CanOutputHealthState
{
	unsigned int Size;
	int RuntimeStatus;
	int FaultReason;
	unsigned long long SessionGeneration;
	unsigned long long RecoveryGeneration;
	unsigned int RetryCount;
	unsigned int RetryBudget;
	unsigned int NextRetryDelayMs;
	int DeviceConnected;
	int DeviceOpened;
	int Armed;
	int OutputActive;
	int RequiresHumanIntervention;
	unsigned int LastSuccessfulReadAgeMs;
	unsigned int LastSuccessfulWriteAgeMs;
	unsigned int UsbWriteFailures;
	unsigned int RealtimeBatchesSubmitted;
	unsigned int RealtimeBatchesReplaced;
	unsigned int RealtimeBatchesWritten;
	unsigned int MaxReliableQueueDepth;
	unsigned int RealtimeLatencyP50Ms;
	unsigned int RealtimeLatencyP95Ms;
	unsigned int RealtimeLatencyP99Ms;
	unsigned int RealtimeLatencyMaxMs;
	unsigned int CanTxOverflows;
	unsigned int CanRxOverflows;
	unsigned int UsbTxOverflows;
	unsigned int UsbRxOverflows;
	unsigned int UsbRxResyncs;
	unsigned int ReliableAccepted;
	unsigned int ReliableDeduplicated;
	unsigned int ReliableRejected;
	unsigned int FirmwareRealtimeReplaced;
	unsigned int CanTxQueueDepth;
	unsigned int CanTxQueueMaxDepth;
	unsigned int MotionCommandAgeMs;
	unsigned int BusOffCount;
	int BusOffActive;
	char StatusText[RB_TEXT_MEDIUM];
	unsigned int ReservedLegacyPadding;
	unsigned int CurrentSafetyQueueDepth;
	unsigned int CurrentControlQueueDepth;
	unsigned int CurrentReliableQueueDepth;
	unsigned int MaxSafetyQueueDepth;
	unsigned int MaxControlQueueDepth;
	unsigned int CanTxAccepted;
	unsigned int ControlAccepted;
	unsigned int ControlDeduplicated;
	unsigned int ControlRejected;
	unsigned int ReservedHealthV2Padding;         // 固定旧 336 字节 ABI 前缀中的尾部对齐空间。
	int UsbDiagnosticsSupported;                 // 当前 APP 是否声明 USB 链路 V4 诊断能力。
	int UsbDiagnosticsAvailable;                 // 当前会话是否已收到至少一份有效 V4 诊断。
	unsigned int UsbDiagnosticsAgeMs;            // 最近 V4 诊断年龄；不可用时为 UINT_MAX。
	unsigned int AppUsbIrqCount;                 // APP USB 中断累计次数。
	unsigned int AppUsbOutCount;                 // APP 收到 USB OUT 包的累计次数。
	unsigned int AppUsbResetCount;               // APP 观察到的 USB Reset 累计次数。
	unsigned int AppUsbConfiguredCount;          // APP 进入 Configured 状态的累计次数。
	unsigned int AppUsbSuspendCount;             // APP USB Suspend 累计次数。
	unsigned int AppUsbResumeCount;              // APP USB Resume 累计次数。
	unsigned int AppUsbHardwareErrorCount;       // APP USB 硬件 ERR 累计次数。
	unsigned int AppUsbHardwareOverrunCount;     // APP USB DOVR 累计次数。
	unsigned int AppUsbOutRearmMaxUs;            // APP OUT 端点重新挂接最大耗时，微秒。
	unsigned int AppUsbRxRingDepth;              // APP 当前 USB RX 环形缓冲深度。
	unsigned int AppUsbRxRingMaxDepth;           // APP USB RX 环形缓冲历史最大深度。
	unsigned int LastUsbWriteError;              // 最近一次实际 USB OUT 失败的 Win32 错误码。
	unsigned int LastUsbAbortPipeError;          // 最近一次 AbortPipe 失败的 Win32 错误码，0 表示成功。
	unsigned int LastUsbResetPipeError;          // 最近一次 ResetPipe 失败的 Win32 错误码，0 表示成功。
	unsigned int AppCpuUsagePercent;             // APP 主循环 CPU 使用率百分比。
	unsigned int CanTxErrorCount;                // 最近 CAN 错误事件中的发送错误计数 TEC。
	unsigned int CanRxErrorCount;                // 最近 CAN 错误事件中的接收错误计数 REC。
	unsigned int LastCanErrorCode;               // 最近 CAN 错误事件中的控制器错误码 LEC。
	unsigned int ReservedHealthV5Padding;        // 固定 424 字节 ABI 并保留后续扩展位置。
};

// 风感模拟运行状态。
struct RB_WindState
{
	int Enabled;                                 // 风感功能是否启用。
	double CurrentInputKmh;                      // 当前输入速度，km/h。
	double CurrentNormalizedPercent;             // 当前基础输出百分比。
	double ChannelNormalizedPercent[RB_WIND_CHANNEL_COUNT]; // 每通道输出百分比。
	long ChannelOutput[RB_WIND_CHANNEL_COUNT];   // 每通道位输出。
	int TestOutputEnabled;                       // 是否启用测试输出。
	double TestOutputPercent;                    // 测试输出百分比。
};

// 安全带模拟运行状态。
struct RB_SeatbeltState
{
	int Enabled;                                 // 安全带功能是否启用。
	int InputMode;                               // 输入模式。
	double CurrentSpeedKmh;                      // 当前车速，km/h。
	double BrakeTensionPercent;                  // Surge/刹车张紧百分比。
	double LeftLateralTensionPercent;            // 左侧横向张紧百分比。
	double RightLateralTensionPercent;           // 右侧横向张紧百分比。
	int CrashActive;                             // 碰撞张紧是否激活。
	double CrashHoldRemainingMs;                 // 碰撞保持剩余时间，毫秒。
	double LeftOutputPercent;                    // 左安全带输出百分比。
	double RightOutputPercent;                   // 右安全带输出百分比。
	long LeftOutput;                             // 左安全带位输出。
	long RightOutput;                            // 右安全带位输出。
	int TestOutputEnabled;                       // 是否启用测试输出。
	double TestLeftPercent;                      // 左测试输出百分比。
	double TestRightPercent;                     // 右测试输出百分比。
};

struct RB_RgbState
{
	int Size;
	int Version;
	int Enabled;
	int Connected;
	char DeviceUid[RB_TEXT_SMALL];
	int ActiveLayerCount;
	int LogicalPixelCount;
	unsigned int GeneratedFrames;
	unsigned int SubmittedFrames;
	unsigned int SentFrames;
	unsigned int DroppedFrames;
	unsigned int FailedFrames;
	int LastError;
	unsigned int EstimatedCurrentMa;
	unsigned int PowerScalePermille;
	double LastTelemetryValue[RB_RGB_EFFECT_LAYER_COUNT];
};

// 后端运行态总快照。前端高频读取此结构体即可刷新主状态 UI。
struct RB_RuntimeState
{
	int Size;                                    // 结构体字节数，由 SDK 填写。
	int Version;                                 // 结构体版本，当前为 2。
	RB_LicenseSnapshot License;                  // 授权状态。
	RB_GameState Game;                           // 当前游戏状态。
	RB_SourceState Source;                       // 遥测源状态。
	RB_RigState Rig;                             // 动感平台状态。
	RB_FunctionState Function;                   // 功能模块摘要。
	RB_OutputState Output;                       // 输出层状态。
	RB_CanState Can;                             // CAN 状态。
	RB_WindState Wind;                           // 风感状态。
	RB_SeatbeltState Seatbelt;                   // 安全带状态。
	char LastAction[RB_TEXT_MEDIUM];             // 最近动作说明。
	char LastError[RB_TEXT_LARGE];               // 最近错误说明。
	int LastRequestSuccess;                      // 最近请求是否成功。
};

// 游戏目录项。
struct RB_GameCatalogItem
{
	int CatalogIndex;                            // 游戏目录索引。
	int SteamAppID;                              // Steam AppID。
	char GameKey[RB_TEXT_SMALL];                 // 稳定游戏 key。
	char GameName[RB_TEXT_MEDIUM];               // 显示名称。
	char ImageName[RB_TEXT_MEDIUM];              // 图片名称。
	char ProcessNames[RB_TEXT_LARGE];            // 进程匹配文本。
	int CanCreateSource;                         // 是否支持创建遥测源。
};

// 游戏目录列表。调用 RB_Game_GetCatalog 填充。
struct RB_GameCatalog
{
	int Size;                                    // 结构体字节数，由 SDK 填写。
	int Version;                                 // 结构体版本，当前为 1。
	int Count;                                   // Items 中有效项数量。
	RB_GameCatalogItem Items[RB_MAX_GAMES];      // 游戏目录项数组。
};

// 遥测值目录项。
struct RB_TelemetryCatalogItem
{
	int Index;                                   // 当前进程内真实遥测索引；目录可能跳过内部保留槽位，不能用数组行号代替。
	unsigned int FieldId;                        // 自定义游戏线协议稳定字段号。
	char Key[RB_TEXT_SMALL];                     // 稳定 key。
	char Name[RB_TEXT_MEDIUM];                   // 显示名称。
	char Unit[RB_TEXT_SMALL];                    // 标准单位；空文本表示无量纲、状态值或尚未标准化。
	int ShouldRound;                             // 显示时是否建议取整。
	int Origin;                                  // 来源类型：原生、派生、兼容等。
	int SupportedByCurrentSource;                // 当前 Source 是否支持该原始值。
};

// 自定义游戏基本信息。注册成功后会像内置游戏一样进入游戏目录和进程自动检测。
struct RB_CustomGameDesc
{
	int Size;                                    // 调用方填写sizeof(RB_CustomGameDesc)。
	int Version;                                 // 当前填写1。
	char GameKey[RB_TEXT_SMALL];                 // 全局稳定唯一key，例如vendor.game_name。
	char GameName[RB_TEXT_MEDIUM];               // 游戏列表显示名称。
	char ProcessNames[RB_TEXT_LARGE];            // 进程名，多个名称使用*分隔。
	char ImageName[RB_TEXT_MEDIUM];              // 游戏图片文件名；图片由宿主前端提供。
	int SteamAppID;                              // 非Steam游戏填写0。
	int Transport;                               // RB_CustomGameTransport。
	int InputMode;                               // RB_CustomGameInputMode。
	int DataTimeoutMs;                           // 没有新包后进入淡出的超时，建议500毫秒。
	int TransitionMs;                            // 恢复/中断时淡入淡出时间，建议2000毫秒。
	int DofOptions;                              // RB_CustomGameDofOptions，仅6DOF模式有效。
};

// 自定义游戏MMF配置。游戏进程创建命名映射，SDK只读打开并周期轮询。
struct RB_CustomGameMmfConfig
{
	char MappingName[RB_TEXT_MEDIUM];            // 建议Local\\RaceBear.CustomGame.vendor.game_name。
	int MappingSize;                             // 映射总字节数，至少容纳MMF头和最大包。
};

// 自定义游戏UDP配置。默认建议只监听127.0.0.1，避免遥测端口暴露到局域网。
struct RB_CustomGameUdpConfig
{
	char ListenAddress[RB_TEXT_SMALL];           // SDK监听地址，例如127.0.0.1或0.0.0.0。
	int ListenPort;                              // 监听端口，范围1-65535。
	char AllowedSenderAddress[RB_TEXT_SMALL];    // 允许的发送端IP；空文本表示不限制。
};

// 自定义游戏完整注册配置。Transport决定使用Mmf或Udp成员。
struct RB_CustomGameRegistration
{
	int Size;                                    // 调用方填写sizeof(RB_CustomGameRegistration)。
	int Version;                                 // 当前填写1。
	RB_CustomGameDesc Game;                      // 游戏目录、进程和输入模式。
	RB_CustomGameMmfConfig Mmf;                  // MMF传输配置。
	RB_CustomGameUdpConfig Udp;                  // UDP传输配置。
};

// 自定义遥测统一包头。MMF和UDP使用相同的小端协议，包体紧随HeaderSize之后。
#pragma pack(push, 1)
struct RB_CustomTelemetryPacketHeader
{
	unsigned int Magic;                          // 固定为RB_CUSTOM_PACKET_MAGIC。
	unsigned short ProtocolVersion;              // 当前为1。
	unsigned short HeaderSize;                   // 填写sizeof(RB_CustomTelemetryPacketHeader)。
	unsigned int PacketSize;                     // 包头和包体总字节数。
	unsigned long long Sequence;                 // 强制逐帧递增的包ID；重复或倒序包会被丢弃。
	unsigned long long GameTimeUs;               // 游戏时间或采集时间，单位微秒；用于诊断。
	unsigned int InputMode;                      // 必须与注册时RB_CustomGameInputMode一致。
	unsigned int ValueCount;                     // 原始模式字段数；6DOF模式固定为6。
};

// 原始遥测包体条目。FieldId来自稳定线协议，不得填写当前SDK运行时Index。
struct RB_CustomTelemetryRawValue
{
	unsigned int FieldId;                        // RaceBear稳定线协议字段号。
	double Value;                                // 按遥测目录Unit标准化后的数值。
};

// 直接6DOF包体。所有分量范围为-1到1，由SDK按当前平台各DOF限制换算。
struct RB_CustomTelemetryDofPayload
{
	double Sway;                                 // 横移归一化值。
	double Surge;                                // 纵移归一化值。
	double Heave;                                // 升降归一化值。
	double Yaw;                                  // 偏航归一化值。
	double Roll;                                 // 横滚归一化值。
	double Pitch;                                // 俯仰归一化值。
};

// MMF映射起始头。游戏写入时先把WriteSequence设为奇数，写完Packet后再设为下一个偶数。
struct RB_CustomGameMmfBlockHeader
{
	volatile unsigned long long WriteSequence;   // MMF并发序号；奇数表示正在写，偶数表示完整帧。
	unsigned int PacketCapacity;                 // Packet区域容量，不得超过RB_CUSTOM_PACKET_MAX_SIZE。
	unsigned int PacketSize;                     // 当前有效包字节数。
};
#pragma pack(pop)

enum RB_CustomTelemetryProtocol
{
	RB_CUSTOM_PACKET_MAGIC = 0x4D544252,         // 小端字节序文本RBTM。
	RB_CUSTOM_PROTOCOL_VERSION = 1               // 当前自定义游戏包协议版本。
};

// 自定义游戏接收状态，用于前端显示监听、包频率和错误原因。
struct RB_CustomGameState
{
	int Size;                                    // 结构体字节数，由SDK填写。
	int Version;                                 // 当前为1。
	int Registered;                              // 游戏是否已经注册。
	int Active;                                  // 当前活动Source是否属于该游戏。
	int Connected;                               // MMF/UDP接收端是否已经启动。
	int Ready;                                   // 是否已经完成淡入。
	unsigned long long LastSequence;             // 最近接受的游戏包ID。
	unsigned long long AcceptedPacketCount;      // 有效包累计数量。
	unsigned long long DuplicatePacketCount;     // 重复或倒序包累计数量。
	double InputFrequencyHz;                     // 最近接收频率。
	double LastPacketAgeMs;                      // 最近有效包距当前时间。
	char StatusText[RB_TEXT_MEDIUM];             // 当前接收状态。
	char LastError[RB_TEXT_LARGE];               // 最近协议、MMF或UDP错误。
};

// 外部游戏每帧提交的单个遥测值。Index必须由RB_Telemetry_FindIndexByKey在当前SDK中解析得到。
struct RB_ExternalTelemetryValue
{
	int Index;                                   // 当前SDK运行时遥测索引，不得持久化。
	double Value;                                // 已按遥测目录Unit转换后的有限数值。
};

// 外部游戏源描述。打开后会替换当前活动游戏源。
struct RB_ExternalSourceDesc
{
	int Size;                                    // 调用方填写sizeof(RB_ExternalSourceDesc)。
	int Version;                                 // 当前填写1。
	char SourceKey[RB_TEXT_SMALL];               // 调用方自定义稳定key，例如vendor.game_name。
	char SourceName[RB_TEXT_MEDIUM];             // 前端显示名称。
	int DataTimeoutMs;                           // 停止收帧后开始淡出的超时，建议500 ms。
	int TransitionMs;                            // 淡入淡出时间，建议2000 ms。
};

// 外部游戏完整遥测帧。每次提交都替换上一帧，未提交槽位会清零。
struct RB_ExternalTelemetryFrame
{
	int Size;                                    // 调用方填写sizeof(RB_ExternalTelemetryFrame)。
	int Version;                                 // 当前填写1。
	unsigned long long Sequence;                 // 持续递增的数据包序号；0表示由SDK自动生成。
	unsigned long long TimestampUs;              // 调用方采集时间，微秒；当前仅保留诊断用途。
	int ValueCount;                              // Values中的有效项数量，范围0-RB_MAX_TELEMETRY_VALUES。
	RB_ExternalTelemetryValue Values[RB_MAX_TELEMETRY_VALUES]; // 本帧完整遥测值数组。
};

// 外部游戏源运行状态。
struct RB_ExternalSourceState
{
	int Size;                                    // 结构体字节数，由SDK填写。
	int Version;                                 // 当前为1。
	int Open;                                    // 是否存在活动外部源。
	int Connected;                               // 外部源是否已连接。
	int Ready;                                   // 是否完成淡入并可稳定输出。
	unsigned long long LastSequence;             // 最近接收序号。
	unsigned long long SubmittedFrameCount;      // 累计接收帧数。
};

// 遥测值目录列表。调用 RB_Telemetry_GetCatalog 填充。
struct RB_TelemetryCatalog
{
	int Size;                                    // 结构体字节数，由 SDK 填写。
	int Version;                                 // 结构体版本，当前为3（包含Unit和FieldId）。
	int Count;                                   // Items 中有效项数量。
	RB_TelemetryCatalogItem Items[RB_MAX_TELEMETRY_VALUES]; // 仅包含公共遥测值；内部兼容槽位不会进入目录。
};

// 输出实例目录项。
struct RB_OutputInstanceItem
{
	int Index;                                   // 输出实例索引。
	char Key[RB_TEXT_SMALL];                     // 输出实例 key。
	int Connected;                               // 是否已连接。
	int RuntimeActive;                           // 运行态是否激活。
};

// 可输出 key 目录项。
struct RB_OutputKeyItem
{
	int Index;                                   // key 索引。
	char Key[RB_TEXT_SMALL];                     // 可填入输出命令的 key。
};

// 输出目录，包含输出实例列表和可输出 key 列表。
struct RB_OutputCatalog
{
	int Size;                                    // 结构体字节数，由 SDK 填写。
	int Version;                                 // 结构体版本，当前为 1。
	int InstanceCount;                           // Instances 中有效项数量。
	RB_OutputInstanceItem Instances[RB_MAX_OUTPUT_INSTANCES]; // 输出实例数组。
	int KeyCount;                                // Keys 中有效项数量。
	RB_OutputKeyItem Keys[RB_MAX_OUTPUT_KEYS];   // 可输出 key 数组。
};

// 运动增强或震动效果目录项。
struct RB_EffectCatalogItem
{
	int Index;                                   // 效果索引。
	char Name[RB_TEXT_MEDIUM];                   // 效果显示名称。
	char Description[RB_TEXT_LARGE];             // 效果说明。
	char OutputText[RB_TEXT_MEDIUM];             // 输出说明。
};

// 效果目录。运动增强和震动共用此结构体。
struct RB_EffectCatalog
{
	int Size;                                    // 结构体字节数，由 SDK 填写。
	int Version;                                 // 结构体版本，当前为 1。
	int Count;                                   // Items 中有效项数量。
	RB_EffectCatalogItem Items[RB_MAX_EFFECTS];  // 效果项数组。
};

// 功能插件目录项。索引只在最近一次刷新后的当前目录中有效。
struct RB_PluginInfo
{
	int Index;                                      // 插件零基目录索引。
	char Name[RB_TEXT_MEDIUM];                      // UTF-8 插件显示名称。
	char Version[RB_TEXT_SMALL];                    // UTF-8 插件版本。
	char StatusText[RB_TEXT_MEDIUM];                // UTF-8 当前状态摘要。
	char IconPath[RB_TEXT_LARGE];                   // UTF-8 图标文件路径，可为空。
	int Launchable;                                 // 1=插件提供可启动的前端程序，0=仅后台运行。
};

struct RB_PluginCatalog
{
	int Size;                                       // 结构体字节数，由 SDK 写入。
	int Version;                                    // 结构体版本，当前为 1。
	int Count;                                      // Items 中有效项数量。
	RB_PluginInfo Items[RB_MAX_PLUGINS];            // 插件目录项。
};

// 串口信息。用于前端枚举系统当前可用串口，不负责打开和读写串口。
struct RB_SerialPortInfo
{
	char PortName[256];                           // 串口名称，例如 COM3。
	char FriendlyName[256];                       // Windows 设备友好名称。
	char Description[256];                        // 设备描述。
	char Manufacturer[256];                       // 厂商名称。
	char Model[256];                              // 设备型号。
	char BusReportedDeviceDesc[256];              // USB 总线上报名称。
	char HardwareId[512];                         // 硬件 ID。
};

// 串口句柄。由 RB_Serial_Create 创建，由 RB_Serial_Destroy 释放。
typedef void* RB_SerialHandle;

// 串口读取事件回调。回调中只通知可读长度，真实数据仍需调用 RB_Serial_Read* 读取。
typedef void (*RB_SerialReadCallback)(RB_SerialHandle handle, const char* portName, unsigned int readBufferLen, void* userData);

// 串口热插拔事件回调。portInfo 只在回调执行期间有效，调用方需要长期保存时应自行复制。
typedef void (*RB_SerialHotPlugCallback)(const char* portName, int isAdd, const RB_SerialPortInfo* portInfo, void* userData);

// 串口操作模式。
enum RB_SerialOperateMode
{
	RB_SERIAL_SYNC = 0,                            // 同步模式。
	RB_SERIAL_ASYNC = 1	                           // 异步模式，后台线程读取到内部队列。
};

// 串口校验位。
enum RB_SerialParity
{
	RB_SERIAL_PARITY_NONE = 0,                     // 无校验。
	RB_SERIAL_PARITY_ODD = 1,                      // 奇校验。
	RB_SERIAL_PARITY_EVEN = 2,                     // 偶校验。
	RB_SERIAL_PARITY_MARK = 3,                     // Mark 校验。
	RB_SERIAL_PARITY_SPACE = 4                     // Space 校验。
};

// 串口停止位。
enum RB_SerialStopBits
{
	RB_SERIAL_STOP_ONE = 0,                        // 1 位停止位。
	RB_SERIAL_STOP_ONE_AND_HALF = 1,               // 1.5 位停止位。
	RB_SERIAL_STOP_TWO = 2                         // 2 位停止位。
};

// 串口流控制。
enum RB_SerialFlowControl
{
	RB_SERIAL_FLOW_NONE = 0,                       // 无流控制。
	RB_SERIAL_FLOW_HARDWARE = 1,                   // 硬件流控制。
	RB_SERIAL_FLOW_SOFTWARE = 2                    // 软件流控制。
};

// 串口超时配置，对应 Windows COMMTIMEOUTS。
struct RB_SerialTimeoutConfig
{
	unsigned int ReadIntervalTimeout;              // 读间隔超时，毫秒。
	unsigned int ReadTotalTimeoutMultiplier;       // 读总超时乘数。
	unsigned int ReadTotalTimeoutConstant;         // 读总超时常量，毫秒。
	unsigned int WriteTotalTimeoutMultiplier;      // 写总超时乘数。
	unsigned int WriteTotalTimeoutConstant;        // 写总超时常量，毫秒。
};

// VOCORE 输入画面的像素排列。设备端统一使用 RGB565，SDK 会在发送线程中完成必要转换。
enum RB_VoCorePixelFormat
{
	RB_VOCORE_PIXEL_RGB565_LE = 1,                 // 每像素 2 字节，小端 RGB565。
	RB_VOCORE_PIXEL_RGB888 = 2,                    // 每像素 3 字节，依次为 R、G、B。
	RB_VOCORE_PIXEL_BGRA8888 = 3,                  // 每像素 4 字节，依次为 B、G、R、A；A 被忽略。
	RB_VOCORE_PIXEL_RGBA8888 = 4                   // 每像素 4 字节，依次为 R、G、B、A；A 被忽略。
};

// SDK内置仪表模板。模板同时包含竖屏和横屏布局。
enum RB_DashboardTemplate
{
	RB_DASHBOARD_TEMPLATE_GT = 0,                  // AIM MXS风格布局。
	RB_DASHBOARD_TEMPLATE_VIRTUAL_RING = 1,        // BOSCH DDU8风格布局。
	RB_DASHBOARD_TEMPLATE_M_STRIP = 2,             // SimpleDash风格布局。
	RB_DASHBOARD_TEMPLATE_FORD_GT = 3              // Ford GT风格布局。
};

// VOCORE 屏幕目录项。DeviceId 用于打开设备，在一次运行期间保持稳定。
struct RB_VoCoreDeviceInfo
{
	int Size;                                      // 调用前设为 sizeof(RB_VoCoreDeviceInfo)。
	int Version;                                   // 结构体版本，当前为 2。
	char DeviceId[RB_TEXT_SMALL];                  // 设备唯一 ID；固件无法提供 ID 时退化为 USB 端口路径。
	char Model[RB_TEXT_MEDIUM];                    // 型号，例如 "VoCore 6.8-inch"。
	char UsbPath[RB_TEXT_MEDIUM];                  // USB 总线和端口路径，用于诊断。
	unsigned int ScreenCode;                       // VOCORE 固件上报的屏幕型号代码。
	unsigned int FirmwareVersion;                  // VOCORE 固件版本代码。
	int Width;                                     // 屏幕原生宽度，单位为像素。
	int Height;                                    // 屏幕原生高度，单位为像素。
	int Supports24Bit;                             // 1=固件支持 24 bit 输入，0=仅确认支持 16 bit。
};

// 已打开 VOCORE 屏幕的运行状态和发送统计。
struct RB_VoCoreState
{
	int Size;                                      // 调用前设为 sizeof(RB_VoCoreState)。
	int Version;                                   // 结构体版本，当前为 1。
	int DevicePresent;                             // 1=设备仍在 USB 总线上，0=已拔出或尚未找到。
	int Open;                                      // 1=USB 接口已打开，0=未连接。
	int Reconnecting;                              // 1=自动重连线程正在等待设备恢复。
	int Width;                                     // 当前设备原生宽度，单位为像素。
	int Height;                                    // 当前设备原生高度，单位为像素。
	int Brightness;                                // 当前目标亮度，范围 0-255。
	int PendingFrame;                              // 1=存在尚未发送的最新画面。
	int LastError;                                 // 最近一次 libusb 错误码；0 表示无错误。
	double ActualFps;                              // 最近统计窗口内实际发送到设备的 FPS。
	unsigned long long SubmittedFrames;            // 前端累计提交帧数。
	unsigned long long SentFrames;                 // 成功发送帧数。
	unsigned long long DroppedFrames;              // 被更新画面覆盖或发送失败后丢弃的帧数。
	char LastErrorText[RB_TEXT_LARGE];             // UTF-8 错误说明。
};

// VOCORE 屏幕句柄。由 RB_VoCore_Open 创建，由 RB_VoCore_Close 释放。
typedef void* RB_VoCoreHandle;

// 浏览器仪表输出配置。服务端异步编码最新帧，并通过局域网 MJPEG 页面输出。
struct RB_DashboardWebConfig
{
	int Size;                                      // 调用前设为 sizeof(RB_DashboardWebConfig)。
	int Version;                                   // 结构体版本，当前为 1。
	int Port;                                      // HTTP 监听端口，建议使用 24738。
	int AllowRemote;                               // 1=局域网设备可访问，0=仅本机浏览器。
	int JpegQuality;                               // JPEG 质量，范围 30-100。
	int MaxFps;                                    // 最大编码帧率，范围 1-60。
};

struct RB_DashboardWebState
{
	int Size;                                      // 调用前设为 sizeof(RB_DashboardWebState)。
	int Version;                                   // 结构体版本，当前为 1。
	int Open;                                      // 1=HTTP 服务正在监听。
	int Port;                                      // 实际监听端口。
	int ClientCount;                               // 当前正在观看视频流的浏览器数量。
	int PendingFrame;                              // 1=存在等待编码的最新帧。
	double ActualFps;                              // 最近统计窗口内实际 JPEG 编码帧率。
	unsigned long long SubmittedFrames;            // 调用方累计提交帧数。
	unsigned long long EncodedFrames;              // 成功编码帧数。
	unsigned long long DroppedFrames;              // 编码前被更新帧覆盖的数量。
	char Url[RB_TEXT_LARGE];                       // 本机或局域网可访问 URL。
	char LastErrorText[RB_TEXT_LARGE];             // UTF-8 错误说明。
};

typedef void* RB_DashboardWebHandle;

// 客户自定义仪表目录项。模板 ID 是配置持久化和打开模板的稳定标识。
struct RB_DashboardPackageInfo
{
	unsigned int Size;                              // 调用前设为 sizeof(RB_DashboardPackageInfo)。
	int Version;                                    // 结构体版本，当前为 1。
	char PackageId[RB_TEXT_SMALL];                  // 稳定模板 ID。
	char Name[RB_TEXT_MEDIUM];                      // 显示名称。
	char Author[RB_TEXT_MEDIUM];                    // 作者。
	char Description[RB_TEXT_LARGE];                // 模板说明或加载错误。
	int FormatVersion;                              // .rbdash 格式版本，当前支持 1。
	int DesignWidth;                                // 设计画布宽度。
	int DesignHeight;                               // 设计画布高度。
	int PageCount;                                  // 页面数量。
	int ElementCount;                               // 全部页面元素数量。
	int BuiltIn;                                    // 1=SDK 随附模板，0=用户模板。
	int Valid;                                      // 1=可加载，0=模板损坏或不兼容。
	char FilePath[RB_TEXT_LARGE];                   // UTF-8 文件路径，仅用于诊断和编辑器定位。
};

// 已打开客户仪表模板的运行状态。
struct RB_DashboardPackageState
{
	unsigned int Size;                              // 调用前设为 sizeof(RB_DashboardPackageState)。
	int Version;                                    // 结构体版本，当前为 1。
	int Open;                                       // 1=模板句柄有效。
	int CurrentPage;                                // 当前页面索引。
	int PageCount;                                  // 页面数量。
	int ElementCount;                               // 当前页面元素数量。
	double LastRenderTimeMs;                        // 最近一帧渲染耗时。
	unsigned long long RenderedFrames;              // 累计成功渲染帧数。
	char LastErrorText[RB_TEXT_LARGE];              // 最近加载或渲染错误。
};

typedef void* RB_DashboardPackageHandle;

// 游戏安装信息。用于首页启动/停止游戏和安装路径显示。
struct RB_GameInstallInfo
{
	char GameKey[RB_TEXT_SMALL];                   // 稳定游戏 key。
	int SteamAppID;                                // Steam AppID。
	char GameName[RB_TEXT_MEDIUM];                 // 游戏名称。
	char ProcessName[RB_TEXT_MEDIUM];              // 匹配到的进程名。
	int ProcessId;                                 // 进程 ID。
	char InstallPath[RB_TEXT_LARGE];               // 安装目录。
	char ExePath[RB_TEXT_LARGE];                   // 可执行文件路径。
	char Source[RB_TEXT_MEDIUM];                   // 发现来源。
	int Found;                                     // 是否找到安装目录。
	int Running;                                   // 是否正在运行。
	int Ambiguous;                                 // 是否有歧义。
};

// 游戏遥测配置。字符串使用固定缓冲，避免 std::string 跨 DLL 边界。
struct RB_GameConfig
{
	int Size;                                      // 结构体字节数，由 SDK 写入。
	int Version;                                   // 结构体版本，当前为 1。
	int SourcePort;                                // 游戏遥测监听端口。
	char SourceIP[RB_TEXT_MEDIUM];                 // 游戏遥测监听或绑定 IP。
	char RemotehostIP[RB_TEXT_MEDIUM];             // 远程主机 IP，部分网络源使用。
	int ForwardingPort;                            // 遥测转发目标端口。
	char ForwardingIP[RB_TEXT_MEDIUM];             // 遥测转发目标 IP。
	int Forwarding;                                // 是否启用遥测转发，1=启用。
	int DirectlyConnected;                         // 是否直连硬件/游戏，1=直连。
	char SerialPort[RB_TEXT_MEDIUM];               // 硬件遥测源串口，例如 COM3。
	int SerialBaudRate;                            // 硬件遥测源串口波特率。
	char Configuration[RB_TEXT_MEDIUM];            // 游戏特定配置名称。
	char GamePath[RB_TEXT_LARGE];                  // 游戏安装目录或可执行文件路径。
};

// 平台配置。L1-L10 保留为独立字段，方便炫彩前端按现有 UI 绑定。
struct RB_PlatformConfig
{
	int Size;                                      // 结构体字节数，由 SDK 写入。
	int Version;                                   // 结构体版本，当前为 1。
	int Type;                                      // 平台类型编号。
	int Index;                                     // 平台配置索引。
	char ConfigName[RB_TEXT_MEDIUM];               // 配置节点名称。
	char Name[RB_TEXT_MEDIUM];                     // 平台显示名称。
	int L1;                                        // 平台几何/行程参数 1。
	int L2;                                        // 平台几何/行程参数 2。
	int L3;                                        // 平台几何/行程参数 3。
	int L4;                                        // 平台几何/行程参数 4。
	int L5;                                        // 平台几何/行程参数 5。
	int L6;                                        // 平台几何/行程参数 6。
	int L7;                                        // 平台几何/行程参数 7。
	int L8;                                        // 平台几何/行程参数 8。
	int L9;                                        // 平台几何/行程参数 9。
	int L10;                                       // 平台几何/行程参数 10。
	int OutBit;                                    // 旧版兼容输出位宽，新输出通常由 OutputConfig 控制。
	int DefaultPos;                                // 执行器默认位置。
	int OutGain;                                   // 平台总输出比例，百分比。
	int OutSmoothing;                              // 平台最后一级输出平滑，百分比。
	int Lateral;                                   // 横向姿态突变过滤强度，0-1000。
	int Longitudinal;                              // 纵向姿态突变过滤强度，0-1000。
	int Vertical;                                  // 垂向姿态突变过滤强度，0-1000。
	int DecelerationSpeed;                         // 平台回中/阻尼速度参数。
	int Percent;                                   // 姿态裁剪或缓冲比例。
	char OutKey[RB_TEXT_MEDIUM];                   // 平台输出 key，例如 A1-A4。
};

// Motion 碰撞/异常保护配置。独立于 RB_PlatformConfig，避免改变既有 ABI 布局。
struct RB_MotionProtectionConfig
{
	int Size;                                      // 结构体字节数，由 SDK 写入。
	int Version;                                   // 结构体版本，当前为 1。
	int Enabled;                                   // 总开关，1=启用。
	int CrashDetectionEnabled;                     // 高加速度碰撞检测开关，1=启用。
	int AbnormalSurgeEnabled;                     // 异常 Surge 触发检测开关，1=启用，不跳帧。
	int DetectionMode;                             // 检测模式：0=Absolute，1=Delta。
	int Preset;                                    // 强度预设：-2 最弱，2 最强，0 均衡。
	double AccelerationThreshold;                  // 碰撞加速度阈值，m/s²。
	double AbnormalSurgeThreshold;                // 异常 Surge 阈值，m/s²。
	double HoldSeconds;                            // 触发后最短保护时间，秒。
	double RecoverySeconds;                       // 保护结束后的恢复时间，秒。
};

// V2 使用独立结构与函数，V1 的大小和字段偏移保持不变；旧接口更新时保留新增字段。
struct RB_MotionProtectionConfigV2
{
	int Size;                                     // sizeof(RB_MotionProtectionConfigV2)。
	int Version;                                  // 固定为 2。
	int Enabled;                                  // 1=启用；默认关闭，升级需主动开启。
	int CrashDetectionEnabled;                    // 三轴无重力加速度检测。
	int AbnormalSurgeEnabled;                     // 极端纵向加速度检测，不跳帧。
	int DetectionMode;                            // 0=各轴绝对峰值，1=三轴差向量长度。
	int Preset;                                   // -2 最弱，2 最强。
	double AccelerationThreshold;                 // m/s²。
	double AbnormalSurgeThreshold;                // m/s²。
	double HoldSeconds;                           // 最短保护时长，默认 0.2s，期间继续受限追随。
	double RecoverySeconds;                      // 接近实时目标后的退出时长，默认 0.5s。
	int AngleDetectionEnabled;                    // 1=检测相邻 Pitch/Roll 角度变化。
	int IgnorePositiveHeave;                      // 1=仅检测时忽略正向 Heave，不改变动态输入。
	double AngleThresholdDegrees;                // |deltaPitch|+|deltaRoll|，默认 10°。
};

// UI 无关的平台预览图元。SDK 计算真实 Rig 几何，调用方把回调映射到自己的绘图库。
struct RB_DrawPointF
{
	float X;
	float Y;
};

struct RB_DrawRectF
{
	float Left;
	float Top;
	float Right;
	float Bottom;
};

enum RB_PlatformPreviewStyle
{
	RB_PLATFORM_PREVIEW_DEBUG = 0,                 // 参数窗口完整调试视图。
	RB_PLATFORM_PREVIEW_COMPACT = 1                // 主页面小预览视图。
};

enum RB_DrawTextAlign
{
	RB_DRAW_TEXT_LEFT = 0,
	RB_DRAW_TEXT_CENTER = 1,
	RB_DRAW_TEXT_RIGHT = 2,
	RB_DRAW_TEXT_VCENTER = 4,
	RB_DRAW_TEXT_BOTTOM = 8
};

enum RB_DrawGradientMode
{
	RB_DRAW_GRADIENT_HORIZONTAL = 0,
	RB_DRAW_GRADIENT_VERTICAL = 1
};

struct RB_DrawCallbacks
{
	int Size;                                      // 必须设为 sizeof(RB_DrawCallbacks)。
	int Version;                                   // 当前固定为 1。
	void* UserData;                                // SDK 原样回传给每个回调。
	void (*SetBrushColor)(void* userData, unsigned int color);
	void (*SetLineWidth)(void* userData, float width);
	void (*SetFontSize)(void* userData, int size);
	void (*SetTextAlign)(void* userData, int align);
	void (*EnableSmoothing)(void* userData, int enabled);
	void (*Clear)(void* userData, unsigned int color);
	void (*GradientFill)(void* userData, const RB_DrawRectF* rect, unsigned int color1, unsigned int color2, int mode);
	void (*DrawLine)(void* userData, float x1, float y1, float x2, float y2);
	void (*FillEllipse)(void* userData, const RB_DrawRectF* rect);
	void (*FillPolygon)(void* userData, const RB_DrawPointF* points, int count);
	void (*DrawPolygon)(void* userData, const RB_DrawPointF* points, int count);
	void (*DrawString)(void* userData, const wchar_t* text, int count, const RB_DrawRectF* rect);
};

// 输出实例配置。
struct RB_OutputConfig
{
	int Size;                                      // 结构体字节数，由 SDK 写入。
	int Version;                                   // 结构体版本，当前为 1。
	int CANindex;                                  // RB_CAN 卡内逻辑索引，范围1-255；-1或0表示尚未绑定。
	int ConnectionSpeed;                           // CAN 连接/软启动速度参数；非 CAN 输出保留读取值。
	int ParkPositionPercent;                       // CAN 停车位置，0-100%。
	int AutoConnect;                               // 1=允许自动连接，0=仅手动连接。
	int OutBit;                                    // 普通 Serial/UDP 输出位宽，常用 8、16 或 32 bit。
	char OutputKind[RB_TEXT_SMALL];                // 输出类型；有效值必须来自 RB_Output_GetKind()。
	int Type;                                      // 旧版兼容类型；新前端按 OutputKind 判断，读取后原样保留。
	int BaudRate;                                  // Serial 波特率，例如 115200；其他输出保留读取值。
	char Port[RB_TEXT_MEDIUM];                     // Serial 端口名，例如 "COM3"。
	char IpAddrs[RB_TEXT_MEDIUM];                  // UDP 远程 IPv4 地址，例如 "127.0.0.1"。
	int UdpPort;                                   // UDP 远程端口，1-65535。
	int UdpLPort;                                  // UDP 本地绑定端口，1-65535。
	char MMFName[RB_TEXT_MEDIUM];                  // MMF 映射名称；仅 MMF 输出使用。
	char StartString[RB_TEXT_LARGE];               // 连接后发送一次的命令模板；空字符串表示不发送。
	char StartTime[RB_TEXT_SMALL];                 // StartString 延迟，十进制毫秒文本，例如 "0"。
	char OutPutString[RB_TEXT_LARGE];              // 周期命令模板，例如 "<A1><A2><A3><A4>"。
	char OutPutTime[RB_TEXT_SMALL];                // 周期输出间隔，十进制毫秒文本，例如 "2"。
	char EndString[RB_TEXT_LARGE];                 // 断开前发送一次的命令模板；空字符串表示不发送。
	char EndTime[RB_TEXT_SMALL];                   // EndString 延迟，十进制毫秒文本，例如 "0"。
};

enum RB_SeatbeltCanChannelMode
{
	RB_SEATBELT_CAN_SINGLE = 1,
	RB_SEATBELT_CAN_DUAL = 2
};

enum RB_SeatbeltCanSingleChannel
{
	RB_SEATBELT_CAN_SINGLE_LEFT = 0,
	RB_SEATBELT_CAN_SINGLE_RIGHT = 1,
	RB_SEATBELT_CAN_SINGLE_CENTER = 2
};

// RB 57 安全带 CAN 输出配置。驱动器参数由 RB 专用上位机设置，SDK 只管理输出绑定和安全边界。
struct RB_SeatbeltCanConfig
{
	int Size;
	int Version;
	int ChannelMode;                              // RB_SEATBELT_CAN_SINGLE / RB_SEATBELT_CAN_DUAL。
	int SingleChannel;                            // 单路信号：0=左，1=右，2=中间公共信号；单路中间通过左节点发送。
	int LeftNodeId;
	int RightNodeId;
	char LeftOutputKey[RB_TEXT_SMALL];
	char RightOutputKey[RB_TEXT_SMALL];
	int LeftReverse;
	int RightReverse;
	double LeftReductionRatio;
	double RightReductionRatio;
	int LeftPositionMin;
	int LeftPositionMax;
	int RightPositionMin;
	int RightPositionMax;
	int HomingOnConnect;
	int HomingMethod;
	int HomingTimeoutMs;
};

struct RB_SeatbeltCanState
{
	int Size;
	int Version;
	int Connected;
	int Ready;
	int Homing;
	int Homed;
	int Fault;
	int LastError;
	int ChannelMode;
	int LeftNodeId;
	int RightNodeId;
	int32_t LeftTargetPosition;
	int32_t RightTargetPosition;
	char StatusText[RB_TEXT_MEDIUM];
};

// 动态滤波单个输入的滤波参数。
struct RB_FilterEffect
{
	int Enabled;                                   // 1=启用该通道滤波链，0=该通道不参与输出。
	int Dof;                                       // 输入模板类型镜像；保存时SDK按InputType规范化，前端无需修改。
	int InMapping;                                 // 范围 0-int最大值；单位跟随输入遥测，启用通道时必须大于0。
	double InGain;                                 // 任意有限小数；1.0原方向，-1.0反向，0关闭有效输出。
	int OutScaling;                                // 范围0-100；最大输出占当前DOF PoseLimit的百分比，0关闭输出。
	int Proportion;                                // 范围0-100；Logistic有效范围占PoseLimit的百分比，0关闭Logistic。
	int Strength;                                  // 范围0-100；Logistic锐度，运行时按value*0.1使用，0关闭Logistic。
	int Smoothing;                                 // 范围0-100；低通平滑强度，0关闭，数值越大响应越慢。
	int DeadZone;                                  // 范围0-100；输出死区占最大输出范围的百分比，0关闭。
	int Washout;                                   // 范围0-100；加速度模板的高通洗出强度，0关闭。
	int Sensitivity;                               // 范围0-100；单路最终变化步长比例，按固定2ms基准换算速度，0关闭。
	int SensitivityStrength;                       // 范围0-100；单路限速强度，非零越大越慢，0关闭，仅Sensitivity>0时有效。
};

struct RB_DynamicInputEffect
{
	int Enabled;                                   // 1=启用该输入通道，0=保留配置但不参与计算。
	int InputType;                                 // SDK 内部输入模板类型；从现有配置读取并保留。
	int TelemetryIndex;                            // 当前运行时遥测索引；使用 RB_Telemetry_FindIndexByKey() 解析。
	char Key[RB_TEXT_SMALL];                       // 稳定通道效果 key，例如 Sway、YawRate、RoadImpact。
	RB_FilterEffect Filter;                        // 该输入对应滤波参数。
};

// 单个目标 DOF 可添加的输入通道效果。候选由 SDK 维护，前端使用此目录填充选择项。
struct RB_DynamicInputCandidateItem
{
	char Key[RB_TEXT_SMALL];                       // 通道效果 key，同时用于用户界面显示。
	int InputType;                                 // 对应的 SDK 输入模板类型。
	int TelemetryIndex;                            // 当前运行时遥测索引。
	char TelemetryKey[RB_TEXT_SMALL];              // 稳定遥测 key，用于诊断和跨运行解析。
};

struct RB_DynamicInputCandidateCatalog
{
	int Size;                                      // 结构体字节数，由 SDK 写入。
	int Version;                                   // 当前为 1。
	int DofIndex;                                  // 本目录对应的目标 DOF，范围 0-7。
	int Count;                                     // Items 中有效候选数量。
	RB_DynamicInputCandidateItem Items[RB_DYNAMIC_MAX_CANDIDATES];
};

struct RB_DynamicDofEffect
{
	int Enabled;                                   // 1=启用整个 DOF，0=禁用该 DOF 的全部输入。
	int InputCount;                                // Inputs 中有效输入数，范围 0-RB_DYNAMIC_MAX_INPUTS。
	RB_DynamicInputEffect Inputs[RB_DYNAMIC_MAX_INPUTS]; // 输入数组。
};

struct RB_DynamicV2Profile
{
	int Size;                                      // 结构体字节数，由 SDK 写入。
	int Version;                                   // 结构体版本，当前为 1。
	RB_DynamicDofEffect Dofs[RB_DYNAMIC_DOF_COUNT]; // 动态 6DOF/附加槽位配置。
};

struct RB_MotionEffectOutputRoute
{
	int Enabled;                                   // 1=启用该输出路由。
	int Dof;                                       // 目标 DOF：0=Sway、1=Surge、2=Heave、3=Yaw、4=Roll、5=Pitch。
	double OutputRatio;                            // 路由输出比例，1.0=100%，0.5=50%。
	double Direction;                              // 输出方向，通常 1.0 或 -1.0。
	double Threshold;                              // 当前路由触发阈值；具体单位由效果目录和输入遥测决定。
	double Frequency;                              // 当前路由脉冲频率，单位 Hz；非脉冲效果保留默认值。
	double Duration;                               // 当前路由单次持续时间，单位秒。
};

struct RB_MotionEffectConfig
{
	int Enabled;                                   // 是否启用该运动增强效果。
	int EffectType;                                // 效果类型索引。
	int InputIndex;                                // 主输入遥测索引。
	int SecondaryInputIndex;                       // 辅助输入遥测索引。
	double Gain;                                   // 效果自身增益。
	int RouteCount;                                // Routes 中有效路由数量。
	RB_MotionEffectOutputRoute Routes[RB_MOTION_ROUTE_COUNT]; // 输出路由数组。
	double Threshold;                              // 旧版/全局触发阈值，保留兼容。
	double Limit;                                  // 旧版输出限幅，保留兼容。
	double Frequency;                              // 旧版频率参数，保留兼容。
	double Decay;                                  // 脉冲衰减系数。
	int OutputDof;                                 // 旧版主输出 DOF，读取旧配置时使用。
	int SecondaryOutputDof;                        // 旧版辅助输出 DOF。
	double SecondaryGain;                          // 旧版辅助输出增益。
	double Direction;                              // 旧版方向参数。
};

struct RB_MotionEffectProfile
{
	int Size;                                      // 结构体字节数，由 SDK 写入。
	int Version;                                   // 结构体版本，当前为 1。
	RB_MotionEffectConfig Effects[RB_MOTION_EFFECT_COUNT]; // 运动增强效果数组。
};

struct RB_HapticEffectConfig
{
	int Enabled;                                   // 是否启用该震动效果。
	int EffectType;                                // 震动效果类型索引。
	int InputIndex;                                // 主输入遥测索引。
	int SecondaryInputIndex;                       // 辅助输入遥测索引。
	int OutputChannel;                             // 输出通道索引，0-6 对应 Haptic1-Haptic7。
	double Gain;                                   // 输出增益；0 表示无输出。
	double Threshold;                              // 触发阈值；具体单位由效果目录和输入遥测决定。
	double MinOutput;                              // 最小位输出，通常 0。
	double MaxOutput;                              // 最大位输出，默认 65535。
	double Frequency;                              // 频率参数，单位 Hz。
	double Direction;                              // 输出方向。
};

struct RB_HapticEffectProfile
{
	int Size;                                      // 结构体字节数，由 SDK 写入。
	int Version;                                   // 结构体版本，当前为 1。
	RB_HapticEffectConfig Effects[RB_HAPTIC_EFFECT_COUNT]; // 震动效果数组。
};

struct RB_WindEffectConfig
{
	int Size;                                      // 结构体字节数，由 SDK 写入。
	int Version;                                   // 结构体版本，当前为 1。
	int Enabled;                                   // 是否启用风感模拟。
	double InputMinKmh;                            // 起风速度，单位 km/h。
	double InputMaxKmh;                            // 满风速度，单位 km/h。
	double OutputMinWorkPercent;                   // 风扇开始有效工作的最低输出，0-100%。
	double Gamma;                                  // 曲线指数，1.0=线性；大于 1 降低低速输出，小于 1 提高低速输出。
	double MasterGainPercent;                      // 风感总增益，0-100%。
	double ChannelGainPercent[RB_WIND_CHANNEL_COUNT]; // Wind1-Wind4 增益补偿，-100% 到 100%，0=不补偿。
	double ChannelOffsetPercent[RB_WIND_CHANNEL_COUNT]; // Wind1-Wind4 偏移补偿，-100% 到 100%，0=不补偿。
};

struct RB_SeatbeltEffectConfig
{
	int Size;                                      // 结构体字节数，由 SDK 写入。
	int Version;                                   // 结构体版本，当前为 2。
	int Enabled;                                   // 是否启用安全带模拟。
	int InputMode;                                 // RB_SeatbeltInputMode；当前推荐 RB_SEATBELT_INPUT_TELEMETRY。
	double BasePreloadPercent;                     // 基础预紧百分比。
	double StartSpeedKmh;                          // 动态张紧起效车速，单位 km/h。
	double FullSpeedKmh;                           // 兼容字段，当前算法不使用；读取后原样保留。
	double MasterGainPercent;                      // 安全带总增益，0-200%。
	double ReleaseSpeedPercent;                    // 输出回落速度，0-100%；越大释放越快。
	int BrakeEnabled;                              // 是否启用刹车张紧。
	double BrakePedalGainPercent;                  // 刹车踏板激活门限，0-100%。
	double BrakeDecelGainPercent;                  // Surge 刹车减速度张紧增益，0-200%。
	int LateralEnabled;                            // 是否启用横向张紧。
	double LateralGainPercent;                     // Sway 横向张紧增益，0-200%。
	double LateralDeadzoneG;                       // 横向加速度死区，单位 G，建议大于等于 0。
	int CrashEnabled;                              // 兼容字段，当前算法不使用；读取后原样保留。
	double CrashThresholdPercent;                  // 兼容字段，当前算法不使用。
	double CrashHoldMs;                            // 兼容字段，单位毫秒，当前算法不使用。
	double CrashOutputPercent;                     // 兼容字段，当前算法不使用。
	int LeftEnabled;                               // 左安全带通道是否启用。
	int RightEnabled;                              // 右安全带通道是否启用。
	double LeftOutputRatioPercent;                 // 左安全带输出比例百分比。
	double RightOutputRatioPercent;                // 右安全带输出比例百分比。
	int LeftReverse;                               // 左通道是否反向。
	int RightReverse;                              // 右通道是否反向。
	int LeftSmoothingSamples;                      // 左通道 DEMALP 样本数，范围 1-1000，默认 4。
	int RightSmoothingSamples;                     // 右通道 DEMALP 样本数，范围 1-1000，默认 4。
	double SurgeInputRangeG;                       // Surge 输入满量程，单位 G，范围 0.05-10.0。
	double SwayInputRangeG;                        // Sway 输入满量程，单位 G，范围 0.05-10.0。
	char SurgeInputKey[RB_TEXT_SMALL];             // Surge 输入稳定 key，默认 rb.motion.longitudinal.acceleration。
	char SwayInputKey[RB_TEXT_SMALL];              // Sway 输入稳定 key，默认 rb.motion.lateral.acceleration。
	char BrakeInputKey[RB_TEXT_SMALL];             // 刹车门控输入稳定 key，默认 rb.vehicle.brake。
	char SpeedInputKey[RB_TEXT_SMALL];             // 低速门限输入稳定 key，默认 rb.vehicle.speed。
};

struct RB_RgbStripConfig
{
	int Enabled;
	int ColorOrder;
	int Reversed;
	int Brightness;
	int LedCount;
	int LogicalStart;
};

struct RB_RgbDeviceConfig
{
	int Size;
	int Version;
	int GlobalBrightness;
	int CurrentLimitMa;
	int PixelCurrentMa;
	int HostTimeoutMs;
	RB_RgbStripConfig Strips[RB_RGB_OUTPUT_COUNT];
};

struct RB_RgbDeviceInfo
{
	int Size;
	int Version;
	char Uid[RB_TEXT_SMALL];
	unsigned int FirmwareVersion;
	unsigned int Capabilities;
	int MaxOutputs;
	int MaxLedsPerOutput;
	int Connected;
	int IapMode;                  // 1 表示设备停在 IAP，只能用于 APP 固件更新。
};

struct RB_RgbDiagnostics
{
	int Size;
	int Version;
	unsigned int UptimeMs;
	unsigned int CommittedFrames;
	unsigned int DisplayedFrames;
	unsigned int RejectedFrames;
	unsigned int UsbRxOverflowCount;
	unsigned int DmaErrorCount;
	unsigned int TimeoutBlackoutCount;
	unsigned int DisconnectBlackoutCount;
	unsigned int EstimatedCurrentMa;
	unsigned int PowerScalePermille;
	unsigned int ActivePixels;
	int OutputBusy;
	int UsbConfigured;
	int UsbSuspended;
	int LastBlackoutReason;
};

// RGBAPP APP 固件异步更新状态。前端轮询此结构时只读取内存，不访问固件文件或 USB。
struct RB_RgbFirmwareUpdateState
{
	int Size;
	int Version;                 // 当前为 1。
	int Running;
	int Complete;
	int Success;
	int Progress;                // 0-100。
	int ErrorCode;               // 0 成功；负数为 RGB 控制器错误。
	char DeviceUid[RB_TEXT_SMALL];
};

// RGB效果语义输入项。调用 RB_Rgb_GetEffectInput() 填充。
struct RB_RgbEffectInputItem
{
	int Size;                              // 结构体字节数，由 SDK 写入。
	int Version;                           // 当前为 1。
	char Key[RB_TEXT_SMALL];               // 稳定遥测或派生效果 Key。
	char Name[RB_TEXT_MEDIUM];             // 面向开发者的 UTF-8 显示名称。
	int TelemetryIndex;                    // 当前进程运行时索引，不能持久化。
	int ValueType;                         // RB_RgbEffectInputValueType。
	int Flags;                             // RB_RgbEffectInputFlags 位组合。
	int SupportedByCurrentSource;          // 当前 Source 是否直接支持或可计算该输入。
	double DefaultMinValue;                // 选择该输入时建议使用的最小值。
	double DefaultMaxValue;                // 选择该输入时建议使用的最大值。
	double DefaultThreshold;               // 选择该输入时建议使用的触发阈值。
};

struct RB_RgbEffectLayerConfig
{
	int Enabled;
	int EffectType;
	char TelemetryKey[RB_TEXT_SMALL];
	int TelemetryIndex;
	int OutputMask;
	int StartPixel;
	int PixelCount;
	int Priority;
	unsigned int ColorA;
	unsigned int ColorB;
	double MinValue;
	double MaxValue;
	double Threshold;
	double BrightnessPercent;
	double SpeedHz;
	int Invert;
	double BlinkDutyPercent; // 闪烁周期内保持点亮的百分比，范围0-100。
	int FillMode;            // RB_RgbFillMode，仅 RPM 条使用。
	int DisplayMode;         // RB_RgbEffectDisplayMode，默认独立 RGB。
};

struct RB_RgbEffectProfile
{
	int Size;
	int Version;                             // 结构体版本，当前为 3。
	int Enabled;
	int FrameRateHz;
	double MasterBrightnessPercent;
	int LayerCount;
	RB_RgbEffectLayerConfig Layers[RB_RGB_EFFECT_LAYER_COUNT];
};

struct RB_SystemConfig
{
	int Size;                                      // 结构体字节数，由 SDK 写入。
	int Version;                                   // 结构体版本，当前为 1。
	int BootStartup;                               // 是否开机启动。
	int UnmannedOperation;                         // 是否无人值守运行。
	int SwitchToTheBackend;                        // 是否启动后切到后台。
	int EnableLogging;                             // 是否启用运行日志。
	int SilentStart;                               // 是否静默启动。
	int TelemetryShow;                             // 是否显示遥测窗口。
	int ShowAllGames;                              // 首页是否显示全部游戏。
	int VRCompensation;                            // 是否启用 VR 补偿。
	char LastGameKey[RB_TEXT_SMALL];               // 上次选择的游戏 key。
	int RigType;                                   // 平台类型。
	int NumberOfAxes;                              // 轴数量。
	int DataSource;                                // 数据来源。
	int DynamicMode;                               // 动态模式。
	int OutType;                                   // 输出类型。
	int DynamicLevel;                              // 动态等级。
	char Language[RB_TEXT_SMALL];                  // 当前语言 key。
};

// 版本和生命周期。

/**
 * @brief 获取 SDK 版本文本。
 * @return SDK 持有的 UTF-8、NUL 结尾字符串，例如 "0.6.1"；调用方不得修改或释放。
 */
RB_API const char* RB_Runtime_GetVersion();

/**
 * @brief 设置配置目录使用的应用名称，省略调用时默认为 "SimRacebear"。
 * @param name UTF-8、NUL 结尾的单个合法路径段；不能为空，也不能包含 Windows 路径非法字符。
 * @return RB_OK 表示设置成功；RB_INVALID_ARGUMENT 表示名称无效；RB_ERROR 表示内部失败。
 * @note 必须在 RB_Runtime_Initialize() 之前调用。
 */
RB_API int RB_Runtime_SetAppDataName(const char* name);

/**
 * @brief 读取当前配置目录使用的应用名称。
 * @param buffer 接收 UTF-8、NUL 结尾文本的调用方缓冲区。
 * @param bufferSize buffer 的总容量，单位为字节且必须大于 0。
 * @return RB_OK 表示成功；RB_INVALID_ARGUMENT 表示缓冲区无效；RB_BUFFER_TOO_SMALL 表示容量不足；RB_ERROR 表示内部失败。
 */
RB_API int RB_Runtime_GetAppDataName(char* buffer, int bufferSize);

/**
 * @brief 初始化授权服务、配置和全部后端运行态。
 * @return RB_OK 表示初始化完成；RB_ERROR 表示初始化失败。
 * @note 每个进程生命周期调用一次；成功后才能调用运行态、数据、命令和输出相关接口。
 */
RB_API int RB_Runtime_Initialize();

/**
 * @brief 停止内部计算循环并释放授权服务和全部后端资源。
 * @return 无返回值。
 * @note 退出前应先停止宿主自己的 SDK 调用线程，确保本函数执行期间没有并发 SDK 调用。
 */
RB_API void RB_Runtime_Shutdown();

/**
 * @brief 启动由 SDK 专用高分辨率调度线程驱动的后端计算循环；已启动时会按新周期间隔重建循环。
 * @param intervalMs 计算周期间隔，单位毫秒；有效范围 1-1000 ms，推荐初始值为 2 ms。
 * @return RB_OK 表示启动成功；RB_INVALID_ARGUMENT 表示间隔不合法；RB_ERROR 表示未初始化或调度线程启动失败。
 * @note 传入 2 表示约每 2 ms 计算一次；传入 1 表示约每 1 ms 计算一次。调度迟到时跳过旧周期，不连续补跑历史计算。
 */
RB_API int RB_Runtime_StartLoop(int intervalMs);

/**
 * @brief 暂停已启动的内部计算循环，但保留调度线程和当前频率。
 * @return RB_OK 表示已暂停；RB_ERROR 表示循环未启动或内部失败。
 */
RB_API int RB_Runtime_PauseLoop();

/**
 * @brief 恢复已暂停的内部计算循环。
 * @return RB_OK 表示已恢复；RB_ERROR 表示循环未启动或内部失败。
 */
RB_API int RB_Runtime_ResumeLoop();

/**
 * @brief 停止并释放内部计算调度线程；未启动时调用也视为成功。
 * @return RB_OK 表示停止完成；RB_ERROR 表示内部失败。
 */
RB_API int RB_Runtime_StopLoop();

/**
 * @brief 修改内部计算循环周期；循环未启动时只保存间隔，已启动时立即重建定时器。
 * @param intervalMs 计算周期间隔，单位毫秒；有效范围 1-1000 ms。
 * @return RB_OK 表示设置成功；RB_INVALID_ARGUMENT 表示间隔不合法；RB_ERROR 表示重建定时器失败。
 */
RB_API int RB_Runtime_SetLoopIntervalMs(int intervalMs);

/**
 * @brief 获取当前保存的内部计算循环周期间隔。
 * @return 成功时返回 1-1000 范围内的间隔，单位毫秒；内部失败时返回 RB_ERROR。
 */
RB_API int RB_Runtime_GetLoopIntervalMs();

/**
 * @brief 查询内部计算调度线程是否已经启动。
 * @return 1 表示已启动，0 表示未启动；内部异常也返回 0。
 */
RB_API int RB_Runtime_IsLoopRunning();

/**
 * @brief 查询已启动的内部计算循环是否处于暂停状态。
 * @return 1 表示已暂停，0 表示未暂停或循环未启动；内部异常也返回 0。
 */
RB_API int RB_Runtime_IsLoopPaused();

/**
 * @brief 手动执行一次完整后端计算，仅供不使用内部循环的高级或调试场景。
 * @return RB_OK 表示计算成功；RB_ERROR 表示内部循环正在运行、已有计算尚未结束或本次计算失败。
 * @note 常规前端应调用 RB_Runtime_StartLoop()，不要自行定时调用本函数。
 */
RB_API int RB_Runtime_DoCalculations();

/**
 * @brief 原子读取前端高频刷新所需的完整运行态快照。
 * @param state 接收状态的结构体指针；SDK 会清零并填写 Size、Version 和全部成员。
 * @return RB_OK 表示读取成功；RB_INVALID_ARGUMENT 表示 state 为空；RB_ERROR 表示内部失败。
 */
RB_API int RB_State_Read(RB_RuntimeState* state);

/**
 * @brief 重新枚举当前连接的 RaceBear CANAPP 设备并刷新设备目录。
 * @return 成功时返回当前目录数量；失败返回负数。
 * @note 本阶段第三方只接入 ProtocolType=1 的 RB_CAN 设备。刷新后旧 CatalogIndex 和 EnumerationIndex 全部失效。
 */
RB_API int RB_Can_RefreshDevices();

/**
 * @brief 返回最近一次 RB_Can_RefreshDevices() 建立的设备目录数量。
 * @return 大于等于 0 的设备数量；失败返回负数。
 * @note 本函数不重新扫描 USB。设备热插拔后应先显式刷新。
 */
RB_API int RB_Can_GetDeviceCount();

/**
 * @brief 读取最近一次设备目录中的一张 CANAPP 身份快照。
 * @param catalogIndex 最近一次刷新返回目录中的零基索引。
 * @param info 接收设备信息；调用前必须把 Size 设为 sizeof(RB_CanDeviceInfo)。
 * @return RB_OK 表示成功；RB_INVALID_ARGUMENT 表示参数或结构大小无效；RB_ERROR 表示目录项已失效或读取失败。
 * @note CatalogIndex 和 EnumerationIndex 只能用于当前目录快照，持久绑定必须保存 Uid。
 */
RB_API int RB_Can_GetDeviceInfo(int catalogIndex, RB_CanDeviceInfo* info);

/**
 * @brief 按稳定 MCU UID 设置一张 RB_CAN 设备的卡内逻辑索引。
 * @param uid RB_CanDeviceInfo::Uid 返回的 24 位大写十六进制 UID。
 * @param logicalIndex 新逻辑索引，范围 1-255，并且必须与其它在线设备唯一。
 * @return 0 表示写入并读回成功；负数表示设备离线、索引冲突、输出运行中或底层访问失败。
 * @note 这是明确的设备维护动作。请先断开相关输出，并由用户操作触发单次调用。
 */
RB_API int RB_Can_SetDeviceLogicalIndex(const char* uid, int logicalIndex);

/**
 * @brief 将一个 RB_CAN 输出实例稳定绑定到指定 UID 和逻辑索引。
 * @param outputIndex 当前输出目录中的零基索引；对应实例的 OutputKind 必须为 RB_CAN。
 * @param uid 最近一次设备目录返回的稳定 UID。
 * @param logicalIndex 要写入设备并保存到输出配置的逻辑索引，范围 1-255。
 * @return 0 表示设备索引和输出绑定均已持久化；负数表示身份、协议、冲突、安全状态或写入失败。
 * @note 绑定完成后应重新刷新设备目录并读取 RB_Can_GetOutputBindingState() 确认，不要直接修改运行时枚举索引。
 */
RB_API int RB_Can_BindOutputDevice(int outputIndex, const char* uid, int logicalIndex);

/**
 * @brief 读取一个 RB_CAN 输出与物理 CANAPP 的稳定绑定状态。
 * @param outputIndex 当前输出目录中的零基索引。
 * @param state 接收绑定状态；调用前必须把 Size 设为 sizeof(RB_CanOutputBindingState)。
 * @return RB_OK 表示成功；RB_INVALID_ARGUMENT 表示参数或结构大小无效；RB_ERROR 表示输出不存在或不是有效 CAN 输出。
 */
RB_API int RB_Can_GetOutputBindingState(int outputIndex, RB_CanOutputBindingState* state);

/** @brief 根据当前输出配置同步 CAN 控制器会话和绑定状态。 */
RB_API int RB_Can_SyncControllerFromOutputConfig();

/** @brief 读取 CAN 控制器状态文本。 */
RB_API int RB_Can_GetControllerStatusText(char* buffer, int bufferSize);

/** @brief 读取当前 CAN 控制器管理的电机数量。 */
RB_API int RB_Can_GetMotorCount();

/** @brief 读取当前 CAN 控制器管理的单个电机状态。 */
RB_API int RB_Can_GetMotorState(int motorIndex, RB_CanMotorState* state);

/** @brief 读取全局 CAN 参数目录数量。 */
RB_API int RB_Can_GetMotorParameterCount();

/** @brief 设置参数维护使用的 CAN 波特率，支持 500 或 1000 Kbit/s。 */
RB_API int RB_Can_SetMotorParameterCanBaudRate(int baudRateK);

/** @brief 进入 CAN 电机参数维护模式，停止实时初始化/监控并保留参数 SDO 通道。 */
RB_API int RB_Can_BeginMotorParameterMaintenance();

/** @brief 退出 CAN 电机参数维护模式并请求重新初始化和回零。 */
RB_API int RB_Can_EndMotorParameterMaintenance();

/** @brief 读取指定参数的显示名称。 */
RB_API int RB_Can_GetMotorParameterName(int parameterIndex, char* buffer, int bufferSize);

/** @brief 读取指定电机按协议支持的参数数量。 */
RB_API int RB_Can_GetMotorParameterCountForMotor(int motorIndex);

/** @brief 返回当前电机的参数文件类型 RB_CAN/FM_CAN/FC_CAN/RDS_CAN；只查询定义，不访问设备。 */
RB_API int RB_Can_GetMotorParameterProfileType(int motorIndex, char* buffer, int bufferSize);

/**
 * @brief 导出调用方提供的当前窗口原始参数值为 UTF-8 JSON .rbmotor 文件，不读取或写入设备。
 * @param values 按 SDK 参数索引排列，count 必须等于该电机参数数量；版本号以 uint32 原始位放入 int。
 * @param packagePath UTF-8 文件路径；验证全部数值后以临时文件替换目标。
 * @param errorParameterIndex 可空；失败的 SDK 参数索引，无具体参数时为 -1。
 * @return RB_OK 成功；RB_INVALID_ARGUMENT 表示类型/数量/数值无效；RB_ERROR 表示文件操作失败。
 */
RB_API int RB_Can_ExportMotorParameterPackage(int motorIndex, const int* values, int count, const char* packagePath, int* errorParameterIndex);

/**
 * @brief 完整校验同类型文件后返回参数草稿，不写设备；失败时两个输出数组保持不变。
 * @param values SDK 参数索引排列的草稿值；仅 selected[i]==1 的值有效。
 * @param selected 同等长度的选择数组；Node-ID、波特率、只读项始终为 0。
 * @param count 两个数组长度，必须等于该电机参数数量。
 * @param errorParameterIndex 可空；具体参数错误时返回 SDK 索引，其余为 -1。
 */
RB_API int RB_Can_ImportMotorParameterPackage(int motorIndex, const char* packagePath, int* values, int* selected, int count, int* errorParameterIndex);

/** @brief 同步执行结果；调用前设置 Size=sizeof(RB_CanMotorParameterSyncResult)。 */
struct RB_CanMotorParameterSyncResult
{
	int Size;                /**< 结构大小，用于兼容性检查。 */
	int AttemptedCount;      /**< 已尝试写入数量，失败/超时的写入也可能已生效。 */
	int ChangedCount;        /**< 已写入且立即回读一致的数量，不代表断电持久化验证。 */
	int UnchangedCount;      /**< 预读已相同、未重复写入的数量。 */
	int ErrorParameterIndex; /**< 失败的 SDK 参数索引，整体校验/保存失败为 -1。 */
	int Stage;               /**< 0校验、1目标预读、2写入、3回读、4保存/重启、5完成。 */
};

/**
 * @brief 同步窗口提供的勾选值到另一同类型电机；必须已进入参数维护模式。
 * @param values SDK 参数索引排列；count 必须等于源/目标参数数量。
 * @param selected 同长度的 0/1 标记；地址、波特率和只读项始终忽略。
 * @param result 必填。完整校验/目标预读后仅写差异，逐项回读，最后调用厂家保存/重启。
 * @return RB_OK 成功，校验失败为 RB_INVALID_ARGUMENT，其它为设备或 SDK 错误。
 * @note 同步调用可能等待设备超时；失败可能已部分生效，不提供事务回滚。源电机不读写。
 */
RB_API int RB_Can_SyncMotorParameters(int sourceMotorIndex, int targetMotorIndex, const int* values, const int* selected, int count, RB_CanMotorParameterSyncResult* result);

/** @brief 读取指定电机的参数能力位。 */
RB_API int RB_Can_GetMotorParameterCapabilitiesForMotor(int motorIndex);

/** @brief 查询指定电机参数是否允许写入。 */
RB_API int RB_Can_IsMotorParameterWritable(int motorIndex, int parameterIndex);

/** @brief 读取指定电机参数名称。 */
RB_API int RB_Can_GetMotorParameterNameForMotor(int motorIndex, int parameterIndex, char* buffer, int bufferSize);

/** @brief 读取指定电机参数值。 */
RB_API int RB_Can_ReadMotorParameter(int motorIndex, int parameterIndex, int* outValue);

/** @brief 写入指定电机参数值。 */
RB_API int RB_Can_WriteMotorParameter(int motorIndex, int parameterIndex, int value);

/** @brief 保存指定电机参数，可选择保存后重启电机。 */
RB_API int RB_Can_SaveMotorParameters(int motorIndex, int rebootAfterSave);

/** @brief 恢复指定电机参数默认值。 */
RB_API int RB_Can_RestoreMotorParameters(int motorIndex);

/** @brief 重启指定电机。 */
RB_API int RB_Can_RebootMotor(int motorIndex);

/** @brief 清除指定电机故障。 */
RB_API int RB_Can_ClearMotorFault(int motorIndex);

/** @brief 在电机参数维护模式中异步升级指定 RB 伺服驱动器固件。 */
RB_API int RB_Can_StartServoFirmwareUpdate(int motorIndex, const char* binFilePath);

/**
 * @brief 返回指定 RB_CAN 输出当前管理的电机数量。
 * @param outputIndex 当前输出目录中的零基索引。
 * @return 大于等于 0 的电机数量；参数无效或状态不可用时返回负数。
 * @note 电机目录属于该输出当前会话，重连或重新绑定后应重新取得数量。
 */
RB_API int RB_Can_GetOutputMotorCount(int outputIndex);

/**
 * @brief 读取指定 RB_CAN 输出中的一个电机状态。
 * @param outputIndex 当前输出目录中的零基索引。
 * @param motorIndex 该输出当前电机目录中的零基索引。
 * @param state 接收电机状态的结构体指针。
 * @return RB_OK 表示成功；RB_INVALID_ARGUMENT 表示索引或指针无效；RB_ERROR 表示状态读取失败。
 */
RB_API int RB_Can_GetOutputMotorState(int outputIndex, int motorIndex, RB_CanMotorState* state);

/**
 * @brief 读取指定 RB_CAN 输出的机器可读健康与恢复状态。
 * @param outputIndex 当前输出目录中的零基索引。
 * @param state 接收健康快照；调用前把 Size 设为 sizeof(RB_CanOutputHealthState)。
 * @return RB_OK 表示成功；RB_INVALID_ARGUMENT 表示参数或结构大小无效；RB_ERROR 表示读取失败。
 */
RB_API int RB_Can_GetOutputHealthState(int outputIndex, RB_CanOutputHealthState* state);

/**
 * @brief 向已连接并已武装的 CAN 运动输出提交一帧最新目标位置。
 * @param outputIndex 当前输出目录中的零基索引；输出类型必须是 RB_CAN、FM_CAN、FC_CAN 或 RDS_CAN。
 * @param positions 按输出配置 OutPutString 中的轴顺序排列的位置数组；位置使用 SDK 现有的 int32 目标位置单位。
 * @param positionCount 位置数量，必须与输出配置中的 CAN 轴数量一致，范围 1-16。
 * @return RB_OK 表示已提交到现有 latest-only 实时位置通道；RB_INVALID_ARGUMENT 表示参数无效；RB_ERROR 表示输出尚未 Ready、被安全状态阻断或提交失败。
 * @note 该接口只提交最新位置，不复制 CAN 协议，也不绕过现有急停、故障和自动恢复状态机。外部桥接程序必须持续发送，并在输入超时后主动发送停车位置或断开输出。
 */
RB_API int RB_Can_SubmitPositions(int outputIndex, const int32_t* positions, int positionCount);

/**
 * @brief 设置指定 CAN 输出由外部宿主接管实时位置帧。
 * @param outputIndex 当前输出目录中的零基索引。
 * @param enabled 非零表示启用；启用后 SDK 内部 CAN 输出模板不再重复发送位置。
 * @return RB_OK 表示设置成功；RB_INVALID_ARGUMENT 表示索引无效；RB_ERROR 表示会话不存在。
 * @note 仅用于 SimHub 等外部运动宿主；普通 SimRacebear 前端不应启用。
 */
RB_API int RB_Can_SetExternalRealtimeMode(int outputIndex, int enabled);

/**
 * @brief 查询指定 CAN 输出是否由外部宿主接管实时位置帧。
 */
RB_API int RB_Can_IsExternalRealtimeMode(int outputIndex);

/**
 * @brief 人工排除故障后显式清除指定输出的恢复限制并启动一次受控恢复。
 * @param outputIndex 当前输出目录中的零基索引。
 * @return RB_OK 表示恢复请求已接受；RB_INVALID_ARGUMENT 表示索引无效；RB_ERROR 表示安全条件或当前状态不允许恢复。
 * @note 仅由用户明确触发。返回成功不表示已经恢复运行，随后继续轮询健康快照。
 */
RB_API int RB_Can_RequestOutputRecovery(int outputIndex);

/**
 * @brief 打开 SDK 内置资源监视器窗口；重复调用会激活已经打开的窗口。
 * @return RB_OK 表示窗口已打开；RB_ERROR 表示窗口线程或窗口创建失败。
 * @note 可在 Debug 或 Release DLL 中使用。窗口只读取状态，不修改运行参数。
 */
RB_API int RB_Debug_OpenMonitor();

/**
 * @brief 关闭资源监视器窗口并等待其 UI 线程退出。
 * @return 无返回值。
 * @note RB_Runtime_Shutdown() 会自动调用本函数，宿主也可以提前关闭窗口。
 */
RB_API void RB_Debug_CloseMonitor();

/**
 * @brief 查询资源监视器窗口是否已经打开。
 * @return 1 表示窗口已打开，0 表示窗口未打开。
 */
RB_API int RB_Debug_IsMonitorOpen();

/**
 * @brief 读取 SDK 支持的完整游戏目录。
 * @param catalog 接收游戏目录的结构体指针；SDK 会清零并填写 Size、Version、Count 和 Items。
 * @return RB_OK 表示成功；RB_INVALID_ARGUMENT 表示 catalog 为空；RB_ERROR 表示内部失败。
 * @note 第三方创建或选择游戏源时应使用 Items[i].GameKey，不要让用户手动输入完整游戏名。
 */
RB_API int RB_Game_GetCatalog(RB_GameCatalog* catalog);

/**
 * @brief 按稳定游戏 key 选择当前游戏并创建对应遥测源。
 * @param gameKey 来自 RB_GameCatalogItem::GameKey 的 UTF-8、NUL 结尾字符串。
 * @return RB_OK 表示已选择；RB_INVALID_ARGUMENT 表示 gameKey 为空或不存在；RB_ERROR 表示创建失败。
 */
RB_API int RB_Game_SelectByKey(const char* gameKey);

/**
 * @brief 按RaceBear稳定遥测key解析当前SDK运行时索引。
 * @param telemetryKey 来自RB_TelemetryCatalogItem::Key的UTF-8字符串。
 * @return 成功返回0到目录数量减1的索引；key不存在或参数无效时返回RB_INVALID_ARGUMENT。
 * @note 返回索引用于当前进程调用；持久化时保存 telemetryKey。
 */
RB_API int RB_Telemetry_FindIndexByKey(const char* telemetryKey);

/**
 * @brief 按稳定遥测key取得MMF/UDP线协议FieldId。
 * @return 成功返回非负FieldId；key不存在时返回RB_INVALID_ARGUMENT。
 * @note FieldId可以写入客户游戏协议；运行时Index不能写入协议。
 */
RB_API int RB_Telemetry_FindFieldIdByKey(const char* telemetryKey);

/** @brief 读取RaceBear遥测目录、稳定key、显示名、单位和当前源支持状态。 */
RB_API int RB_Telemetry_GetCatalog(RB_TelemetryCatalog* catalog);

/** @brief 读取指定运行时索引的原始输入遥测值；索引应通过RB_Telemetry_FindIndexByKey解析。 */
RB_API double RB_Telemetry_GetRawValue(int telemetryIndex);

/** @brief 读取指定运行时索引经过数据源过渡处理后的处理值。 */
RB_API double RB_Telemetry_GetProcessedValue(int telemetryIndex);

/**
 * @brief 创建并连接SDK外部游戏源，替换当前活动内置游戏源。
 * @return RB_OK表示成功；RB_INVALID_ARGUMENT表示结构版本或字段无效；RB_ERROR表示创建失败。
 */
RB_API int RB_ExternalSource_Open(const RB_ExternalSourceDesc* desc);

/**
 * @brief 提交一帧完整外部遥测数据；函数只复制数据，不在调用线程执行平台计算。
 * @return RB_OK表示接收成功；RB_INVALID_ARGUMENT表示帧或值无效；RB_ERROR表示外部源未打开。
 */
RB_API int RB_ExternalSource_SubmitFrame(const RB_ExternalTelemetryFrame* frame);

/** @brief 读取当前外部游戏源状态。 */
RB_API int RB_ExternalSource_GetState(RB_ExternalSourceState* state);

/** @brief 关闭并释放外部游戏源；未打开时调用也安全。 */
RB_API void RB_ExternalSource_Close();

/** @brief 注册或更新自定义游戏并持久化到CustomGamesV1.json。 */
RB_API int RB_Game_RegisterCustom(const RB_CustomGameRegistration* registration);

/** @brief 注销自定义游戏；内置游戏不能通过本接口删除。 */
RB_API int RB_Game_UnregisterCustom(const char* gameKey);

/** @brief 读取指定自定义游戏的注册和接收状态。 */
RB_API int RB_Game_GetCustomState(const char* gameKey, RB_CustomGameState* state);

/** @brief 重新读取CustomGamesV1.json并刷新游戏目录。 */
RB_API int RB_Game_ReloadCustomRegistry();

/**
 * @brief 按固定数据 key 读取 UTF-8 JSON 包。
 * @param key 数据包 key；支持范围见 RaceBearSDK_API.md 的“JSON 数据包”章节。
 * @param buffer 接收 UTF-8、NUL 结尾 JSON 的调用方缓冲区。
 * @param bufferSize buffer 的总容量，单位为字节且必须大于 0。
 * @return RB_OK 表示成功；RB_INVALID_ARGUMENT 表示 key 或缓冲区无效；RB_BUFFER_TOO_SMALL 表示容量不足；RB_ERROR 表示读取失败。
 */
RB_API int RB_Data_ReadJson(const char* key, char* buffer, int bufferSize);

/**
 * @brief 按固定配置 key 写入完整 UTF-8 JSON 配置文档。
 * @param key 可写配置包 key；支持范围见 RaceBearSDK_API.md 的“JSON 数据包”章节。
 * @param jsonText UTF-8、NUL 结尾的完整 JSON 文本，不能为空。
 * @return RB_OK 表示写入成功；RB_INVALID_ARGUMENT 表示 key 或文本无效；RB_ERROR 表示保存失败。
 * @note 保存后是否需要执行 runtime.reloadConfig 命令，取决于对应配置的使用场景。
 */
RB_API int RB_Data_WriteJson(const char* key, const char* jsonText);

/**
 * @brief 读取指定游戏的遥测配置。
 * @param gameName 游戏配置名，通常使用 RB_GameCatalogItem::GameKey。
 * @param config 接收配置的结构体指针。
 * @return RB_OK/CONFIG_SUCCESS 表示成功；负数表示失败或参数无效。
 */
RB_API int RB_Game_ReadConfig(const char* gameName, RB_GameConfig* config);

/**
 * @brief 保存指定游戏的遥测配置。
 * @param gameName 游戏配置名，通常使用 RB_GameCatalogItem::GameKey。
 * @param config 完整游戏配置结构体。
 * @return RB_OK/CONFIG_SUCCESS 表示成功；负数表示失败或参数无效。
 */
RB_API int RB_Game_SaveConfig(const char* gameName, const RB_GameConfig* config);

/**
 * @brief 重新扫描受支持游戏的安装目录和运行进程。
 * @return 大于等于 0 的安装信息数量；失败返回负数。
 * @note 扫描属于低频操作，只在启动、页面刷新或用户明确要求时调用。
 */
RB_API int RB_Game_RefreshInstallations();

/** @brief 返回最近一次安装扫描的条目数量；失败返回负数。 */
RB_API int RB_Game_GetInstallCount();

/**
 * @brief 按扫描结果索引读取安装和运行状态。
 * @param index 零基扫描结果索引。
 * @param info 接收完整安装信息的结构体指针。
 * @return RB_OK 表示成功；RB_INVALID_ARGUMENT 表示索引或指针无效。
 */
RB_API int RB_Game_GetInstallInfo(int index, RB_GameInstallInfo* info);

/**
 * @brief 按稳定 gameKey 查找安装和运行状态。
 * @param gameKey 来自 RB_GameCatalogItem::GameKey。
 * @param info 接收安装信息的结构体指针。
 * @return RB_OK 表示找到安装目录或运行进程；RB_ERROR 表示未找到。
 */
RB_API int RB_Game_FindInstallByKey(const char* gameKey, RB_GameInstallInfo* info);

/**
 * @brief 按稳定 gameKey 读取游戏安装路径。
 * @return RB_OK、RB_BUFFER_TOO_SMALL、RB_INVALID_ARGUMENT 或 RB_ERROR。
 */
RB_API int RB_Game_FindInstallPathByKey(const char* gameKey, char* buffer, int bufferSize);

/**
 * @brief 按游戏目录索引和完整遥测配置创建或重新连接当前 Source。
 * @param catalogIndex RB_GameCatalogItem::CatalogIndex。
 * @param config 先由 RB_Game_ReadConfig() 读取并修改的完整配置。
 * @return RB_OK 表示 Source 已成功创建或连接；其他值表示配置或连接失败。
 */
RB_API int RB_Game_AutoConnect(int catalogIndex, const RB_GameConfig* config);

/**
 * @brief 对当前游戏执行 SDK 内置的一键遥测配置。
 * @return RB_OK 表示游戏侧配置成功；RB_ERROR 表示当前游戏不支持或配置失败。
 */
RB_API int RB_Game_AutoConfigureCurrent();

/**
 * @brief 为当前游戏安装 SDK 内置的游戏侧遥测插件。
 * @param gamePath UTF-8 游戏安装目录；应来自 RB_GameInstallInfo::InstallPath。
 * @return RB_OK 表示安装成功；其他值表示路径无效、不支持或安装失败。
 */
RB_API int RB_Game_InstallCurrentPlugin(const char* gamePath);

/**
 * @brief 检查当前游戏侧配置是否与给定遥测配置匹配。
 * @return RB_OK 表示配置可用；RB_ERROR 表示缺失、不匹配或当前游戏不支持检查。
 */
RB_API int RB_Game_CheckCurrentSideConfig(const RB_GameConfig* config);

/**
 * @brief 一次读取功能插件目录；目录刷新后旧索引失效。
 * @param catalog 接收插件目录的结构体指针；调用前零初始化。
 * @return RB_OK 表示成功；RB_INVALID_ARGUMENT 表示 catalog 为空。
 */
RB_API int RB_Plugin_GetCatalog(RB_PluginCatalog* catalog);

/** @brief 重新扫描功能插件并返回插件数量；失败返回负数。 */
RB_API int RB_Plugin_Refresh();

/**
 * @brief 启动指定插件提供的前端程序。
 * @param pluginIndex 最近一次插件目录中的零基索引。
 * @return RB_OK 表示已启动；RB_ERROR 表示插件不可启动或启动失败。
 */
RB_API int RB_Plugin_Launch(int pluginIndex);

/**
 * @brief 启动全部功能插件后台运行态。
 * @return 大于等于 0 的已启动/已存在运行态数量；失败返回负数。
 */
RB_API int RB_Plugin_StartRuntimes();

/**
 * @brief 读取系统配置；配置不存在时返回 SDK 默认值。
 */
RB_API int RB_System_ReadConfigOrDefault(RB_SystemConfig* config);

/**
 * @brief 保存系统配置。
 */
RB_API int RB_System_SaveConfig(const RB_SystemConfig* config);

/**
 * @brief 按平台索引读取平台结构和运行参数。
 */
RB_API int RB_Platform_ReadConfigByIndex(int platformIndex, RB_PlatformConfig* config);

/**
 * @brief 按平台索引保存平台结构和运行参数。
 */
RB_API int RB_Platform_SaveConfigByIndex(int platformIndex, const RB_PlatformConfig* config);
/** @brief 导出指定平台的完整配置包。 */
RB_API int RB_Platform_ExportPackage(int platformIndex, const char* packagePath);
/** @brief 导入平台配置包并覆盖包内 platformKey 对应的平台节点。 */
RB_API int RB_Platform_ImportPackage(const char* packagePath, char* outPlatformKey, int bufferSize);

/**
 * @brief 设置当前选中的平台状态缓存。
 */
RB_API int RB_Platform_SetSelectedState(int platformIndex, const RB_PlatformConfig* config);

/**
 * @brief 将平台配置应用到当前运行中的平台模型。
 */
RB_API int RB_Platform_ApplyRuntimeConfigToCurrentRig(const RB_PlatformConfig* config);

/**
 * @brief 按平台索引读取 Motion 碰撞/异常保护配置。
 */
RB_API int RB_Platform_ReadMotionProtectionConfigByIndex(int platformIndex, RB_MotionProtectionConfig* config);

/**
 * @brief 按平台索引保存 Motion 碰撞/异常保护配置。
 */
RB_API int RB_Platform_SaveMotionProtectionConfigByIndex(int platformIndex, const RB_MotionProtectionConfig* config);

/**
 * @brief 将 Motion 碰撞/异常保护配置热应用到当前运行中的 Rig。
 */
RB_API int RB_Platform_ApplyMotionProtectionConfigToCurrentRig(const RB_MotionProtectionConfig* config);

/** @brief 读取六自由度结果保护 V2 配置，SDK 写入 Size/Version 和全部字段。 */
RB_API int RB_Platform_ReadMotionProtectionConfigV2ByIndex(int platformIndex, RB_MotionProtectionConfigV2* config);
/** @brief 保存 V2 配置，Size 和 Version 必须正确，失败不部分保存。 */
RB_API int RB_Platform_SaveMotionProtectionConfigV2ByIndex(int platformIndex, const RB_MotionProtectionConfigV2* config);
/** @brief 热应用 V2 配置，需有效授权和当前 Rig。 */
RB_API int RB_Platform_ApplyMotionProtectionConfigV2ToCurrentRig(const RB_MotionProtectionConfigV2* config);

/**
 * @brief 保存并应用指定平台配置到当前运行中的平台模型。
 */
RB_API int RB_Platform_ApplyConfigToCurrentRig(int platformIndex, const RB_PlatformConfig* config);

/**
 * @brief 返回 SDK 支持的平台数量。
 * @return 返回大于等于 0 的平台数量；初始化失败时返回 0。
 * @note 有效平台索引范围为 [0, 返回值)，索引仅用于当前 SDK 版本的目录访问。
 */
RB_API int RB_Platform_GetCount();

/**
 * @brief 按目录索引读取平台名称。
 * @param platformIndex 平台零基目录索引。
 * @param buffer 接收 UTF-8、NUL 结尾平台名称的缓冲区。
 * @param bufferSize buffer 的总字节数。
 * @return RB_OK 表示成功；RB_INVALID_ARGUMENT 表示索引或缓冲区无效。
 */
RB_API int RB_Platform_GetName(int platformIndex, char* buffer, int bufferSize);

/**
 * @brief 查询平台是否支持指定 6DOF。
 * @param platformIndex 平台零基目录索引。
 * @param dofIndex DOF 索引，0-5 依次为 Sway/Surge/Heave/Yaw/Roll/Pitch。
 * @return 1 表示支持，0 表示不支持或参数无效。
 */
RB_API int RB_Platform_IsDofSupported(int platformIndex, int dofIndex);

/**
 * @brief 读取 L1-L7 几何参数的显示名称。
 * @param platformIndex 平台零基目录索引。
 * @param geometryIndex 参数编号，范围 1-7。
 * @param buffer 接收 UTF-8、NUL 结尾名称的缓冲区；未使用参数返回空字符串。
 * @param bufferSize buffer 的总字节数。
 * @return RB_OK 表示调用成功；RB_INVALID_ARGUMENT 表示参数无效。
 */
RB_API int RB_Platform_GetGeometryParameterName(int platformIndex, int geometryIndex, char* buffer, int bufferSize);

/**
 * @brief 读取 L8-L10 执行器行程参数的显示名称。
 * @param platformIndex 平台零基目录索引。
 * @param strokeIndex 参数编号，范围 8-10。
 * @param buffer 接收 UTF-8、NUL 结尾名称的缓冲区；未使用参数返回空字符串。
 * @param bufferSize buffer 的总字节数。
 * @return RB_OK 表示调用成功；RB_INVALID_ARGUMENT 表示参数无效。
 */
RB_API int RB_Platform_GetStrokeParameterName(int platformIndex, int strokeIndex, char* buffer, int bufferSize);

/** @brief 读取当前持久化选择的平台零基索引；失败返回负数。 */
RB_API int RB_Platform_ReadSelectedIndex();

/** @brief 保存当前平台零基索引；返回 RB_OK 或错误码。 */
RB_API int RB_Platform_SaveSelectedIndex(int platformIndex);

/**
 * @brief 计算指定平台配置下某个 DOF 的正负预览范围。
 * @return 线性 DOF 返回毫米，旋转 DOF 返回度；不可达或参数无效时返回 0。
 */
RB_API double RB_Platform_GetPreviewPoseLimit(int platformIndex, const RB_PlatformConfig* config, int dofIndex);

/** @brief 判断指定平台配置是否可以预览该 DOF；1=可以，0=不可达或参数无效。 */
RB_API int RB_Platform_IsPreviewPosePossible(int platformIndex, const RB_PlatformConfig* config, int dofIndex);

/**
 * @brief 启用或关闭真实运行平台的手动姿态测试输入。
 * @param enabled 1=启用，0=关闭并恢复正常遥测输入。
 * @return RB_OK 表示状态已更新；RB_ERROR 表示运行平台尚未创建。
 */
RB_API int RB_ManualPose_SetTestEnabled(int enabled);

/**
 * @brief 设置单个 DOF 的手动测试值。
 * @param dofIndex 0-5 依次为 Sway/Surge/Heave/Yaw/Roll/Pitch。
 * @param value 线性 DOF 单位 mm，旋转 DOF 单位度；应限制在 RB_Platform_GetPreviewPoseLimit() 返回范围内。
 * @return RB_OK 表示已设置；RB_INVALID_ARGUMENT 或 RB_ERROR 表示输入无效或平台不可用。
 */
RB_API int RB_ManualPose_SetDrive(int dofIndex, double value);

/** @brief 将全部手动姿态输入复位为 0；返回 RB_OK 或 RB_ERROR。 */
RB_API int RB_ManualPose_ResetDrive();

/** @brief 读取单个 DOF 当前手动测试值；参数无效或平台不可用时返回 0。 */
RB_API double RB_ManualPose_GetDrive(int dofIndex);

/**
 * @brief 按输出实例索引读取输出配置。
 * @param instanceIndex 输出实例零基索引，范围 [0, RB_Output_GetInstanceCount())。
 * @param config 接收完整配置的结构体指针；调用前零初始化。
 * @return RB_OK 表示成功；RB_INVALID_ARGUMENT 表示索引或指针无效；RB_ERROR 表示读取失败。
 * @note 编辑页面先读取完整结构体，再修改目标字段并保存，以保留未展示字段。
 */
RB_API int RB_Output_ReadInstanceConfigByIndex(int instanceIndex, RB_OutputConfig* config);
/** @brief 读取指定输出实例的运行态配置缓存，不访问配置文件；适合状态栏和高频 UI 刷新。 */
RB_API int RB_Output_GetInstanceConfigSnapshotByIndex(int instanceIndex, RB_OutputConfig* config);

/**
 * @brief 按输出实例索引保存输出配置。
 * @param instanceIndex 输出实例零基索引。
 * @param config 要保存的完整配置；字符串均为 UTF-8、NUL 结尾。
 * @return RB_OK 表示已保存；其他值表示参数无效或写入失败。
 * @note 修改 Port、IP、端口、CAN 索引等连接参数前，应先断开该实例。
 */
RB_API int RB_Output_SaveInstanceConfigByIndex(int instanceIndex, const RB_OutputConfig* config);

/**
 * @brief 按输出实例索引将输出配置应用到运行态。
 * @param instanceIndex 输出实例零基索引。
 * @param config 要应用的完整配置。
 * @return RB_OK 表示运行态已接受配置；其他值表示实例不存在或应用失败。
 * @note Apply 不表示用户要求立即连接，也不能替代 Save。
 */
RB_API int RB_Output_ApplyInstanceConfigByIndex(int instanceIndex, const RB_OutputConfig* config);

/**
 * @brief 读取或保存 CAN 输出空闲下使能超时，单位秒；0 禁止自动下使能。
 * @note 不扩展 RB_OutputConfig；只有 RB_CAN/FM_CAN/FC_CAN 支持该设置，
 *       并且仅在输出配置的 AutoConnect 为 1 时执行。
 */
RB_API int RB_Output_GetIdleDisableTimeoutSeconds(int instanceIndex, int* timeoutSeconds);
RB_API int RB_Output_SetIdleDisableTimeoutSeconds(int instanceIndex, int timeoutSeconds);

/**
 * @brief 一次读取输出实例目录和当前可用于命令模板的输出 Key。
 * @param catalog 接收目录的结构体指针；调用前零初始化。
 * @return RB_OK 表示成功；RB_INVALID_ARGUMENT 表示 catalog 为空。
 * @note Key 列表会随当前平台轴数和已创建功能变化，前端不要写死。
 */
RB_API int RB_Output_GetCatalog(RB_OutputCatalog* catalog);

/**
 * @brief 返回当前输出实例数量。
 * @return 返回大于等于 0 的实例数量；初始化失败时返回 0。
 * @note 有效实例索引范围为 [0, 返回值)。
 */
RB_API int RB_Output_GetInstanceCount();

/**
 * @brief 返回 SDK 支持的输出类型数量。
 * @return 大于等于 0 的类型数量；初始化前也可以调用。
 */
RB_API int RB_Output_GetKindCount();

/**
 * @brief 按索引读取可传给 RB_Output_AddInstance() 的输出类型 key。
 * @param kindIndex 输出类型零基索引，范围 [0, RB_Output_GetKindCount())。
 * @param buffer 接收 UTF-8、NUL 结尾类型 key 的缓冲区。
 * @param bufferSize buffer 总字节数。
 * @return RB_OK、RB_BUFFER_TOO_SMALL 或 RB_INVALID_ARGUMENT。
 */
RB_API int RB_Output_GetKind(int kindIndex, char* buffer, int bufferSize);

/**
 * @brief 读取安全带 CAN 默认配置。
 */
RB_API int RB_SeatbeltCan_GetDefaultConfig(RB_SeatbeltCanConfig* config);

/**
 * @brief 读取指定输出实例的安全带 CAN 配置。
 */
RB_API int RB_SeatbeltCan_ReadConfig(int outputInstanceIndex, RB_SeatbeltCanConfig* config);

/**
 * @brief 保存指定输出实例的安全带 CAN 配置。
 */
RB_API int RB_SeatbeltCan_SaveConfig(int outputInstanceIndex, const RB_SeatbeltCanConfig* config);

/**
 * @brief 将安全带 CAN 配置应用到已创建但未连接的输出实例。
 */
RB_API int RB_SeatbeltCan_ApplyConfig(int outputInstanceIndex, const RB_SeatbeltCanConfig* config);

/**
 * @brief 读取指定安全带 CAN 输出实例的连接、回零和目标位置状态。
 */
RB_API int RB_SeatbeltCan_GetState(int outputInstanceIndex, RB_SeatbeltCanState* state);

/**
 * @brief 请求指定安全带 CAN 输出实例执行一次回零。
 */
RB_API int RB_SeatbeltCan_RequestHoming(int outputInstanceIndex);

/**
 * @brief 确保至少存在一个默认 Serial 输出实例。
 * @return RB_OK 表示配置已经存在或创建成功；负数表示初始化或写入失败。
 */
RB_API int RB_Output_EnsureDefaultConfig();

/**
 * @brief 返回启用 AutoConnect 的输出实例数量。
 * @return 大于等于 0 的实例数量；失败按 0 处理并读取 SDK 日志。
 */
RB_API int RB_Output_GetAutoConnectInstanceCount();

/**
 * @brief 读取指定输出实例的稳定 key。
 * @return RB_OK、RB_BUFFER_TOO_SMALL 或 RB_INVALID_ARGUMENT。
 */
RB_API int RB_Output_GetInstanceKey(int instanceIndex, char* buffer, int bufferSize);

/**
 * @brief 添加输出实例。
 * @param outputKind "Serial"、"UDP"、"MMF"、"RB_CAN"、"FM_CAN"、"FC_CAN"、"RDS_CAN"、"Sim-FAN" 或 "Vibration"。
 * @return 成功时返回新实例的零基索引；失败返回负数错误码。
 */
RB_API int RB_Output_AddInstance(const char* outputKind);

/**
 * @brief 删除指定输出实例及其配置。
 * @note 删除前必须先断开实例；删除后后续实例索引可能变化，应刷新目录。
 */
RB_API int RB_Output_DeleteInstanceByIndex(int instanceIndex);

/**
 * @brief 连接指定输出实例。
 * @param instanceIndex 输出实例零基索引。
 * @return RB_OK 表示连接流程已成功启动；其他值表示索引无效或启动失败。
 */
RB_API int RB_Output_ConnectInstanceByIndex(int instanceIndex);

/**
 * @brief 查询指定输出实例是否已经连接。
 * @param instanceIndex 输出实例零基索引。
 * @return 1 表示已连接，0 表示未连接或索引无效。
 */
RB_API int RB_Output_IsInstanceConnected(int instanceIndex);

/**
 * @brief 查询指定实例是否已进入周期输出运行态。
 * @param instanceIndex 输出实例零基索引。
 * @return 1 表示正在周期输出；0 表示未运行或索引无效。
 */
RB_API int RB_Output_IsInstanceRuntimeActive(int instanceIndex);

/**
 * @brief 查询是否存在任意周期输出运行态。
 * @return 1 表示至少一个实例正在输出；0 表示全部未运行。
 */
RB_API int RB_Output_IsAnyRuntimeActive();

/**
 * @brief 断开指定输出实例。
 * @param instanceIndex 输出实例零基索引。
 * @return RB_OK 表示成功，其他值表示索引无效或断开失败。
 */
RB_API int RB_Output_DisconnectInstanceByIndex(int instanceIndex);

/**
 * @brief 连接全部输出实例。
 * @return 大于等于 0 表示成功发起连接的实例数量；负数表示整体操作失败。
 */
RB_API int RB_Output_ConnectAll();

/**
 * @brief 只连接 AutoConnect=1 的输出实例。
 * @return 大于等于 0 表示成功发起连接的实例数量；负数表示整体操作失败。
 */
RB_API int RB_Output_ConnectAuto();

/**
 * @brief 断开全部输出实例。
 * @return 大于等于 0 表示成功发起断开的实例数量；负数表示整体操作失败。
 */
RB_API int RB_Output_DisconnectAll();

/**
 * @brief 查询全部输出是否已完成断开。
 * @return 1 表示全部断开；0 表示仍有连接或正在执行断开状态机。
 */
RB_API int RB_Output_AreAllDisconnected();

/**
 * @brief 请求全部输出按各自结束命令和状态机安全断开。
 * @return RB_OK 表示请求已提交；调用方应继续轮询 RB_Output_AreAllDisconnected()。
 */
RB_API int RB_Output_RequestDisconnectAll();

/**
 * @brief 按配置名读取 6DOF 动态配置。
 * @param profileName UTF-8 配置名，例如 "_Default"。
 * @param profile 接收完整配置的结构体指针。
 */
RB_API int RB_Dynamic_ReadProfile(const char* profileName, RB_DynamicV2Profile* profile);

/**
 * @brief 按动态配置索引读取 6DOF 动态配置。
 */
RB_API int RB_Dynamic_ReadProfileByIndex(int profileIndex, RB_DynamicV2Profile* profile);

/**
 * @brief 按动态配置索引保存 6DOF 动态配置。
 */
RB_API int RB_Dynamic_SaveProfileByIndex(int profileIndex, const RB_DynamicV2Profile* profile);

/**
 * @brief 按配置名保存 6DOF 动态配置。
 */
RB_API int RB_Dynamic_SaveProfile(const char* profileName, const RB_DynamicV2Profile* profile);
/** @brief 导出指定动态 profile 的 DynamicV2 与同名 MotionEffect 配置包。 */
RB_API int RB_Dynamic_ExportPackage(int profileIndex, const char* packagePath);
/** @brief 导入动态配置包；profile 不存在时自动创建，存在时完整覆盖。 */
RB_API int RB_Dynamic_ImportPackage(const char* packagePath, char* outProfileName, int bufferSize);

/**
 * @brief 将动态配置应用到当前运行中的平台模型。
 */
RB_API int RB_Dynamic_ApplyProfileToCurrentRig(const RB_DynamicV2Profile* profile);

/**
 * @brief 返回动态滤波配置数量。
 * @return 返回大于等于 0 的配置数量；初始化失败时返回 0。
 * @note 有效配置索引范围为 [0, 返回值)。
 */
RB_API int RB_Dynamic_GetProfileCount();

/**
 * @brief 按索引读取动态滤波配置显示名称。
 * @param profileIndex 动态配置零基索引。
 * @param buffer 接收 UTF-8、NUL 结尾名称的缓冲区。
 * @param bufferSize buffer 的总字节数。
 * @return RB_OK 表示成功；RB_INVALID_ARGUMENT 表示索引或缓冲区无效。
 */
RB_API int RB_Dynamic_GetProfileName(int profileIndex, char* buffer, int bufferSize);

/**
 * @brief 读取指定目标 DOF 可以添加的输入通道效果目录。
 * @param dofIndex 目标 DOF：0=Sway、1=Surge、2=Heave、3=Yaw、4=Roll、5=Pitch、6=SwayToRoll、7=SurgeToPitch。
 * @param catalog 接收候选目录；调用前零初始化。
 * @return RB_OK 表示成功；RB_INVALID_ARGUMENT 表示索引或指针无效；RB_ERROR 表示目录读取失败。
 * @note 前端显示 Items[].Key，使用目录候选，并通过 Key 保持同一 DOF 中的输入唯一。
 */
RB_API int RB_Dynamic_GetInputCandidateCatalog(int dofIndex, RB_DynamicInputCandidateCatalog* catalog);

/**
 * @brief 返回当前游戏绑定的动态配置索引。
 * @return 有效零基索引；无当前游戏、无绑定或读取失败时返回负数。
 */
RB_API int RB_Dynamic_ReadCurrentGameProfileIndex();

/**
 * @brief 把指定动态配置索引绑定到当前游戏。
 * @param profileIndex 动态配置零基索引。
 * @return RB_OK 表示保存成功；其他值表示无当前游戏、索引无效或写入失败。
 */
RB_API int RB_Dynamic_SaveCurrentGameProfileIndex(int profileIndex);

/**
 * @brief 删除指定动态配置及同名运动增强、震动配置。
 * @param profileIndex 动态配置零基索引。
 * @return RB_OK 表示删除成功；其他值表示索引无效或删除失败。
 * @note 删除后配置索引会变化，前端必须刷新配置目录。
 */
RB_API int RB_Dynamic_DeleteProfileByIndex(int profileIndex);

/**
 * @brief 一次读取运动增强效果目录。
 * @param catalog 接收效果名称、说明和输出说明；调用前零初始化。
 * @return RB_OK 表示成功；RB_INVALID_ARGUMENT 表示 catalog 为空。
 */
RB_API int RB_MotionEffect_GetCatalog(RB_EffectCatalog* catalog);

/**
 * @brief 返回运动增强配置数量。
 * @return 大于等于 0 的配置数量；失败返回负数。
 * @note 运动增强拥有独立配置目录；索引只能与 RB_MotionEffect_GetProfileName() 配套使用，不能复用动态滤波索引。
 */
RB_API int RB_MotionEffect_GetProfileCount();

/**
 * @brief 按索引读取运动增强配置名。
 * @param profileIndex 配置零基索引。
 * @param buffer 接收 UTF-8、NUL 结尾名称的缓冲区。
 * @param bufferSize buffer 总字节数。
 * @return RB_OK、RB_BUFFER_TOO_SMALL 或 RB_INVALID_ARGUMENT。
 */
RB_API int RB_MotionEffect_GetProfileName(int profileIndex, char* buffer, int bufferSize);

/**
 * @brief 删除指定名称的运动增强配置。
 * @param profileName UTF-8 配置名。
 * @return RB_OK 表示删除成功；其他值表示名称无效或删除失败。
 * @note 本函数只删除运动增强配置；动态滤波和震动配置不受影响。
 */
RB_API int RB_MotionEffect_DeleteProfile(const char* profileName);

/**
 * @brief 按配置名读取运动增强配置。
 */
RB_API int RB_MotionEffect_ReadProfile(const char* profileName, RB_MotionEffectProfile* profile);

/**
 * @brief 按配置索引读取运动增强配置。
 */
RB_API int RB_MotionEffect_ReadProfileByIndex(int profileIndex, RB_MotionEffectProfile* profile);

/**
 * @brief 按配置名保存运动增强配置。
 */
RB_API int RB_MotionEffect_SaveProfile(const char* profileName, const RB_MotionEffectProfile* profile);

/**
 * @brief 按配置索引保存运动增强配置。
 */
RB_API int RB_MotionEffect_SaveProfileByIndex(int profileIndex, const RB_MotionEffectProfile* profile);

/**
 * @brief 将运动增强配置应用到当前运行中的平台模型。
 */
RB_API int RB_MotionEffect_ApplyProfileToCurrentRig(const RB_MotionEffectProfile* profile);

/**
 * @brief 读取指定运动增强效果的默认参数。
 */
RB_API int RB_MotionEffect_GetDefaultConfig(int effectIndex, RB_MotionEffectConfig* config);

/**
 * @brief 一次读取震动效果目录。
 * @param catalog 接收目录的结构体指针；调用前零初始化。
 * @return RB_OK 表示成功；RB_INVALID_ARGUMENT 表示 catalog 为空。
 */
RB_API int RB_Haptic_GetCatalog(RB_EffectCatalog* catalog);

/**
 * @brief 按配置名读取震动效果配置。
 */
RB_API int RB_Haptic_ReadProfile(const char* profileName, RB_HapticEffectProfile* profile);

/**
 * @brief 按动态配置索引读取同名震动配置。
 * @param profileIndex 动态配置零基索引。
 * @param profile 接收完整震动配置的结构体指针。
 * @return RB_OK 表示成功；其他值表示索引无效或读取失败。
 * @note 缺少同名配置时读取默认配置。
 */
RB_API int RB_Haptic_ReadProfileByIndex(int profileIndex, RB_HapticEffectProfile* profile);

/**
 * @brief 按配置名保存震动效果配置。
 */
RB_API int RB_Haptic_SaveProfile(const char* profileName, const RB_HapticEffectProfile* profile);

/**
 * @brief 按动态配置索引保存同名震动配置。
 * @param profileIndex 动态配置零基索引。
 * @param profile 要保存的完整震动配置。
 * @return RB_OK 表示保存成功；其他值表示参数无效或写入失败。
 */
RB_API int RB_Haptic_SaveProfileByIndex(int profileIndex, const RB_HapticEffectProfile* profile);

/**
 * @brief 将震动配置应用到当前运行态。
 * @param profile 要应用的完整震动配置。
 * @return RB_OK 表示应用成功；其他值表示参数无效或运行态不可用。
 * @note Apply 不写配置文件。
 */
RB_API int RB_Haptic_ApplyProfileToCurrentFunction(const RB_HapticEffectProfile* profile);

/**
 * @brief 读取指定震动效果的默认参数。
 * @param effectIndex RB_Haptic_GetCatalog() 返回的效果索引。
 * @param config 接收默认参数的结构体指针。
 * @return RB_OK 表示成功；RB_INVALID_ARGUMENT 表示索引或指针无效。
 */
RB_API int RB_Haptic_GetDefaultConfig(int effectIndex, RB_HapticEffectConfig* config);

/**
 * @brief 读取默认风感配置。
 */
RB_API int RB_Wind_GetDefaultConfig(RB_WindEffectConfig* config);

/**
 * @brief 读取当前风感配置。
 */
RB_API int RB_Wind_ReadConfig(RB_WindEffectConfig* config);

/**
 * @brief 保存风感配置。
 */
RB_API int RB_Wind_SaveConfig(const RB_WindEffectConfig* config);

/**
 * @brief 将风感配置应用到当前运行中的风感功能。
 */
RB_API int RB_Wind_ApplyConfigToCurrentFunction(const RB_WindEffectConfig* config);

/**
 * @brief 启用或关闭风感测试输出。
 * @param enabled 1=测试，0=恢复当前运行配置。
 * @param outputPercent 四通道统一测试输出，范围 0-100%。
 * @return RB_OK 表示测试状态已更新；其他值表示运行态不可用。
 */
RB_API int RB_Wind_SetTestOutput(int enabled, double outputPercent);

/**
 * @brief 读取默认安全带配置。
 */
RB_API int RB_Seatbelt_GetDefaultConfig(RB_SeatbeltEffectConfig* config);

/**
 * @brief 读取当前安全带配置。
 */
RB_API int RB_Seatbelt_ReadConfig(RB_SeatbeltEffectConfig* config);

/**
 * @brief 保存安全带配置。
 */
RB_API int RB_Seatbelt_SaveConfig(const RB_SeatbeltEffectConfig* config);

/**
 * @brief 将安全带配置应用到当前运行中的安全带功能。
 */
RB_API int RB_Seatbelt_ApplyConfigToCurrentFunction(const RB_SeatbeltEffectConfig* config);

/**
 * @brief 启用或关闭左右安全带测试输出。
 * @param enabled 1=测试，0=恢复当前运行配置。
 * @param leftPercent 左通道测试输出，范围 0-100%。
 * @param rightPercent 右通道测试输出，范围 0-100%。
 * @return RB_OK 表示测试状态已更新；其他值表示运行态不可用。
 */
RB_API int RB_Seatbelt_SetTestOutput(int enabled, double leftPercent, double rightPercent);

/** @brief 刷新 RGBAPP WinUSB 设备目录并返回设备数量。 */
RB_API int RB_Rgb_RefreshDevices();
/** @brief 返回最近一次刷新后的 RGBAPP 设备数量。 */
RB_API int RB_Rgb_GetDeviceCount();
/** @brief 读取设备目录项；index 为零基目录索引。 */
RB_API int RB_Rgb_GetDeviceInfo(int index, RB_RgbDeviceInfo* info);
/** @brief 按稳定 MCU UID 连接 RGBAPP。 */
RB_API int RB_Rgb_Connect(const char* uid);
/** @brief 熄灭并断开当前 RGBAPP。 */
RB_API void RB_Rgb_Disconnect();
/** @brief 读取连接或应用配置时更新的内存快照，不访问 USB；适合 UI 页面刷新。 */
RB_API int RB_Rgb_GetDeviceConfigSnapshot(RB_RgbDeviceConfig* config);
/** @brief 读取设备当前四路灯带与功率保护配置。 */
RB_API int RB_Rgb_ReadDeviceConfig(RB_RgbDeviceConfig* config);
/** @brief 将灯带配置应用到设备运行态，不写 Flash。 */
RB_API int RB_Rgb_ApplyDeviceConfig(const RB_RgbDeviceConfig* config);
/** @brief 将设备当前灯带配置明确保存到 Flash。 */
RB_API int RB_Rgb_SaveDeviceConfig();
/** @brief 返回指定效果可选择的语义输入数量；静态色返回 0。 */
RB_API int RB_Rgb_GetEffectInputCount(int effectType);
/** @brief 按效果类型和候选索引读取语义输入；RPM和换挡效果返回固定输入。 */
RB_API int RB_Rgb_GetEffectInput(int effectType, int inputIndex, RB_RgbEffectInputItem* item);
/** @brief 读取默认遥测 RGB 效果配置。 */
RB_API int RB_Rgb_GetDefaultEffectProfile(RB_RgbEffectProfile* profile);
/** @brief 从 ExtraEffectsV2.json 读取 RGB 效果配置。 */
RB_API int RB_Rgb_ReadEffectProfile(RB_RgbEffectProfile* profile);
/** @brief 明确保存 RGB 效果配置；不会自动连接设备。 */
RB_API int RB_Rgb_SaveEffectProfile(const RB_RgbEffectProfile* profile);
/** @brief 将 RGB 效果配置应用到当前运行态；Apply 不读取或写入配置文件。 */
RB_API int RB_Rgb_ApplyEffectProfile(const RB_RgbEffectProfile* profile);
/** @brief 读取内存中的 RGB 运行状态，不访问配置文件或设备。 */
RB_API int RB_Rgb_GetState(RB_RgbState* state);
/** @brief 设置固件测试图案和 RGB888 颜色；硬件诊断测试不依赖运行输出授权。 */
RB_API int RB_Rgb_SetTestPattern(int pattern, int red, int green, int blue);
/** @brief 立即请求设备熄灭。 */
RB_API int RB_Rgb_Blackout();
/** @brief 主动向设备读取一次诊断快照。 */
RB_API int RB_Rgb_ReadDiagnostics(RB_RgbDiagnostics* diagnostics);
/**
 * @brief 按稳定 UID 异步更新 RGBAPP 的 APP 固件。
 * @param uid 24 个十六进制字符的 MCU UID；设备可处于 APP 或 IAP 模式。
 * @param binFilePath RGBAPP APP `.bin` 文件路径。SDK 在擦除前校验大小、向量地址和 CRC。
 * @return RB_OK 表示后台任务已启动；RB_INVALID_ARGUMENT 表示参数无效；RB_ERROR 表示已有任务或运行态不可用。
 */
RB_API int RB_Rgb_StartAppFirmwareUpdate(const char* uid, const char* binFilePath);
/**
 * @brief 按稳定 UID 异步更新 RGBAPP 的 Stage-1/IAP 固件。
 * @param uid 24 个十六进制字符的 MCU UID；目标必须当前处于 APP 模式。
 * @param binFilePath RGBAPP_IAP `.bin` 文件路径。SDK 会校验 Stage-1 向量、清单、大小和 CRC。
 * @return RB_OK 表示后台任务已启动；RB_INVALID_ARGUMENT 表示参数或镜像无效；RB_ERROR 表示设备或运行态不可用。
 */
RB_API int RB_Rgb_StartIapFirmwareUpdate(const char* uid, const char* binFilePath);
/** @brief 读取 RGBAPP APP 固件更新内存状态。 */
RB_API int RB_Rgb_GetFirmwareUpdateState(RB_RgbFirmwareUpdateState* state);
/** @brief 返回 RGBAPP APP 固件更新线程当前是否运行。 */
RB_API int RB_Rgb_IsFirmwareUpdateRunning();

/**
 * @brief 执行统一动作命令，并可返回命令结果 JSON。
 * @param command UTF-8、NUL 结尾的命令 key；支持范围见 RaceBearSDK_API.md 的“Command”章节。
 * @param argsJson UTF-8、NUL 结尾的参数对象；无参数命令应传 "{}"。
 * @param resultBuffer 可选的结果缓冲区；不需要结果时可传 nullptr。
 * @param resultBufferSize resultBuffer 的总容量，单位为字节；传入缓冲区时必须大于 0。
 * @return RB_OK 表示命令执行成功；RB_INVALID_ARGUMENT 表示命令或参数无效/不支持；RB_BUFFER_TOO_SMALL 表示结果缓冲区不足；RB_ERROR 表示命令执行失败。
 */
RB_API int RB_Command_Run(const char* command, const char* argsJson, char* resultBuffer, int resultBufferSize);

/**
 * @brief 从 SDK 运行日志队列中弹出一条日志文本。
 * @param buffer 接收 UTF-8、NUL 结尾日志的调用方缓冲区；队列为空时写入空字符串。
 * @param bufferSize buffer 的总容量，单位为字节且必须大于 0。
 * @return RB_OK 表示读取完成；RB_INVALID_ARGUMENT 表示缓冲区无效；RB_BUFFER_TOO_SMALL 表示容量不足；RB_ERROR 表示内部失败。
 */
RB_API int RB_Log_Read(char* buffer, int bufferSize);


// VOCORE USB 仪表屏。画面发送在独立线程中执行，不阻塞 SDK 后端 Loop。

/**
 * @brief 枚举当前可访问的 VOCORE USB 屏幕数量。
 * @return 大于或等于 0 表示设备数量；libusb 运行库不可用或枚举失败时返回 RB_ERROR。
 * @note SDK 目录必须同时存在 VS2019 x64 版 libusb-1.0.dll，设备需安装 VOCORE 官方 LibUSBK 驱动。
 */
RB_API int RB_VoCore_GetDeviceCount();

/**
 * @brief 按当前枚举索引读取 VOCORE 屏幕信息。
 * @param index 设备索引，范围为 0 到 RB_VoCore_GetDeviceCount()-1。
 * @param info 调用方结构体；调用前必须将 Size 设为 sizeof(RB_VoCoreDeviceInfo)。
 * @return RB_OK 表示成功；RB_INVALID_ARGUMENT 表示参数或索引无效；RB_ERROR 表示设备无法访问。
 */
RB_API int RB_VoCore_GetDeviceInfo(int index, RB_VoCoreDeviceInfo* info);

/**
 * @brief 打开一块 VOCORE 屏幕并启动独立发送线程。
 * @param deviceId RB_VoCoreDeviceInfo.DeviceId；传 nullptr 或空字符串时打开第一块可用屏幕。
 * @param autoReconnect 1=拔插或 USB 错误后每 1000ms 自动重连，0=连接丢失后保持断开。
 * @param handle 接收新句柄的指针；成功后由调用方负责调用 RB_VoCore_Close()。
 * @return RB_OK 表示线程已启动；RB_INVALID_ARGUMENT 表示 handle 为空；RB_ERROR 表示设备打开失败且未启用自动重连。
 */
RB_API int RB_VoCore_Open(const char* deviceId, int autoReconnect, RB_VoCoreHandle* handle);

/**
 * @brief 停止发送线程、释放 USB 接口并销毁 VOCORE 句柄。
 * @param handle RB_VoCore_Open() 返回的句柄；nullptr 会被忽略。
 * @note 调用后句柄立即失效；不得与同一句柄的其他接口并发调用。
 */
RB_API void RB_VoCore_Close(RB_VoCoreHandle handle);

/**
 * @brief 异步提交一帧完整画面，尚未发送的旧帧会被最新帧覆盖。
 * @param handle 已打开的 VOCORE 句柄。
 * @param pixels 指向输入像素首字节，函数返回前会完成复制，调用方随后可复用该内存。
 * @param dataSize pixels 可读总字节数，至少为 stride*height。
 * @param width 输入画面宽度。旋转 0/180 时等于屏幕宽度，旋转 90/270 时等于屏幕高度。
 * @param height 输入画面高度。旋转 0/180 时等于屏幕高度，旋转 90/270 时等于屏幕宽度。
 * @param stride 每行字节数，允许包含行尾填充。
 * @param pixelFormat RB_VoCorePixelFormat 中的值。
 * @param rotation 顺时针旋转角度，只允许 0、90、180、270。
 * @return RB_OK 表示画面已进入最新帧队列；RB_INVALID_ARGUMENT 表示尺寸、格式或句柄无效；RB_LICENSE_REQUIRED 表示授权输出已锁定。
 */
RB_API int RB_VoCore_SubmitFrame(RB_VoCoreHandle handle, const void* pixels, int dataSize,
	int width, int height, int stride, int pixelFormat, int rotation);

/**
 * @brief 设置 VOCORE 背光亮度。
 * @param handle 已打开的 VOCORE 句柄。
 * @param brightness 亮度，范围 0-255；命令由发送线程异步执行。
 * @return RB_OK 表示设置已排队；RB_INVALID_ARGUMENT 表示句柄或亮度无效。
 */
RB_API int RB_VoCore_SetBrightness(RB_VoCoreHandle handle, int brightness);

/**
 * @brief 运行时启用或关闭设备自动重连。
 * @param handle 已打开的 VOCORE 句柄。
 * @param enabled 1=启用，0=关闭。
 * @return RB_OK 表示设置完成；RB_INVALID_ARGUMENT 表示句柄无效。
 */
RB_API int RB_VoCore_SetAutoReconnect(RB_VoCoreHandle handle, int enabled);

/**
 * @brief 读取 VOCORE 连接状态、实际 FPS 和帧统计。
 * @param handle 已打开的 VOCORE 句柄。
 * @param state 调用方结构体；调用前必须将 Size 设为 sizeof(RB_VoCoreState)。
 * @return RB_OK 表示成功；RB_INVALID_ARGUMENT 表示句柄、结构体或 Size 无效。
 */
RB_API int RB_VoCore_GetState(RB_VoCoreHandle handle, RB_VoCoreState* state);

/** @brief 启动浏览器仪表 HTTP 服务。 */
RB_API int RB_DashboardWeb_Open(const RB_DashboardWebConfig* config, RB_DashboardWebHandle* handle);

/** @brief 停止浏览器仪表服务并释放所有客户端连接。 */
RB_API void RB_DashboardWeb_Close(RB_DashboardWebHandle handle);

/** @brief 异步提交一帧 BGRA8888 画面；尚未编码的旧帧会被覆盖。 */
RB_API int RB_DashboardWeb_SubmitFrame(RB_DashboardWebHandle handle, const void* pixels, int dataSize,
	int width, int height, int stride, int pixelFormat, int rotation);

/** @brief 读取 Web 服务、客户端和编码统计。 */
RB_API int RB_DashboardWeb_GetState(RB_DashboardWebHandle handle, RB_DashboardWebState* state);

/**
 * @brief 使用SDK内置模板生成一帧BGRA8888仪表画面。
 * @param state RB_State_Read()读取到的当前运行态。
 * @param pixels 接收画面的调用方缓冲区，容量至少为stride*height字节。
 * @param width 输出画面宽度；竖屏通常为480，横屏通常为854。
 * @param height 输出画面高度；竖屏通常为854，横屏通常为480。
 * @param stride 每行字节数，不能小于width*4。
 * @param templateIndex RB_DashboardTemplate中的模板值。
 * @return RB_OK表示生成成功；RB_INVALID_ARGUMENT表示参数、尺寸或模板无效；RB_ERROR表示GDI资源创建失败。
 * @note 本函数只生成画面，不执行USB发送；生成结果可直接交给RB_VoCore_SubmitFrame()。
 */
RB_API int RB_Dashboard_RenderFrame(const RB_RuntimeState* state, void* pixels, int width, int height, int stride, int templateIndex);

/** @brief 重新扫描内置和用户 .rbdash 模板；返回模板数量或负数错误码。 */
RB_API int RB_Dashboard_RefreshPackages();

/** @brief 返回最近一次扫描得到的客户仪表模板数量。 */
RB_API int RB_Dashboard_GetPackageCount();

/** @brief 按目录索引读取客户仪表模板信息。 */
RB_API int RB_Dashboard_GetPackageInfo(int index, RB_DashboardPackageInfo* info);

/** @brief 按稳定模板 ID 打开客户仪表并创建缓存渲染句柄。 */
RB_API int RB_Dashboard_OpenPackage(const char* packageId, RB_DashboardPackageHandle* handle);

/** @brief 关闭客户仪表句柄并释放图片、字体和绑定缓存。 */
RB_API void RB_Dashboard_ClosePackage(RB_DashboardPackageHandle handle);

/** @brief 设置客户仪表当前页面。 */
RB_API int RB_Dashboard_SetPackagePage(RB_DashboardPackageHandle handle, int pageIndex);

/** @brief 使用客户仪表模板生成一帧 BGRA8888 画面。 */
RB_API int RB_Dashboard_RenderPackageFrame(RB_DashboardPackageHandle handle, const RB_RuntimeState* state,
	void* pixels, int width, int height, int stride);

/** @brief 读取客户仪表句柄状态和渲染统计。 */
RB_API int RB_Dashboard_GetPackageState(RB_DashboardPackageHandle handle, RB_DashboardPackageState* state);

/** @brief 将外部 .rbdash 文件导入当前 AppDataName 的用户仪表目录。 */
RB_API int RB_Dashboard_ImportPackage(const char* sourcePath, char* packageIdBuffer, int packageIdBufferSize);

/** @brief 删除当前 AppDataName 用户目录中的指定模板；内置模板不能删除。 */
RB_API int RB_Dashboard_DeletePackage(const char* packageId);

/** @brief 返回当前 AppDataName 的用户仪表目录 UTF-8 路径。 */
RB_API int RB_Dashboard_GetUserPackageDirectory(char* buffer, int bufferSize);



// 串口枚举接口。用于输出配置页、遥测配置页刷新 COM 口列表。

/**
 * @brief 枚举当前系统可用串口的数量。
 * @return 成功时返回大于或等于 0 的端口数量；内部失败时返回 RB_ERROR。
 */
RB_API int RB_Serial_GetAvailablePortCount();

/**
 * @brief 按枚举索引读取一个串口的完整设备信息。
 * @param index 端口索引，范围为 0 到 RB_Serial_GetAvailablePortCount()-1。
 * @param info 接收信息的结构体指针；SDK 会先清零整个结构体。
 * @return RB_OK 表示成功；RB_INVALID_ARGUMENT 表示 info 为空；RB_ERROR 表示索引无效或枚举失败。
 */
RB_API int RB_Serial_GetAvailablePortInfo(int index, RB_SerialPortInfo* info);

/**
 * @brief 获取 SDK 内部串口库版本文本。
 * @return 串口库持有的 NUL 结尾字符串；调用方不得修改或释放。
 */
RB_API const char* RB_Serial_GetVersion();

// 串口对象生命周期和连接。

/**
 * @brief 创建一个独立串口对象。
 * @return 成功时返回有效 RB_SerialHandle；内存分配或内部初始化失败时返回 nullptr。
 * @note 每个成功创建的句柄最终都必须交给 RB_Serial_Destroy()。
 */
RB_API RB_SerialHandle RB_Serial_Create();

/**
 * @brief 断开事件、关闭端口并销毁串口对象。
 * @param handle RB_Serial_Create() 返回的句柄；nullptr 会被忽略。
 * @return 无返回值。
 * @note 调用后句柄立即失效，不得再次使用；销毁时不得有并发读写或回调注册操作。
 */
RB_API void RB_Serial_Destroy(RB_SerialHandle handle);

/**
 * @brief 使用 8 数据位、无校验、1 停止位、无流控和 4096 字节读缓冲初始化串口参数。
 * @param handle 有效串口句柄。
 * @param portName UTF-8、NUL 结尾的端口名，例如 "COM3"。
 * @param baudRate 波特率，例如 115200，必须大于 0。
 * @return RB_OK 表示参数已设置；RB_INVALID_ARGUMENT 表示句柄或端口名无效；RB_ERROR 表示内部失败。
 * @note 本函数只配置对象，不会打开串口；随后调用 RB_Serial_Open()。
 */
RB_API int RB_Serial_Init(RB_SerialHandle handle, const char* portName, int baudRate);

/**
 * @brief 使用完整通信参数初始化串口对象。
 * @param handle 有效串口句柄。
 * @param portName UTF-8、NUL 结尾的端口名，例如 "COM3"。
 * @param baudRate 波特率，必须大于 0。
 * @param parity RB_SerialParity 中的值。
 * @param dataBits 数据位数，通常为 5、6、7 或 8。
 * @param stopBits RB_SerialStopBits 中的值。
 * @param flowControl RB_SerialFlowControl 中的值。
 * @param readBufferSize 异步模式内部读缓冲容量，单位为字节且必须大于 0。
 * @return RB_OK 表示参数已设置；RB_INVALID_ARGUMENT 表示句柄或端口名无效；RB_ERROR 表示参数设置失败。
 * @note 本函数只配置对象，不会打开串口；随后调用 RB_Serial_Open()。
 */
RB_API int RB_Serial_InitEx(RB_SerialHandle handle, const char* portName, int baudRate, int parity, int dataBits, int stopBits, int flowControl, unsigned int readBufferSize);

/**
 * @brief 按已初始化的参数打开串口。
 * @param handle 已通过 RB_Serial_Init() 或 RB_Serial_InitEx() 配置的有效句柄。
 * @return RB_OK 表示打开成功；RB_INVALID_ARGUMENT 表示句柄无效；RB_ERROR 表示设备打开失败。
 */
RB_API int RB_Serial_Open(RB_SerialHandle handle);

/**
 * @brief 关闭串口连接但保留对象及其配置。
 * @param handle 有效串口句柄；nullptr 会被忽略。
 * @return 无返回值。
 */
RB_API void RB_Serial_Close(RB_SerialHandle handle);

/**
 * @brief 查询串口当前是否已打开。
 * @param handle 有效串口句柄。
 * @return 1 表示已打开，0 表示未打开、句柄无效或内部失败。
 */
RB_API int RB_Serial_IsOpen(RB_SerialHandle handle);

// 串口读写。此组函数成功时返回字节数，失败时返回负数，不使用“0 表示成功”的 RB_Result 约定。

/**
 * @brief 向串口写入二进制数据。
 * @param handle 已打开的有效串口句柄。
 * @param data 待发送数据首地址。
 * @param size 请求写入的字节数，必须大于 0。
 * @return 大于或等于 0 表示实际写入字节数；负数表示参数、连接或写入失败，可调用 RB_Serial_GetLastError() 查询原因。
 */
RB_API int RB_Serial_Write(RB_SerialHandle handle, const void* data, int size);

/**
 * @brief 按当前超时配置读取最多 size 字节；异步模式下从内部队列读取并等待所需数据。
 * @param handle 已打开的有效串口句柄。
 * @param data 接收数据的缓冲区。
 * @param size 缓冲区容量及请求字节数，必须大于 0。
 * @return 大于或等于 0 表示实际读取字节数；负数表示参数、连接或读取失败。
 */
RB_API int RB_Serial_Read(RB_SerialHandle handle, void* data, int size);

/**
 * @brief 非强制满量读取当前可用数据，最多读取 size 字节。
 * @param handle 已打开的有效串口句柄。
 * @param data 接收数据的缓冲区。
 * @param size 缓冲区容量，必须大于 0。
 * @return 正数表示读取字节数，0 表示当前无数据，负数表示参数、连接或读取失败。
 */
RB_API int RB_Serial_ReadSome(RB_SerialHandle handle, void* data, int size);

/**
 * @brief 读取当前已经可用的全部数据，但最多写入 size 字节。
 * @param handle 已打开的有效串口句柄。
 * @param data 接收数据的缓冲区。
 * @param size 缓冲区容量，必须大于 0。
 * @return 正数表示读取字节数，0 表示当前无数据，负数表示参数、连接或读取失败。
 */
RB_API int RB_Serial_ReadAll(RB_SerialHandle handle, void* data, int size);

/**
 * @brief 读取到指定分隔符或缓冲区上限，并在结果末尾写入 NUL；分隔符不写入结果。
 * @param handle 已打开的有效串口句柄。
 * @param data 接收文本的缓冲区。
 * @param size 缓冲区总容量，单位为字节且必须大于 0，其中包含末尾 NUL。
 * @param delimiter 行结束分隔符，例如 '\n'。
 * @return 大于或等于 0 表示结果文本字节数，不包含 NUL；负数表示参数、连接或读取失败。
 */
RB_API int RB_Serial_ReadLine(RB_SerialHandle handle, void* data, int size, char delimiter);

/**
 * @brief 获取当前可立即读取的字节数；异步模式下返回内部队列长度。
 * @param handle 已打开的有效串口句柄。
 * @return 当前可读字节数；0 可能表示暂无数据，也可能表示句柄无效、端口未打开或查询失败。
 */
RB_API unsigned int RB_Serial_GetReadBufferUsedLen(RB_SerialHandle handle);

// 串口事件。

/**
 * @brief 注册串口可读事件回调。
 * @param handle 有效串口句柄。
 * @param callback 非空回调函数；回调可能运行在 SDK 工作线程中。
 * @param userData 调用方上下文指针，SDK 不解析并在回调时原样传回，可为 nullptr。
 * @return RB_OK 表示注册成功；RB_INVALID_ARGUMENT 表示句柄或回调为空；RB_ERROR 表示注册失败。
 * @note userData 指向的对象必须至少存活到断开回调或销毁串口对象。
 */
RB_API int RB_Serial_ConnectReadEvent(RB_SerialHandle handle, RB_SerialReadCallback callback, void* userData);

/**
 * @brief 断开串口可读事件回调并释放内部适配器。
 * @param handle 有效串口句柄。
 * @return RB_OK 表示断开成功；RB_INVALID_ARGUMENT 表示句柄无效；RB_ERROR 表示断开失败。
 */
RB_API int RB_Serial_DisconnectReadEvent(RB_SerialHandle handle);

/**
 * @brief 注册系统串口热插拔事件回调。
 * @param handle 有效串口句柄；热插拔监听与该对象生命周期绑定。
 * @param callback 非空回调函数；回调可能运行在 SDK 工作线程中。
 * @param userData 调用方上下文指针，SDK 不解析并在回调时原样传回，可为 nullptr。
 * @return RB_OK 表示注册成功；RB_INVALID_ARGUMENT 表示句柄或回调为空；RB_ERROR 表示注册失败。
 */
RB_API int RB_Serial_ConnectHotPlugEvent(RB_SerialHandle handle, RB_SerialHotPlugCallback callback, void* userData);

/**
 * @brief 断开系统串口热插拔事件回调并释放内部适配器。
 * @param handle 有效串口句柄。
 * @return RB_OK 表示断开成功；RB_INVALID_ARGUMENT 表示句柄无效；RB_ERROR 表示断开失败。
 */
RB_API int RB_Serial_DisconnectHotPlugEvent(RB_SerialHandle handle);

// 串口常用参数、重连和错误信息。

/**
 * @brief 设置串口同步或异步操作模式。
 * @param handle 有效串口句柄。
 * @param operateMode RB_SerialOperateMode 中的值。
 * @return RB_OK 表示设置成功；RB_INVALID_ARGUMENT 表示句柄无效；RB_ERROR 表示内部失败。
 */
RB_API int RB_Serial_SetOperateMode(RB_SerialHandle handle, int operateMode);

/**
 * @brief 启用或关闭串口断线自动重连。
 * @param handle 有效串口句柄。
 * @param enabled 0 表示关闭，非 0 表示启用。
 * @return RB_OK 表示设置成功；RB_INVALID_ARGUMENT 表示句柄无效；RB_ERROR 表示内部失败。
 */
RB_API int RB_Serial_SetAutoReconnectEnabled(RB_SerialHandle handle, int enabled);

/**
 * @brief 查询串口自动重连是否启用。
 * @param handle 有效串口句柄。
 * @return 1 表示启用，0 表示关闭、句柄无效或内部失败。
 */
RB_API int RB_Serial_IsAutoReconnectEnabled(RB_SerialHandle handle);

/**
 * @brief 设置自动重连尝试间隔。
 * @param handle 有效串口句柄。
 * @param milliseconds 重连间隔，单位毫秒。
 * @return RB_OK 表示设置成功；RB_INVALID_ARGUMENT 表示句柄无效；RB_ERROR 表示内部失败。
 */
RB_API int RB_Serial_SetAutoReconnectInterval(RB_SerialHandle handle, unsigned int milliseconds);

/**
 * @brief 获取自动重连尝试间隔。
 * @param handle 有效串口句柄。
 * @return 重连间隔，单位毫秒；句柄无效或内部失败时返回 0。
 */
RB_API unsigned int RB_Serial_GetAutoReconnectInterval(RB_SerialHandle handle);

/**
 * @brief 不等待下一次自动重连周期，立即执行一次重连尝试。
 * @param handle 有效串口句柄。
 * @return RB_OK 表示重连成功；RB_INVALID_ARGUMENT 表示句柄无效；RB_ERROR 表示重连失败。
 */
RB_API int RB_Serial_TryReconnectNow(RB_SerialHandle handle);

/**
 * @brief 设置串口读写超时参数。
 * @param handle 有效串口句柄。
 * @param config 非空超时配置指针，各字段语义与 Windows COMMTIMEOUTS 对应。
 * @return RB_OK 表示设置成功；RB_INVALID_ARGUMENT 表示句柄或 config 为空；RB_ERROR 表示内部失败。
 */
RB_API int RB_Serial_SetTimeoutConfig(RB_SerialHandle handle, const RB_SerialTimeoutConfig* config);

/**
 * @brief 读取串口当前读写超时参数。
 * @param handle 有效串口句柄。
 * @param config 接收超时配置的非空结构体指针。
 * @return RB_OK 表示读取成功；RB_INVALID_ARGUMENT 表示句柄或 config 为空；RB_ERROR 表示内部失败。
 */
RB_API int RB_Serial_GetTimeoutConfig(RB_SerialHandle handle, RB_SerialTimeoutConfig* config);

/**
 * @brief 设置串口 DTR 控制线状态。
 * @param handle 有效串口句柄。
 * @param set 0 表示清除 DTR，非 0 表示置位 DTR。
 * @return RB_OK 表示操作已提交；RB_INVALID_ARGUMENT 表示句柄无效；RB_ERROR 表示内部失败。
 */
RB_API int RB_Serial_SetDtr(RB_SerialHandle handle, int set);

/**
 * @brief 设置串口 RTS 控制线状态。
 * @param handle 有效串口句柄。
 * @param set 0 表示清除 RTS，非 0 表示置位 RTS。
 * @return RB_OK 表示操作已提交；RB_INVALID_ARGUMENT 表示句柄无效；RB_ERROR 表示内部失败。
 */
RB_API int RB_Serial_SetRts(RB_SerialHandle handle, int set);

/**
 * @brief 获取该串口对象最近一次底层操作的错误码。
 * @param handle 有效串口句柄。
 * @return 有效句柄时返回串口库错误码，0 通常表示无错误；句柄无效返回 RB_INVALID_ARGUMENT，内部异常返回 RB_ERROR。
 * @note 返回值属于串口库错误码域，不应按 RB_Result 解释。
 */
RB_API int RB_Serial_GetLastError(RB_SerialHandle handle);

/**
 * @brief 获取该串口对象最近一次底层操作的错误说明。
 * @param handle 有效串口句柄。
 * @param buffer 接收 UTF-8、NUL 结尾错误文本的调用方缓冲区。
 * @param bufferSize buffer 的总容量，单位为字节且必须大于 0。
 * @return RB_OK 表示读取成功；RB_INVALID_ARGUMENT 表示句柄或缓冲区无效；RB_BUFFER_TOO_SMALL 表示容量不足；RB_ERROR 表示内部失败。
 */
RB_API int RB_Serial_GetLastErrorMsg(RB_SerialHandle handle, char* buffer, int bufferSize);
