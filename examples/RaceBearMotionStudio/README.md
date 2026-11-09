# RaceBear Motion Studio

SDK 1.0.2 的原生 Win32 C++17 控制面板示例，使用 VS2019/v142 x64。
仅导入统一 ABI；页面通过 `../common/ExampleServices.h` 调用公开对象服务，
高频运行态和遥测使用完整对象帧。没有 Python 或旧业务 DLL 绑定。

打开 `RaceBearMotionStudio.sln`，选择 Debug/Release、x64；或从 SDK 根目录构建：

```powershell
cmake -S examples -B obj/cpp-examples -G "Visual Studio 16 2019" -A x64 -T v142
cmake --build obj/cpp-examples --config Release
.\obj\cpp-examples\RaceBearMotionStudio\Release\RaceBearMotionStudio.exe
```

两种构建都复制同配置 SDK DLL 和 libusb，链接同配置导入库。
示例数据目录使用 `RaceBearMotionStudio`，启动时会初始化独立目录并开启 SDK 2 ms 循环，
不会自动连接真实输出。连接、串口打开和测试需要用户明确操作。

Motion 仍受授权约束；Wind、RGB、安全带和 Haptic 沿用各自无需授权的产品合同，
设备状态、急停、释放和机械保护继续由 SDK 校验。示例未覆盖 SDK 的每项功能。
同步服务可能阻塞，此演示的页面操作不能作为生产 UI 的后台调度方案。

关闭窗口先停止 UI 定时器和页面回调、清零测试、请求断开，再 Shutdown。
协议传输异常在 Win32 回调边界报告，清理阶段失败不会跳过其它清理步骤。
离线编译和导入表检查不替代真实 UI、游戏、USB 长稳及机械保护验收。

参阅[接入指南](../../docs/RaceBearSDK_FRONTEND_GUIDE.md)、
[对象协议](../../docs/RaceBearSDK_OBJECT_API.md)与
[服务目录](../../docs/RaceBearSDK_SERVICE_API.md)。
