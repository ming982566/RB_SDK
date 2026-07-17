#pragma once

// RaceBear SDK public C ABI.
// 公开接口只使用 C 基础类型和固定长度字符数组，避免 std::string/vector/bool 跨 DLL 边界导致 ABI 不稳定。
//
// Developer contract:
// 1. Include this header and link RaceBearSDK.lib, then ship RaceBearSDK.dll with the host app.
// 2. Call RB_Runtime_SetAppDataName() first if the host app does not want to use the default Racebear config directory.
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

// 内置配置文件类型。配置读写走 JSON，便于第三方前端保存完整参数。
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
	RB_SEATBELT_OUTPUT_COUNT = 2     // 安全带输出通道数。
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
	int Lateral;                                   // 横向几何/混合参数。
	int Longitudinal;                              // 纵向几何/混合参数。
	int Vertical;                                  // 垂向几何/混合参数。
	int DecelerationSpeed;                         // 平台回中/阻尼速度参数。
	int Percent;                                   // 姿态裁剪或缓冲比例。
	char OutKey[RB_TEXT_MEDIUM];                   // 平台输出 key，例如 A1-A4。
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
	int CANindex;                                  // CAN 设备零基索引；非 CAN 输出保留读取值，当前单设备通常为 0。
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
	int Sensitivity;                               // 范围0-100；GUARD安全区占有效输出范围的百分比，0关闭保护。
	int SensitivityStrength;                       // 范围0-100；GUARD保护强度，0关闭，仅Sensitivity>0时有效。
};

struct RB_DynamicInputEffect
{
	int Enabled;                                   // 1=启用该输入通道，0=保留配置但不参与计算。
	int InputType;                                 // SDK 内部输入模板类型；从现有配置读取并保留。
	int TelemetryIndex;                            // 当前运行时遥测索引；使用 RB_Telemetry_FindIndexByKey() 解析。
	char Key[RB_TEXT_SMALL];                       // 稳定通道效果 key，例如 Sway、YawRate、RoadImpact。
	RB_FilterEffect Filter;                        // 该输入对应滤波参数。
};

// 单个目标 DOF 可添加的输入通道效果。候选由 SDK 维护，前端不得自行列出全部遥测字段。
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
	int Version;                                   // 结构体版本，当前为 1。
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
 * @return SDK 持有的 UTF-8、NUL 结尾字符串，例如 "0.2.0-vs2019-x64"；调用方不得修改或释放。
 */
RB_API const char* RB_Runtime_GetVersion();

/**
 * @brief 设置配置目录使用的应用名称，省略调用时默认为 "Racebear"。
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
 * @brief 启动由 SDK MultimediaTimer 驱动的后端计算循环；已启动时会按新周期间隔重建循环。
 * @param intervalMs 计算周期间隔，单位毫秒；有效范围 1-1000 ms，推荐初始值为 2 ms。
 * @return RB_OK 表示启动成功；RB_INVALID_ARGUMENT 表示间隔不合法；RB_ERROR 表示未初始化或定时器启动失败。
 * @note 传入 2 表示约每 2 ms 计算一次；传入 1 表示约每 1 ms 计算一次。
 */
RB_API int RB_Runtime_StartLoop(int intervalMs);

/**
 * @brief 暂停已启动的内部计算循环，但保留定时器和当前频率。
 * @return RB_OK 表示已暂停；RB_ERROR 表示循环未启动或内部失败。
 */
RB_API int RB_Runtime_PauseLoop();

/**
 * @brief 恢复已暂停的内部计算循环。
 * @return RB_OK 表示已恢复；RB_ERROR 表示循环未启动或内部失败。
 */
RB_API int RB_Runtime_ResumeLoop();

/**
 * @brief 停止并释放内部计算定时器；未启动时调用也视为成功。
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
 * @brief 查询内部计算定时器是否已经启动。
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
 * @note 返回索引只用于当前进程调用，不得写入配置文件；持久化必须保存telemetryKey。
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

/**
 * @brief 设置当前选中的平台状态缓存。
 */
RB_API int RB_Platform_SetSelectedState(int platformIndex, const RB_PlatformConfig* config);

/**
 * @brief 将平台配置应用到当前运行中的平台模型。
 */
RB_API int RB_Platform_ApplyRuntimeConfigToCurrentRig(const RB_PlatformConfig* config);

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
 * @note 编辑页面必须先读取完整结构体，再修改目标字段，不能从空结构体直接保存。
 */
RB_API int RB_Output_ReadInstanceConfigByIndex(int instanceIndex, RB_OutputConfig* config);

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
 * @param outputKind "Serial"、"UDP"、"MMF" 或 "CAN"。
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
 * @note 前端应显示 Items[].Key，只允许添加目录中的候选，并通过 Key 防止同一 DOF 重复添加。
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
