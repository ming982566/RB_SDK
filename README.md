# RaceBear SDK

## 统一对象 ABI（1.0.2）

新增 7 个 `RB_Object_*` 入口，配合原版本/初始化/关闭入口组成主要 10 个接入函数。
当前目录覆盖 9 个状态/草稿对象、29 个同步服务域、20 个异步动作。
剩余 265 个公开业务操作均可走 NATIVE_CALL，参数映射见
[同步服务目录](docs/RaceBearSDK_SERVICE_API.md)；第一方兼容操作不属于第三方合同。
旧业务函数已从第三方公开 ABI 和 DLL 导出表移除；当前标准公开头和 DLL 仅保留上述 10 个入口。
旧业务实现只作为 SDK 内部服务的实现细节保留，第三方不得定义内部迁移宏或直接导入旧符号。
并发、版本冲突、缓冲区、生命周期和覆盖限制见
[统一对象协议](docs/RaceBearSDK_OBJECT_API.md)。
公开 SDK 仅保留 C++ 示例，全部使用 10 个统一 ABI；不再提供 Python 示例或旧业务函数绑定。
协议版本为 1，运行时为 1.0.2；启动时应检查 10 个导出并读取 Describe 的协议版本。
本地开发和压缩包验证不等于真实设备、游戏或 USB 长稳验收。

## 1.0.2

- 仅分发 C++ 示例，控制台、UDP 中继、object_api、MotionStudio 全部使用十入口 ABI。
- 示例提供宿主侧同步服务包装，区分传输和业务失败，匹配 Debug/Release 依赖。
- 修正文档生命周期和授权说明，发布前对实际 ZIP 解压后的示例和匹配产物独立验收。
- 保持对象协议、结构体布局、计算、设备保护和授权边界不变。

## 1.0.1

- 统一对象 ABI 的公开开发文档已切换为对象、服务操作 key 和异步命令说明。
- 同步服务目录不再暴露旧业务函数名映射；前端应通过 10 个统一入口接入。
- 保持对象协议、公开结构体布局、授权边界和设备安全合同不变。

## 1.0.0

- 公开 ABI 收敛为 3 个生命周期入口和 7 个对象协议入口。
- 旧业务 DLL 导出和标准公开头声明移除；第三方前端必须通过对象目录、同步服务和异步命令接入。
- 这是不兼容旧类型化导入的主版本变更；现有旧 ABI 客户端必须迁移后才能使用 1.0.0。

## 0.6.4

- 补齐公开 `RaceBearSDK.h` 的结构体、枚举、回调和句柄注释，补充字段用途、
  单位、范围、生命周期和授权边界说明。
- 不改变公开结构体布局、函数签名、枚举值或对象协议；本版本与 0.6.3
  保持 ABI 和运行时行为兼容。

## 0.6.3

- 发布统一对象 ABI 的公开开发文档和同步服务目录，第三方可按稳定 key、
  目录签名和公开头完成对象、异步命令及同步服务接入。
- 保留旧公开导出和结构体兼容；统一对象 ABI 不改变业务校验、授权、并发、
  句柄所有权或设备安全边界。

## 0.6.2

- 运行时版本升级为 `0.6.2`，保留已有公开结构体布局和调用约定；
  新增安全带保护后显式重新启用入口 `RB_SeatbeltCan_RequestReenable()`。
- 整合下列安全带控制器保护、USB 调度与 RGB 修复；普通宿主仍由 SDK
  持有输出线程，只提交配置/请求并轮询状态，不能自行绕过保护。
- 补齐 AI 开发总入口、UI 规范、仪表开发指南、完整前端 Skill 和示例分发。
- `RB_Log_Read()` 缓冲区不足时保留队首日志和类型，扩容后可再次读取。
- 安全带最新链路恢复要求 APP `2026110103`；固件不包含在公开 x64 SDK 包中。
  本地探针未复现原约 2 秒 Abort 阻塞，本版不宣称已根治；实际频率见下方说明。

### 2026-11-02 修复

- 修复实机探针确认的误清边界：OUT迟到但完整成功且未操作端点时，不再清理完整
  命令流；原期限失败与诊断保留，部分/失败完成及端点恢复后仍清边界。
- 实际发送节拍采用可被安全、心跳和控制请求唤醒的高分辨率等待，保留单写者、
  严格最小间隔和最新值覆盖；旧系统回退条件变量，配置频率仍非实测保证。
- 4ms写预算、5000ms失联保护及显式故障恢复不变，未改公开ABI或固件。
  原约2秒Abort长阻塞尚未在本机自然复现，不能认为本次已根治。

### 2026-11-01 修复

- 安全带APP `2026110103`配套SDK修复失败USB写入后的残帧失步：
  在途请求结束后由独立控制端点清理接收边界并取故障快照，未确认恢复时暂停Bulk输出。
  正常运行不增加控制请求或日志，不改变输出节拍、保护或显式重新启用规则。
  原始OUT超时及Windows Abort长停顿仍需实机验证，不能据离线通过认定已根治。
- 安全带APP `2026110102` 修正约71分钟微秒计数回绕导致的失联与TPDO误判；
  长时间运行测试须更新此APP，配套SDK改为应用统一取消短写并补充故障快照缺失诊断。
  固件5000ms保护门限不变，USB驱动长阻塞仍须实机验收。
- 安全带APP `2026110101` 新增完整RGB帧异步传输；SDK按设备能力选择，
  新模式不等待逐帧固件应答，旧APP仍使用原协议。电机保护最低APP仍为 `2026103102`。
- 安全带RGB独立按最高60Hz更新，灯效配置帧率仍有效；独立RGB控制器按自身容量运行。
  接收与实际灯带输出通过低频状态反馈复核，正常运行不逐帧输出日志。
- 安全带输出实例的 `OutPutTime` 按毫秒设置，最小2ms（目标500Hz），
  4ms可用于250Hz实机对比；位置与颜色仅保留最新值，安全/心跳优先。
- Windows USB调度仍需实机验收；新增异常阶段快照用于区分排队、写入、取消回收和管道恢复。

### 2026-10-31 修复

- 专用安全带控制器新增失联/急停释放：按已配置的单路或双路节点，回绝对0并逐轴确认下使能。
- `RB_SeatbeltCan_RequestReenable` 用于保护锁定后的显式重新启用；连接恢复不会自动拉紧。
- 安全带正常状态监测对齐RB_CAN，使用TPDO+SDO连续失败确认；主机失联采用5000ms期限，
  RGB/诊断不延后电机释放。正常断开确认回绝对0且下使能后才停止周期TPDO。
- 需要安全带APP固件 `2026103102` 或更新版本；旧固件拒绝进入动作就绪状态。
  默认释放速度及机械零位仍须空载实测，详见 API 的安全带 CAN 章节。

## 0.6.1

- 运行时版本升级为 `0.6.1`。
- 保持已有公开结构体布局、调用约定和协议兼容；新增导出函数不会破坏
  `0.6.0` 调用方。
- 扩展 CAN 维护公开接口：参数维护波特率切换、按稳定参数键导入/导出、
  同类型电机参数差异同步，以及 RB 伺服固件异步升级。
- 新增 CAN 输出空闲下使能超时设置。默认 60 秒，设置为 `0` 可关闭；
  只有启用自动连接的 `RB_CAN`、`FM_CAN` 和 `FC_CAN` 输出会执行该计时。

## 0.6.0

- 运行时版本升级为 `0.6.0`。
- 新增平台配置包和动态配置包的导入导出接口，支持按稳定平台 Key 或动态配置名称覆盖配置。
- 新增面向 SimHub 等外部 Motion 宿主的 CAN 最新位置提交接口；外部宿主可复用 SDK 的设备绑定、Ready、故障、急停和恢复安全状态机。
- 外部实时位置接管保持为可选能力，普通 SimRacebear 输出默认继续由 SDK 内部位置通道发送。
- 0.6.0 的标准第三方 SDK 公开 `RB_CAN` 设备管理、状态诊断和外部位置提交；
  电机参数维护、`FM_CAN`、`FC_CAN`、`RDS_CAN`、`SEATBELT_CAN`、底层
  SDO/PDO、固件内部升级协议和私有前端接口仍不属于当时的公开交接范围。

## 0.5.1

- 在 SDK 内部统一缓存已解析的配置 JSON，正常读取不再反复打开和解析 `DynamicV2`、`PlatformV2`、`Output`、系统及附加效果配置文件。
- 配置保存成功后自动更新内存缓存；通过 `RB_Data_WriteJson()` 替换原始配置后自动使对应缓存失效，显式重载可以读取外部修改。
- 输出实例目录改为完整内存配置，AutoConnect 统计、自动连接和输出摘要不再逐实例读取 `OutPutV2.json`。
- 新增配置缓存回归测试，覆盖缓存命中、原始 JSON 替换失效和平台配置读取校验。
- 新增 `RB-A6X` 六边形六滑台六自由度平台：六条滑台分别位于六边形底架六条边上，相对边轴线平行，支持六自由度逆解和预览绘制。
- 公开 C ABI、结构体布局和固件协议保持不变；运行时版本更新为 `0.5.1`。

## 0.5.0

- 公开头新增 RB_CAN 设备发现、逻辑索引、稳定 UID 绑定、绑定状态和单输出电机状态接口；完整流程见 `docs/RaceBearSDK_RB_CAN_GUIDE.md`。
- 新增客户自定义仪表 `.rbdash` V1：SDK 负责目录扫描、校验、稳定 `rb.*` 数据绑定、页面选择和 BGRA8888 渲染。
- 新增 `tools/DashboardDesigner` 单机设计器，可直接打开、编辑、模拟并导出单文件仪表模板。
- Dashboard Designer 提供分类数据选择、圆形指针表、换挡灯、驾驶告警、轮胎和踏板等常用车辆组件；双击 `Start-DashboardDesigner.cmd` 会直接使用系统默认浏览器打开静态 HTML，不依赖 Python、Node 或本机服务。
- 随 SDK 提供 `RaceBear Starter` 示例；客户模板与原四套内置模板可同时用于 VOCORE、Web 或宿主窗口输出。

## 0.4.9

- 新增 SDK 内置仪表渲染器，提供 AIM MXS、BOSCH DDU8、SimpleDash 和 Ford GT 四套风格模板。
- 每套模板分别提供竖屏与横屏布局，调用方不再需要实现GDI仪表绘制。
- 新增浏览器 MJPEG 仪表服务，可读取服务状态、客户端数量和编码统计，并支持显式关闭。
- VOCORE 句柄支持运行时切换自动重连策略。
- 新增 `RB-A7H` 七轴平台：A1-A4 承担 Heave/Roll/Pitch，A5 承担 Surge，A6/A7 共同表达 Sway/Yaw。
- 公开 CAN 输出健康快照和人工恢复请求；电机参数、固件更新和底层维护能力暂未纳入公开 API。

## 0.4.8

- 新增 VOCORE USB 仪表屏后端：设备枚举、型号识别、亮度、旋转、异步帧提交、发送统计和自动重连。
- VOCORE 发送采用最新帧覆盖队列，USB 短时阻塞不会在恢复后补发过期仪表画面。
- Release/Debug 输出目录新增 `libusb-1.0.dll`，用于兼容 VOCORE 官方 LibUSBK 驱动。

## 0.4.7

- 公共遥测目录隐藏历史 SimTools 专用兼容槽位，内部 `50-61` 索引保持不变。
- 遥测目录允许跳过内部保留槽位，调用方必须使用每项返回的 `Index`，不能使用数组行号代替。

## 0.4.6

- 未授权或试用到期后停止专有运动计算，并安全断开已经连接的输出。
- 运行状态继续返回授权与连接元数据，但最终遥测、6DOF、执行器、运动增强、震动、风感和安全带结果统一清零。
- 运行态 Apply、手动测试和输出连接统一返回 `RB_LICENSE_REQUIRED`；配置、目录、原始遥测、断开和复位功能保持可用。
- 授权恢复后沿用当前遥测连接，并从零重新执行淡入过渡，避免姿态瞬间跳变。

## 0.4.5

- CAN实时位置发送改为最新值覆盖模式，USB短时阻塞恢复后不再补发过期轨迹。
- CAN实时USB写入使用有限超时，失败后复位输出管道并继续发送最新位置。
- 修复动态配置将输入模板类型误判为0-5输出DOF，导致合法配置保存返回-2的问题。
- 修复动态配置回读时按旧候选表过滤输入，导致公共遥测目录创建的通道被静默丢弃的问题。
- 公开按目标DOF划分的动态输入效果候选目录，前端不再从完整遥测目录猜测可添加通道。

VS2019 x64 DLL SDK distribution for the RaceBear backend.

## Start Here

Application developers should read:

- `docs/RaceBearSDK_AI_DEVELOPMENT.md`: single entry for AI-led frontend planning, implementation, validation, packaging, and delivery.
- `docs/RaceBearSDK_FRONTEND_UI_SPEC.md`: page/container map, controls, drafts, task states, localization, and visual acceptance.
- `include/RaceBearSDK.h`: minimal public C ABI header for normal frontends.
- `docs/RaceBearSDK_FRONTEND_GUIDE.md`: page-by-page frontend implementation guide and authoritative configuration ownership rules.
- `docs/RaceBearSDK_API.md`: integration guide, call order, field units, and common failure causes.
- `docs/RaceBearSDK_DASHBOARD_DEVELOPMENT.md`: dashboard template, data binding, rendering, VOCORE, Web output, threading, and deployment guide.
- `docs/RaceBearSDK_RB_CAN_GUIDE.md`: RB_CAN device discovery, logical-index assignment, stable UID binding, diagnostics, and recovery workflow.
- `tools/skills/racebear-frontend-panel`: reusable, tool-neutral AI coding Skill for frontend implementation, review, and debugging.

The public header, import library, DLL, documentation, and Skill provide the
complete supported integration surface for application development.

## AI-Assisted Frontend Development

For a complete frontend, start with [the AI development entry](docs/RaceBearSDK_AI_DEVELOPMENT.md).
It includes a ready-to-use task prompt, staged workflow, scope/field matrices, offline and hardware
test boundaries, and delivery requirements. It can be read directly without installing a Skill.
Use [the UI specification](docs/RaceBearSDK_FRONTEND_UI_SPEC.md) to place capabilities in suitable
pages and containers rather than exposing each API as a button.

`tools/skills/racebear-frontend-panel` packages the frontend lifecycle, catalog ownership,
stable-key, configuration, licensing, handle, dashboard, deployment, and validation rules for
AI coding agents. It does not replace the public header or manuals; it tells the agent which
contract to read and which checks must pass before an integration is considered complete.

Supply the complete Skill directory to the AI coding tool using that tool's
own Skill or reusable-instruction import mechanism. Installation locations,
invocation syntax, rule filenames, and optional interface metadata vary by
tool, so this SDK does not prescribe them.

After importing it, give the agent a concrete task, for example:

```text
Add RaceBear game selection, runtime state, license controls, and dashboard
output to this Qt application using the provided RaceBear frontend Skill.
```

The Skill supports C/C++, C#, Qt, web/native bridges, Python `ctypes`, and other hosts that can
consume the public C ABI. Framework-specific code must still follow the target application's
existing architecture and use a header/DLL/LIB set from the same SDK build.

`include/RaceBearSDK.h` is the public SDK header. Application-specific convenience wrappers can live in the host adapter while preserving this stable ABI boundary.

## Build The C++ Examples

Install Visual Studio 2019 C++ tools (v142, Windows SDK) and CMake. From the
extracted package root:

```powershell
cmake -S examples -B build-examples -G "Visual Studio 16 2019" -A x64 -T v142
cmake --build build-examples --config Release
cmake --build build-examples --config Debug
```

This builds the console, UDP bridge, object protocol and MotionStudio programs.
The build copies matching SDK and libusb DLLs beside each executable.
MotionStudio also includes its own Visual Studio solution under
`examples/RaceBearMotionStudio`. The SDK implementation project is private and
is not needed to build a host application.

Release dependencies:

- `bin/x64/Release/RaceBearSDK.dll`
- `bin/x64/Release/RaceBearSDK.lib`
- `bin/x64/Release/libusb-1.0.dll`（使用 VOCORE 屏幕时必须随 DLL 一起发布）
- `bin/x64/Release/DashboardAssets/RaceBear`（使用 Ford GT 或随附客户仪表时部署到 DLL 同级 `DashboardAssets/RaceBear`）
- `tools/DashboardDesigner/index.html`（无需安装或服务器，浏览器直接打开）

## Architecture Boundaries

This package supports Windows x64 hosts with VS2019 v142. Headers, import
libraries, DLLs and host processes must use one matching architecture and build.
Link Debug applications to `bin/x64/Debug` and Release applications to
`bin/x64/Release`; deploy the DLLs from the same directory.

## Public Header

Host applications include:

```cpp
#include "RaceBearSDK.h"
```

The public API uses a C ABI with fixed-layout data types so C++, C#, Qt, Python,
and other hosts can share the same stable DLL boundary.

The supported call surface is the ten-function object ABI:

- `RB_Runtime_GetVersion`, `RB_Runtime_Initialize`, `RB_Runtime_Shutdown`
- `RB_Object_Describe`, `RB_Object_Resolve`, `RB_Object_Read`, `RB_Object_Write`
- `RB_Object_Invoke`, `RB_Object_Poll`, `RB_Object_GetLastError`

Use the object and service documentation to resolve stable `rb.*` keys, read
snapshots and drafts, submit synchronous native service calls, and poll actions.
Product-specific convenience functions belong in the host adapter and must not
be added to the DLL boundary.

## App Data Name

The SDK stores local config under `%APPDATA%\<AppDataName>\configV2` and related
user files under `Documents\<AppDataName>`. The default app data name is
`SimRacebear`.

Host applications can set their own name through the synchronous
`rb.runtime.set_app_data_name` service before runtime initialization. See
`docs/RaceBearSDK_API.md` for the native-call envelope and lifetime rules.

## License Runtime

`RB_Runtime_Initialize()` starts the SDK license runtime. Missing or invalid local
licenses enter a local trial countdown first; when the countdown reaches zero, or
when a timed license expires or is revoked/disabled, the SDK restricts protected
Motion and ordinary platform outputs. RGB, seatbelt, Wind and Haptic retain their
own product and safety contracts. Perpetual licenses are validated locally and do not run a periodic
online heartbeat.

When protected Motion output is locked:

- Telemetry/config/query APIs remain available.
- Activation/deactivation commands remain available.
- New protected output connections are rejected.
- Existing protected outputs are driven through the normal safe disconnect path when
  possible.

Read license state from `rb.runtime.state` and use `rb.command.run` through
`rb.service.command` for activation or deactivation. Do not import legacy typed functions.

## Data Packages And Commands

Configuration, catalog and command payloads are documented by
`RaceBearSDK_OBJECT_API.md` and `RaceBearSDK_SERVICE_API.md`. Use
`RB_Object_Read`, `RB_Object_Write`, `RB_Object_Invoke` and `RB_Object_Poll`;
the C++ examples use host-side object service wrappers rather than legacy DLL imports.

## Package Layout

- `include/` public SDK headers.
- `bin/x64/Debug` and `bin/x64/Release` matching DLL/LIB and dependencies.
- `docs/` complete integration, object and service manuals.
- `examples/` C++ sources using only the unified ABI.
- `tools/` public release tools, Dashboard Designer and the frontend Skill.

## Notes

- Example builds require VS2019 toolset `v142`.
- Use the API and object manuals for task ownership, shutdown and buffer rules.

