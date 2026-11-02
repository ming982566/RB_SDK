# Python 接入示例

本示例使用 Python 标准库 `ctypes` 调用 `RaceBearSDK.dll`，不需要安装任何 pip 包。它展示了读取状态、读取 JSON、执行命令、提交外部遥测和读取 SDK 日志。

## 环境要求

- Windows 10/11 x64
- 64 位 Python 3.8 或更高版本
- SDK 包中的 `bin\x64\Release\RaceBearSDK.dll`
- SDK 包中的 `bin\x64\Release\libusb-1.0.dll`
- Microsoft Visual C++ 2015-2022 Redistributable x64

检查 Python 位数：

```powershell
py -3 -c "import struct; print(struct.calcsize('P') * 8)"
```

输出必须是 `64`。Python 32 位进程不能加载 x64 SDK。

## 运行

从 SDK 包根目录执行：

```powershell
cd examples\python
py -3 example.py
```

`racebear_sdk.py` 默认从 SDK 根目录的 `bin\x64\Release` 查找 DLL。也可以显式指定路径：

```python
from racebear_sdk import RaceBearSDK

sdk = RaceBearSDK(r"C:\MyProduct\RaceBearSDK.dll")
sdk.initialize("MyProduct", interval_ms=2)
try:
    state = sdk.read_state()
finally:
    sdk.close()
```

`RaceBearSDK.dll` 和 `libusb-1.0.dll` 必须位于同一目录，或者该目录已加入 Windows DLL 搜索路径。示例默认使用独立的 `RaceBearPythonExample` 配置身份；运行账户必须能写入 `%APPDATA%\RaceBearPythonExample` 和 `Documents\RaceBearPythonExample`。不要为了绕过权限问题改用 `SimRacebear`，否则示例会覆盖正式软件的配置。

## 文件说明

| 文件 | 作用 |
| --- | --- |
| `racebear_sdk.py` | 公开 C ABI 的 ctypes 结构体、函数签名和常用 Python 方法 |
| `example.py` | 十秒钟可运行的控制台接入流程 |

封装没有依赖第三方包，便于直接复制到自己的 Windows 工具中。结构体字段顺序、数组长度和基础类型必须与同一版本的 `include\RaceBearSDK.h` 保持一致。

## 示例流程

`example.py` 的调用顺序是：

1. 创建 `RaceBearSDK`。
2. 设置独立的应用目录并初始化。
3. 启动 SDK 内部计算循环。
4. 读取游戏目录和授权命令结果。
5. 打开合成外部遥测源。
6. 每帧提交稳定 key 对应的遥测值。
7. 读取运行状态和日志。
8. 关闭外部源并关闭 SDK。

Python 循环只是示例 UI 刷新，不负责驱动 SDK 后端计算。正式程序可以用自己的定时器读取 `read_state()`，建议把配置读取、设备扫描和保存操作放在低频任务中。

## 配置示例

封装提供了常用设置的“读取、修改、应用、保存”方法：

```python
wind = sdk.read_wind()
wind.InputMinKmh = 20.0
wind.InputMaxKmh = 180.0
wind.MasterGainPercent = 100.0
sdk.save_wind(wind, apply=True)
```

Apply 只更新当前运行态，Save 才写入磁盘。读取完整结构体后再修改字段，可以避免覆盖页面没有显示的设置。

## ctypes 注意事项

- DLL、Python 和宿主程序必须同为 64 位。
- 不要给 SDK 结构体自行添加 `_pack_ = 1`；SDK 使用 MSVC 默认对齐。
- SDK 升级后要重新核对结构体大小、关键字段偏移、数组长度和实际调用结果。
- 如果使用 SDK 回调，必须长期保存回调对象，避免被 Python 垃圾回收。
- Qt、Tk 或其它 UI 框架的回调如果来自 SDK 工作线程，应通过线程安全队列或框架信号转发到 UI 线程。

## 常见问题

- **WinError 193**：通常是加载了 32 位 Python 或 32 位 DLL。
- **找不到 DLL**：使用绝对路径创建 `RaceBearSDK`，或确认 SDK 目录结构没有被拆散。
- **DLL 文件存在但仍无法加载**：确认 `libusb-1.0.dll` 与 `RaceBearSDK.dll` 在同一目录，并安装 Microsoft Visual C++ 2015-2022 Redistributable x64。
- **状态没有变化**：示例只使用合成遥测；真实游戏需要先配置并连接游戏源。
- **配置修改无效**：确认调用了 `save_xxx(..., apply=True)`，并检查返回的异常信息。
- **`RB_Runtime_Initialize` 返回 -1**：优先检查独立 AppDataName 对应的 `%APPDATA%` 和 `Documents` 目录是否可创建、可写，以及是否有另一个进程正在占用该目录。
- **中文内容异常**：`decode_utf8()` 用于读取 SDK 返回的 UTF-8 固定字符数组。
