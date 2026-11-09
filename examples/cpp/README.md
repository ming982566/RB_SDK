# C++ 控制台示例

本示例使用 10 个统一 ABI。`rb_example::*` 是 `../common/ExampleServices.h`
中的宿主 C++ 包装，通过 Resolve/Write 调用公开服务，不链接旧业务函数。

| 文件 | 用途 |
|---|---|
| `main.cpp` | 初始化前默认值检查；`--runtime` 启动隔离会话并提交约十秒合成遥测 |
| `configuration_examples.cpp/.h` | 配置读取、修改、应用和保存片段，main 不自动执行 |
| `custom_game_example.cpp/.h` | 自定义 UDP 游戏注册与数据包构造 |
| `simhub_udp_bridge` | 需要用户配置真实 CAN 输出的可选中继 |

在 SDK 根目录构建和运行：

```powershell
cmake -S examples/cpp -B obj/cpp-example -G "Visual Studio 16 2019" -A x64 -T v142
cmake --build obj/cpp-example --config Release
.\obj\cpp-example\Release\RaceBearSDKCppExample.exe
.\obj\cpp-example\Release\RaceBearSDKCppExample.exe --runtime
```

Debug 同样可用，CMake 自动选择对应 SDK DLL/LIB 和 libusb，不能混配。
`--runtime` 使用独立 `RaceBearCppExample` 配置目录，要求当前账户有写权限。
示例不自动执行配置片段，也不自动连接真实输出。

调用顺序：同步服务 `rb.runtime.set_app_data_name` → Initialize →
`rb.runtime.start_loop` → Read(`rb.runtime.state`)/公开服务 → 关闭外部源 → Shutdown。
遥测索引由稳定 key 在当前运行时解析，不持久化列表索引。
JSON 只读数据可扩容重读；动作只执行一次，容量不足报告失败。
日志容量不足时保留队首，扩容后读取同一条日志；每个 SDK 会话只指定一个日志消费者。

业务返回码由调用点检查；传输失败抛出异常，由 main 捕获并报告，RAII 关闭 SDK。
真实产品仍需停止工作线程、清零测试、断开设备并消费任务后再关闭 SDK。
完整边界见[示例入口](../README.md)和[同步服务目录](../../docs/RaceBearSDK_SERVICE_API.md)。
