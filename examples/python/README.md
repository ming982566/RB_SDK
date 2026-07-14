# Python 示例

本示例使用 Python 标准库 `ctypes`，不需要安装任何 pip 包。

`racebear_sdk.py` 已映射动态滤波、输出、运动增强、震动、风感和安全带结构体。常规设置页面应使用 `read_dynamic_profile()`、`read_output()`、`read_wind()` 等结构体方法，不需要读取或手工编辑配置 JSON。

例如修改风感配置：

```python
wind = sdk.read_wind()
wind.InputMinKmh = 20.0
wind.InputMaxKmh = 180.0
wind.MasterGainPercent = 100.0
sdk.save_wind(wind, apply=True)

# 测试结束必须关闭，否则测试值会继续覆盖实时风感。
sdk.set_wind_test(True, 50.0)
sdk.set_wind_test(False)
```

要求：

- Windows x64。
- 64 位 Python 3.8 或更高版本。
- Microsoft Visual C++ 2015-2022 Redistributable x64。
- `bin\x64\Release\RaceBearSDK.dll` 已由 SDK 项目生成。

运行：

```powershell
cd D:\AI_Workspace\RaceBear_SDK\examples\python
py -3 example.py
```

`racebear_sdk.py` 默认从 SDK 目录的 `bin\x64\Release` 查找 DLL，也可以手动传入路径：

```python
from racebear_sdk import RaceBearSDK

sdk = RaceBearSDK(r"C:\MyApp\RaceBearSDK.dll")
sdk.initialize("MyProduct", 100)
try:
    state = sdk.read_state()
finally:
    sdk.close()
```

Python 结构体布局必须与同一版本的 `RaceBearSDK.h` 保持一致。升级 SDK 后，如果 `RB_RuntimeState.Version` 或公开结构体发生变化，需要同步更新 `racebear_sdk.py`。

SDK 或串口回调可能在非 Python UI 线程触发。PySide/PyQt 前端应通过线程安全队列或 Qt Signal 把结果转发到主线程，并长期保存 `ctypes` 回调对象，防止被垃圾回收。
