# C++ 示例

本示例使用公开 C ABI，演示 SDK 初始化、内部计算循环、运行状态、JSON 数据包、Command、日志和正常退出。

`configuration_examples.cpp` 另外提供按 DOF 候选目录添加动态通道、Serial 输出、运动增强、震动、风感、游戏遥测 Source、功能插件和手动平台测试代码。运动增强索引来自独立的运动增强目录；震动 `ByIndex` 接口使用动态配置索引查找同名节点。为了避免示例启动后覆盖用户配置或驱动真实平台，这些函数只参与编译验证，不由 `main.cpp` 自动调用。

## 构建

在 Visual Studio 2019 Developer PowerShell 中执行：

```powershell
cd D:\AI_Workspace\RaceBear_SDK\examples\cpp
cmake -S . -B build -G "Visual Studio 16 2019" -A x64
cmake --build build --config Release
```

运行：

```powershell
.\build\Release\RaceBearSDKCppExample.exe
```

CMake 会把 `RaceBearSDK.dll` 自动复制到示例 EXE 目录。目标电脑需要 Microsoft Visual C++ 2015-2022 Redistributable x64。

示例源码为 UTF-8 无 BOM，并包含中文注释。CMake 已自动为 VS2019 加入 `/utf-8`；如果把 `main.cpp` 加入自己的 VS 工程，也要在“C/C++ -> 命令行”中加入 `/utf-8`。

示例运行十秒后正常退出，不会自动连接硬件。真实前端应把 `RB_State_Read()` 放入自己的 UI 定时器，并在退出前停止所有前端定时器和工作线程。
