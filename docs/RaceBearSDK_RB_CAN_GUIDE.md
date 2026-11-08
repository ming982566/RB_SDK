# RaceBear SDK RB_CAN 接入指南

本文说明 Windows x64 前端如何通过 RaceBear SDK 管理 `RB_CAN` 输出和 RaceBear CANAPP 设备。接口以 `include/RaceBearSDK.h` 为准。
采用对象 ABI 时可通过 [同步服务目录](RaceBearSDK_SERVICE_API.md) 的 CAN/输出/安全带服务
调用本文相同业务；同步分派成功仍须检查业务返回值。维护、固件、UID、重新启用和授权规则不变，
不得因通用入口而跳过维护状态机或自动重试动作。

当前公开工作流覆盖 `ProtocolType == 1` 的 `RB_CAN`：

- 设备发现和身份读取；
- 逻辑索引设置；
- 输出实例与设备 UID 的稳定绑定；
- 绑定、运行、电机和健康状态；
- 用户确认安全后的受控恢复。
- 外部 Motion 宿主接管实时位置帧。
- 0.6.1 的电机参数维护、`.rbmotor` 参数包、同类型参数同步和 RB 伺服升级。

`FM_CAN`、`FC_CAN` 和 `RDS_CAN` 可通过类型化参数维护接口读取各自受支持的
参数表。`SEATBELT_CAN` 使用独立的 `RB_SeatbeltCan_*` 类型化接口，见本节下文；
底层 SDO/PDO、固件内部协议和急停控制不属于公开接入 API。
RB 伺服固件升级只适用于 RB 伺服，不适用于其它协议。

`SEATBELT_CAN` 是独立产品，不依赖 SDK 主授权。未授权时仍可应用安全带效果和
安全带 CAN 配置、连接专用输出、请求回零或重新启用；普通 `RB_CAN`、`FM_CAN`、
`FC_CAN` 和 `RDS_CAN` 平台输出仍受主授权限制。安全带的急停、释放、心跳、
USB 超时和固件保护边界保持不变。

### 专用安全带控制器

`SEATBELT_CAN` 的单/双路、节点和运行状态属于所选输出实例。单左/居中使用左节点，
单右使用右节点，未启用节点不访问。双路节点必须不同，不能复用平台HM51恢复流程。
专用安全带APP要求2026103102或更新版本；本轮SDK DLL与APP须配套更新。
长时间运行测试应使用APP `2026110102`或更新版本：该版本修正约71分钟微秒回绕
导致的失联/反馈误判，保护和节点监测统一使用连续毫秒；兼容最低版本门禁没有改变。
本次OUT残帧恢复需APP `2026110103`及配套SDK DLL：失败请求完成回收后，
SDK通过独立控制端点恢复接收边界并读取故障快照，旧APP仍走兼容路径。
这项传输恢复不会重新启用电机、清除保护锁或改变失联期限。

硬件约定（2026-10-31 用户确认）：RB_CAN 平台伺服电机与安全带步进电机的驱动器
使用同一份驱动固件，设置、参数和通信协议基本通用。该约定不表示两种 USB 控制器
使用同一个 APP，也不表示所有参数值、初始化流程或固件升级接口可互换。
安全带单/双路、释放、急停与恢复仍使用本节的专用流程，不能仅凭电机类型
推断其需要更快的反馈周期。

通过 `RB_Can_GetOutputBindingState` 读取安全带输出保存的UID、逻辑索引及目录身份；
公开 `RB_OutputConfig` 不包含 `CANUid` 字段。查询不创建平台CAN会话、不打开USB。
已绑定UID离线时保留原身份，不把同索引的其它设备当作目标。
绑定就绪只表示身份匹配；旧APP也可进入宿主维护流程，是否可以正常运行仍由
最低固件要求和 `RB_SeatbeltCan_GetState` 决定。安全带的释放和急停状态使用
安全带专用状态接口，不从平台逐卡急停确认字段推断。

安全带连接初始化期间会立即启动独立主机心跳，覆盖保护配置、PDO映射和NMT启动等
较慢的SDO流程；这些步骤不能被误判为主机失联。连接调用成功后仍需继续轮询状态，
直到 `Ready` 或明确故障。

安全带固件在主机完成保护配置后监护失联，在“模式已就绪”确认时检查有效坐标；
正常运行的电机状态由SDK复核，不因TPDO短暂停报直接触发固件锁定。
软件未启动、未连接或仅给电机供电时，不会因为尚未由 SDK 设置 TPDO 而触发保护。
SDK 配置状态 TPDO 时先切换到预操作、禁用旧
映射，再写入 `6041/603F/6064` 的完整 8 字节映射，最后恢复 COB-ID 并切回运行态。

当前安全带 TPDO1 为类型255，有效节点的事件定时器按300ms、310ms错开，
单路为300ms；事件周期不是实际发送频率的硬上限。SDK状态超过1000ms未更新时
用SDO复核，失败后每节点至少间隔200ms重试，连续失败至少3次且失败持续至少
1200ms才判离线。成功SDO或新TPDO重置失败窗口，SDO成功刷新状态有效期，
不会伪造真实TPDO时间。无有效缓存时也等待同一失败确认窗口。
运行监测连续三次读取固件释放状态失败会锁定主机输出，这是独立查询规则。

固件采用5000ms主机失联期限，独立心跳和已接受实时运动命令都超过期限才接管；
USB失联/挂起、急停和Bus-Off另行接管。RGB和普通诊断不续期运动主机存活时间。
释放期间仍要求真实TPDO在1000ms内有效，并检查回0、下使能确认；不能把
正常运行容错与释放到位证据混用。故障反馈不可用时不宣称主动回0成功。

SDK等待驱动既有上电传感器回零，再建立位置输出；保护触发后的释放使用绝对0，
不寻找传感器。固件独立处理主机心跳超时、USB挂起/复位和急停，确认到位后下使能。
电机进入保护锁定或初始化故障时只停止电机位置输出，安全带控制器上的 RGB 传输仍保持
独立；RGB 传输失败也不会重置电机模式。
故障轴不能回0时尝试停机并保留故障，另一轴独立处理；“锁定”不能当作释放成功。

运行中安全带停止而RGB继续显示时，应先读取安全带状态及运行日志，不能直接判定为
整张USB设备断开。`heartbeat paused: position input stale`表示计算输入停止，
`heartbeat enqueue failed`表示心跳未能提交，`heartbeat scheduler gap`表示心跳线程
调度延迟；`host heartbeat USB OUT failed`及`host heartbeat delivery gap`分别记录实际
USB写失败和成功写入之间的长间隔。保留这些日志供诊断，不在轮询回调里自动重新启用。
心跳线程使用可中断的高分辨率单次定时器按绝对截止时间维持约200ms周期，迟到时只发送
当前心跳，不补发历史周期；定时器不可用时退回可唤醒的条件变量等待。该机制减少正常
Windows调度抖动，但不把系统整体挂起变成实时保证。

安全带运行会话统一由SDK后台写线程执行USB OUT，释放/急停及心跳优先；
控制、状态、位置和RGB按有界队列调度，位置继续只保留最新目标。新协议RGB帧
到期时优先提交一次RGB帧，再处理同时排队的状态查询；这样状态查询的取消回收
不会把RGB灯效拖住数秒。RGB未到期时，旧协议RGB与状态查询继续公平轮转。
RGB数据帧本身是单向输出，正常运行不读取`RESP_SEATBELT_RGB_STATUS`。该回包只用于
异常诊断：SDK检测到RGB写失败或USB写线程停顿后，下一次安全带保护状态查询才会
附带一次诊断请求，并在短暂收尾等待后清除请求。诊断回包缺失只记录诊断信息，不影响
释放状态、电机输出或RGB调度；不要把RGB状态回包当作电机心跳或灯效逐帧确认。
安全带APP `2026110101` 或更新且报告对应能力时，RGB完整帧异步提交，无逐帧
应答等待；SDK仅在异常诊断请求时复核接收与完成灯带输出。旧APP仍由独立RGB任务等待原协议应答，
不占住USB写线程或电机监测线程。SDK成功缓存不等于灯带已显示。
安全带灯效最高60Hz，较低的配置帧率仍有效；独立RGB控制器使用自身独立节拍。
安全带输出实例`OutPutTime`按毫秒解释，生效范围2..1000ms：
0/1/2使用2ms（目标500Hz），4使用4ms（目标250Hz）。采样/断流检测仍每帧更新，
限速仅作用于实际发送；调度延迟不补发历史位置或颜色，不保证Windows绝对实时频率。
实际发送保持最小间隔，节拍等待使用每设备复用的高分辨率单次定时器，
不再将剩余微秒取整成毫秒；新目标、心跳、安全/控制请求和停止均可唤醒同一写线程。
唤醒后重查优先级与期限，不提前发送或积攒历史目标；系统不支持该定时器时退回
原条件变量等待。未增加发送线程、忙等或全局定时器分辨率设置。
高分辨率等待仍有系统唤醒延迟，配置500/250Hz是目标上限，不能当成实测频率。
计时基准在批次首包完成后续期，首包的写入/回收耗时不抵扣下一次间隔，
防止选中批次后被系统抢占造成相邻物理提交过近；这也使实测频率略低于目标上限。
排队期限、物理传输期限与应答期限分别计算，过期未发送请求主动撤销。
RGB日志的`win32`是本帧当前阶段的写入结果：1460表示排队期限到期，
121表示物理传输超时，170表示发送准入忙；应答超时可能在写入成功后发生，
不能将`win32=0`理解为整笔事务成功。Windows调度和取消完成没有绝对实时保证。
旧协议RGB事务失败只进行有限重试；新协议物理失败保留最新颜色供下一节拍重试，
普通RGB失败沿用有限重试；安全带USB短写超时先确认完成，仍在途时才恢复Bulk OUT端点，
新模式的接收/输出状态停滞和CRC/DMA异常仅记首次，
不改变电机状态；健康时不逐帧记录状态。
`Seatbelt USB writer stall`异常快照区分排队、物理写入、取消回收与管道恢复，
包含当前`phase`、命令和耗时。取消、Abort、Reset和完成回收分别计时：
phase 5/7/8/9对应这四个阶段，`cancel/abort/reset/drainMs`记录各调用耗时；
phase 10为取消后的完成等待，`cancelWaitMs`记录该等待耗时。
phase 11为新APP的独立接收边界恢复。`requested/reported`分别是提交字节数和
驱动报告值，只有`byteCountValid=1`时后者才是成功完成得到的确定字节数；
驱动错误时报告值可能无效，不能据此判定固件收到了多少数据。
安全带会话关闭WinUSB内部管道超时定时器，由SDK的4ms写入预算统一发起取消，
避免驱动超时和应用取消同时作用于同一请求；应用截止时间、停止与最终回收仍有效。
普通CAN/IAP的管道策略不变，不能据此理解成关闭了固件失联保护或允许无限排队。
安全带短写先按4ms应用期限记录超时，再给原请求120ms自然完成窗口；只有仍未完成
才取消请求，并以64ms为取消后的完成事件等待预算，通过非阻塞查询确认完成；
已完成时直接回收，不调用Abort/Reset，`ioCompleted=1`不代表传输成功或执行过重置。
事件已置位但查询仍报告未完成时，继续保留请求并执行终止OUT、重置OUT及最终回收。
驱动提交/完成已报告121时不再无条件恢复端点；连续超时计数也不会重新恢复同次已完成
请求。真正仍在途的请求保留端点恢复；关闭期间只取消回收，不再重置端点。
phase 12表示4ms期限后的自然完成窗口，`cancelWaitMs`记录该窗口或取消完成等待的实际耗时。
该处理不取消Bulk IN，也不放宽心跳或固件失联保护门限；仍须完成回收后才能复用
内存，Windows驱动调用没有硬实时保证，不能把4ms写预算理解为总恢复时限。
心跳、RGB和实时输出首次异常或长写停顿可触发一次固件诊断快照（需固件能力支持），
请求间隔至少5秒；丢失应答不永久关闭采样。该快照在恢复通信后取得，不是异常瞬间
的实时测量，累计计数及其差值应结合基线有效标志解读。
若设备身份/能力不支持快照、请求排队失败，或发起后1秒仍无应答，会输出对应的
`fault snapshot unavailable/enqueue failed/response missing`首次异常。
应答缺失只表示未取得诊断证据，不表示设备断开，也不自动解除保护或额外重置端点。

APP `2026110103`的故障恢复不依赖上述Bulk快照请求。失败OUT确实结束后，
SDK通过独立控制端点清理固件接收残帧和旧硬件接收包，再返回真实V4诊断。
若仅超过应用期限，但最终成功完成、有效字节数等于全部请求长度，且未调用过
Abort/Reset，则命令流完整，跳过此清理；仍返回原期限失败并保留迟到诊断，
不补发旧目标。成功但部分完成、失败完成，或动过端点后才完整完成，仍需恢复边界。
`boundary recovery snapshot transport=EP0`是故障现场的补充证据：
`droppedRxBytes`仅表示恢复时清理的固件残留字节，不能当成该主机请求的完整传输量。
正常输出不触发此请求或日志；故障过程仍按首次门控。
恢复快照正文可超过260字节。宿主必须按`RB_Log_Read`的容量不足合同扩容重试，
保留完整原文；不能把缓冲不足返回的空串视为没有日志或恢复没有执行。
完整读取示例见API手册“日志接入”。此修复恢复诊断证据，不代表原始USB停顿已解决。

当安全带短写请求确实仍未完成时，SDK不会让实时写线程同步等待
`WinUsb_AbortPipe`。该请求的OVERLAPPED、事件和发送缓冲区会移交给独立恢复线程；
恢复线程完成取消、Abort、Reset及最终回收后才释放它们。恢复期间新的位置、心跳、
状态和RGB OUT暂缓发送，恢复后只继续最新目标，不补发过期包。即使驱动的Abort
仍耗时数秒，这段时间也不会把实时写线程同步卡住。关闭会话时仍等待原OVERLAPPED
真正结束，不能提前释放请求资源。该机制不改变固件失联门限、心跳周期、RGB帧率
或公开API；恢复失败继续保持安全带保护和OUT边界锁定。
恢复能力按已验证的当前APP版本及安全带能力确认，不用可能沿用历史配置的
逻辑协议标签代替固件能力。旧APP、普通CAN和IAP不发送该恢复请求。
USB写入及排队异常附`appFirmware`、`identityProtocol`、`capabilities`、
`identityValid`和`boundaryRecovery`，分别表示缓存的实际固件版本、逻辑协议、
能力位、身份有效性及恢复路径是否可用；这些字段不是恢复已经成功的证明。
若恢复握手失败或应答校验不通过，SDK暂停新的Bulk命令，至少间隔500ms再尝试；
确认恢复后的过期目标也不补发。持续无法恢复时仍由原失联保护接管，
不以不断发送心跳掩盖运动断流。原4ms写期限、5000ms失联保护和显式重新启用权限不变。
控制请求的100ms等待预算也不保证Windows取消/最终回收在100ms内结束。
本版修复已复现的残帧失步；原始OUT超时和同步Abort长阻塞仍需测试机验收，
不能把出现成功恢复快照视为根因已消除。

上述CAN USB读写与安全带诊断同类异常只输出首次，不因帧号、累计计数或重试
持续变化反复输出；稳定正常至少5秒后静默重新允许下一次故障记录，严重升级
仍可单独记录。安全带上层不再重复输出USB累计失败计数，计数与最新目标重试
继续运行。RGB完整重试成功、正常释放和恢复成功不输出诊断。日志安静不代表
故障已恢复，应继续读取真实状态；此去重只影响日志，不改变任何保护时限或动作。

轮询 `RB_SeatbeltCan_GetState` 展示释放中与锁定；用户排障、解除急停后显式调用
`RB_SeatbeltCan_RequestReenable`，请求接受不代表已恢复。重连和RequestHoming
都不能解除故障保护。正常断开会后台等待释放，最长约16.5秒；确认所有有效节点
回0且下使能后，才由固件停止周期TPDO。释放未完成或故障时保留反馈与保护，
不因主动停报重新判为运行故障。仅干净的正常退出终态允许下次连接自动恢复；
SDK确认的节点故障按故障释放记录，不可冒充正常退出。状态不可用时保持诊断，
控制器复位后由用户显式断开重连。详见
[API安全带章节](RaceBearSDK_API.md#815-安全带参数)及前端指南。

安全带控制器APP/IAP升级由宿主内置维护功能提供；当前公开C头没有提供
安全带控制器刷写入口，第三方前端不能根据内部导出符号自行调用。
维护前断开全部输出，并等待安全带后台释放和USB关闭完成；升级不能通过
普通平台CAN会话初始化或重新使能电机。旧APP不满足正常运行的最低版本时，
仍可在断开状态升级。APP必须选择安全带专用镜像，IAP选择公共IAP镜像；
原始APP BIN的向量与范围校验不能替代业务类型清单。公共IAP恢复须由用户
明确选择UID，成功以同UID APP重新枚举为准，升级后不自动恢复运动输出。

## 1. 四种索引和一个稳定身份

RB_CAN 页面会使用以下几类标识：

| 标识 | 用途 | 能否持久化 |
|---|---|---|
| `outputIndex` | 当前 `RB_Output` 目录中的 `OUT_n` 零基索引 | 否，增删输出后重新取得 |
| `CatalogIndex` | 最近一次 CAN 设备刷新中的目录行号 | 否，下次刷新后失效 |
| `EnumerationIndex` | SDK 当前设备快照中的运行时访问索引 | 否，仅用于当前快照 |
| `LogicalIndex` | 写入 CANAPP 的逻辑索引，范围 `1-255` | 是，但多卡必须唯一 |
| `Uid` | CANAPP MCU 的 24 位大写十六进制 UID | 是，物理设备稳定身份 |

输出配置使用 `{Uid, LogicalIndex}` 稳定绑定物理设备；`CatalogIndex`、`EnumerationIndex` 和 USB 枚举顺序仅服务当前设备快照。

## 2. 初始化和线程边界

```cpp
if (RB_Runtime_SetAppDataName("YourProductName") != RB_OK)
    return false;
if (RB_Runtime_Initialize() != RB_OK)
    return false;
if (RB_Runtime_StartLoop(2) != RB_OK) {
    RB_Runtime_Shutdown();
    return false;
}
```

- SDK 初始化、输出连接和设备刷新与应用退出按生命周期顺序执行。
- UI 状态定时器可以读取绑定、健康和电机状态。
- USB 设备刷新适合在进入设备页、用户点击刷新或收到热插拔提示后执行，无需放入高频定时器。
- 设置索引和绑定由用户明确发起；失败时显示诊断并等待用户决定下一步。

## 3. 创建或定位 RB_CAN 输出

输出类型选择框由 SDK 目录填充，用户可以直接选择有效类型。

```cpp
int rbCanOutputIndex = -1;
const int outputCount = RB_Output_GetInstanceCount();
for (int i = 0; i < outputCount; ++i) {
    RB_OutputConfig config{};
    if (RB_Output_ReadInstanceConfigByIndex(i, &config) == RB_OK &&
        std::strcmp(config.OutputKind, "RB_CAN") == 0) {
        rbCanOutputIndex = i;
        break;
    }
}

if (rbCanOutputIndex < 0) {
    rbCanOutputIndex = RB_Output_AddInstance("RB_CAN");
}
```

保存 `outputIndex` 只适合当前输出目录快照。添加或删除任意输出后，应重新读取输出目录并重新定位目标实例。

## 4. 刷新和显示设备目录

```cpp
const int count = RB_Can_RefreshDevices();
if (count < 0) {
    // 显示刷新失败并读取 SDK 日志。
    return;
}

for (int i = 0; i < count; ++i) {
    RB_CanDeviceInfo info{};
    info.Size = sizeof(info);
    if (RB_Can_GetDeviceInfo(i, &info) != RB_OK)
        continue;

    if (info.ProtocolType != 1) {
        // 当前页面只接入 RB_CAN，忽略其它协议身份。
        continue;
    }

    // 显示 Uid、LogicalIndex、FirmwareVersion、IapMode、
    // IdentityValid、DuplicateLogicalIndex 和 StatusText。
}
```

`RB_Can_GetDeviceCount()` 返回最近一次刷新得到的数量，不重新扫描 USB。刷新后使用新的目录项，并按 UID 恢复选择。

设备只有满足下列条件时才适合绑定：

- `ProtocolType == 1`；
- `IapMode == 0`；
- `IdentityValid != 0`；
- `Uid` 非空；
- 目标逻辑索引没有被另一张设备占用。

`DuplicateLogicalIndex != 0` 不应让整个页面失去操作能力。用户仍需能够按 UID 选中其中一张设备，并为它分配新的唯一索引。

## 5. 单独修改设备逻辑索引

只修改设备索引、不绑定输出时使用：

```cpp
const char* uid = selectedDevice.Uid;
const int newLogicalIndex = 2;

const int rc = RB_Can_SetDeviceLogicalIndex(uid, newLogicalIndex);
if (rc == 0) {
    RB_Can_RefreshDevices();
}
```

调用前请确认：

1. 所有可能使用该设备的输出已经断开；
2. 新索引在当前在线设备中唯一；
3. 用户按 UID 确认了目标设备；
4. 当前没有退出、连接或其它维护动作并发执行。

返回 `0` 表示写入并读回成功。负数是设备维护错误码，与 `RB_Result` 的语义不同；前端可结合 SDK 日志和刷新后的 `StatusText` 给出具体提示。

## 6. 将输出稳定绑定到设备

通常应直接使用绑定接口。它把目标逻辑索引写入设备，并将稳定 UID 和逻辑索引持久化到对应输出配置。

```cpp
if (RB_Output_IsInstanceRuntimeActive(rbCanOutputIndex)) {
    // 等待输出完全停止后再允许绑定。
    return;
}

const int rc = RB_Can_BindOutputDevice(
    rbCanOutputIndex,
    selectedDevice.Uid,
    requestedLogicalIndex);

if (rc == 0) {
    RB_Can_RefreshDevices();

    RB_CanOutputBindingState binding{};
    binding.Size = sizeof(binding);
    if (RB_Can_GetOutputBindingState(rbCanOutputIndex, &binding) == RB_OK &&
        binding.BindingStatus == RB_CAN_BINDING_READY) {
        // 绑定确认完成。
    }
}
```

`RB_OutputConfig::CANindex` 是兼容字段；稳定 UID 绑定通过 `RB_Can_BindOutputDevice()` 完成。

多张设备拥有相同旧逻辑索引时，仍然按以下顺序处理：

1. 刷新设备目录；
2. 用户按 UID 选中目标设备；
3. 输入当前未被占用的新逻辑索引；
4. 调用 `RB_Can_BindOutputDevice()`；
5. 再次刷新并读取绑定状态。

整个过程无需拔掉另一张重复索引的设备；目标设备始终通过 UID 确认。

## 7. 绑定状态

```cpp
RB_CanOutputBindingState binding{};
binding.Size = sizeof(binding);
if (RB_Can_GetOutputBindingState(rbCanOutputIndex, &binding) == RB_OK) {
    switch (binding.BindingStatus) {
    case RB_CAN_BINDING_UNBOUND:
        break;
    case RB_CAN_BINDING_READY:
        break;
    case RB_CAN_BINDING_OFFLINE:
        break;
    case RB_CAN_BINDING_CONFLICT:
        break;
    case RB_CAN_BINDING_IAP:
        break;
    case RB_CAN_BINDING_ERROR:
        break;
    }
}
```

页面应同时显示：

- `Uid`、`LogicalIndex` 和 `ProtocolType`；
- `DeviceOpened` 和 `Ready`；
- `EStopLatched`、`EStopConfirmed` 和 `EStopCommandStatus`；
- `FirmwareVersion`、`Capabilities` 和 `StatusText`。

只有 `BindingStatus == RB_CAN_BINDING_READY`、协议身份为 `1` 且安全状态允许时，才向用户提供连接动作。最终连接许可仍由 SDK 判断。

## 8. 连接和断开

```cpp
const int connectRc = RB_Output_ConnectInstanceByIndex(rbCanOutputIndex);
if (connectRc != RB_OK && connectRc != RB_LICENSE_REQUIRED) {
    // 显示连接失败和 SDK 日志。
}

// 页面刷新只读取状态。
const int connected = RB_Output_IsInstanceConnected(rbCanOutputIndex);
const int active = RB_Output_IsInstanceRuntimeActive(rbCanOutputIndex);

// 用户断开。
RB_Output_DisconnectInstanceByIndex(rbCanOutputIndex);
```

连接调用成功表示流程已经启动，不保证电机立即进入运行。继续读取绑定、健康和电机状态，直到 SDK 报告就绪、运行或明确故障。

## 9. 读取单输出电机状态

```cpp
const int motorCount = RB_Can_GetOutputMotorCount(rbCanOutputIndex);
for (int i = 0; i < motorCount; ++i) {
    RB_CanMotorState motor{};
    if (RB_Can_GetOutputMotorState(rbCanOutputIndex, i, &motor) == RB_OK) {
        // 显示 NodeId、AxisKey、Online、Homed、Ready、Fault、
        // StatusWord、ErrorCode、Temperature 和 OperationMode。
    }
}
```

`motorIndex` 是当前输出会话中的行号，不是 CAN Node-ID。重连、重新绑定或设备拓扑变化后，重新取得电机数量并重建列表。

## 10. 健康诊断和人工恢复

```cpp
RB_CanOutputHealthState health{};
health.Size = sizeof(health);
if (RB_Can_GetOutputHealthState(rbCanOutputIndex, &health) == RB_OK) {
    // RuntimeStatus 和 FaultReason 用于机器判断。
    // StatusText、USB/CAN 计数和延迟用于诊断显示。
}
```

`RequiresHumanIntervention != 0` 时，前端停止自动重试并显示诊断。用户确认以下条件后即可使用恢复按钮：

- 物理急停已经释放；
- 设备 UID、逻辑索引和协议身份正确；
- USB/CAN 连接恢复；
- 电机和机械结构处于安全状态；
- 当前没有其它维护动作。

```cpp
const int rc = RB_Can_RequestOutputRecovery(rbCanOutputIndex);
```

`RB_OK` 表示 SDK 已接受一次恢复请求。页面继续轮询健康状态，恢复接口等待下一次用户明确操作。

## 11. 外部 Motion 宿主和 SimHub 中继

0.6.0 提供公开接口让 SimHub 等外部 Motion 宿主接管已有 `RB_CAN` 输出的实时位置提交。宿主不直接实现 CAN 协议，也不修改设备参数。

SimHub Generic UDP 推荐发送：

```text
RBH1,<Axis1a>,<Axis2a>,<Axis3a>,<Axis4a>
```

桥接程序应校验 magic、轴数量、解析结果和位置范围，然后启用接管并提交最新位置：

```cpp
int32_t positions[] = { 0, 21845, 43690, 65535 };
if (RB_Can_SetExternalRealtimeMode(outputIndex, 1) == RB_OK)
    RB_Can_SubmitPositions(outputIndex, positions, 4);
```

调用约束：

- `positionCount` 必须与该输出配置的 CAN 轴数量一致，外部宿主应以当前输出配置为准。
- 接管模式启用后，宿主必须持续提交最新位置；不要重复启用 SDK 内部位置模板。
- UDP 或上游 Motion 输入超时后，必须提交停车位置或断开输出，再关闭接管模式。
- 急停、Ready、设备绑定、授权、故障检测和人工恢复仍由 SDK 控制；该接口不能绕过安全状态机。
- 只支持公开 `RB_CAN` 运动输出，不支持 `SEATBELT_CAN` 或其它私有 CAN 协议。

退出或切回 SDK 内部位置计算前，应先停止外部输入，提交停车位置，断开或确认输出安全后调用：

```cpp
RB_Can_SetExternalRealtimeMode(outputIndex, 0);
```

## 12. 电机参数维护和 RB 伺服升级

参数维护必须由用户明确触发。前端先停止所有承载运动的输出，再进入维护：

```cpp
if (RB_Can_BeginMotorParameterMaintenance() == RB_OK) {
    // 读取当前电机目录和类型。
    // 需要时切换 500/1000 Kbit/s，SDK 会重开会话并重新进入维护。
    // 读取、写入、保存、导入/导出或同步参数。
    RB_Can_EndMotorParameterMaintenance();
}
```

进入维护后 SDK 会暂停自动初始化和监控，并安全下使能相关节点。参数页面应
使用 `RB_Can_GetMotorParameterCountForMotor()`、`RB_Can_GetMotorParameterProfileType()`
和 `RB_Can_IsMotorParameterWritable()` 动态建表，不把显示名称、界面行号或
聚合电机索引写入文件。`.rbmotor` 导入/导出只处理 UTF-8 文件，导入先生成
草稿；确认后再调用单项写入和保存。同步要求源、目标为同一类型且都在维护模式，
失败可能部分生效，页面必须提示用户重新读取确认。

维护波特率只支持 `500` 和 `1000`。切换时当前维护会话会短暂结束，SDK 按新
速率重新打开 CAN 卡并重新进入维护；超时或设备离线时，页面应显示失败并重新
读取状态，不要继续使用旧维护句柄。

RB 伺服固件升级使用：

```cpp
if (RB_Can_StartServoFirmwareUpdate(motorIndex, utf8BinPath) == RB_OK) {
    RB_RuntimeState runtime{};
    RB_State_Read(&runtime);
    while (runtime.Can.FirmwareUpdateRunning) {
        // 读取进度、状态文本和错误码，刷新 UI。
        RB_State_Read(&runtime);
    }
}
```

该接口只适用于已进入维护模式、输出未运行的 RB 伺服。升级期间不能切换设备、
退出维护或关闭 SDK；完成后检查 `FirmwareUpdateSuccess` 和错误码，再退出维护并
等待 CAN 会话重新初始化。固件文件兼容性和最终结果必须在目标硬件上验证。

## 13. 推荐页面状态机

1. 页面打开或热插拔提示后刷新设备目录。
2. 使用 UID 恢复设备选择，不保存目录行号。
3. 读取当前 RB_CAN 输出的绑定状态。
4. 未绑定或冲突时允许用户选择 UID 和唯一逻辑索引。
5. 绑定成功后刷新设备和绑定状态。
6. 用户明确点击连接后启动输出连接流程。
7. 运行期间只读取连接、绑定、电机和健康状态。
8. 故障需要人工处理时停止自动操作并显示恢复按钮。
9. 页面关闭不应断开正在运行的输出；应用退出必须执行安全断开。

## 14. 安全退出

```cpp
RB_Output_RequestDisconnectAll();

// 使用宿主定时器和有界等待，保持 UI 线程可响应。
for (int i = 0; i < 300 && !RB_Output_AreAllDisconnected(); ++i) {
    Sleep(10);
}

RB_Runtime_Shutdown();
```

退出期间停止设备刷新、绑定、恢复和其它新命令，再按 SDK 关闭流程完成断开和资源释放。

## 15. 最低验收清单

- 单张 RB_CAN 设备可以刷新并显示 UID、索引、固件和身份状态。
- 两张设备同时在线且旧逻辑索引相同时，可以按 UID 为其中一张设置新的唯一索引，不要求拔出另一张。
- 输出绑定后重启应用，仍能按 UID 和逻辑索引恢复绑定。
- 拔出和重新插入设备后，旧目录索引不会被继续使用。
- 连接、回零、运行、断开和退出状态能够完整显示。
- 设备离线、身份冲突、Bus-Off、节点离线、驱动故障和急停能够区分。
- 人工恢复只由用户触发，不会形成无限重试。
- 自动连接 CAN 输出的空闲下使能超时可读取、保存和关闭；到期后不再后台反复初始化。
- 参数维护能够安全进入和退出，波特率切换后仍能重新进入维护并回读参数。
- `.rbmotor` 导入失败不改变窗口草稿；同类型同步显示阶段、修改数和可能的部分生效。
- RB 伺服升级显示运行、完成、失败、进度和错误码，升级期间不能操作其它设备。
- 应用退出前全部输出完成安全断开。

## 16. 安全带 USB OUT 恢复边界

安全带设备的电机位置、RGB、心跳和状态查询共用 SDK 的单一 USB OUT 调度。
宿主只提交输出请求并读取状态，不要自行创建第二个 USB 发送线程，也不要在
请求超时后重放旧位置目标。

当一次 Bulk OUT 未在短写期限内完成时，SDK 会先取消并尝试 `AbortPipe`，再确认
OVERLAPPED 是否已经终结；只有请求仍在途时才进入 `ResetPipe` 兜底，最终完成
确认后才释放请求资源并继续调度。Abort和Reset都已在实机出现约2秒阻塞，
调整调用顺序并不保证消除卡顿；Reset仍保留为端点未终结时的兜底。日志中的单次写失败、心跳
间隔或RGB状态年龄不能单独证明控制器掉电、RGB计算错误或电机故障，页面仍应
根据连接状态、设备健康状态和安全带释放状态做判断。

组合传输可以减少同一端点上的调用次数，但不能替代上述恢复顺序；第三方前端
不需要新增协议或配置字段。

当SDK把未完成的Bulk OUT移交给后台恢复线程时，USB OUT会保持暂停，直到配套
安全带APP完成接收边界确认。原调用仍返回本次写入期限错误；恢复期间的新写入会
被拒绝，不能重放刚才失败的旧位置包。边界确认失败时继续保持锁定，需按设备健康
状态和安全带控制流程处理。

安全带 Bulk OUT 不设置 WinUSB 内部传输超时（值为0）。应用层仍按4ms判断实时
写入是否按期完成，并由SDK负责取消和回收在途请求。实机ETW证明20ms内部超时会
先触发xHCI停止端点，再放大到约2秒的端点恢复，因此不能重新启用；该策略由SDK
管理，第三方前端不需要配置。

当端点恢复调用本身迟迟不返回时，SDK还会在 `AbortPipe` 运行超过50ms或
`ResetPipe` 运行超过100ms后请求取消该同步调用；SDK仍会等待原调用真正结束，
不会在WinUSB仍使用句柄、事件或缓冲区时提前复用资源。该机制只属于SDK内部故障
恢复，第三方前端不需要新增线程、配置或重试逻辑。

异常日志中的 `abortWatchdog=次数/错误码` 和 `resetWatchdog=次数/错误码`
记录看门狗实际结果：`0/0` 表示未尝试，`0/非零` 表示初始化失败，
`非零/0` 只表示取消请求被接受，仍须结合原调用耗时判断是否及时结束；
`非零/非零` 表示最后一次取消失败。Abort/Reset原始错误码单独保留，
不能把取消结果当成端点已经恢复，也不能据此自动重新启用电机。看门狗仅用于
安全带会话，普通RB_CAN保持原恢复路径。正常运行不增加轮询或日志。
