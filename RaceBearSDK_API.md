# RaceBear SDK 接入手册

本文面向需要使用 RaceBear SDK 开发 Windows 前端的第三方开发者。按照本文完成配置后，前端可以读取遥测和运动状态、管理配置、控制游戏及输出、显示授权状态，并使用 SDK 提供的串口能力。

当前 SDK 版本可通过 `RB_Runtime_GetVersion()` 获取。SDK 使用 Visual Studio 2019、`v142`、x64 构建，只支持 Windows x64 进程。

## 1. SDK 包内容

接入时需要以下文件：

| 文件                              | 用途                                                    |
| --------------------------------- | ------------------------------------------------------- |
| `include/RaceBearSDK.h`           | 唯一公开 C/C++ 头文件，包含函数、结构体、枚举和字段说明 |
| `bin/x64/Release/RaceBearSDK.lib` | Visual C++ 导入库，链接阶段使用                         |
| `bin/x64/Release/RaceBearSDK.dll` | SDK 运行库，部署时放在宿主 EXE 同目录                   |
| `RaceBearSDK_API.md`              | 本接入手册                                              |
| `RaceBearSDK.pdb`                 | 可选，仅用于分析 SDK 内部崩溃位置                       |

目标电脑需要安装 Microsoft Visual C++ 2015-2022 Redistributable x64。`RaceBearSDK.dll` 的其它依赖均为 Windows 系统组件。

不要混用不同版本 SDK 包中的 `.h`、`.lib` 和 `.dll`。三者必须来自同一次发布。

SDK 目录提供了三套现成示例：

| 目录              | 内容                                            |
| ----------------- | ----------------------------------------------- |
| `examples/cpp`    | 可使用 VS2019/CMake 构建的 C++ 控制台示例       |
| `examples/python` | 无第三方依赖的 Python `ctypes` 封装和运行示例   |
| `examples/qt`     | Qt 5/Qt 6 的 CMake、qmake、适配类和线程模型说明 |

## 2. 配置 Visual Studio 项目

以 Visual Studio 2019 为例：

1. 将项目平台设置为 `x64`。
2. 在“C/C++ -> 常规 -> 附加包含目录”中加入 SDK 的 `include` 目录。
3. 在“链接器 -> 常规 -> 附加库目录”中加入 SDK 的 `bin/x64/Release` 目录。
4. 在“链接器 -> 输入 -> 附加依赖项”中加入 `RaceBearSDK.lib`。
5. 编译后将 `RaceBearSDK.dll` 复制到宿主程序 EXE 所在目录。

CMake 项目可以使用下面的最小配置：

```cmake
add_executable(MyMotionApp main.cpp)

target_include_directories(MyMotionApp PRIVATE
    "D:/SDK/RaceBearSDK/include"
)

target_link_libraries(MyMotionApp PRIVATE
    "D:/SDK/RaceBearSDK/bin/x64/Release/RaceBearSDK.lib"
)

add_custom_command(TARGET MyMotionApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "D:/SDK/RaceBearSDK/bin/x64/Release/RaceBearSDK.dll"
        "$<TARGET_FILE_DIR:MyMotionApp>"
)
```

## 3. 第一个可运行程序

下面的程序完成 SDK 初始化、启动内部计算循环、读取状态并正常关闭：

```cpp
#include "RaceBearSDK.h"

#include <chrono>
#include <cstdio>
#include <thread>

int main()
{
    // 第三方软件应使用自己的产品目录名。必须在初始化前设置。
    if (RB_Runtime_SetAppDataName("MyMotionApp") != RB_OK) {
        std::puts("Invalid application data name.");
        return 1;
    }

    if (RB_Runtime_Initialize() != RB_OK) {
        std::puts("RaceBear SDK initialization failed.");
        return 2;
    }

    // 参数单位是 ms。2 表示约每 2 ms 计算一次。
    if (RB_Runtime_StartLoop(2) != RB_OK) {
        std::puts("RaceBear SDK runtime loop failed to start.");
        RB_Runtime_Shutdown();
        return 3;
    }

    for (int i = 0; i < 300; ++i) {
        RB_RuntimeState state{};
        if (RB_State_Read(&state) == RB_OK) {
            std::printf(
                "game=%s, telemetry=%d, output=%d, license=%d\n",
                state.Game.GameName,
                state.Source.TelemetryActive,
                state.Output.AnyRuntimeActive,
                state.License.State);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    RB_Runtime_Shutdown();
    return 0;
}
```

正常调用顺序固定为：

```text
RB_Runtime_SetAppDataName()   可选，但第三方软件建议设置
RB_Runtime_Initialize()       必须
RB_Runtime_StartLoop()        常规前端必须
读取状态、配置、目录和日志
执行命令
停止宿主自己的定时器和工作线程
RB_Runtime_Shutdown()         退出前必须
```

### 计算循环周期

`RB_Runtime_StartLoop(int intervalMs)` 的参数是计算周期间隔，单位毫秒。

| 传入值 | 实际含义 |
| ------ | -------- |
| `1` | 目标周期约 1 ms |
| `2` | 目标周期约 2 ms，当前推荐初始值 |
| `10` | 目标周期约 10 ms |

有效输入范围为 1-1000 ms。实际执行周期仍受 Windows 调度、计算负载和设备链路影响。UI 刷新周期与后端计算周期是两回事：后端可以按 2 ms 运行，而 UI 只需按约 16-33 ms 调用 `RB_State_Read()`。

`RB_Runtime_SetLoopIntervalMs()` 使用相同的毫秒单位，`RB_Runtime_GetLoopIntervalMs()` 返回当前目标周期间隔毫秒数。

SDK 0.4.2 将本参数从 Hz 改为 ms。旧代码中的 `RB_Runtime_StartLoop(100)` 必须按实际需求改为毫秒值，不能直接沿用。

SDK 0.4.3 补齐动态滤波、输出实例、运动增强、震动、风感和安全带的结构体配置流程，并支持全新 `AppDataName` 自动生成默认运行配置。常规前端不再需要直接编辑配置 JSON。

SDK 0.4.4 公开游戏安装与遥测源配置、功能插件目录、平台数值诊断和手动测试接口，并在结构体 Save/Apply 入口增加基础参数校验。CAN 和 LAN 的完整设备管理仍不属于当前第三方公开范围。

宿主程序不需要为 SDK 调用 `CoInitializeEx()` 或 `CoUninitialize()`。SDK 在自己的线程中管理 COM 生命周期。

## 4. 应用目录名和配置位置

`RB_Runtime_SetAppDataName()` 用于隔离不同产品的数据。例如传入 `MyMotionApp` 后，SDK 会在当前用户目录下使用该名称保存配置和授权。

首次使用全新名称时，SDK 会自动创建平台默认配置、`_Default` 动态配置及后续功能配置文件。第三方产品不应为了取得默认参数而共用 `%APPDATA%\Racebear`，也不要复制 SimRacebear 用户目录作为模板。

应用目录名要求：

- 使用 UTF-8、NUL 结尾字符串。
- 不能为空。
- 只能是单个目录名，不能传完整路径。
- 不能包含 `\ / : * ? " < > |` 等 Windows 路径非法字符。
- 必须在 `RB_Runtime_Initialize()` 之前调用。

未调用时默认使用 `Racebear`。主要配置保存在 `%APPDATA%\<应用目录名>\configV2`，授权数据保存在 `%APPDATA%\<应用目录名>\license`，运行日志和部分用户文件保存在 `Documents\<应用目录名>`。第三方软件应使用稳定且不会随版本变化的产品名。

## 5. 前端只需要理解五个核心入口

除生命周期和串口外，一个完整前端主要围绕下面五个函数实现：

| 函数                  | 用途                          | 建议调用频率                   |
| --------------------- | ----------------------------- | ------------------------------ |
| `RB_State_Read()`     | 读取高频运行状态结构体        | UI 刷新时调用，通常 30-100 Hz  |
| `RB_Data_ReadJson()`  | 读取配置、目录或低频状态 JSON | 页面打开、刷新或配置变化后调用 |
| `RB_Data_WriteJson()` | 保存完整配置 JSON             | 用户点击保存时调用             |
| `RB_Command_Run()`    | 执行连接、启动、激活等动作    | 用户操作或自动流程触发时调用   |
| `RB_Log_Read()`       | 从运行日志队列弹出一条日志    | 日志定时器中循环读取           |

所有 `char*`、`const char*` 和固定 `char[]` 都使用 UTF-8。所有布尔状态使用 `int`：`0` 表示假，非 `0` 表示真。

通用返回值：

| 返回值                       | 含义                                 |
| ---------------------------- | ------------------------------------ |
| `RB_OK` (`0`)                | 调用成功                             |
| `RB_ERROR` (`-1`)            | 执行失败或内部错误                   |
| `RB_INVALID_ARGUMENT` (`-2`) | 空指针、无效 key、无效命令或索引越界 |
| `RB_BUFFER_TOO_SMALL` (`-3`) | 调用方提供的文本缓冲区不足           |

串口读写函数是例外：成功时返回实际读写字节数，失败时返回负数。具体说明见第 11 节。

## 6. 读取实时运行状态

`RB_State_Read()` 是前端最常用的接口。它一次返回游戏、遥测、平台、输出、CAN、风感、安全带和授权状态，不需要为每个字段分别调用函数。

```cpp
RB_RuntimeState state{};
const int rc = RB_State_Read(&state);
if (rc == RB_OK) {
    // state.Size    结构体字节数
    // state.Version 当前结构体版本
}
```

常用状态分组：

| 成员               | 前端用途                                             |
| ------------------ | ---------------------------------------------------- |
| `state.License`    | 授权状态、问题、剩余时间、激活码、输出许可           |
| `state.Game`       | 当前游戏、游戏 Key、进程是否运行                     |
| `state.Source`     | 遥测源连接状态、数据状态、速度、转速、档位和派生效果 |
| `state.Rig`        | 当前平台、轴数量和 6DOF 分层输出                     |
| `state.Function`   | 风感、安全带、震动等功能摘要                         |
| `state.Output`     | 输出数量、连接状态和发送间隔                         |
| `state.Can`        | CAN 状态摘要；完整设备管理当前不作为第三方公共能力   |
| `state.Wind`       | 风感当前输入和各通道输出                             |
| `state.Seatbelt`   | 安全带张紧输入、左右输出和测试状态                   |
| `state.LastAction` | 最近一次后端动作说明                                 |
| `state.LastError`  | 最近一次错误说明                                     |

6DOF 数组顺序固定为：

```text
0 Sway
1 Surge
2 Heave
3 Yaw
4 Roll
5 Pitch
```

例如读取平台最终滤波输出：

```cpp
const double heave = state.Rig.MotionState.RigFiltered[2];
const double pitch = state.Rig.MotionState.RigFiltered[5];
```

结构体所有字段、单位和有效范围都写在 `RaceBearSDK.h` 中。前端应使用字段名访问，不要自行计算结构体偏移。

## 7. 读取 JSON 数据包

JSON 接口用于低频目录和完整配置。调用方负责提供缓冲区。下面的辅助函数会在缓冲区不足时自动扩容：

```cpp
#include "RaceBearSDK.h"

#include <string>
#include <vector>

bool ReadSdkJson(const char* key, std::string& result)
{
    std::vector<char> buffer(64 * 1024);

    while (buffer.size() <= 16 * 1024 * 1024) {
        const int rc = RB_Data_ReadJson(
            key,
            buffer.data(),
            static_cast<int>(buffer.size()));

        if (rc == RB_OK) {
            result.assign(buffer.data());
            return true;
        }
        if (rc != RB_BUFFER_TOO_SMALL) {
            return false;
        }
        buffer.resize(buffer.size() * 2);
    }
    return false;
}
```

可读取的数据 key：

| Key                    | 内容                   | 典型用途             |
| ---------------------- | ---------------------- | -------------------- |
| `runtime.snapshot`     | 简化运行状态 JSON      | 脚本前端或调试工具   |
| `config.telemetry`     | 当前遥测配置           | 遥测设置页面         |
| `config.dynamicV2`     | 当前动态滤波和运动配置 | 动感平台设置页面     |
| `config.platform`      | 当前平台结构配置       | 平台参数页面         |
| `config.output`        | 全部输出实例配置       | 输出设置页面         |
| `game.catalog`         | 游戏目录               | 游戏列表和搜索       |
| `telemetry.catalog`    | 遥测值目录             | 数据源选择框         |
| `output.catalog`       | 输出实例和可输出 Key   | 输出页面和命令编辑器 |
| `motionEffect.catalog` | 运动增强效果目录       | 运动增强页面         |
| `haptic.catalog`       | 震动效果目录           | 震动页面             |
| `can.state`            | CAN 状态摘要 JSON      | 只读状态或诊断显示   |
| `plugin.catalog`       | 功能插件目录           | 插件页面             |

调用示例：

```cpp
std::string gamesJson;
if (ReadSdkJson("game.catalog", gamesJson)) {
    // 使用调用方选择的 JSON 库解析 gamesJson。
}
```

`game.catalog` 示例：

```json
{
  "count": 1,
  "items": [
    {
      "catalogIndex": 0,
      "steamAppId": 2754090,
      "gameKey": "Aces Of Thunder",
      "gameName": "Aces Of Thunder",
      "imageName": "Aces Of Thunder.png",
      "processNames": "aces.exe*aces_BE.exe*",
      "canCreateSource": 1
    }
  ]
}
```

第三方前端应保存和传递 `gameKey`，不要让用户手写完整游戏名。`gameName` 只用于显示，`catalogIndex` 只适合当前目录快照内使用。

也可以直接使用结构体接口读取游戏目录：

```cpp
RB_GameCatalog catalog{};
if (RB_Game_GetCatalog(&catalog) == RB_OK) {
    for (int i = 0; i < catalog.Count; ++i) {
        const RB_GameCatalogItem& item = catalog.Items[i];

        // UI 显示 item.GameName，内部保存 item.GameKey。
        printf("%s [%s]\n", item.GameName, item.GameKey);
    }
}
```

按用户选择的游戏创建当前遥测源：

```cpp
const char* selectedGameKey = "Assetto Corsa";
const int rc = RB_Game_SelectByKey(selectedGameKey);
if (rc == RB_OK) {
    RB_Command_Run("runtime.reloadConfig", "{}", nullptr, 0);
}
```

### 接入第三方自研游戏

自研游戏不需要实现SDK内部Source类。调用方先读取`RB_TelemetryCatalog`，配置和代码只保存`RB_TelemetryCatalogItem::Key`；启动时通过`RB_Telemetry_FindIndexByKey()`解析当前进程索引。索引可能随SDK版本调整，禁止写入配置或网络协议。

常用RaceBear key和单位示例：

| key | 单位 | 含义 |
| --- | --- | --- |
| `rb.vehicle.speed` | `m/s` | 车辆合速度 |
| `rb.vehicle.engine.rpm` | `rpm` | 发动机转速 |
| `rb.motion.lateral.acceleration` | `m/s2` | 本地横向加速度 |
| `rb.motion.longitudinal.acceleration` | `m/s2` | 本地纵向加速度 |
| `rb.motion.vertical.acceleration` | `m/s2` | 本地垂直加速度 |
| `rb.motion.yaw.position` | `deg` | 偏航角 |
| `rb.motion.roll.position` | `deg` | 横滚角 |
| `rb.motion.pitch.position` | `deg` | 俯仰角 |
| `rb.stream.packet.id.time` | 无 | 持续变化的数据包ID或游戏时间 |

```cpp
const int speed = RB_Telemetry_FindIndexByKey("rb.vehicle.speed");
const int yaw = RB_Telemetry_FindIndexByKey("rb.motion.yaw.position");

RB_ExternalSourceDesc desc{};
desc.Size = sizeof(desc);
desc.Version = 1;
strcpy_s(desc.SourceKey, "vendor.custom_game");
strcpy_s(desc.SourceName, "Custom Game");
desc.DataTimeoutMs = 500;
desc.TransitionMs = 2000;
RB_ExternalSource_Open(&desc);

RB_ExternalTelemetryFrame frame{};
frame.Size = sizeof(frame);
frame.Version = 1;
frame.Sequence = packetId;
frame.ValueCount = 2;
frame.Values[0] = { speed, speedMps };
frame.Values[1] = { yaw, yawDegrees };
RB_ExternalSource_SubmitFrame(&frame);
```

每帧必须提交当前游戏支持的完整值集合。未提交槽位会清零，避免刹车、碰撞或开关状态沿用上一帧。`Sequence`停止变化或停止提交超过`DataTimeoutMs`后，Source自动淡出；恢复提交后自动淡入。提交函数只复制数据，SDK计算仍由`RB_Runtime_StartLoop()`启动的内部定时器完成。

结束游戏或切换回内置游戏前调用`RB_ExternalSource_Close()`。完整可运行代码见`examples/cpp/main.cpp`和`examples/python/example.py`。

### 注册正式自定义游戏（MMF或UDP）

`RB_ExternalSource_*`适合前端自己采集数据。客户自己开发游戏时，应使用`RB_Game_RegisterCustom()`把游戏名、进程名、图片和传输方式注册进正式游戏目录。注册配置保存在`%APPDATA%\<AppDataName>\configV2\CustomGamesV1.json`，下次启动由SDK自动加载。

```cpp
RB_CustomGameRegistration registration{};
registration.Size = sizeof(registration);
registration.Version = 1;
registration.Game.Size = sizeof(registration.Game);
registration.Game.Version = 1;
strcpy_s(registration.Game.GameKey, "vendor.custom_racing");
strcpy_s(registration.Game.GameName, "Custom Racing");
strcpy_s(registration.Game.ProcessNames, "CustomRacing.exe*CustomRacing-Win64.exe");
strcpy_s(registration.Game.ImageName, "custom_racing.png");
registration.Game.Transport = RB_CUSTOM_TRANSPORT_UDP;
registration.Game.InputMode = RB_CUSTOM_INPUT_RAW;
registration.Game.DataTimeoutMs = 500;
registration.Game.TransitionMs = 2000;
strcpy_s(registration.Udp.ListenAddress, "127.0.0.1");
registration.Udp.ListenPort = 27890;
strcpy_s(registration.Udp.AllowedSenderAddress, "127.0.0.1");

const int rc = RB_Game_RegisterCustom(&registration);
```

注册后`RB_Game_GetCatalog()`会返回该游戏，进程扫描会按`ProcessNames`自动创建Source。前端也可以调用`RB_Game_SelectByKey()`立即启动UDP监听或MMF等待，方便开发调试。

MMF和UDP使用相同的小端包：

```text
RB_CustomTelemetryPacketHeader
  + RB_CustomTelemetryRawValue[ValueCount]   // RAW模式
  或
  + RB_CustomTelemetryDofPayload             // 6DOF模式
```

原始遥测包不能使用运行时Index。游戏初始化时根据SDK文档或配套字段表确定稳定FieldId；开发工具可以通过`RB_Telemetry_FindFieldIdByKey()`生成配置：

```cpp
const int speedField = RB_Telemetry_FindFieldIdByKey("rb.vehicle.speed");
const int yawField = RB_Telemetry_FindFieldIdByKey("rb.motion.yaw.position");
```

每个UDP数据报必须是一帧完整包，不能拆包。`Sequence`从1开始严格递增；重复和倒序包不会刷新Source超时。SDK只保留尚未处理的最大Sequence包，实时遥测不积压旧帧。

直接6DOF模式的六个值全部为`-1.0`到`1.0`。SDK按当前平台PoseLimit换算，跳过动态遥测映射，但仍保留Source淡入淡出、姿态裁剪、平台几何、急停和最终安全输出。注册时设置`RB_CUSTOM_DOF_ADD_MOTION_EFFECTS`才会继续叠加RaceBear运动增强。

MMF映射布局为：

```text
RB_CustomGameMmfBlockHeader
unsigned char Packet[PacketCapacity]
```

游戏进程写MMF必须使用奇偶序号锁：

```cpp
// 先设为奇数，告诉SDK当前帧尚未写完。
InterlockedIncrement64(reinterpret_cast<volatile LONG64*>(&block->WriteSequence));
MemoryBarrier();

block->PacketSize = packetSize;
memcpy(packetArea, packetBytes, packetSize);
MemoryBarrier();

// 写完后变为偶数。SDK只有在读取前后序号一致且为偶数时才接受该帧。
InterlockedIncrement64(reinterpret_cast<volatile LONG64*>(&block->WriteSequence));
```

不要使用跨进程Mutex保护MMF。游戏崩溃时Mutex可能保持占用，序号锁则只会让SDK忽略未完成帧并继续等待。状态和错误通过`RB_Game_GetCustomState()`读取。

`runtime.snapshot` 的结构示例：

```json
{
  "license": {
    "state": 3,
    "problem": 0,
    "outputAllowed": true,
    "remainingSeconds": 0,
    "activationCode": "..."
  },
  "source": {
    "connected": true,
    "status": "Running"
  },
  "output": {
    "instanceCount": 1,
    "anyRuntimeActive": true,
    "allDisconnected": false
  },
  "logs": {
    "count": 0,
    "lastType": 0
  }
}
```

目录 JSON 都包含 `count` 和 `items`，具体字段由 SDK 返回内容决定。调用方应使用正式 JSON 解析库，不要用字符串查找或拼接解析 JSON。

## 8. 配置接口

正式 C++、Qt、炫彩或其他原生前端应优先使用结构体接口。JSON 接口只用于脚本、迁移工具、诊断工具和需要保留未来未知字段的高级场景，不能要求最终用户手工编辑 JSON 文件或在产品界面中直接修改 JSON 文本。

各配置页面统一遵循以下流程：

1. 从 SDK 目录取得有效索引、名称和可选 key。
2. 调用对应 `Read...` 接口读取完整结构体。
3. 只修改用户调整的字段，保留其他读取值。
4. 调用 `Apply...` 更新运行态预览。
5. 用户确认后调用 `Save...` 持久化。
6. 再次读取并刷新 UI，确认保存结果。

常规前端不需要编辑以下 JSON 包，直接使用对应结构体接口：

| JSON 包 | 主要结构体接口 |
| ------- | -------------- |
| `config.telemetry` | `RB_Game_ReadConfig()` / `RB_Game_SaveConfig()` |
| `config.dynamicV2` | `RB_Dynamic_ReadProfileByIndex()` / `RB_Dynamic_SaveProfileByIndex()` / `RB_Dynamic_ApplyProfileToCurrentRig()` |
| `config.platform` | `RB_Platform_ReadConfigByIndex()` / `RB_Platform_SaveConfigByIndex()` / `RB_Platform_ApplyConfigToCurrentRig()` |
| `config.output` | `RB_Output_ReadInstanceConfigByIndex()` / `RB_Output_SaveInstanceConfigByIndex()` / `RB_Output_ApplyInstanceConfigByIndex()` |
| 运动增强配置 | `RB_MotionEffect_ReadProfileByIndex()` / `RB_MotionEffect_SaveProfileByIndex()` / `RB_MotionEffect_ApplyProfileToCurrentRig()` |
| 震动配置 | `RB_Haptic_ReadProfileByIndex()` / `RB_Haptic_SaveProfileByIndex()` / `RB_Haptic_ApplyProfileToCurrentFunction()` |
| 风感配置 | `RB_Wind_ReadConfig()` / `RB_Wind_SaveConfig()` / `RB_Wind_ApplyConfigToCurrentFunction()` |
| 安全带配置 | `RB_Seatbelt_ReadConfig()` / `RB_Seatbelt_SaveConfig()` / `RB_Seatbelt_ApplyConfigToCurrentFunction()` |

### 8.0 JSON 高级接口

只有以下 key 可以写入：

```text
config.telemetry
config.dynamicV2
config.platform
config.output
```

配置保存采用“完整文档覆盖”方式。推荐流程：

1. 调用 `RB_Data_ReadJson()` 读取 SDK 当前配置。
2. 使用 JSON 库解析配置。
3. 只修改用户在前端调整的字段，并保留不认识的字段。
4. 将完整 JSON 文档传给 `RB_Data_WriteJson()`。
5. 执行 `runtime.reloadConfig` 命令，让运行态重新加载配置。

示例：

```cpp
std::string outputJson;
if (ReadSdkJson("config.output", outputJson)) {
    // 1. 使用 JSON 库修改 outputJson。
    // 2. 保存完整文档。
    const int saveRc = RB_Data_WriteJson(
        "config.output",
        outputJson.c_str());

    if (saveRc == RB_OK) {
        char result[256]{};
        RB_Command_Run(
            "runtime.reloadConfig",
            "{}",
            result,
            sizeof(result));
    }
}
```

不要从空字符串手工创建完整配置。首次运行时先初始化 SDK，再读取 SDK 已生成或已加载的配置作为编辑基础。

## 8.1 结构体配置接口

结构体接口是常规前端的主要接入方式。调用方不需要定位配置文件，也不需要了解内部 JSON 节点名称。

### 8.1.1 动态滤波参数

动态滤波使用 `RB_DynamicV2Profile` 整体传递。`profileIndex` 是动态配置列表中的零基索引，必须满足 `0 <= profileIndex < RB_Dynamic_GetProfileCount()`；`dofIndex` 是 `Dofs` 数组索引，`inputIndex` 是该 DOF 的输入通道索引。

固定 DOF 顺序如下：

| `dofIndex` | DOF |
| ---------- | --- |
| `0` | Sway |
| `1` | Surge |
| `2` | Heave |
| `3` | Yaw |
| `4` | Roll |
| `5` | Pitch |
| `6-7` | SDK 附加槽位，使用前应读取现有配置，不要自行假定含义 |

完整调用流程：

配置选择框可以这样填充：

```cpp
const int profileCount = RB_Dynamic_GetProfileCount();
for (int i = 0; i < profileCount; ++i) {
    char profileName[RB_TEXT_MEDIUM]{};
    if (RB_Dynamic_GetProfileName(i, profileName, sizeof(profileName)) == RB_OK) {
        // 把 profileName 显示在 UI 中，把 i 作为该选项对应的 profileIndex 保存。
    }
}
```

```cpp
const int profileIndex = 0; // 由前端配置列表的用户选择得到。
const int dofIndex = 0;     // Sway。
const int inputIndex = 0;   // Sway 的第一个输入通道。

if (profileIndex < 0 || profileIndex >= RB_Dynamic_GetProfileCount()) {
    return; // 配置索引无效。
}

RB_DynamicV2Profile profile{};
int rc = RB_Dynamic_ReadProfileByIndex(profileIndex, &profile);
if (rc != RB_OK) {
    return; // 先读取完整配置，避免覆盖其他 DOF 和输入通道。
}

RB_DynamicDofEffect& dof = profile.Dofs[dofIndex];
if (inputIndex < 0 || inputIndex >= dof.InputCount) {
    return;
}

RB_DynamicInputEffect& input = dof.Inputs[inputIndex];
input.Enabled = 1;
input.Filter.Enabled = 1;
input.Filter.InGain = 1.0;
input.Filter.Strength = 20;
input.Filter.Smoothing = 15;
input.Filter.DeadZone = 2;
input.Filter.Washout = 0;

// Apply 只更新当前运行模型，适合滑动条拖动时预览，不写配置文件。
rc = RB_Dynamic_ApplyProfileToCurrentRig(&profile);
if (rc != RB_OK) {
    return;
}

// Save 把整个 profile 写入 profileIndex 对应的配置文件。
// 保存按钮或延迟自动保存定时器触发时调用，不要在每一帧调用。
rc = RB_Dynamic_SaveProfileByIndex(profileIndex, &profile);
if (rc != RB_OK) {
    return;
}
```

动态输入和滤波字段：

| 字段 | 传入内容 |
| ---- | -------- |
| `RB_DynamicDofEffect::Enabled` | `1` 启用整个 DOF，`0` 禁用 |
| `InputCount` | `Inputs` 中有效通道数量；读取后通常不要由前端随意修改 |
| `RB_DynamicInputEffect::Enabled` | `1` 启用当前输入通道 |
| `InputType` | SDK 输入类型；应保留读取值，除非前端正在执行数据源切换 |
| `TelemetryIndex` | 当前运行时遥测索引；不要持久化自行计算的旧索引 |
| `Key` | 稳定遥测 key，例如 `rb.motion.linear.lateral_acceleration` |
| `Filter.Enabled` | 是否启用当前通道滤波 |
| `Filter.Dof` | 当前输入输出到的 DOF 索引 |
| `Filter.InMapping` | 输入达到满输出所需的绝对量；必须与所选遥测值单位一致，0 表示无有效映射 |
| `Filter.InGain` | 输入增益和方向；`1.0` 原方向，`-1.0` 反向，可使用小数 |
| `Filter.OutScaling` | 最大输出占当前 DOF `PoseLimit` 的百分比，建议 `0-100` |
| `Filter.Proportion` | Logistic 有效范围占 `PoseLimit` 的百分比；`0` 关闭 Logistic |
| `Filter.Strength` | Logistic 曲线强度；`0` 关闭，运行时按该值乘 `0.1` 使用 |
| `Filter.Smoothing` | 低通平滑；`0` 关闭，数值越大响应越慢 |
| `Filter.DeadZone` | 输出死区占最大输出范围的百分比；`0` 关闭 |
| `Filter.Washout` | 加速度输入高通洗出强度；`0` 关闭，姿态输入不会应用 |
| `Filter.Sensitivity` | GUARD 输出保护安全区百分比；`0` 关闭 |
| `Filter.SensitivityStrength` | GUARD 保护强度百分比；仅 `Sensitivity>0` 时生效 |

增加输入通道时，不要手写 `TelemetryIndex`。先从 `RB_Telemetry_GetCatalog()` 取得稳定 `Key`，再调用 `RB_Telemetry_FindIndexByKey()` 解析当前索引：

```cpp
RB_DynamicDofEffect& dof = profile.Dofs[dofIndex];
if (dof.InputCount >= RB_DYNAMIC_MAX_INPUTS) {
    return;
}

const char* telemetryKey = "rb.motion.linear.lateral_acceleration";
const int telemetryIndex = RB_Telemetry_FindIndexByKey(telemetryKey);
if (telemetryIndex < 0) {
    return;
}

RB_DynamicInputEffect& input = dof.Inputs[dof.InputCount];
input = {};
input.Enabled = 1;
input.TelemetryIndex = telemetryIndex;
strcpy_s(input.Key, sizeof(input.Key), telemetryKey);
input.Filter.Enabled = 1;
input.Filter.Dof = dofIndex;
input.Filter.InMapping = 1;
input.Filter.InGain = 1.0;
input.Filter.OutScaling = 100;
++dof.InputCount;
```

删除输入通道时需要保持数组连续：

```cpp
for (int i = inputIndex; i + 1 < dof.InputCount; ++i) {
    dof.Inputs[i] = dof.Inputs[i + 1];
}
if (dof.InputCount > 0) {
    --dof.InputCount;
    dof.Inputs[dof.InputCount] = {};
}
```

最后保存整个 `RB_DynamicV2Profile`。不能只传单个 `RB_FilterEffect`，因为同一个 DOF 的多个输入通道拥有各自独立参数。

配置创建、绑定和删除：

```cpp
RB_DynamicV2Profile copy{};
if (RB_Dynamic_ReadProfileByIndex(sourceIndex, &copy) == RB_OK &&
    RB_Dynamic_SaveProfile("MyProfile", &copy) == RB_OK) {
    // 新配置已经进入目录，刷新目录后取得它的新索引。
}

RB_Dynamic_SaveCurrentGameProfileIndex(selectedIndex);

// 删除会同时清理同名动态、运动增强和震动配置；随后必须刷新全部配置目录。
RB_Dynamic_DeleteProfileByIndex(selectedIndex);
```

### 8.1.2 运动增强参数

读取并修改运动增强参数：

```cpp
RB_MotionEffectProfile effects{};
if (RB_MotionEffect_ReadProfileByIndex(0, &effects) == RB_OK) {
    // effectIndex 可从 motionEffect.catalog 的 items[index].index 获取。
    RB_MotionEffectConfig& gearShift = effects.Effects[0];
    gearShift.Enabled = 1;
    gearShift.Threshold = 50;
    gearShift.Frequency = 25;
    gearShift.Gain = 1.0;

    // 每条 Routes 表示一个输出通道路由，Enabled=0 的路由会被忽略。
    gearShift.Routes[0].Enabled = 1;
    gearShift.Routes[0].Dof = 2;
    gearShift.Routes[0].OutputRatio = 1.0;
    gearShift.Routes[0].Frequency = 25;
    gearShift.Routes[0].Duration = 0.08;

    RB_MotionEffect_SaveProfileByIndex(0, &effects);
    RB_MotionEffect_ApplyProfileToCurrentRig(&effects);
}
```

运动增强目录与字段：

```cpp
RB_EffectCatalog effectCatalog{};
if (RB_MotionEffect_GetCatalog(&effectCatalog) == RB_OK) {
    // 使用 Items[i].Index 访问 Effects[effectIndex]，Name/Description 仅用于显示。
}
```

| 字段 | 含义 |
| ---- | ---- |
| `Enabled` | 是否启用效果 |
| `EffectType` | 效果类型；使用默认配置或读取值，不要与目录索引脱离 |
| `InputIndex`、`SecondaryInputIndex` | 主/辅助遥测运行时索引，使用遥测 key 重新解析 |
| `Gain` | 效果自身增益，`1.0=100%` |
| `RouteCount` | `Routes` 中连续有效路由数量，范围 `0-RB_MOTION_ROUTE_COUNT` |
| `Routes[i].Dof` | 目标 DOF，范围 `0-5` |
| `OutputRatio` | 当前路由比例，`1.0=100%` |
| `Direction` | 通常为 `1.0` 或 `-1.0` |
| `Threshold` | 当前路由触发阈值，单位随效果输入而变化 |
| `Frequency` | 当前路由频率，单位 Hz |
| `Duration` | 当前路由持续时间，单位秒 |

`MotionEffectConfig` 尾部的 `Threshold`、`Limit`、`Frequency`、`Decay`、`OutputDof`、`SecondaryOutputDof`、`SecondaryGain` 和 `Direction` 是旧配置兼容字段。新前端主要编辑 `Routes`；没有明确迁移需求时应保留读取值。

读取默认效果参数：

```cpp
RB_MotionEffectConfig config{};
if (RB_MotionEffect_GetDefaultConfig(0, &config) == RB_OK) {
    // 可把默认值填入 UI，再由用户修改后保存到 RB_MotionEffectProfile。
}
```

配置名称与动态配置保持对应。可使用 `RB_MotionEffect_GetProfileCount()` 和 `RB_MotionEffect_GetProfileName()` 枚举，也可以直接使用同索引的动态配置名。删除整组配置优先调用 `RB_Dynamic_DeleteProfileByIndex()`。

### 8.1.3 震动效果参数

```cpp
RB_EffectCatalog hapticCatalog{};
if (RB_Haptic_GetCatalog(&hapticCatalog) != RB_OK) {
    return;
}

RB_HapticEffectProfile haptic{};
if (RB_Haptic_ReadProfileByIndex(profileIndex, &haptic) != RB_OK) {
    return;
}

const int effectIndex = hapticCatalog.Items[0].Index;
RB_HapticEffectConfig& effect = haptic.Effects[effectIndex];
effect.Enabled = 1;
effect.OutputChannel = 0; // Haptic1。
effect.Gain = 1.0;
effect.MinOutput = 0.0;
effect.MaxOutput = 65535.0;

RB_Haptic_ApplyProfileToCurrentFunction(&haptic);
RB_Haptic_SaveProfileByIndex(profileIndex, &haptic);
```

| 字段 | 含义 |
| ---- | ---- |
| `OutputChannel` | `0-6`，对应 `Haptic1-Haptic7` |
| `Gain` | 输出增益；`0` 表示不输出 |
| `Threshold` | 触发阈值，单位取决于效果输入 |
| `MinOutput`、`MaxOutput` | 最小/最大位输出，默认范围 `0-65535` |
| `Frequency` | 频率或节奏参数，单位 Hz |
| `Direction` | 通常为 `1.0` 或 `-1.0` |

新增效果时先调用 `RB_Haptic_GetDefaultConfig(effectIndex, &config)`，不要从全零结构体猜默认值。震动配置与动态配置共用配置名和索引。

### 8.1.4 风感参数

读取并修改风感参数：

```cpp
RB_WindEffectConfig wind{};
if (RB_Wind_ReadConfig(&wind) != RB_OK) {
    RB_Wind_GetDefaultConfig(&wind);
}

wind.Enabled = 1;
wind.InputMinKmh = 30;
wind.InputMaxKmh = 180;
wind.Gamma = 1.2;
wind.MasterGainPercent = 100;

RB_Wind_SaveConfig(&wind);
RB_Wind_ApplyConfigToCurrentFunction(&wind);

// 测试按钮按下时启用，松开、离开页面和程序退出前必须关闭。
RB_Wind_SetTestOutput(1, 50.0);
RB_Wind_SetTestOutput(0, 0.0);
```

| 字段 | 单位和范围 |
| ---- | ---------- |
| `InputMinKmh` | 起风速度，km/h，必须大于等于 0 |
| `InputMaxKmh` | 满风速度，km/h，必须大于 `InputMinKmh` |
| `OutputMinWorkPercent` | 风扇最低有效输出，`0-100%` |
| `Gamma` | `1.0` 线性；大于 1 降低低速输出，小于 1 提高低速输出 |
| `MasterGainPercent` | 总增益，`0-100%` |
| `ChannelGainPercent[0-3]` | Wind1-Wind4 增益补偿，`-100%` 到 `100%` |
| `ChannelOffsetPercent[0-3]` | Wind1-Wind4 偏移补偿，`-100%` 到 `100%` |

### 8.1.5 安全带参数

读取并修改安全带参数：

```cpp
RB_SeatbeltEffectConfig seatbelt{};
if (RB_Seatbelt_ReadConfig(&seatbelt) != RB_OK) {
    RB_Seatbelt_GetDefaultConfig(&seatbelt);
}

seatbelt.Enabled = 1;
seatbelt.InputMode = RB_SEATBELT_INPUT_TELEMETRY;
seatbelt.BasePreloadPercent = 5;
seatbelt.StartSpeedKmh = 20;
seatbelt.MasterGainPercent = 100;
seatbelt.BrakeEnabled = 1;
seatbelt.BrakePedalGainPercent = 80;
seatbelt.BrakeDecelGainPercent = 80;
seatbelt.LateralEnabled = 1;
seatbelt.LateralGainPercent = 60;

RB_Seatbelt_SaveConfig(&seatbelt);
RB_Seatbelt_ApplyConfigToCurrentFunction(&seatbelt);

RB_Seatbelt_SetTestOutput(1, 40.0, 40.0);
RB_Seatbelt_SetTestOutput(0, 0.0, 0.0);
```

| 字段 | 单位和范围 |
| ---- | ---------- |
| `BasePreloadPercent` | 固定基础预紧，`0-100%`，不随车速增加 |
| `StartSpeedKmh` | 动态张紧起效速度，km/h |
| `FullSpeedKmh` | 旧版兼容字段，当前算法不使用，保留读取值 |
| `MasterGainPercent` | 总增益，`0-200%` |
| `ReleaseSpeedPercent` | 回落速度，`0-100%`，越大释放越快 |
| `BrakePedalGainPercent` | 刹车踏板激活门限，`0-100%` |
| `BrakeDecelGainPercent` | Surge 刹车减速度增益，`0-200%` |
| `LateralGainPercent` | Sway 横向张紧增益，`0-200%` |
| `LateralDeadzoneG` | 横向加速度死区，单位 G |
| `LeftOutputRatioPercent`、`RightOutputRatioPercent` | 左右通道输出比例，`0-100%` |
| `CrashEnabled` 及三个 Crash 参数 | 旧版兼容字段，当前算法不使用 |

`RB_SEATBELT_INPUT_HID` 和 `RB_SEATBELT_INPUT_MIXED` 只有在宿主接入 HID 输入源后才有意义。普通游戏遥测前端应使用 `RB_SEATBELT_INPUT_TELEMETRY`。

### 8.1.6 输出参数

每个输出实例对应一个 `RB_OutputConfig`。`instanceIndex` 是当前输出实例列表的零基索引，范围由 `RB_Output_GetInstanceCount()` 决定。先读取再修改，能够保留当前输出类型不认识或当前页面未显示的字段。

```cpp
const int instanceIndex = 0; // 由输出列表中用户选择的实例得到。
if (instanceIndex < 0 || instanceIndex >= RB_Output_GetInstanceCount()) {
    return;
}

RB_OutputConfig output{};
int rc = RB_Output_ReadInstanceConfigByIndex(instanceIndex, &output);
if (rc != RB_OK) {
    return;
}

// 串口输出示例。字符串必须是 UTF-8、NUL 结尾，且不能超过目标数组容量。
strcpy_s(output.OutputKind, sizeof(output.OutputKind), "Serial");
strcpy_s(output.Port, sizeof(output.Port), "COM3");
output.BaudRate = 115200;
output.OutBit = 16;
output.AutoConnect = 1;
output.ConnectionSpeed = 50;
strcpy_s(output.StartString, sizeof(output.StartString), "START");
strcpy_s(output.StartTime, sizeof(output.StartTime), "100");
strcpy_s(output.OutPutString, sizeof(output.OutPutString), "HA<A1><A2><A3><A4>");
strcpy_s(output.OutPutTime, sizeof(output.OutPutTime), "2"); // 单位 ms。
strcpy_s(output.EndString, sizeof(output.EndString), "STOP");
strcpy_s(output.EndTime, sizeof(output.EndTime), "100");

// Save 写入 OUT_x 配置文件，适合保存按钮或延迟自动保存。
rc = RB_Output_SaveInstanceConfigByIndex(instanceIndex, &output);
if (rc != RB_OK) {
    return;
}

// Apply 把配置送入现有运行实例。已连接输出的连接类参数通常应先断开再修改。
rc = RB_Output_ApplyInstanceConfigByIndex(instanceIndex, &output);
if (rc != RB_OK) {
    return;
}
```

不同输出类型使用的主要字段：

| 输出类型 | 必填或常用字段 |
| -------- | -------------- |
| Serial | `OutputKind`、`Port`、`BaudRate`、`OutBit`、命令和延时字段 |
| UDP | `OutputKind`、`IpAddrs`、`UdpPort`、`UdpLPort`、命令和延时字段 |
| MMF | `OutputKind`、`MMFName`、命令模板 |
| CAN | `OutputKind`、`CANindex`、`ConnectionSpeed`、`ParkPositionPercent`、`AutoConnect` |
| Sim-FAN | Serial 字段和 Wind1-Wind4 输出命令模板 |
| Vibration | Serial 字段和 Haptic1-Haptic7 输出命令模板 |

通用输出字段：

| 字段 | 说明 |
| ---- | ---- |
| `OutputKind` | 输出类型 key。必须来自 `RB_Output_GetKind()`，不要让用户自由输入 |
| `Type` | 旧版兼容字段；新前端按 `OutputKind` 判断类型，读取后原样保留 |
| `AutoConnect` | `1` 表示 SDK 自动连接该实例 |
| `ConnectionSpeed` | 连接、回零或过渡速度百分比，具体约束以当前输出类型为准 |
| `ParkPositionPercent` | 停放位置百分比 |
| `StartString` / `StartTime` | 连接后的起始命令及延时文本 |
| `OutPutString` / `OutPutTime` | 周期输出模板及输出间隔文本 |
| `EndString` / `EndTime` | 断开前的结束命令及延时文本 |

三个时间字段都是十进制毫秒文本，不是 Hz：`"2"` 表示约每 2 ms 输出一次，`"100"` 表示延迟 100 ms。使用文本是为了兼容现有配置格式，前端保存前必须验证为非负十进制数。

输出实例的完整生命周期：

```cpp
// 输出类型下拉框必须由 SDK 枚举，不能硬编码或让用户自由输入。
for (int i = 0; i < RB_Output_GetKindCount(); ++i) {
    char kind[RB_TEXT_SMALL]{};
    if (RB_Output_GetKind(i, kind, sizeof(kind)) == RB_OK) {
        // 把 kind 添加到 UI，并把原文保存为选项数据。
    }
}

// 首次运行没有输出实例时创建一个 Serial 实例。
if (RB_Output_GetInstanceCount() == 0) {
    const int newIndex = RB_Output_AddInstance("Serial");
    if (newIndex < 0) {
        return;
    }
}

const int instanceIndex = 0;
RB_OutputConfig config{};
if (RB_Output_ReadInstanceConfigByIndex(instanceIndex, &config) != RB_OK) {
    return;
}

// 连接参数只能在断开后修改。
if (RB_Output_IsInstanceConnected(instanceIndex)) {
    if (RB_Output_DisconnectInstanceByIndex(instanceIndex) != RB_OK) {
        return;
    }
}

strcpy_s(config.Port, sizeof(config.Port), "COM3");
config.BaudRate = 115200;
if (RB_Output_SaveInstanceConfigByIndex(instanceIndex, &config) == RB_OK &&
    RB_Output_ApplyInstanceConfigByIndex(instanceIndex, &config) == RB_OK) {
    RB_Output_ConnectInstanceByIndex(instanceIndex);
}

// 删除前先断开；删除后索引变化，必须刷新列表。
RB_Output_DisconnectInstanceByIndex(instanceIndex);
RB_Output_DeleteInstanceByIndex(instanceIndex);
```

自动连接和程序退出：

```cpp
RB_Output_ConnectAuto();

// 退出时让每个实例执行自己的结束命令，并等待状态机完成。
RB_Output_RequestDisconnectAll();
while (!RB_Output_AreAllDisconnected()) {
    Sleep(10);
}
```

不要在 UI 线程中无限等待。正式程序应使用定时器轮询并设置合理的退出超时。

UDP 参数示例：

```cpp
strcpy_s(output.OutputKind, sizeof(output.OutputKind), "UDP");
strcpy_s(output.IpAddrs, sizeof(output.IpAddrs), "192.168.1.100");
output.UdpPort = 33741;
output.UdpLPort = 33742;
```

输出实例正在连接时，不要直接覆盖串口端口、波特率、UDP 地址或 CAN 索引。连接参数的推荐修改流程：

```cpp
if (RB_Output_IsInstanceConnected(instanceIndex)) {
    const int disconnectRc = RB_Output_DisconnectInstanceByIndex(instanceIndex);
    if (disconnectRc != RB_OK) {
        return;
    }
}

const int saveRc = RB_Output_SaveInstanceConfigByIndex(instanceIndex, &output);
if (saveRc == RB_OK) {
    RB_Output_ApplyInstanceConfigByIndex(instanceIndex, &output);
}
```

保存和应用不会自动代表“用户要求立即连接”。是否重新连接应由用户操作或该实例的 `AutoConnect` 策略决定。

结构体接口的通用规则：

1. 先读现有结构体，再修改需要的字段，最后保存或应用。
2. 不要手动构造完整结构体再写入，除非调用了对应的默认配置接口。
3. 保存函数只写配置；运行中立即生效需要再调用对应 `Apply...` 接口，或执行 `runtime.reloadConfig`。
4. 字符数组字段都是 UTF-8、NUL 结尾文本；写入时要保证不超过头文件里的固定数组长度。

## 8.2 平台目录和平台参数

第三方前端不得写死平台下拉框。平台目录索引是零基索引，只用于调用当前 SDK；`RB_PlatformConfig::Type` 是平台类型编号，当前为 1-11，两者不能混用。

```cpp
const int platformCount = RB_Platform_GetCount();
for (int platformIndex = 0; platformIndex < platformCount; ++platformIndex) {
    char platformName[RB_TEXT_MEDIUM]{};
    if (RB_Platform_GetName(platformIndex, platformName, sizeof(platformName)) == RB_OK) {
        // UI 显示 platformName，并把 platformIndex 保存为选项数据。
    }
}
```

| 目录索引 | 名称 | `Type` | 轴数 | 支持的 DOF |
| -------- | ---- | -------- | ---- | ---------- |
| 0 | RB-A1L | 1 | 1 | Yaw |
| 1 | RB-A2L | 2 | 2 | Heave、Roll、Pitch |
| 2 | RB-A2V | 3 | 2 | Surge、Yaw |
| 3 | RB-A2T | 4 | 2 | Surge、Yaw |
| 4 | RB-A3LI | 5 | 3 | Heave、Roll、Pitch |
| 5 | RB-A3LII | 6 | 3 | Heave、Roll、Pitch |
| 6 | RB-A4L | 7 | 4 | Heave、Roll、Pitch |
| 7 | RB-A4LA | 8 | 4 | Heave、Yaw、Roll、Pitch |
| 8 | RB-A6T | 9 | 6 | Surge、Heave、Yaw、Roll、Pitch |
| 9 | RB-A6V | 10 | 6 | Surge、Heave、Yaw、Roll、Pitch |
| 10 | RB-A6L | 11 | 6 | Sway、Surge、Heave、Yaw、Roll、Pitch |

前端仍应调用 `RB_Platform_IsDofSupported()` 动态决定是否显示 DOF，而不是依赖上表。

### 8.2.1 L1-L10 通用规则

- `L1-L7` 是平台几何尺寸或节点坐标，单位均为毫米。
- `L8-L10` 是执行器总物理行程，单位均为毫米；SDK 内部使用总行程的一半作为正负范围。
- 未使用字段应保留读取值，不要显示为可编辑项，也不要写入猜测值。
- 修改前必须调用 `RB_Platform_ReadConfigByIndex()` 读取完整结构体。
- 参数名称接口返回空字符串时，表示当前平台未使用该字段。

```cpp
RB_PlatformConfig platform{};
if (RB_Platform_ReadConfigByIndex(platformIndex, &platform) != RB_OK) {
    return;
}

for (int number = 1; number <= 7; ++number) {
    char label[RB_TEXT_MEDIUM]{};
    RB_Platform_GetGeometryParameterName(platformIndex, number, label, sizeof(label));
    // label 为空时隐藏对应 L 输入框。
}
for (int number = 8; number <= 10; ++number) {
    char label[RB_TEXT_MEDIUM]{};
    RB_Platform_GetStrokeParameterName(platformIndex, number, label, sizeof(label));
}
```

### 8.2.2 各平台 L 参数含义

| 平台 | L1-L7 几何参数 | L8-L10 行程参数 |
| ---- | -------------- | --------------- |
| RB-A1L | L1 上平台宽；L2 上平台长；L3 底座节点 X；L4 底座节点 Y；L5 杆端节点 Y；L6 杆端节点 X；L7 未使用 | L8 A1 总行程；L9-L10 未使用 |
| RB-A2L | L1 平台宽度；L2 平台长度；L3-L7 未使用 | L8 A1-A2 总行程；L9-L10 未使用 |
| RB-A2V | L1 杆端宽度；L2 外壳固定端宽度；L3 旋转中心偏移；L4 上框宽度；L5 上框长度；L6-L7 未使用 | L8 A1-A2 总行程；L9-L10 未使用 |
| RB-A2T | L1 上框宽度；L2 上框长度；L3 Yaw 连接距离；L4 Yaw 外壳固定端 Y；L5 Yaw 杆端 Y；L6 Yaw 杆端 X；L7 Surge 外壳固定端 Y | L8 Surge 轴 A1 总行程；L9 Yaw 轴 A2 总行程；L10 未使用 |
| RB-A3LI | L1 平台宽度；L2 平台长度；L3-L7 未使用 | L8 A1-A3 总行程；L9-L10 未使用 |
| RB-A3LII | L1 平台宽度；L2 平台长度；L3-L7 未使用 | L8 A1-A3 总行程；L9-L10 未使用 |
| RB-A4L | L1 平台宽度；L2 平台长度；L3-L7 未使用 | L8 A1-A4 总行程；L9-L10 未使用 |
| RB-A4LA | L1 前底座宽度；L2 前顶部宽度；L3 后底座宽度；L4 后顶部宽度；L5 框架长度；L6 旋转中心高度；L7 Yaw 中心距前边距离 | L8 A1-A4 总行程；L9-L10 未使用 |
| RB-A6T | L1 上框宽度；L2 上框长度；L3 Yaw 连接距离；L4 Yaw 外壳固定端 Y；L5 Yaw 杆端 Y；L6 Yaw 杆端 X；L7 Surge 外壳固定端 Y | L8 上层 A1-A4 总行程；L9 Surge 轴 A5 总行程；L10 Yaw 轴 A6 总行程 |
| RB-A6V | L1 下层杆端宽度；L2 下层外壳固定端宽度；L3 下层旋转中心偏移；L4 上框宽度；L5 上框长度；L6-L7 未使用 | L8 上层 A1-A4 总行程；L9 下层 A5-A6 总行程；L10 未使用 |
| RB-A6L | L1 上平台相邻点距离；L2 上平台对角点距离；L3 下平台相邻点距离；L4 下平台对角点距离；L5 执行器中间长度；L6 Yaw 中心 Y 偏移，正值向前；L7 旋转中心 Z 高度，正值向上 | L8 A1-A6 总行程；L9-L10 未使用 |

### 8.2.3 保存和应用平台参数

```cpp
RB_PlatformConfig platform{};
if (RB_Platform_ReadConfigByIndex(platformIndex, &platform) != RB_OK) {
    return;
}
platform.L1 = 690;          // 当前平台 L1，单位 mm。
platform.OutGain = 100;     // 总输出比例，百分比。
platform.OutSmoothing = 20; // 最后一级输出平滑，百分比。

if (RB_Platform_SaveConfigByIndex(platformIndex, &platform) == RB_OK) {
    RB_Platform_SetSelectedState(platformIndex, &platform);
    RB_Platform_ApplyConfigToCurrentRig(platformIndex, &platform);
}
```

`RB_Platform_ApplyRuntimeConfigToCurrentRig()` 用于同一平台运行参数热更新。切换平台类型或完整重建前应先断开全部输出。

## 8.3 输出 Key 和命令模板

输出 Key 必须从 `RB_Output_GetCatalog()` 动态读取。Key 数量取决于当前平台轴数以及已创建的风感、安全带和震动功能。

```cpp
RB_OutputCatalog catalog{};
if (RB_Output_GetCatalog(&catalog) == RB_OK) {
    for (int i = 0; i < catalog.KeyCount; ++i) {
        const char* key = catalog.Keys[i].Key;
        // 双击列表项时向命令框插入："<" + key + ">"。
    }
}
```

| Key | 来源 | 含义 |
| --- | ---- | ---- |
| `A1`-`A6` | 当前平台 | 当前平台实际存在的执行器位输出；轴数不足时高编号不会出现 |
| `Wind1`-`Wind4` | 风感模拟 | 四路风感位输出 |
| `SeatbeltLeft`、`SeatbeltRight` | 安全带模拟 | 左右安全带张紧器位输出 |
| `Haptic1`-`Haptic7` | 震动模拟 | 七路震动效果位输出 |
| `SWAY`、`SURGE`、`HEAVE`、`YAW`、`ROLL`、`PITCH` | 平台姿态层 | 与当前平台动态映射和叠加设置同步的六个处理后 DOF 值 |

Key 区分大小写，必须使用目录返回的原文。不要把遥测目录中的其他 `rb.*` Key 作为普通输出命令候选项。

命令模板规则：

- 普通文本直接按字节输出，例如 `HA`。
- `<Key>` 插入运行值，例如 `HA<A1><A2><A3><A4>`。
- `<123>` 插入数值常量，主要用于兼容旧协议。
- 未识别占位符在文本/整数输出中按 0 处理，保存前必须验证 Key 存在于当前目录。
- 执行器和功能 Key 按其位宽输出；普通输出位宽由 `RB_OutputConfig::OutBit` 控制。
- 六个基础 DOF 是处理后的平台姿态值，不是执行器位值；线性 DOF 使用平台长度单位，旋转 DOF 使用平台角度单位。

完整输出页面至少应实现：读取实例目录、读取实例配置、编辑参数、断开、保存、应用、重新连接，以及从 Key 目录双击插入占位符。只实现全部连接和全部断开不能算完整输出配置页面。

## 8.4 游戏安装和遥测源配置

游戏目录说明 SDK 支持哪些游戏，安装目录说明本机实际找到哪些游戏。两者使用稳定 `gameKey` 关联，不能依靠列表索引互相对应。

```cpp
const int installCount = RB_Game_RefreshInstallations();
for (int i = 0; i < installCount; ++i) {
    RB_GameInstallInfo info{};
    if (RB_Game_GetInstallInfo(i, &info) == RB_OK) {
        // Found=1 表示找到安装目录；Running=1 表示检测到进程。
        // Ambiguous=1 时不要自动启动，应让用户确认候选路径。
    }
}
```

打开遥测设置页面时先按稳定 key 读取完整配置：

```cpp
RB_GameConfig config{};
if (RB_Game_ReadConfig(selectedGameKey, &config) != RB_OK) {
    return;
}

config.SourcePort = 9996;
strcpy_s(config.SourceIP, sizeof(config.SourceIP), "127.0.0.1");
config.Forwarding = 0;

if (RB_Game_SaveConfig(selectedGameKey, &config) == RB_OK) {
    // CatalogIndex 来自同一个 RB_GameCatalogItem，不是安装列表索引。
    RB_Game_AutoConnect(selectedGame.CatalogIndex, &config);
}
```

`RB_GameConfig` 字段：

| 字段 | 含义 |
| ---- | ---- |
| `SourcePort` | 游戏遥测监听端口，`0-65535`；是否允许 0 取决于游戏 Source |
| `SourceIP` | 本地监听/绑定地址，通常为 `127.0.0.1` 或本机网卡地址 |
| `RemotehostIP` | 需要主动连接远端主机的 Source 使用 |
| `ForwardingPort`、`ForwardingIP`、`Forwarding` | 遥测转发目标和开关 |
| `DirectlyConnected` | 硬件或游戏 Source 是否使用直连模式 |
| `SerialPort`、`SerialBaudRate` | 硬件遥测 Source 使用 |
| `Configuration` | 当前游戏绑定的动态配置名 |
| `GamePath` | 游戏安装目录；优先来自安装扫描结果 |

自动配置和游戏侧插件是可选能力：

```cpp
// 不支持自动配置的游戏会返回 RB_ERROR，前端应保留手动说明。
RB_Game_AutoConfigureCurrent();

RB_GameInstallInfo install{};
if (RB_Game_FindInstallByKey(selectedGameKey, &install) == RB_OK && install.Found) {
    RB_Game_InstallCurrentPlugin(install.InstallPath);
}

const int sideConfigReady = RB_Game_CheckCurrentSideConfig(&config);
```

安装扫描、配置检查和插件安装都属于用户操作或页面刷新动作，不要放进高频状态定时器。

## 8.5 功能插件

插件列表使用一个目录包读取，第三方不需要逐字段调用 getter：

```cpp
if (RB_Plugin_Refresh() < 0) {
    return;
}

RB_PluginCatalog plugins{};
if (RB_Plugin_GetCatalog(&plugins) == RB_OK) {
    for (int i = 0; i < plugins.Count; ++i) {
        const RB_PluginInfo& item = plugins.Items[i];
        // Name/Version/StatusText 用于显示；Launchable 控制启动按钮。
    }
}
```

- `RB_Plugin_StartRuntimes()` 启动全部插件后台运行态，通常在 SDK 初始化后调用一次。
- `RB_Plugin_Launch(index)` 只启动该插件提供的独立前端程序，不等同于启动后台运行态。
- 每次 `RB_Plugin_Refresh()` 后旧索引失效，按钮应保存最近一次目录中的 `Index`。
- 插件状态绘制属于宿主 UI，不在标准 SDK 中提供炫彩绘制函数。

## 8.6 平台诊断和手动测试

```cpp
const int platformIndex = RB_Platform_ReadSelectedIndex();
RB_PlatformConfig platform{};
if (RB_Platform_ReadConfigByIndex(platformIndex, &platform) != RB_OK) {
    return;
}

for (int dof = 0; dof < RB_DOF_COUNT; ++dof) {
    const int possible = RB_Platform_IsPreviewPosePossible(platformIndex, &platform, dof);
    const double limit = RB_Platform_GetPreviewPoseLimit(platformIndex, &platform, dof);
    // possible=1 时，滑条范围使用 -limit 到 +limit。
}
```

手动测试值直接进入真实运行 Rig：线性 DOF 使用毫米，Yaw/Roll/Pitch 使用度。

```cpp
if (RB_ManualPose_SetTestEnabled(1) == RB_OK) {
    RB_ManualPose_SetDrive(2, 10.0); // Heave +10 mm。
}

// 关闭页面、切换平台和程序退出前固定执行复位和关闭。
RB_ManualPose_ResetDrive();
RB_ManualPose_SetTestEnabled(0);
```

前端必须把输入限制在预览范围内，并在平台不可达、输出状态不允许、急停或授权锁定时禁用测试。SDK 最终输出安全链仍然有效，但前端不能把它当成越界输入的替代品。

## 8.7 保存参数校验

0.4.4 起，结构体 Save/Apply 接口会在写文件或修改运行态之前执行基础校验。以下情况返回 `RB_INVALID_ARGUMENT`：

- 数组计数超出固定容量，DOF、路由或输出通道索引越界。
- 固定字符数组没有 NUL 结尾。
- 数值包含 NaN/Inf，百分比或端口明显越界。
- `InputMaxKmh <= InputMinKmh`、震动最小输出大于最大输出。
- 未知输出类型、非法输出时间文本或必填连接字段为空。

校验失败不会写配置，也不会部分应用。前端应保留用户输入、标记对应控件，并显示 `state.LastError` 或自己的字段提示。

## 9. 执行命令

所有动作统一通过 `RB_Command_Run()` 执行：

```cpp
char result[4096]{};
const int rc = RB_Command_Run(
    "output.connectAll",
    "{}",
    result,
    sizeof(result));

if (rc == RB_OK) {
    // result 示例：{"ok":true,"count":1}
}
```

`argsJson` 必须是 UTF-8 JSON 对象。无参数命令统一传入 `{}`。不需要结果时，`resultBuffer` 可以传 `nullptr`，同时将 `resultBufferSize` 设为 `0`。

当前公开命令：

| Command                | argsJson                      | 成功结果示例            | 用途                       |
| ---------------------- | ----------------------------- | ----------------------- | -------------------------- |
| `license.check`        | `{}`                          | `{"ok":true}`           | 立即重新检查授权           |
| `license.activate`     | `{"activationCode":"激活码"}` | `{"ok":true}`           | 激活当前设备               |
| `license.deactivate`   | `{}`                          | `{"ok":true}`           | 反激活当前设备             |
| `runtime.reloadConfig` | `{}`                          | `{"ok":true}`           | 重新加载全部配置           |
| `game.restoreLast`     | `{}`                          | `{"ok":true}`           | 恢复上次选择的游戏         |
| `game.autoDetect`      | `{}`                          | `{"ok":true}`           | 自动检测正在运行的游戏     |
| `game.launch`          | `{"gameKey":"游戏Key"}`       | `{"ok":true,"code":0}`  | 启动游戏                   |
| `game.terminate`       | `{"gameKey":"游戏Key"}`       | `{"ok":true,"code":0}`  | 结束游戏进程               |
| `output.connectAll`    | `{}`                          | `{"ok":true,"count":1}` | 连接全部输出实例           |
| `output.connectAuto`   | `{}`                          | `{"ok":true,"count":1}` | 连接启用自动连接的输出实例 |
| `output.disconnectAll` | `{}`                          | `{"ok":true,"count":1}` | 断开全部输出实例           |
| `logs.clear`           | `{}`                          | `{"ok":true}`           | 清空运行日志队列           |

未知命令返回 `RB_INVALID_ARGUMENT`，结果 JSON 为：

```json
{ "ok": false, "message": "unknown command" }
```

命令返回 `RB_OK` 只表示本次命令被成功执行。对于输出连接等持续过程，前端还应继续读取 `RB_RuntimeState` 中的状态，直到流程完成或出现错误。

## 10. 授权接入

授权完全由 SDK 管理。第三方前端不生成机器码、不读写授权文件，也不需要直接调用授权服务器。

前端只需要读取：

```cpp
RB_RuntimeState state{};
if (RB_State_Read(&state) == RB_OK) {
    const int licenseState = state.License.State;
    const int problem = state.License.Problem;
    const int outputAllowed = state.License.OutputAllowed;
    const long long remaining = state.License.RemainingSeconds;
    const char* activationCode = state.License.ActivationCode;
}
```

授权状态：

| `RB_LicenseState`          | 含义                               | 前端建议显示             |
| -------------------------- | ---------------------------------- | ------------------------ |
| `RB_LICENSE_UNKNOWN`       | 尚未完成检查                       | 正在准备授权             |
| `RB_LICENSE_CHECKING`      | 正在检查服务器或本地授权           | 正在检查授权             |
| `RB_LICENSE_OUTPUT_LOCKED` | 授权无效，真实输出被锁定           | 未授权或授权异常         |
| `RB_LICENSE_PERPETUAL`     | 永久授权                           | 已永久激活               |
| `RB_LICENSE_TIMED`         | 限时授权                           | 显示剩余时间             |
| `RB_LICENSE_TRIAL`         | 本地试用倒计时，倒计时期间允许输出 | 显示未授权和剩余试用时间 |

`RemainingSeconds` 在限时授权和本地试用状态下有效；永久授权为 `0`。永久授权只做本地校验，不会周期联网心跳。授权问题原因使用 `RB_LicenseProblem`。例如网络错误、授权过期、硬件不匹配或授权被撤销。前端应根据 `State` 和 `Problem` 组合显示，不要只判断剩余秒数。

激活示例：

```cpp
std::string activationCode = "USER-INPUT-CODE";
std::string args =
    "{\"activationCode\":\"" + activationCode + "\"}";

char result[256]{};
const int rc = RB_Command_Run(
    "license.activate",
    args.c_str(),
    result,
    sizeof(result));
```

正式产品应使用 JSON 库生成激活参数，避免用户输入中的引号或反斜杠破坏 JSON。

授权无效时 SDK 不会强制退出宿主程序。状态读取、配置和激活功能仍可使用，但所有真实硬件输出会被锁定为不可用。

## 11. 串口接入

串口能力独立于运动运行态，可以用于前端串口列表、设备配置和自定义硬件通信。

### 11.1 枚举串口

```cpp
const int count = RB_Serial_GetAvailablePortCount();
for (int i = 0; i < count; ++i) {
    RB_SerialPortInfo info{};
    if (RB_Serial_GetAvailablePortInfo(i, &info) == RB_OK) {
        std::printf("%s - %s\n", info.PortName, info.FriendlyName);
    }
}
```

### 11.2 打开、发送和关闭

```cpp
RB_SerialHandle serial = RB_Serial_Create();
if (serial == nullptr) {
    return;
}

if (RB_Serial_Init(serial, "COM3", 115200) == RB_OK &&
    RB_Serial_Open(serial) == RB_OK) {
    const unsigned char packet[] = {0xAA, 0x01, 0x55};
    const int written = RB_Serial_Write(
        serial,
        packet,
        static_cast<int>(sizeof(packet)));

    if (written < 0) {
        char errorText[512]{};
        RB_Serial_GetLastErrorMsg(serial, errorText, sizeof(errorText));
        std::printf("serial error: %s\n", errorText);
    }

    RB_Serial_Close(serial);
}

RB_Serial_Destroy(serial);
serial = nullptr;
```

串口对象顺序固定为：

```text
RB_Serial_Create()
RB_Serial_Init() 或 RB_Serial_InitEx()
RB_Serial_Open()
读写或注册事件
断开事件
RB_Serial_Close()
RB_Serial_Destroy()
```

`RB_Serial_Write()`、`RB_Serial_Read()`、`RB_Serial_ReadSome()`、`RB_Serial_ReadAll()` 和 `RB_Serial_ReadLine()` 成功时返回字节数，失败时返回负数。返回 `0` 通常表示当前没有读到数据，不应当作错误。

串口读取事件和热插拔事件可能在 SDK 工作线程中触发。回调内不要直接操作 UI 控件，应将数据复制到自己的线程安全队列，再通知 UI 线程刷新。

传给回调的 `userData` 由调用方管理，其生命周期必须覆盖整个回调注册期。销毁串口对象前先断开回调，并确保没有其它线程继续使用该句柄。

## 12. 日志接入

`RB_Log_Read()` 每次从 SDK 日志队列弹出一条 UTF-8 文本。队列为空时返回空字符串。

```cpp
void DrainSdkLogs()
{
    char text[4096]{};
    for (;;) {
        const int rc = RB_Log_Read(text, sizeof(text));
        if (rc != RB_OK || text[0] == '\0') {
            break;
        }
        std::printf("[RaceBear] %s\n", text);
    }
}
```

建议每 100-500 ms 清空一次日志队列。不要只在 Debug 构建中显示日志，正式版也应保存必要的 SDK 运行日志，便于排查设备和现场问题。

## 13. 线程、刷新频率和退出

推荐线程模型：

| 工作                | 建议线程                                                    |
| ------------------- | ----------------------------------------------------------- |
| SDK 后端计算        | 由 `RB_Runtime_StartLoop(intervalMs)` 创建的内部 MultimediaTimer 负责 |
| UI 状态刷新         | UI 定时器，通常 30-60 Hz；需要更顺滑预览时可到 100 Hz       |
| JSON 目录和配置读取 | 页面加载或用户刷新时调用，不要高频轮询                      |
| 串口和设备回调      | SDK 工作线程触发，转发到 UI 线程                            |
| 日志读取            | 独立低频 UI 定时器或日志线程                                |

不要在内部循环运行时调用 `RB_Runtime_DoCalculations()`。该函数只用于明确不启动内部循环的高级调试场景。

正确退出顺序：

1. 禁止新的 UI 命令和配置写入。
2. 停止宿主自己的状态、日志和设备定时器。
3. 断开宿主创建的串口回调并销毁串口对象。
4. 等待宿主工作线程退出，确保没有线程继续调用 SDK。
5. 在主线程调用一次 `RB_Runtime_Shutdown()`。
6. 之后不再调用任何 SDK 函数。

不要在 DLL 卸载、全局静态对象析构或仍有工作线程运行时才调用 `RB_Runtime_Shutdown()`。明确的主程序退出流程最可靠。

## 14. 完整前端最低功能矩阵

能成功初始化 SDK 不代表前端已经完整。面向最终用户的软件至少应覆盖下表；不需要某项硬件时可以隐藏对应页面，但不能用仅修改提示文字的空按钮代替 SDK 调用。

| 页面或服务 | 必须读取 | 必须支持的写入或动作 |
| ---------- | -------- | -------------------- |
| 全局运行态 | `RB_State_Read()` | 启动、暂停、恢复和停止 SDK 内部循环 |
| 授权 | `state.License` | `license.activate`、`license.deactivate`、`license.check` |
| 游戏 | 游戏目录和安装目录 | 选择、启动/停止、安装扫描、游戏侧配置和保存游戏配置 |
| 遥测 | `RB_Telemetry_GetCatalog()`、`state.Source` | 遥测源配置、连接、自动配置和当前值查看 |
| 平台 | 平台目录、DOF 支持、`RB_PlatformConfig` | 选择平台、编辑 L1-L10、保存、应用和受限手动测试 |
| 动态滤波 | 动态配置目录和 `RB_DynamicV2Profile` | 每个 DOF 的输入通道增删、每通道滤波参数、预览、保存和应用 |
| 运动增强 | 效果目录和 `RB_MotionEffectProfile` | 每效果启用、输入、增益及每输出路由参数 |
| 风感 | `RB_Wind_ReadConfig()`、`state.Wind` | 编辑、测试、保存和应用 |
| 安全带 | `RB_Seatbelt_ReadConfig()`、`state.Seatbelt` | 编辑、测试、保存和应用 |
| 震动 | `RB_Haptic_ReadProfile()`、运行摘要 | 编辑效果、输出通道、保存和应用 |
| 输出 | `RB_Output_GetCatalog()`、`RB_OutputConfig` | 增删实例、编辑参数、Key 模板、连接和断开 |
| 设备/CAN | `state.Can` 状态摘要 | 可选只读显示；完整设备管理当前不要求第三方实现 |
| 日志 | `RB_Log_Read()` | 显示、持久化和清理日志 |
| 自定义游戏 | 自定义游戏状态 | 注册 MMF/UDP 游戏、提交协议和注销 |
| 功能插件 | `RB_Plugin_GetCatalog()` | 刷新目录、启动后台运行态和可启动插件前端 |

### 14.1 常用运行值单位

| 字段 | SDK 单位 | 常见 UI 显示 |
| ---- | -------- | ------------ |
| `state.Source.FinalSpeed` | m/s | km/h，需要乘以 3.6 |
| `FinalRPM` | rpm | rpm |
| `FinalGear` | 档位数值 | 前端自行格式化 N/R/前进档 |
| `TransitionPercent` | 0-1 | 百分比，需要乘以 100 |
| 线性位置、平台几何和执行器行程 | mm | mm |
| Yaw/Roll/Pitch 姿态 | 度 | 度 |
| 风感和安全带百分比字段 | 0-100 | % |
| 授权剩余时间 | 秒 | 天/小时/分钟 |

所有 SDK 字符串均为 UTF-8。Win32/WinUI 前端必须用 UTF-8 到 UTF-16 的正式转换函数，不能使用 `std::wstring(text.begin(), text.end())` 逐字节扩展，否则中文名称和日志会乱码。

### 14.2 页面实现验收规则

一个配置页面至少满足以下条件才算真正接入：

1. 页面打开时从 SDK 读取现有配置，而不是使用硬编码默认值。
2. UI 控件修改的是完整配置结构体或完整 JSON 文档。
3. Apply 操作确实调用对应运行态接口，并检查返回码。
4. Save 操作确实写入配置，并在需要时重新加载或应用。
5. 页面重新打开后能够回读刚才保存的值。
6. 无效索引、未初始化、输出已连接和授权锁定状态有明确提示。
7. 所有列表均来自 SDK 目录，不依赖手写游戏名、平台名、效果名或输出 Key。

## 15. 常见问题

### 初始化返回 `RB_ERROR`

确认 `RaceBearSDK.dll` 位于 EXE 同目录，目标进程为 x64，并已安装 VC++ 2015-2022 x64 运行库。随后读取运行日志确认具体原因。

### 程序启动后没有运动输出

依次检查：

1. `RB_Runtime_StartLoop()` 是否返回 `RB_OK`。
2. `state.Source.TelemetryActive` 是否为 `1`。
3. `state.Output.AnyRuntimeActive` 是否为 `1`。
4. `state.License.OutputAllowed` 是否为 `1`。
5. `state.LastError` 和运行日志是否有错误。

### `RB_Data_ReadJson()` 返回 `RB_BUFFER_TOO_SMALL`

扩大缓冲区后重试。目录和完整配置可能超过固定小缓冲区，建议使用第 7 节的自动扩容辅助函数。

### 写入配置后界面或输出没有变化

确认写入的是完整 JSON 文档，并在保存成功后执行了 `runtime.reloadConfig`。不要只传某一个字段或局部 JSON 片段。

### 未授权时程序是否必须退出

不需要。SDK 只锁定真实输出，前端仍可显示状态、编辑配置和执行激活操作。

### 第三方前端如何绘制平台或曲线

公开 SDK 不绑定任何 UI 框架。调用方从 `RB_RuntimeState` 和 JSON 数据包读取状态后，使用自己的绘图库实现界面。炫彩、MFC、Qt、WinUI、Unity 等前端可以采用各自的绘制方式。

## 16. 发布前检查

第三方应用发布前至少完成以下验证：

1. Release 包中的 `.h`、`.lib`、`.dll` 来自同一 SDK 版本。
2. 目标程序和所有插件均为 x64。
3. 连续启动、初始化、关闭程序多次不会异常退出。
4. SDK 内部循环由 `RB_Runtime_StartLoop()` 驱动，前端没有重复计算循环。
5. 授权有效、过期、无网络和反激活状态均能正确显示。
6. 未授权时真实输出确实被锁定。
7. 游戏检测、遥测连接、输出连接和断开流程均能恢复。
8. JSON 保存后保留未知字段，没有用字符串拼接破坏配置。
9. 串口热插拔和读取回调不会直接跨线程操作 UI。
10. 退出前停止所有前端定时器和工作线程，再调用 `RB_Runtime_Shutdown()`。

函数的精确参数、返回值、结构体字段和单位以同一发布包中的 `RaceBearSDK.h` 为准。本手册负责说明接入流程，头文件负责说明单个接口合同。
