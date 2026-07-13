# RaceBear SDK 示例

这些示例只依赖公开的 `include/RaceBearSDK.h` 和 `bin/x64/Release/RaceBearSDK.dll`，用于给第三方前端验证最小接入流程。

| 目录 | 适用开发方式 | 是否可直接运行 |
| --- | --- | --- |
| `cpp` | Visual Studio 2019、CMake、标准 C++17 | 是 |
| `python` | 64 位 Python 3.8+、`ctypes` | 是，不需要 pip 依赖 |
| `qt` | Qt 5/Qt 6 C++、PySide/PyQt | 当前提供完整接入代码和项目配置，本机未安装 Qt，未做 Qt 编译 |

建议先运行 `cpp` 或 `python` 示例确认 SDK 环境正常，再把相同生命周期接入正式前端。

所有示例都遵循相同顺序：设置应用目录名、初始化 SDK、启动内部计算循环、读取状态或执行命令、停止前端调用、关闭 SDK。

关键约束：

- SDK 当前只提供 Windows x64 版本，C++ 示例按 VS2019/v142 验证。
- 示例使用独立 `appDataName`，避免覆盖正式产品的配置和授权文件。
- 后端计算由 SDK 内部 `MultimediaTimer` 驱动，前端只需要定时读取状态、写配置或发送命令。
- 退出前应先停止前端自己的 UI 定时器、串口线程和工作线程，再调用 `RB_Runtime_Shutdown()`。
- 真实发布时，把 `RaceBearSDK.dll`、同目录依赖库和 Microsoft Visual C++ 2015-2022 Redistributable x64 一起纳入部署检查。
