# 命名方案与便携包

从 SDK 根目录构建 examples，运行 `RaceBearProfilePackages.exe` 只显示帮助，不初始化。

```powershell
RaceBearProfilePackages.exe --rgb-roundtrip C:\Temp\lighting.json
RaceBearProfilePackages.exe --motion-roundtrip C:\Temp\motion.json
RaceBearProfilePackages.exe --haptic-roundtrip C:\Temp\haptic.json
```

显式 roundtrip 使用独立 `RaceBearProfileDemo` AppDataName，保存一个演示方案，
导出、导入并读回；不初始化计算循环、不连接设备、不修改 SimRacebear 配置。
输出文件由参数指定；重复运行会覆盖这个演示名称及目标包。
Haptic 示例先建立同名 Dynamic，再保存震动附属方案。
所有调用都经 `ExampleServices.h` 使用十入口 ABI，不导入旧函数。
业务返回 0 才成功，传输异常在 main 捕获并报告。
