# C++ 接入示例

本示例使用公开的 C ABI，演示一个 Windows 原生宿主从加载 DLL 到正常退出的完整流程：

- 检查 SDK 版本并设置独立配置目录
- 初始化 SDK 并启动内部计算循环
- 读取运行状态、游戏目录和日志
- 注册外部遥测源并提交合成数据
- 使用 JSON 命令读取授权状态
- 通过 RAII 保证退出时调用 Shutdown

示例不会自动连接游戏、平台、电机或串口，运行它不会驱动真实设备。

## 目录说明

| 文件 | 作用 |
| --- | --- |
| `main.cpp` | 最小可运行宿主和外部遥测提交循环 |
| `configuration_examples.cpp/.h` | 读取、修改、应用、保存配置的独立片段 |
| `custom_game_example.cpp/.h` | 自定义 UDP 游戏注册和数据包构造 |
| `simhub_udp_bridge/` | 可选的 SimHub Generic UDP 中继示例 |
| `CMakeLists.txt` | VS2019 x64 构建定义 |

`configuration_examples.cpp` 中的设置函数只用于展示 API 用法，`main.cpp` 不会自动调用它们，避免示例覆盖用户配置或触发真实输出。

## 构建

在 `examples\cpp` 目录打开 Visual Studio 2019 Developer PowerShell：

```powershell
$build = Join-Path $env:TEMP "RaceBearSDKCppExample-build"
cmake -S . -B $build -G "Visual Studio 16 2019" -A x64
cmake --build $build --config Release
```

运行主示例：

```powershell
& (Join-Path $env:TEMP "RaceBearSDKCppExample-build\Release\RaceBearSDKCppExample.exe")
```

运行可选的 SimHub UDP 中继：

```powershell
& (Join-Path $env:TEMP "RaceBearSDKCppExample-build\Release\RaceBearSimHubUdpBridge.exe") --help
```

CMake 会把 `bin\x64\Release\RaceBearSDK.dll` 和 `libusb-1.0.dll` 复制到每个示例的输出目录。C++ 示例使用 SDK 包中的公开头和导入库，不应包含 `src` 下的私有头文件。

如果需要 Debug 构建：

```powershell
cmake --build $build --config Debug
```

当前 CMake 文件按 Release SDK 导入库配置。要做 Debug 接入验证，应把导入库、`RaceBearSDK.dll` 和 `libusb-1.0.dll` 的复制路径一起切换到 `bin\x64\Debug`，不能只替换其中一个文件。

## 关键调用顺序

```text
RB_Runtime_SetAppDataName
RB_Runtime_Initialize
RB_Runtime_StartLoop
    ├─ RB_State_Read
    ├─ RB_Data_ReadJson / RB_Command_Run
    └─ RB_ExternalSource_SubmitFrame
RB_ExternalSource_Close
RB_Runtime_Shutdown
```

JSON 返回缓冲区大小不固定，遇到 `RB_BUFFER_TOO_SMALL` 时要扩大缓冲区重试。示例中的 `ReadJson` 和 `RunCommand` 展示了这种写法。

外部遥测每帧都应提交完整的值集合；遥测索引必须通过 `RB_Telemetry_FindIndexByKey()` 在当前 SDK 运行时解析，不能把一次运行得到的整数索引写进配置文件。

## 接入自己的程序

建议把 `RuntimeSession` 改成宿主的单例后端对象，把 `RB_State_Read()` 放入 UI 定时器或专用工作线程。UI 线程只消费快照，不要在绘制回调里执行设备扫描、配置保存或阻塞式命令。

接入真实输出时，退出顺序应是：

1. 停止 UI 定时器和宿主工作线程。
2. 停止手动测试、运动输出和自有串口/CAN 操作。
3. 等待自己的异步任务结束。
4. 关闭外部遥测源。
5. 调用 `RB_Runtime_Shutdown()`。

控制台示例的简单析构只说明 SDK 生命周期，不等于真实设备的安全退出流程已经完成。

## 常见问题

- **找不到 DLL**：确认 `RaceBearSDK.dll` 与 EXE 同目录，并确认 EXE 和 DLL 都是 x64。
- **加载 DLL 失败但文件存在**：确认 `libusb-1.0.dll` 也在 EXE 同目录，并确认安装了 Microsoft Visual C++ 2015-2022 Redistributable x64。
- **链接失败**：确认使用 `bin\x64\Release\RaceBearSDK.lib`，并使用 VS2019/v142。
- **`RB_Runtime_Initialize` 返回错误**：确认当前账户可以创建和写入 `%APPDATA%\<AppDataName>`、`Documents\<AppDataName>`；示例的独立目录不能使用只读目录。
- **状态一直没有遥测**：示例只提交合成遥测；真实游戏需要先配置并连接对应游戏源。
- **配置没有保存**：Apply 只更新当前运行态，必须显式调用 Save。
- **中文日志乱码**：源码按 UTF-8 保存，CMake 已加入 `/utf-8`。
