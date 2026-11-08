# RaceBear SDK 前端开发指南

本文指导第三方前端使用 RaceBear SDK 的统一对象 ABI。公开接入只使用 `RB_Runtime_*` 生命周期入口和 `RB_Object_*` 对象入口；业务功能通过 `rb.*` key、同步服务目录和异步命令完成。

## 1. 接入准备

使用匹配的 `RaceBearSDK.h`、`RaceBearSDK.lib`、`RaceBearSDK.dll` 和运行依赖。x64 Debug 与 Release 必须成套使用。前端不需要 SDK 私有源码，也不应把对象 ID、任务 ID 或目录索引写入持久化配置。

启动后先调用 `RB_Object_Describe(0, ...)` 读取目录，检查 `protocolVersion`、对象 key、命令 key 和公开服务。只使用 `firstParty=false` 的服务。

## 2. 生命周期和线程

应用只有一个进程级 SDK owner：

1. 初始化前通过 `rb.service.runtime` 的 `rb.runtime.set_app_data_name` 设置独立产品目录。
2. 调用 `RB_Runtime_Initialize` 一次。
3. 用 `RB_Object_Invoke("rb.runtime.start", "{\"intervalMs\":2}", ...)` 启动 SDK 计算循环。
4. UI 以 30-60 Hz 读取 `rb.runtime.state` 或 `rb.telemetry.snapshot`。
5. 日志、目录、配置和设备操作放在独立的低频工作线程。
6. 退出时停止 UI 定时器和任务提交，关闭测试、回调、设备句柄和输出，再 Invoke `rb.runtime.stop`，最后调用 `RB_Runtime_Shutdown`。

同一对象可以被多个调用源读取。每个调用方必须使用自己的缓冲区和 `RB_ObjectReadInfo`，不得共享正在写入的内存。高频遥测 Read 只复制已发布快照，不执行计算、不读 USB、不读文件。

## 3. 页面与对象映射

| 页面 | 主要 key | 说明 |
| --- | --- | --- |
| 首页/授权 | `rb.runtime.state` | 读取授权、游戏、Source、输出和功能状态 |
| 遥测 | `rb.telemetry.catalog`、`rb.telemetry.snapshot` | 目录低频读取，快照高频读取 |
| 风感 | `rb.wind.config`、`rb.wind.state` | 共享草稿，使用 `rb.wind.apply/save/reload` |
| 安全带 | `rb.seatbelt.config`、`rb.seatbelt.state` | 独立于主授权，测试命令需显式停止 |
| RGB | `rb.rgb.effects`、`rb.rgb.state` | 灯效草稿和设备状态分开 |
| 平台/输出 | `rb.service.platform`、`rb.service.output` | 通过同步服务目录完成配置和连接 |
| CAN | `rb.service.can`、`rb.service.seatbeltcan` | UID、逻辑索引、健康和维护操作 |
| 仪表 | `rb.service.dashboard`、`rb.service.vocore`、`rb.service.dashboardweb` | 同步服务借用像素和句柄 |
| 串口 | `rb.service.serial` | 句柄、回调和关闭顺序沿服务目录合同 |
| 日志 | `rb.service.log` | 单消费者读取，缓冲区不足时扩容重读 |

页面不应按 ABI 入口数量生成按钮。用户动作应映射到完整工作流，显示目标、草稿、忙碌、错误和终态。

## 4. 读取快照

```cpp
int telemetryId = 0;
RB_Object_Resolve("rb.telemetry.snapshot", &telemetryId);

RB_ObjectReadInfo info{};
info.Size = sizeof(info);
info.Version = 1;

RB_ObjectTelemetrySnapshot snapshot{};
int rc = RB_Object_Read(
    telemetryId,
    RB_OBJECT_BINARY,
    &snapshot,
    sizeof(snapshot),
    &info);
```

实际结构体大小和版本以公开头为准。若返回 `RB_BUFFER_TOO_SMALL`，按 `PayloadBytes` 扩容并重读。读取失败时不能用默认结构体覆盖现有编辑状态。

## 5. 草稿编辑

草稿对象由 SDK 共享，同一产品的多个页面看到同一份修订号。标准流程是 Read、保留未编辑字段、按字段校验、Write、Invoke Apply 或 Save、Poll 终态、ReadBack。Write 只改内存草稿；Save 和 Apply 的提交快照彼此独立。

`expectedRevision=0` 不表示强制覆盖。收到 `RB_OBJECT_CONFLICT` 时重读并由宿主合并，不能自动重放旧的整组数据。JSON 只用于对象文档明确允许的配置；复杂 RGB 和安全带配置优先使用完整 Binary 结构体。

## 6. 同步服务封装

服务域先 Resolve，例如 `rb.service.rgb`；操作 key、签名、参数顺序和返回类型从 `RaceBearSDK_SERVICE_API.md` 查询。使用 `RB_OBJECT_NATIVE_CALL`：

```cpp
RB_ObjectNativeCall call{};
call.Size = sizeof(call);
call.Version = 1;
call.PointerBytes = sizeof(void*);
call.Operation = "rb.rgb.connect";
call.Signature = /* 从服务目录读取 */;
call.ArgumentCount = 1;
call.Arguments[0] = /* 按位复制 uid 指针 */;
call.Result = &resultBits;
RB_Object_Write(serviceId, RB_OBJECT_NATIVE_CALL,
                &call, sizeof(call), 0, &revision);
```

封套是同进程本机协议，不能跨进程或发送到 Web。同步调用可能阻塞，宿主应放到工作线程；像素、句柄、字符串和回调的有效期遵循服务目录。同步服务不走异步任务队列，也不自动提供事务。

## 7. 异步动作

对 `rb.runtime.*`、`rb.wind.*`、`rb.seatbelt.*`、`rb.rgb.*` 和输出连接命令，Invoke 一次后保存任务 ID，重复 Poll 直到终态。`RB_Object_Poll` 的缓冲区不足只表示结果复制空间不足，不表示动作失败，也不能重新 Invoke。终态必须检查 `ResultCode`、消息和必要的 `backendCode`。

测试页面离开、设备丢失、授权丢失和应用退出时，都要提交明确的停止或复位命令。没有每任务强制取消接口，宿主要限制队列长度并在关闭时等待已提交任务收敛。

## 8. 授权和独立产品

`rb.runtime.state` 中的授权快照只门控 Motion、普通平台输出和其它受保护运行能力。RGB、Wind、安全带和 Haptic 页面仍可读取状态、编辑配置和执行各自合同允许的测试或输出。具体设备和安全条件以对应服务目录和专题指南为准。

## 9. 目录和稳定身份

游戏、输出、CAN、RGB、仪表包、插件和串口目录都拥有独立索引空间。索引只在当前快照内有效；保存配置应使用稳定 key、UID、PackageId 或公开业务身份。刷新目录、删除对象或设备热插拔后必须按稳定身份重新定位。

## 10. 关闭验收

关闭前停止所有可能调用 SDK 的宿主线程，停止 UI 和仪表定时器，注销回调，复位手动测试，断开输出，关闭 VOCORE、Web、串口、仪表包和调试句柄，等待自有异步任务进入终态，最后调用 `RB_Runtime_Shutdown`。任何线程仍可能进入 SDK 时都不能卸载 DLL。

前端交付需分别记录代码完成、构建通过、对象协议离线测试、视觉验收和真实硬件验收。用户手动编译 SimRacebear 的 Debug64 和 Release64 后，再检查最终运行目录的 DLL、依赖和资源。
