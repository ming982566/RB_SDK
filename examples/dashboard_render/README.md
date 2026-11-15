# Win32 客户仪表渲染

从 SDK 根目录构建 examples，运行 `RaceBearDashboardRender.exe`。
默认使用明确标识的离线预览状态，不初始化计算、不访问用户配置或硬件；
`--runtime` 才使用独立 RaceBearDashboardDemo 配置初始化 SDK 并启动 2 ms 循环。

```powershell
RaceBearDashboardRender.exe
RaceBearDashboardRender.exe --runtime
RaceBearDashboardRender.exe --snapshot C:\Temp\dashboard.bmp
```

窗口提供模板下拉、页面下拉和导入文件动作。程序实际演示：
refresh_packages → count/info → open_package → set_package_page →
render_package_frame → get_package_state → DIBSection → StretchBlt。
导入只在用户选择文件后执行，成功后刷新并选定返回的 PackageId。
切换包先停止旧句柄使用，再关闭旧包；关闭定时器后释放 DIB、句柄，再 Shutdown。

输出采用固定 854×480 像素、32 位 BGRA、stride=width×4 的 top-down DIB。
窗口缩放只改变贴图目标矩形，保持宽高比；DPI 不改变 SDK 像素步长。
刷新定时器为 33 ms，不跟随 SDK 2 ms 循环。渲染失败清空有效帧标记并显示错误，
不把上一次帧当作成功。传输和业务返回码分别检查，状态来自同一个已打开包句柄。
未知绑定不等于打开失败；SDK 按模板绑定规则显示零/空值，不能据此显示“实时数据正常”。
像素缓冲只在同步渲染期间借用，示例 UI 线程是渲染/关闭的唯一所有者。
生产程序可放入专属渲染线程，但必须等在途帧结束后才能关闭句柄。

`--snapshot` 将同一 DIB 像素写为 BMP，便于不连接硬件的非空图像校验。
该模式不证明真实遥测或硬件输出正常。实际部署必须带同配置 DLL、libusb
和完整 DashboardAssets；详情见 [仪表指南](../../docs/RaceBearSDK_DASHBOARD_DEVELOPMENT.md)。
