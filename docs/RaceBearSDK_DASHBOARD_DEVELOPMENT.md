# RaceBear SDK 仪表开发指南

本文面向使用 RaceBear SDK 开发仪表、USB 屏幕、浏览器仪表或宿主窗口预览的应用开发者。文档说明从模板制作、数据绑定、渲染到输出部署的完整流程。

本文以 SDK `1.0.1` 的公开 C ABI 为准。函数的精确签名、结构体布局、返回值和字段容量仍以同一版本的 `include/RaceBearSDK.h` 为最终依据；仪表接入使用对象协议和服务目录，不直接导入旧业务符号。
采用对象 ABI 时，本文 Dashboard/VOCORE/Web 操作可用 [同步服务映射](RaceBearSDK_SERVICE_API.md)。
像素指针和句柄通过 NATIVE_CALL 同步借用，不转 JSON、不进入对象任务队列；
原业务对帧的复制、最新帧替换、授权和关闭规则不变。不能把本机指针封套发送到网页或网络。
本文示例中的 `rb.dashboard.*`、`rb.vocore.*` 和 `rb.dashboardweb.*` 是服务操作 key 的概念记法；
实际调用必须先 Resolve 对应 `rb.service.*`，再用 `RB_ObjectNativeCall` 按目录签名分派。

## 1. 能力边界

仪表链路由三个独立层组成：

```text
rb.runtime.state()
        |
        v
仪表渲染器
  |              |
  |              +--> rb.dashboard.render_frame()
  |              +--> rb.dashboard.render_package_frame()
  |
  +--> 调用方窗口、纹理或视频编码器
  +--> rb.vocore.submit_frame()
  +--> rb.dashboardweb.submit_frame()
```

SDK 负责读取运行态、解析稳定数据绑定、生成 BGRA8888 像素帧、管理客户模板和提供 VOCORE/Web 输出后端。宿主应用负责窗口布局、刷新定时器、输出目标选择、错误提示和生命周期管理。

当前版本支持：

- 四套 SDK 内置仪表模板；
- 客户自定义 `.rbdash` V1 单文件模板；
- 设计器 `tools/DashboardDesigner`；
- VOCORE USB 屏幕异步发送；
- 浏览器 MJPEG 仪表服务；
- 将同一帧交给宿主自己的窗口、纹理或编码器。

当前版本不支持：

- 在模板内执行 JavaScript、Lua、DLL 或其它脚本；
- 模板访问文件、网络、进程或 SDK 控制接口；
- 从仪表模板反向修改运动、输出、配置或设备状态；
- 触摸输入和仪表内交互控制；
- 任意表达式语言。数据绑定只能使用 SDK 认可的稳定 `rb.*` key。

## 2. 选择仪表类型

内置模板适合快速集成。使用 `rb.dashboard.render_frame()` 时只需要传入运行态、目标画布和模板枚举值，不需要维护模板文件。

```cpp
RB_DASHBOARD_TEMPLATE_GT          // AIM MXS 风格
RB_DASHBOARD_TEMPLATE_VIRTUAL_RING // BOSCH DDU8 风格
RB_DASHBOARD_TEMPLATE_M_STRIP    // SimpleDash 风格
RB_DASHBOARD_TEMPLATE_FORD_GT    // Ford GT 风格
```

客户模板适合需要自行决定布局、颜色、字体、页面和数据绑定的产品。客户模板使用 `rb.dashboard.refresh_packages()` 扫描，按稳定 `PackageId` 打开，并通过 `rb.dashboard.render_package_frame()` 渲染。

内置模板保存的是模板枚举值；客户模板保存的是 `PackageId`。目录索引只在当前扫描快照内有效，不能写入产品配置作为长期标识。

## 3. 运行前准备

宿主进程、头文件、导入库和 DLL 必须使用同一 SDK 构建和同一架构：

- Windows x64 宿主使用 `bin/x64/Debug` 或 `bin/x64/Release` 中匹配的 `.h/.lib/.dll`；
- 使用 VOCORE 时，将同一目录的 `libusb-1.0.dll` 放在宿主 EXE 同级；
- 使用 Ford GT 或随附客户模板时，将 `DashboardAssets/RaceBear` 保持目录结构部署到 `RaceBearSDK.dll` 同级；
- 目标电脑需要 Microsoft Visual C++ 2015-2022 Redistributable x64。

初始化顺序：

```cpp
rb.runtime.set_app_data_name("MyDashboardApp");
RB_Runtime_Initialize();
rb.runtime.start(2); // 单位为 ms；仪表刷新不跟随这个周期
```

仪表刷新建议由宿主自己的 16-33 ms 定时器驱动。SDK 后端循环可以按 1-2 ms 计算，但不应让每个后端 Tick 都生成一张仪表画面。

## 4. 使用 Dashboard Designer

设计器位于 `tools/DashboardDesigner`，不依赖 Python、Node 或本机 HTTP 服务。

1. 直接打开 `index.html`，或双击 `Start-DashboardDesigner.cmd`。
2. 选择画布尺寸和页面。
3. 在元素面板中添加文本、矩形、椭圆、直线、进度条、图片或圆形指针表。
4. 为需要显示实时值的元素选择稳定 `rb.*` 数据绑定。
5. 使用模拟数据检查刻度、颜色区间、文本格式和可见条件。
6. 导出单个 `.rbdash` 文件。
7. 在 SDK 中重新导入并校验；设计器通过不代表运行时一定接受，SDK 校验才是最终结果。

随 SDK 提供的 `assets/Dashboard/RaceBear/Custom/RaceBearStarter.rbdash` 是最小可编辑示例，设计尺寸为 `854x480`，包含车速、档位、转速表和转速条。

## 5. `.rbdash` V1 文件格式

`.rbdash` 是 UTF-8 无 BOM 的单文件 JSON。根对象必须包含 `formatVersion: 1`、合法 `id`、设计尺寸和至少一个页面。

### 5.1 根字段

| 字段 | 规则 |
| --- | --- |
| `formatVersion` | 固定为整数 `1`。 |
| `id` | 稳定模板 ID，只允许 ASCII 字母、数字、点、下划线和短横线。 |
| `name` | UI 显示名称，UTF-8 文本。 |
| `author` | 作者或产品名称。 |
| `description` | 模板说明；加载错误也会写入目录信息的说明字段。 |
| `designWidth` / `designHeight` | 设计画布尺寸，范围 `64-4096`。 |
| `backgroundColor` | `#RRGGBB` 或 `#AARRGGBB`。 |
| `assets` | 可选图片资源对象，资源内容必须是 PNG/JPEG Data URI。 |
| `pages` | 至少 1 页，最多 16 页。 |

每页有 `name` 和 `elements`。单页最多 512 个元素。元素使用设计坐标 `x/y/width/height`，渲染器会保持设计宽高比等比缩放并居中。

### 5.2 V1 元素

| 类型 | 作用 |
| --- | --- |
| `text` | 固定文本或绑定数值/文本。 |
| `rect` | 矩形背景、边框或面板。 |
| `ellipse` | 椭圆或圆形装饰。 |
| `line` | 分隔线和刻度线。 |
| `bar` | 按绑定数值填充的水平或垂直条。 |
| `image` | 内嵌 PNG/JPEG 图片。 |
| `gauge` | SDK 原生圆形指针仪表，支持刻度、指针和颜色区间。 |

`gauge` 常用字段包括 `minimum`、`maximum`、`startAngle`、`endAngle`、`tickCount`、`lineWidth`、`needleColor` 和 `showValue`。`bar` 和 `gauge` 都可以使用 `zones`，按 `minimum` 从小到大选择最后一个匹配的颜色。

### 5.3 数值显示字段

绑定数值的元素可以使用：

- `scale`：先乘以比例；
- `offset`：再加上偏移；
- `minimum` / `maximum`：数值范围和可视化夹紧范围；
- `decimals`：小数位数；
- `prefix` / `suffix`：显示前后缀；
- `format`：`number`、`integer`、`percent`、`gear` 或 `time`。

可见条件使用 `visibleBinding`、`visibleMinimum` 和 `visibleMaximum`。V1 不提供任意布尔表达式，复杂条件应由 SDK 提供稳定状态 key 或由宿主选择另一页。

### 5.4 最小模板示例

下面的片段显示一个绑定车速的文本元素：

```json
{
  "formatVersion": 1,
  "id": "example.speed",
  "name": "Speed Example",
  "designWidth": 854,
  "designHeight": 480,
  "backgroundColor": "#FF101318",
  "pages": [
    {
      "name": "Main",
      "elements": [
        {
          "type": "text",
          "x": 40,
          "y": 40,
          "width": 300,
          "height": 60,
          "binding": "rb.dashboard.speed_kmh",
          "format": "integer",
          "suffix": " km/h",
          "fontSize": 32
        }
      ]
    }
  ]
}
```

模板不能保存运行时索引、数组位置或目录行号。数据绑定必须保存稳定 key。

## 6. 稳定数据绑定

模板加载后，SDK 会把稳定 key 解析为当前进程中的遥测索引。模板作者不需要知道索引值，也不能把索引写入 `.rbdash`。

普通遥测使用公开 `rb.*` key。仪表专用只读 key 包括：

| Key | 类型 | 含义 |
| --- | --- | --- |
| `rb.dashboard.game` | 文本 | 当前游戏名称。 |
| `rb.dashboard.vehicle` | 文本 | 当前车辆名称。 |
| `rb.dashboard.track` | 文本 | 当前赛道名称。 |
| `rb.dashboard.position` | 数值 | 当前比赛位置。 |
| `rb.dashboard.active` | 数值 | 当前仪表运行态，0 或 1。 |
| `rb.dashboard.speed_kmh` | 数值 | 由 SDK 转换的 km/h 车速。 |
| `rb.dashboard.license_output_allowed` | 数值 | 当前是否允许真实输出，0 或 1。 |
| `rb.dashboard.rig.sway` | 数值 | 平台最终 Sway。 |
| `rb.dashboard.rig.surge` | 数值 | 平台最终 Surge。 |
| `rb.dashboard.rig.heave` | 数值 | 平台最终 Heave。 |
| `rb.dashboard.rig.yaw` | 数值 | 平台最终 Yaw。 |
| `rb.dashboard.rig.roll` | 数值 | 平台最终 Roll。 |
| `rb.dashboard.rig.pitch` | 数值 | 平台最终 Pitch。 |

车速、转速和平台数据的单位以遥测目录和公开 API 为准。模板不应通过名称猜测单位；需要换算时使用 `scale`、`offset` 和格式字段。

授权锁定时，仪表仍可以显示游戏和目录信息，但受保护的最终输出值可能为 0。需要提示用户时，使用 `rb.dashboard.license_output_allowed` 控制警告元素可见性。

## 7. 客户模板目录和生命周期

SDK 扫描两个位置：

- 内置客户模板：DLL 同级 `DashboardAssets/RaceBear/Custom`；
- 用户模板：通常为 `Documents/<AppDataName>/Dashboards`，实际路径通过 `rb.dashboard.get_user_package_directory()` 查询。

启动或用户点击刷新时：

```cpp
const int count = rb.dashboard.refresh_packages();
if (count < 0) {
    // 读取日志或向用户显示刷新失败。
}

for (int i = 0; i < rb.dashboard.get_package_count(); ++i) {
    RB_DashboardPackageInfo info{};
    info.Size = sizeof(info);
    if (rb.dashboard.get_package_info(i, &info) == RB_OK) {
        // 只把 info.Valid == 1 的模板放入可选列表。
        // 持久化 info.PackageId，不持久化 i。
    }
}
```

`RB_DashboardPackageInfo` 的 `Valid` 为 0 时，模板仍可用于诊断页面显示错误，但不应作为可打开项目。`FilePath` 仅用于诊断和编辑器定位，不应作为跨电脑配置中的稳定标识。

导入和删除：

```cpp
char packageId[RB_TEXT_SMALL]{};
int rc = rb.dashboard.import_package(
    sourcePathUtf8,
    packageId,
    static_cast<int>(sizeof(packageId)));

if (rc == RB_OK) {
    rb.dashboard.refresh_packages();
    // 使用返回的 packageId 重新定位模板。
}

rb.dashboard.delete_package(packageId);
```

导入接口会把文件复制到当前 AppDataName 的用户目录并重新校验。删除接口只允许删除已扫描到的用户模板，不能删除 DLL 随附的内置模板。

打开、切页、渲染和关闭：

```cpp
RB_DashboardPackageHandle package = nullptr;
if (rb.dashboard.open_package(packageId, &package) == RB_OK) {
    rb.dashboard.set_package_page(package, 0);
    // 在刷新定时器中重复调用 rb.dashboard.render_package_frame。

    RB_DashboardPackageState packageState{};
    packageState.Size = sizeof(packageState);
    rb.dashboard.get_package_state(package, &packageState);

    rb.dashboard.close_package(package);
    package = nullptr;
}
```

选择另一个模板前先关闭旧句柄。`rb.dashboard.close_package()` 之后句柄立即失效，不得继续读状态或渲染。

## 8. BGRA8888 渲染

内置和客户模板都生成调用方提供的 BGRA8888 帧。调用方必须保证：

- `stride >= width * 4`；
- `dataSize >= stride * height`（提交到输出后端时）；
- `pixels` 在调用期间可写；
- `width`、`height` 和 `stride` 与缓冲区实际布局一致。

内置模板：

```cpp
const int width = 854;
const int height = 480;
const int stride = width * 4;
std::vector<unsigned char> pixels(
    static_cast<size_t>(stride) * height);

RB_RuntimeState runtime{};
if (rb.runtime.state(&runtime) == RB_OK &&
    rb.dashboard.render_frame(
        &runtime,
        pixels.data(),
        width,
        height,
        stride,
        RB_DASHBOARD_TEMPLATE_GT) == RB_OK) {
    // pixels 是一帧可交给窗口、VOCORE 或 Web 的 BGRA8888 画面。
}
```

客户模板只替换渲染函数：

```cpp
rb.dashboard.render_package_frame(
    package,
    &runtime,
    pixels.data(),
    width,
    height,
    stride);
```

SDK 内置模板通常以 `854x480` 为横屏设计基准，客户模板使用文件中的 `designWidth/designHeight`。渲染器会按目标画布等比缩放并居中，不会拉伸改变设计比例。VOCORE 的旋转属于输出适配，不属于模板渲染。

## 9. 输出到宿主窗口

仪表渲染函数只写像素，不创建窗口，也不依赖 MFC、Qt、炫彩或其它 UI 框架。宿主可以：

1. 在窗口绘制回调中把 BGRA8888 转换为目标纹理；
2. 使用 GDI、Direct2D、Direct3D 或 Qt 图像类显示；
3. 把帧交给自己的视频编码器；
4. 同时提交到多个输出目标。

窗口绘制线程只应读取已经生成好的帧。不要在窗口绘制回调中直接执行慢速模板导入、磁盘读取或设备打开。

## 10. 输出到 VOCORE

VOCORE 设备每块屏幕使用一个独立句柄和发送线程。

```cpp
RB_VoCoreHandle screen = nullptr;
if (rb.vocore.open(deviceIdUtf8, 1, &screen) == RB_OK) {
    const int rc = rb.vocore.submit_frame(
        screen,
        pixels.data(),
        static_cast<int>(pixels.size()),
        width,
        height,
        stride,
        RB_VOCORE_PIXEL_BGRA8888,
        0); // 允许 0、90、180、270
}
```

设备枚举使用 `rb.vocore.get_device_count()` 和 `rb.vocore.get_device_info()`。如果屏幕可能在程序启动后才插入，可以传入空 `deviceId` 并启用 `autoReconnect=1`，然后通过 `rb.vocore.get_state()` 显示等待设备、重连和发送统计。

`rb.vocore.submit_frame()` 会复制调用方的像素数据，返回后调用方可以复用缓冲区。未发送的旧帧会被新帧覆盖，不能把 `PendingFrame` 当成必须清零的发送确认。

设备状态页面建议显示：

- `DevicePresent`、`Open`、`Reconnecting`；
- `ActualFps`；
- `SubmittedFrames`、`SentFrames`、`DroppedFrames`；
- `LastErrorText`。

## 11. 输出到浏览器 Web 仪表

Web 输出启动一个轻量 HTTP MJPEG 服务。配置和状态结构体必须先填写 `Size` 和 `Version`。

```cpp
RB_DashboardWebConfig config{};
config.Size = sizeof(config);
config.Version = 1;
config.Port = 24738;
config.AllowRemote = 0; // 仅本机；局域网访问时设为 1
config.JpegQuality = 85;
config.MaxFps = 30;

RB_DashboardWebHandle web = nullptr;
if (rb.dashboardweb.open(&config, &web) == RB_OK) {
    RB_DashboardWebState state{};
    state.Size = sizeof(state);
    if (rb.dashboardweb.get_state(web, &state) == RB_OK) {
        // 在宿主 UI 中显示 state.Url。
    }
}
```

在刷新定时器中提交同一张像素帧：

```cpp
rb.dashboardweb.submit_frame(
    web,
    pixels.data(),
    static_cast<int>(pixels.size()),
    width,
    height,
    stride,
    RB_VOCORE_PIXEL_BGRA8888,
    0);
```

`AllowRemote=1` 会监听本地网络接口，首次运行可能触发 Windows 防火墙提示。只需要本机预览时保持为 0。

通过 `rb.dashboardweb.get_state()` 读取 `Url`、`ClientCount`、`ActualFps`、`SubmittedFrames`、`EncodedFrames`、`DroppedFrames` 和 `LastErrorText`。慢客户端不会让旧帧无限排队，尚未编码的旧帧会被最新提交覆盖。

## 12. 刷新频率、线程和最新帧策略

推荐线程模型：

| 工作 | 线程和频率 |
| --- | --- |
| SDK 运动和遥测计算 | SDK 自有 `rb.runtime.start()` 线程。 |
| `rb.runtime.state()` | 宿主状态或仪表线程，通常 30-60 Hz。 |
| 仪表渲染 | 独立 16-33 ms 定时器，默认 30 FPS。 |
| VOCORE/Web 提交 | 与仪表渲染线程串行提交，后端异步发送或编码。 |
| 目录刷新和模板导入 | 用户操作或页面打开时，不能高频轮询。 |
| 状态统计 | UI 定时器读取，不在输出回调中直接修改 UI。 |

不要在仪表线程调用 `rb.runtime.do_calculations()`，也不要使用 1-2 ms 的 SDK 后端周期驱动 GDI、图片解码或网络编码。

每一帧的推荐流程是：

1. 调用 `rb.runtime.state()` 获取一个完整快照；
2. 使用同一份快照渲染内置或客户模板；
3. 把完整帧提交给需要的输出目标；
4. 立即进入下一次刷新，不等待 `PendingFrame` 归零；
5. 根据状态结构体中的实际 FPS 和丢帧数调节显示。

同一 `RB_DashboardPackageHandle` 不应被多个线程同时设置页面和渲染。需要切页时在渲染线程或受互斥保护的 UI 命令队列中执行。

## 13. 退出顺序

退出前必须先停止宿主定时器和工作线程，确保不再提交新帧。然后按以下顺序释放：

```cpp
if (web != nullptr) {
    rb.dashboardweb.close(web);
    web = nullptr;
}

if (screen != nullptr) {
    rb.vocore.close(screen);
    screen = nullptr;
}

if (package != nullptr) {
    rb.dashboard.close_package(package);
    package = nullptr;
}

RB_Runtime_Shutdown();
```

多块屏幕要分别关闭每个 VOCORE 句柄。关闭后不能再调用对应状态、提交或重连接口。`RB_Runtime_Shutdown()` 应在所有宿主线程停止、所有仪表和设备句柄关闭之后调用一次。

## 14. 资源和安装部署

发布包至少检查：

- `RaceBearSDK.dll`、`.lib`、公开头来自同一构建；
- 使用 VOCORE 时 DLL 同级存在匹配的 `libusb-1.0.dll`；
- 使用 Ford GT 或随附客户模板时 DLL 同级存在 `DashboardAssets/RaceBear`；
- `tools/DashboardDesigner` 作为开发工具随包提供；
- 客户模板被导入到目标电脑当前 `AppDataName` 对应的用户目录；
- 不把开发机的绝对 `FilePath` 写入产品配置；
- 不把用户模板目录中的文件复制回 SDK 真源或公共仓库。

安装器只需要把公开运行时依赖和资源放在规定目录。用户模板由 SDK 的导入 API 管理，应用不应自行解析、修改或重命名已导入文件来绕过校验。

## 15. 错误处理和诊断

常见返回值：

| 返回值 | 处理 |
| --- | --- |
| `RB_OK` | 操作成功。 |
| `RB_INVALID_ARGUMENT` | 检查句柄、结构体 `Size`、尺寸、stride、页索引和模板 ID。 |
| `RB_BUFFER_TOO_SMALL` | 扩大文本缓冲区后重试。 |
| `RB_LICENSE_REQUIRED` | 授权不允许真实输出；保留状态显示和配置浏览。 |
| `RB_ERROR` | 读取日志并检查资源、DLL、USB 驱动或模板文件。 |

客户模板加载失败时先读取 `RB_DashboardPackageInfo::Description`；打开后渲染失败时读取 `RB_DashboardPackageState::LastErrorText`。VOCORE 和 Web 的运行错误分别读取各自状态结构体的 `LastErrorText`。

出现黑屏或画面异常时按以下顺序检查：

1. `rb.runtime.state()` 是否成功；
2. `stride >= width * 4` 且像素缓冲区容量正确；
3. `rb.dashboard.render_frame()` 或 `rb.dashboard.render_package_frame()` 返回值；
4. 客户模板 `Valid`、`FormatVersion` 和 `PageCount`；
5. VOCORE 的屏幕尺寸、旋转和 `DevicePresent`；
6. Web 的 `Open`、端口和浏览器访问地址；
7. DLL 同级资源目录和 `libusb-1.0.dll` 是否来自同一发布包。

## 16. 安全限制和兼容策略

SDK 对 `.rbdash` 做完整校验：

- 单文件最大 16 MiB；
- 单图片解码后最大 16 MiB；
- 图片最大 4096x4096；
- 页面最多 16 个；
- 每页最多 512 个元素；
- 只接受 PNG/JPEG Base64 Data URI；
- 未知元素拒绝加载，未知字段可忽略；
- 不执行脚本，不访问文件系统、网络、进程或控制接口。

V1 模板应只使用 V1 字段。后续格式升级必须保持旧模板可以继续加载；新增字段不能改变已有字段的含义。模板应用应检查 `FormatVersion`、`Valid` 和目录错误说明，不要只根据文件扩展名判断可用性。

当 SDK 增加新的稳定绑定 key 时，旧模板不需要改动。模板作者要避免依赖容易变化的显示名称，配置中只保存稳定 key 和 `PackageId`。

## 17. 开发验收清单

### 模板

- [ ] 文件为 UTF-8 无 BOM JSON。
- [ ] `formatVersion` 为 1，`id` 符合稳定 ID 规则。
- [ ] 设计尺寸在 `64-4096` 范围。
- [ ] 页面和元素数量没有超过 V1 限制。
- [ ] 所有动态元素使用稳定 `rb.*` key。
- [ ] 图片是 PNG/JPEG Data URI，未引用外部文件。
- [ ] 横屏和竖屏目标尺寸下内容没有裁切或拉伸变形。

### SDK 集成

- [ ] 初始化、内部循环和退出顺序正确。
- [ ] 目录刷新后保存 `PackageId`，没有持久化目录索引。
- [ ] 模板切换前关闭旧句柄。
- [ ] 渲染缓冲区容量、stride 和像素格式正确。
- [ ] 画面刷新使用独立 16-33 ms 定时器。
- [ ] VOCORE、Web 和窗口输出可以独立启停。
- [ ] `PendingFrame` 不被当成发送确认，丢帧统计可见。
- [ ] 退出前关闭 Web、VOCORE 和客户模板句柄。

### 发布

- [ ] `.h/.lib/.dll` 来自同一次 Debug 或 Release 构建。
- [ ] VOCORE 的 `libusb-1.0.dll` 版本匹配。
- [ ] `DashboardAssets/RaceBear` 目录结构完整。
- [ ] 用户模板导入、刷新、打开、渲染和删除均已验证。
- [ ] 授权锁定时不会误把 0 显示成真实车辆数据。
- [ ] 文档、示例和公开包中的仪表说明与当前 API 一致。

## 18. 常见问题

### 为什么模板已经导出，SDK 仍然拒绝？

设计器只负责生成文件，SDK 还会检查格式版本、ID、尺寸、元素数量、图片大小、Data URI 类型和字段范围。使用 `rb.dashboard.get_package_info()` 查看 `Valid` 和 `Description`。

### 为什么保存目录索引会导致升级后模板错乱？

模板目录可能因新增、删除或排序变化而改变。目录索引只属于当前扫描快照，应用配置必须保存 `PackageId`。

### 为什么 Web 或 VOCORE 画面有延迟？

确认宿主没有等待 `PendingFrame` 清零，也没有把每一帧加入自己的无限队列。SDK 输出后端只保留最新待处理帧；应按目标 FPS 继续提交，并观察 `DroppedFrames`。

### 为什么 Ford GT 模板缺少图片？

检查 `DashboardAssets/RaceBear` 是否位于 `RaceBearSDK.dll` 同级，并且来自同一构建包。其它纯绘制模板不依赖该目录。

### 仪表能否反向控制平台或输出？

不能。`.rbdash` 是声明式视觉模板，只能读取 SDK 暴露的稳定数据绑定。平台、输出、授权和设备操作必须由宿主应用通过对应的公开 API 完成。

