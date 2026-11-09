# C++ 接入示例

仅提供 C++ 示例，不包含 Python。所有示例只依赖公开头和 10 个统一 ABI。

| 目录 | 内容 |
|---|---|
| `object_api` | 直接演示目录、默认值、版本草稿、异步任务和整帧遥测 |
| `cpp` | 控制台生命周期、外部遥测、配置片段和自定义 UDP 数据包 |
| `cpp/simhub_udp_bridge` | 接收 SimHub Generic UDP 最终轴值，提交 SDK CAN 输出 |
| `RaceBearMotionStudio` | Win32 控制面板，含平台、动态、效果、输出和串口页面 |
| `common` | 示例共用的本机服务包装和按配置匹配的构建设置 |

从 SDK 根目录使用 VS2019/v142 x64 构建全部示例：

```powershell
cmake -S examples -B obj/cpp-examples -G "Visual Studio 16 2019" -A x64 -T v142
cmake --build obj/cpp-examples --config Debug
cmake --build obj/cpp-examples --config Release
.\obj\cpp-examples\object_api\Release\RaceBearObjectExample.exe
.\obj\cpp-examples\cpp\Release\RaceBearSDKCppExample.exe
.\obj\cpp-examples\cpp\Release\RaceBearSimHubUdpBridge.exe --help
```

也可单独构建子目录；必须保留相邻的 `common` 目录。构建会复制同配置
`RaceBearSDK.dll` 和 `libusb-1.0.dll`，导入库也使用相同配置，不能混配。
运行机器需要 Microsoft Visual C++ x64 运行库。

两个控制台示例默认只做初始化前的只读检查。显式加 `--runtime` 才初始化
独立数据目录：`RaceBearObjectDemo` 或 `RaceBearCppExample`；运行账户须有
AppData/Documents 写权限。不会连接真实输出，但会创建隔离目录。
UDP 中继按其 README 配置并连接真实 CAN 输出；MotionStudio 由用户明确点击连接或测试。

`common/ExampleServices.h` 是宿主源码包装，使用 Resolve + Write(NATIVE_CALL)，
不是旧函数导出或额外 ABI。`ObjectClient.h` 说明按位封装、每次调用独占结果、
传输失败与业务结果的区别。高频状态/遥测通过 Read 读取完整对象帧。
同步服务仍可能阻塞；生产 UI 应把扫描、命令和设备操作放到自己的后台线程。

异步 Invoke 只提交一次，Poll 扩容读取原任务结果，不能重发动作。
object_api 等待真实终态；示例没有强制取消机制。退出前先停止宿主调用、测试和回调，
关闭自有句柄、消费任务，再 Shutdown；失败返回与硬件安全状态必须分别确认。

参数与边界以[公开头](../include/RaceBearSDK.h)、
[对象协议](../docs/RaceBearSDK_OBJECT_API.md)、
[同步服务目录](../docs/RaceBearSDK_SERVICE_API.md)和
[接入指南](../docs/RaceBearSDK_FRONTEND_GUIDE.md)为准。
