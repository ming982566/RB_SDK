# RaceBear SDK 第三方接入示例

这里的示例面向需要把 RaceBear SDK 接入自己软件的开发者。示例代码保持小而完整，重点展示 DLL 加载、运行时生命周期、状态读取、外部遥测提交和配置读写。

## 先看哪个示例

| 目录 | 适合场景 | 依赖 |
| --- | --- | --- |
| `cpp` | Windows 原生程序、Visual Studio、CMake | VS2019/v142、C++17 |
| `python` | 工具、脚本、原型和数据采集 | 64 位 Python 3.8+、标准库 `ctypes` |

Qt 示例已移除。需要 GUI 时，可以把这两个示例里的 SDK 会话放进自己的窗口程序；SDK 本身不要求使用特定 UI 框架。

## SDK 包结构

开发时至少需要以下文件：

```text
RaceBear_SDK/
  include/RaceBearSDK.h
  bin/x64/Release/RaceBearSDK.dll
  bin/x64/Release/RaceBearSDK.lib       # 仅 C/C++ 链接需要
  bin/x64/Release/libusb-1.0.dll        # RaceBearSDK.dll 的 USB 运行依赖
  examples/cpp/
  examples/python/
```

`RaceBearSDK.dll` 和 `libusb-1.0.dll` 必须和最终 EXE 位于同一目录，或者位于 Windows DLL 搜索路径中。正式部署还要检查 Microsoft Visual C++ 2015-2022 Redistributable x64。

示例使用独立的 `AppDataName`，因此运行账户必须能够创建和写入 `%APPDATA%\<AppDataName>` 与 `Documents\<AppDataName>`。安装器或产品首次启动时应预先创建这两个目录；如果宿主没有目录写权限，`RB_Runtime_Initialize()` 会失败。不要直接把示例改成 `SimRacebear` 来绕过权限问题，否则会读写正式软件的配置和授权数据。

## 两个示例共有的生命周期

1. 为宿主设置独立的 `appDataName`，避免覆盖 SimRacebear 的配置。
2. 初始化 SDK。
3. 启动 SDK 内部计算循环。
4. 在自己的定时器或工作线程中读取状态、提交遥测或执行命令。
5. 停止自己的定时器、串口线程和设备操作。
6. 关闭外部遥测源，再调用 SDK Shutdown。

`RB_Runtime_StartLoop()` 已经启动 SDK 的后端计算，宿主不需要高频调用 `RB_Runtime_DoCalculations()`。状态读取可以按界面需要使用 30-100 Hz 定时器；目录、配置和端口枚举属于低频操作，不要放进高频刷新循环。

## 配置和数据规则

- 修改配置时先读取完整结构体，修改需要改变的字段，再执行 Apply 和 Save。
- Apply 只改变当前运行态，Save 才写入磁盘。
- 游戏、遥测和设备的持久化标识使用稳定 key 或 UID，不要保存当前列表索引。
- JSON 接口适合兼容未知字段的工具；固定结构体接口适合正式设置页面。
- 示例默认使用合成遥测，不会自动连接真实游戏、平台或电机。

## 进一步阅读

- [公开 C API 说明](../docs/RaceBearSDK_API.md)
- [前端接入指南](../docs/RaceBearSDK_FRONTEND_GUIDE.md)
- [RB-CAN 接入指南](../docs/RaceBearSDK_RB_CAN_GUIDE.md)
- [公开头文件](../include/RaceBearSDK.h)

这些示例用于说明接入方式，不是完整的产品界面。正式产品仍需自行实现窗口、错误提示、配置编辑、线程协调和退出状态机。
