# RaceBear SDK 第三方前端实现指南

本文用于指导第三方开发者从零实现一个完整的 RaceBear 前端。调用方只需要
`RaceBearSDK.h`、对应配置的 `RaceBearSDK.lib`、`RaceBearSDK.dll` 和本文，
不需要阅读 SDK 源码。

字段定义、固定数组容量和每个导出函数的参数说明以
`include/RaceBearSDK.h` 为准；完整参数表和协议说明见
`RaceBearSDK_API.md`。本文重点说明如何把这些接口正确组合成可用软件。

## 1. 前端与 SDK 的职责边界

SDK 负责：

- 游戏遥测源、运动计算、功能效果和输出运行态。
- 后端计算定时器、内部互斥和线程安全。
- 配置读取、校验、持久化和运行态应用。
- 授权判断与真实输出锁定。
- 输出、串口、插件和自定义游戏的后端能力。

前端负责：

- 页面、控件、提示、多语言和绘制。
- 低频目录刷新和配置编辑。
- 从 `RB_RuntimeState` 刷新状态看板。
- 把用户动作转换为 SDK 调用，并显示返回结果。
- 正确执行测试归零、设备断开和退出顺序。

前端不得自行创建第二套运动计算循环，也不得在多个页面重复初始化 SDK。

## 2. 最小生命周期

```cpp
int StartBackend()
{
    // 必须在 Initialize 之前调用；参数是目录名称，不是完整路径。
    int rc = RB_Runtime_SetAppDataName("YourProductName");
    if (rc != RB_OK)
        return rc;

    rc = RB_Runtime_Initialize();
    if (rc != RB_OK)
        return rc;

    // 单位是毫秒。2 ms 约等于 500 Hz，1 ms 约等于 1000 Hz。
    rc = RB_Runtime_StartLoop(2);
    if (rc != RB_OK)
        RB_Runtime_Shutdown();
    return rc;
}
```

正常运行期间：

- SDK 内部循环由 `RB_Runtime_StartLoop()` 驱动。
- UI 不得周期调用 `RB_Runtime_DoCalculations()`。
- UI 定时器只读取状态，推荐 30-60 Hz。
- 日志定时器推荐 100-500 ms 排空一次。
- 游戏、平台、效果和输出目录只在页面打开或用户刷新时读取。

退出顺序：

```cpp
void StopBackend()
{
    // 先停止宿主定时器和线程，禁止产生新的 SDK 调用。
    RB_ManualPose_ResetDrive();
    RB_ManualPose_SetTestEnabled(0);
    RB_Wind_SetTestOutput(0, 0.0);
    RB_Seatbelt_SetTestOutput(0, 0.0, 0.0);

    RB_Output_RequestDisconnectAll();
    for (int i = 0; i < 100 && !RB_Output_AreAllDisconnected(); ++i)
        Sleep(10);

    RB_Debug_CloseMonitor();
    RB_Runtime_Shutdown();
}
```

串口回调、宿主线程和 UI 定时器必须在 `RB_Runtime_Shutdown()` 前停止。关闭 SDK
后不得再调用任何 SDK 函数。

## 3. 必须先理解的配置关系

不同配置类型不是一套索引，也不能因为名称相同就混用索引。

| 配置类型 | 存储方式 | 目录来源 | 是否独立 | 正确读写方式 |
| --- | --- | --- | --- | --- |
| 系统设置 | 全局单配置 | 无目录 | 是 | `RB_System_ReadConfigOrDefault/SaveConfig` |
| 游戏遥测配置 | 每游戏一份 | `RB_Game_GetCatalog` | 是 | `RB_Game_ReadConfig/SaveConfig` |
| 平台配置 | 平台目录 | `RB_Platform_GetCount/GetName` | 是 | 平台索引只能来自平台目录 |
| 动态滤波 | 命名配置目录 | `RB_Dynamic_GetProfileCount/GetProfileName` | 是 | 动态索引只能来自动态目录 |
| 运动增强 | 命名配置目录 | `RB_MotionEffect_GetProfileCount/GetProfileName` | 是 | 运动增强索引只能来自运动增强目录 |
| 震动 | 按名称保存 | 当前 `ByIndex` 接口按动态配置名解析 | 内容独立，目录暂时关联动态滤波 | 使用动态目录索引，或直接按名称读写 |
| 风感 | 全局单配置 | 无目录 | 是 | `RB_Wind_ReadConfig/SaveConfig` |
| 安全带 | 全局单配置 | 无目录 | 是 | `RB_Seatbelt_ReadConfig/SaveConfig` |
| 输出 | 多实例目录 | `RB_Output_GetCatalog` | 是 | 实例索引来自最新输出目录 |

### 3.1 运动增强不是动态滤波的子配置

运动增强拥有独立目录。正确枚举方式：

```cpp
const int count = RB_MotionEffect_GetProfileCount();
for (int i = 0; i < count; ++i) {
    char name[RB_TEXT_MEDIUM]{};
    if (RB_MotionEffect_GetProfileName(i, name, sizeof(name)) == RB_OK) {
        // UI 显示 name，并把 i 保存为 motionProfileIndex。
    }
}
```

不得使用 `RB_Dynamic_GetProfileCount()` 填充运动增强配置选择框。即使两个目录
当前具有相同名称，它们的数量、顺序和生命周期也可能不同。

### 3.2 震动目录的当前限制

当前 SDK 没有公开独立的震动配置目录枚举和删除接口：

- `RB_Haptic_ReadProfileByIndex(dynamicProfileIndex, ...)` 会取得同名震动配置。
- `RB_Haptic_SaveProfileByIndex(dynamicProfileIndex, ...)` 保存到该动态配置同名节点。
- 也可以取得动态配置名后，调用 `RB_Haptic_ReadProfile(name, ...)` 和
  `RB_Haptic_SaveProfile(name, ...)`。
- 震动结构体与动态滤波结构体完全独立，只是配置名称目录暂时关联。

前端必须把这个限制明确封装在震动页面中，不能把运动增强也错误地套用同一规则。

### 3.3 风感和安全带不是配置目录

风感和安全带当前各只有一份全局配置。前端不应伪造配置方案选择框。页面只需要
“读取、应用、保存、测试、停止测试”。

## 4. 通用配置页面模板

每个配置页面都遵循同一个事务流程：

1. 从对应 SDK 目录取得索引和名称。
2. 读取完整结构体。
3. UI 只修改用户可见字段，保留其它字段。
4. `Apply` 只更新运行态，不表示已经保存。
5. `Save` 持久化完整结构体。
6. 保存成功后再次读取，并用 SDK 回读结果刷新 UI。

```cpp
RB_OutputConfig config{};
int rc = RB_Output_ReadInstanceConfigByIndex(index, &config);
if (rc != RB_OK)
    return rc;

// 修改 config 中用户编辑的字段。

rc = RB_Output_SaveInstanceConfigByIndex(index, &config);
if (rc != RB_OK)
    return rc;

RB_OutputConfig saved{};
rc = RB_Output_ReadInstanceConfigByIndex(index, &saved);
if (rc == RB_OK)
    config = saved; // SDK 回读结构是新的事实来源。
return rc;
```

不要对整个结构体执行 `memcmp`。结构体包含保留字段、固定字符串剩余空间和 SDK
规范化字段；字节不同不代表保存失败。保存接口和回读接口都返回 `RB_OK` 即表示
该保存流程成功。

## 5. 总览和状态看板

高频状态统一从一个结构体读取：

```cpp
RB_RuntimeState state{};
if (RB_State_Read(&state) == RB_OK) {
    // state.License  授权状态
    // state.Game     当前游戏
    // state.Source   遥测源状态和最终车辆值
    // state.Rig      平台与六自由度结果
    // state.Output   输出摘要
    // state.Wind     风感状态
    // state.Seatbelt 安全带状态
    // state.Can      CAN 摘要
}
```

常用单位：

| 字段 | SDK 单位 | UI 常见显示 |
| --- | --- | --- |
| `Source.FinalSpeed` | m/s | 乘 3.6 显示 km/h |
| `Source.FinalRPM` | rpm | rpm |
| `Source.TransitionPercent` | 0-1 | 乘 100 显示百分比 |
| `Rig.MotionState` 线性自由度 | mm | mm |
| Yaw/Roll/Pitch | 度 | 度 |
| 授权剩余时间 | 秒 | 天、小时、分钟 |

`RB_State_Read()` 是状态快照，不会代替目录和完整配置接口。

## 6. 授权页面

显示只读取 `state.License`。操作统一使用命令：

```cpp
RB_Command_Run("license.check", "{}", result, resultSize);
RB_Command_Run("license.activate",
               "{\"activationCode\":\"用户输入的激活码\"}",
               result, resultSize);
RB_Command_Run("license.deactivate", "{}", result, resultSize);
```

未授权时不要强制关闭宿主程序。配置、目录、原始遥测、状态和激活页面仍可用；SDK 会停止专有计算并安全断开真实输出。

前端必须按下面规则处理锁定状态：

- `state.License.OutputAllowed == 0` 时禁用 Apply、测试启用和连接按钮，但保留保存、复位和断开按钮。
- `RB_State_Read()` 仍然成功，受保护的最终遥测、6DOF、执行器和附加效果结果为 0。
- 运行态 Apply、测试启用或输出连接返回 `RB_LICENSE_REQUIRED` 时，显示授权提示，不要重试循环调用。
- `RB_Telemetry_GetRawValue()` 可继续用于遥测诊断；`RB_Telemetry_GetProcessedValue()` 在锁定状态返回 0。
- 授权恢复由 SDK 自动从零淡入，前端不需要重建第二套过渡。
所有真实连接和手动测试按钮还应检查 `state.License.OutputAllowed`。

## 7. 游戏和遥测页面

### 7.1 游戏目录与稳定键

```cpp
RB_GameCatalog games{};
RB_Game_GetCatalog(&games);
```

- UI 显示 `Items[i].GameName`。
- 内部保存 `Items[i].GameKey`。
- `CatalogIndex` 只用于需要目录索引的调用，不等同于安装列表索引。
- 列表刷新后旧索引不可跨快照长期保存。

选择游戏：

```cpp
RB_Game_SelectByKey(selected.GameKey);
```

保存按钮必须先确认当前有非空 `GameKey`，不能把空字符串传给配置接口。

### 7.2 游戏配置

先读后改：

```cpp
RB_GameConfig config{};
int rc = RB_Game_ReadConfig(selected.GameKey, &config);
if (rc == RB_OK) {
    config.SourcePort = 9996;
    strcpy_s(config.SourceIP, sizeof(config.SourceIP), "127.0.0.1");
    rc = RB_Game_SaveConfig(selected.GameKey, &config);
}
```

新的 AppData 目录中某个游戏可能尚无独立配置。此时前端应显示“尚未创建配置”，
并在用户保存前完整初始化所有可写字段；不得把未初始化内存传给 SDK。

安装扫描使用：

- `RB_Game_RefreshInstallations()`
- `RB_Game_GetInstallInfo()`
- `RB_Game_FindInstallByKey()`
- `RB_Game_FindInstallPathByKey()`

启动、结束和自动检测使用命令：`game.launch`、`game.terminate`、
`game.autoDetect`。自动配置、插件安装和游戏侧配置检查使用对应的 `RB_Game_*`
结构体接口。

### 7.3 遥测目录

```cpp
RB_TelemetryCatalog telemetry{};
RB_Telemetry_GetCatalog(&telemetry);
```

持久化和协议使用 `Items[i].Key`；`Items[i].Index` 只在当前 SDK 进程有效。
公共目录会跳过 SDK 内部保留和历史兼容槽位，因此 `Items[i].Index` 不保证等于
数组行号 `i`。读取数值和提交外部遥测时必须使用结构体返回的 `Index`，不能自行
按 `0..Count-1` 生成索引。
调试页面可以用 `RB_Telemetry_GetRawValue()` 和
`RB_Telemetry_GetProcessedValue()` 显示当前值。

## 8. 平台页面

平台选择框来自：

- `RB_Platform_GetCount()`
- `RB_Platform_GetName()`
- `RB_Platform_ReadSelectedIndex()`

读取 `RB_PlatformConfig` 后，使用
`RB_Platform_GetGeometryParameterName()` 和
`RB_Platform_GetStrokeParameterName()` 决定 L1-L10 中哪些字段显示。返回空名称的
字段应隐藏或禁用，不能猜测其含义。

保存平台配置的推荐顺序：

```cpp
int rc = RB_Platform_SaveConfigByIndex(platformIndex, &config);
if (rc == RB_OK)
    rc = RB_Platform_SaveSelectedIndex(platformIndex);
if (rc == RB_OK)
    rc = RB_Platform_SetSelectedState(platformIndex, &config);
if (rc == RB_OK)
    rc = RB_Platform_ApplyRuntimeConfigToCurrentRig(&config);
```

切换平台类型前先断开输出。预览和手动测试范围来自：

- `RB_Platform_IsDofSupported()`
- `RB_Platform_IsPreviewPosePossible()`
- `RB_Platform_GetPreviewPoseLimit()`

离开页面和退出程序时必须复位手动姿态并关闭测试模式。

## 9. 动态滤波页面

动态配置选择框只能使用动态目录。每个自由度的可添加通道只能来自
`RB_Dynamic_GetInputCandidateCatalog(dofIndex, ...)`，不能把完整遥测目录直接
展示成效果列表。

添加通道：

```cpp
RB_DynamicInputCandidateCatalog candidates{};
RB_Dynamic_GetInputCandidateCatalog(dofIndex, &candidates);

RB_DynamicV2Profile profile{};
RB_Dynamic_ReadProfileByIndex(dynamicProfileIndex, &profile);

RB_DynamicDofEffect& dof = profile.Dofs[dofIndex];
const RB_DynamicInputCandidateItem& source = candidates.Items[candidateIndex];
RB_DynamicInputEffect& input = dof.Inputs[dof.InputCount++];
input = {};
input.Enabled = 1;
input.InputType = source.InputType;
input.TelemetryIndex = source.TelemetryIndex;
strcpy_s(input.Key, sizeof(input.Key), source.Key);
input.Filter.Enabled = 1;
input.Filter.Dof = input.InputType;
input.Filter.InMapping = 20;
input.Filter.InGain = 1.0;
input.Filter.OutScaling = 100;
dof.Enabled = 1;
```

删除通道后必须移动后续项、减少 `InputCount` 并清零最后一个槽位。每个通道的
`RB_FilterEffect` 参数互相独立。

滤波参数控件不能全部使用同一种范围：

| 字段 | SDK 接受范围 | 推荐控件 |
| ---- | ------------ | -------- |
| `InMapping` | 非负 `int`；启用通道时必须大于 `0` | 非负整数输入框；单位显示候选遥测的 `Unit`，不设置统一最大值 |
| `InGain` | 任意有限 `double`；`0` 不产生有效输出 | 支持正负号和小数的输入框；常用值为 `1.0` 和 `-1.0` |
| `OutScaling`、`Proportion`、`Strength`、`Smoothing`、`DeadZone`、`Washout`、`Sensitivity`、`SensitivityStrength` | 整数 `0-100` | 范围固定为 `0-100` 的滑杆或步进框 |

其中 `OutScaling`、`Proportion`、`DeadZone`、`Sensitivity` 使用百分比语义；
`Strength`、`Smoothing`、`Washout` 和 `SensitivityStrength` 是 `0-100` 的 UI 标度，
不能标成毫秒或 Hz。各字段的精确换算、0 值行为和生效条件见
`RaceBearSDK_API.md` 的“动态输入和滤波字段”表。

`RB_Dynamic_SaveCurrentGameProfileIndex()` 只绑定动态配置。它不会把运动增强目录
索引自动变成同一个含义。

## 10. 运动增强页面

页面需要两个目录：

- 配置方案：`RB_MotionEffect_GetProfileCount/GetProfileName`
- 效果类型：`RB_MotionEffect_GetCatalog`

先按运动增强索引读取完整 `RB_MotionEffectProfile`。每个效果可以包含多个
`Routes`，每条路由有自己独立的输出自由度、比例、方向、阈值、频率和持续时间。

```cpp
RB_MotionEffectProfile profile{};
RB_MotionEffect_ReadProfileByIndex(motionProfileIndex, &profile);

RB_MotionEffectConfig& effect = profile.Effects[effectIndex];
effect.Enabled = 1;
effect.Gain = 1.0;

RB_MotionEffectOutputRoute& route = effect.Routes[routeIndex];
route.Enabled = 1;
route.Dof = 2;
route.OutputRatio = 0.5;
route.Direction = 1.0;
route.Threshold = 20.0;
route.Frequency = 25.0;
route.Duration = 0.08;

RB_MotionEffect_ApplyProfileToCurrentRig(&profile);
RB_MotionEffect_SaveProfileByIndex(motionProfileIndex, &profile);
```

创建独立方案使用 `RB_MotionEffect_SaveProfile(name, ...)`；删除使用
`RB_MotionEffect_DeleteProfile(name)`。默认方案通常不应允许用户删除。

## 11. 震动页面

效果目录来自 `RB_Haptic_GetCatalog()`。`Items[i].Index` 用于访问
`RB_HapticEffectProfile::Effects`。

当前配置方案名称与动态目录关联，因此页面应先取得动态配置名，再按名称读取和
保存震动结构体。不要使用运动增强索引。

```cpp
char profileName[RB_TEXT_MEDIUM]{};
RB_Dynamic_GetProfileName(dynamicProfileIndex, profileName, sizeof(profileName));

RB_HapticEffectProfile profile{};
RB_Haptic_ReadProfile(profileName, &profile);

RB_HapticEffectConfig& effect = profile.Effects[effectIndex];
effect.Enabled = 1;
effect.OutputChannel = 0; // Haptic1
effect.Gain = 1.0;

RB_Haptic_ApplyProfileToCurrentFunction(&profile);
RB_Haptic_SaveProfile(profileName, &profile);
```

`OutputChannel` 范围是 0-6，对应 Haptic1-Haptic7。恢复单个效果默认值时使用
`RB_Haptic_GetDefaultConfig()`。

## 12. 风感和安全带页面

两者都是全局单配置：

```cpp
RB_WindEffectConfig wind{};
if (RB_Wind_ReadConfig(&wind) != RB_OK)
    RB_Wind_GetDefaultConfig(&wind);
// 修改字段
RB_Wind_ApplyConfigToCurrentFunction(&wind);
RB_Wind_SaveConfig(&wind);
```

安全带使用同样的 `GetDefault/Read/Apply/Save` 流程。测试输出只在用户明确启用时
调用；松开按钮、切换页面和退出程序时固定归零。

## 13. 输出页面

先调用 `RB_Output_EnsureDefaultConfig()`，再读取 `RB_Output_GetCatalog()`。

目录提供：

- 当前实例及连接状态。
- 当前可插入命令模板的输出 Key。
- 输出实例索引。

输出类型选择框来自 `RB_Output_GetKindCount/GetKind`。新增实例必须传目录返回的
类型原文，不能传本地翻译文本。

连接参数修改流程：

1. 判断 `RB_Output_IsInstanceConnected(index)`。
2. 已连接则先断开。
3. 读取完整 `RB_OutputConfig`。
4. 修改端口、IP、波特率、命令模板等字段。
5. 保存并回读。
6. 调用 Apply 更新运行态。
7. 只有用户要求连接或 AutoConnect 策略允许时才重新连接。

输出 Key 必须从最新目录读取。`A1-A6` 是实际轴位输出，`Wind1-Wind4`、
`SeatbeltLeft/Right`、`Haptic1-Haptic7` 是功能输出，六个大写 DOF Key 是
平台处理后的六自由度值。Key 区分大小写。

## 14. 自定义游戏和外部数据源

宿主已经采集到原始遥测并直接提交时，使用：

- `RB_ExternalSource_Open()`
- `RB_ExternalSource_SubmitFrame()`
- `RB_ExternalSource_GetState()`
- `RB_ExternalSource_Close()`

每帧的运行时索引必须在当前进程中用 `RB_Telemetry_FindIndexByKey()` 解析。

需要注册成正式游戏、支持进程检测并通过 MMF/UDP 接收时，使用：

- `RB_Game_RegisterCustom()`
- `RB_Game_GetCustomState()`
- `RB_Game_UnregisterCustom()`
- `RB_Game_ReloadCustomRegistry()`

线协议使用 `RB_Telemetry_FindFieldIdByKey()` 返回的 FieldId，不能使用运行时索引。
完整 MMF/UDP 包结构见 `RaceBearSDK_API.md`。

## 15. 串口页面

基本流程：枚举、创建、初始化、注册回调、打开、读写、断开回调、关闭、销毁。

```cpp
RB_SerialPortInfo port{};
if (RB_Serial_GetAvailablePortInfo(selectedPortIndex, &port) != RB_OK)
    return;

RB_SerialHandle handle = RB_Serial_Create();
if (!handle)
    return;

int rc = RB_Serial_InitEx(
    handle,
    port.PortName,
    115200,
    RB_SERIAL_PARITY_NONE,
    8,
    RB_SERIAL_STOP_ONE,
    RB_SERIAL_FLOW_NONE,
    8192);

if (rc == RB_OK)
    rc = RB_Serial_SetOperateMode(handle, RB_SERIAL_ASYNC);
if (rc == RB_OK)
    rc = RB_Serial_ConnectReadEvent(handle, OnSerialRead, userData);
if (rc == RB_OK)
    rc = RB_Serial_ConnectHotPlugEvent(handle, OnSerialHotPlug, userData);
if (rc == RB_OK)
    rc = RB_Serial_Open(handle);

// 发送成功返回实际字节数，不是固定返回 RB_OK。
const char command[] = "test";
const int written = RB_Serial_Write(handle, command, sizeof(command) - 1);

RB_Serial_DisconnectReadEvent(handle);
RB_Serial_DisconnectHotPlugEvent(handle);
RB_Serial_Close(handle);
RB_Serial_Destroy(handle);
```

同步模式下由宿主定时读取；异步模式下回调只通知“现在有数据可读”，随后调用
`RB_Serial_ReadSome/ReadAll/ReadLine`。读写函数成功时通常返回字节数，调用方不能
只用 `result == RB_OK` 判断发送是否成功。

串口回调可能在 SDK 工作线程触发。回调中只复制数据或设置线程安全标志，不得直接
操作 UI 控件。窗口销毁前必须先断开回调，再销毁句柄。

## 16. 插件、日志和诊断

插件目录需要先刷新：

```cpp
if (RB_Plugin_Refresh() >= 0) {
    RB_PluginCatalog plugins{};
    RB_Plugin_GetCatalog(&plugins);
}
```

`RB_Plugin_StartRuntimes()` 启动后台运行态；`RB_Plugin_Launch()` 启动插件自己的
前端。每次刷新后旧插件索引都应视为失效。

日志循环读取到空字符串：

```cpp
char line[4096]{};
while (RB_Log_Read(line, sizeof(line)) == RB_OK && line[0] != '\0') {
    // 显示或写入宿主日志文件。
}
```

SDK 资源监视器可通过 `RB_Debug_OpenMonitor()` 打开，只用于诊断，不替代宿主前端。

## 17. 命令和 JSON 的使用边界

动作使用 `RB_Command_Run()`；结构化配置优先使用类型化结构体接口。无参数命令
仍传入 `{}`。命令结果必须同时检查返回码和 JSON 内容。

`RB_Data_ReadJson/WriteJson` 适用于脚本、迁移工具和高级编辑器。普通配置页面
不应要求最终用户编辑 JSON，也不能写入局部 JSON 片段覆盖完整配置。

## 18. 最低页面验收清单

一个可发布前端至少满足：

- SDK 初始化、内部循环、状态刷新和正常退出完整。
- 授权状态、激活、反激活和手动检查可用。
- 游戏目录、安装扫描、选择、启动、停止和遥测配置可用。
- 平台目录、参数、选择、保存、应用和受限手动测试可用。
- 动态滤波按自由度候选目录增删通道，并保存每通道参数。
- 运动增强使用自己的配置目录，并支持每效果多路由参数。
- 震动按当前动态配置名称读写独立结构体。
- 风感、安全带支持读取、应用、保存、测试和离页归零。
- 输出支持实例增删、完整参数、Key 模板、连接和断开。
- 日志能够在 Release 产品中持久化。
- 所有目录、索引、Key 和类型来自 SDK，不使用硬编码替代。
- 所有 Save 后执行回读，不使用整结构体 `memcmp` 判断业务成功。

完成以上项目后，前端才算真正接入 SDK，而不是只做了状态展示。
