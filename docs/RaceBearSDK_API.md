# RaceBear SDK API 手册

本文是第三方开发者的公开接入手册。当前公开 ABI 只有 10 个入口：3 个生命周期入口和 7 个对象协议入口。所有游戏、遥测、Motion、输出、RGB、安全带、Wind、CAN、串口、仪表和诊断能力都通过稳定的 `rb.*` 对象、同步服务或异步命令访问。

## 1. SDK 包与版本

使用同一次构建生成的以下文件：

| 文件 | 用途 |
| --- | --- |
| `include/RaceBearSDK.h` | 唯一公开头文件、结构体和枚举 |
| `bin/x64/Release/RaceBearSDK.lib` | x64 导入库 |
| `bin/x64/Release/RaceBearSDK.dll` | x64 运行库 |
| `bin/x64/Release/libusb-1.0.dll` | 使用 VOCORE 或 USB 设备时的依赖 |
| `docs/RaceBearSDK_OBJECT_API.md` | 对象协议、并发、草稿和任务规则 |
| `docs/RaceBearSDK_SERVICE_API.md` | 同步服务的公开 key、签名和参数 |

运行时使用 `RB_Runtime_GetVersion()` 获取版本。能力检测以 `RB_Object_Describe(0, ...)` 返回的 `protocolVersion`、对象和命令目录为准，不要只比较版本字符串。

## 2. 十个公开入口

| 入口 | 作用 |
| --- | --- |
| `RB_Runtime_GetVersion` | 返回 SDK 版本字符串，调用方不释放 |
| `RB_Runtime_Initialize` | 初始化一次进程级 SDK |
| `RB_Runtime_Shutdown` | 停止任务、释放 SDK；调用方不得再使用其它入口 |
| `RB_Object_Describe` | 查询完整目录或单个对象描述 |
| `RB_Object_Resolve` | 将稳定 key 解析为本次进程的对象 ID |
| `RB_Object_Read` | 读取 Binary 快照、JSON 目录或配置草稿 |
| `RB_Object_Write` | 写入配置草稿，或以 NATIVE_CALL 执行同步服务 |
| `RB_Object_Invoke` | 提交异步命令并返回任务 ID |
| `RB_Object_Poll` | 读取任务状态和终态结果 |
| `RB_Object_GetLastError` | 读取当前线程的入口错误详情 |

对象 ID、任务 ID 和遥测索引只在当前 SDK 生命周期或当前目录快照中有效，不得写入产品配置。

## 3. 最小生命周期

应用目录名通过同步服务 `rb.service.runtime` 的操作 `rb.runtime.set_app_data_name` 设置。该操作使用 `RB_ObjectNativeCall`，必须在初始化前完成。下面的代码省略了 `CallNativeService` 的封装，封装实现见对象协议“同步本机服务”一节。

```cpp
#include "RaceBearSDK.h"

int StartRaceBear()
{
    if (CallNativeService(
            "rb.service.runtime",
            "rb.runtime.set_app_data_name",
            "StableProductName") != RB_OK)
        return RB_ERROR;

    int rc = RB_Runtime_Initialize();
    if (rc != RB_OK)
        return rc;

    uint64_t taskId = 0;
    return RB_Object_Invoke(
        "rb.runtime.start",
        "{\"intervalMs\":2}",
        &taskId);
}

void StopRaceBear()
{
    uint64_t taskId = 0;
    RB_Object_Invoke("rb.runtime.stop", "{}", &taskId);
    RB_Runtime_Shutdown();
}
```

`RB_Object_Invoke` 返回 `RB_OK` 只表示命令已排队。必须用 `RB_Object_Poll` 读取 `ResultCode`，再决定是否显示成功或失败。关闭前停止新的调用，等待自有任务进入终态，停止测试、断开设备并关闭句柄。

## 4. 目录、key 和格式

首次接入时调用 `RB_Object_Describe(0, nullptr, 0, &requiredBytes)` 查询容量，再分配 UTF-8 缓冲区读取目录。目录中的 key 分为四类：

| 类别 | 示例 | 调用方式 |
| --- | --- | --- |
| 状态/草稿对象 | `rb.telemetry.snapshot`、`rb.wind.config` | Resolve 后 Read/Write |
| 同步服务域 | `rb.service.rgb`、`rb.service.can` | Resolve 后 Write，格式为 `RB_OBJECT_NATIVE_CALL` |
| 异步命令 | `rb.runtime.start`、`rb.rgb.connect` | Invoke 后 Poll |
| 遥测字段 | `rb.vehicle.speed` | 先读取遥测目录，再按快照索引访问 |

稳定 key 可以持久化。对象 ID 需要每次启动重新 Resolve。公开同步服务以 `RaceBearSDK_SERVICE_API.md` 为准，服务目录中没有可导入的旧业务函数名。

格式常量：

- `RB_OBJECT_BINARY=1`：公开结构体的原生 x64 布局。
- `RB_OBJECT_JSON=2`：UTF-8 JSON，容量包含结尾 NUL。
- `RB_OBJECT_NATIVE_CALL=3`：同进程、同架构的同步服务封套，不能发送到网络、网页或其它进程。

## 5. 高频遥测和状态

高频读取使用 `rb.telemetry.snapshot` 的 Binary 快照。读取成功后检查 `RB_ObjectReadInfo.Sequence`、`SampleTimeUs`、`Sampled` 和 `PayloadBytes`；序号重复表示没有新帧，不代表读取失败。原始值支持标志必须和原始数值一起显示，不能把“不支持”解释成零。

低频综合状态使用 `rb.runtime.state`。状态对象覆盖授权、游戏、Source、Motion、平台、输出、CAN、Wind、RGB 和安全带摘要。配置对象包括：

- `rb.wind.config`
- `rb.seatbelt.config`
- `rb.rgb.effects`

这些对象的 Write 只更新 SDK 内存草稿，不保存、不应用、不连接设备。写入必须携带上次成功 Read 返回的 `Revision`；发生 `RB_OBJECT_CONFLICT` 时重读、合并后再提交。

## 6. 配置、保存和应用

推荐流程是：

1. Resolve 配置对象。
2. Read 完整结构体和 `Revision`。
3. 只修改用户确实编辑的字段，保留其它字段。
4. Write 时传回 `Revision`。
5. 使用对应的 `*.save` 或 `*.apply` 命令。
6. Poll 到终态，检查 `ResultCode` 和结果 JSON。
7. 再 Read 验证业务字段。

当前异步配置命令包括 `rb.wind.apply/save/reload`、`rb.seatbelt.apply/save/reload` 和 `rb.rgb.apply/save/reload`。Save 成功不表示运行态已经应用；Apply 成功也不表示配置已持久化。

## 7. 同步服务

同步服务通过 `RB_ObjectNativeCall` 调用。封套要求：

- `Size=sizeof(RB_ObjectNativeCall)`、`Version=1`、`PointerBytes=sizeof(void*)`；
- `Operation` 是目录中的完整操作 key；
- `Signature` 和 `ArgumentCount` 必须与目录完全一致；
- 参数按目录顺序按位复制到 16 个 `uint64_t` 槽；
- `Result` 指向调用方独占的结果槽；
- 所有指针、字符串、句柄和回调只在同步调用合同规定的生命周期内有效。

同步 Write 返回 `RB_OK` 只代表封套已分派。业务返回值从 `Result` 槽恢复；传输错误通过返回值和 `RB_Object_GetLastError` 判断。同步服务不进入异步任务队列，也不改变配置草稿修订号。

高频遥测不要逐字段调用服务。日志保持单消费者；设备句柄、回调和可变配置由宿主协调所有权。

## 8. 异步命令和错误

`RB_Object_Invoke` 的任务状态为 QUEUED、RUNNING、COMPLETE 或 CANCELLED。使用 `consume=0` 可重复读取；只有完整复制终态结果后才使用 `consume=1`。缓冲区不足时扩容并重试 Poll，不要重新提交命令。

常用入口错误：

| 值 | 含义 |
| --- | --- |
| `RB_OK` | 入口调用成功；异步调用仍需检查任务结果 |
| `RB_INVALID_ARGUMENT` | 指针、长度、版本、key 或参数错误 |
| `RB_BUFFER_TOO_SMALL` | 按返回容量扩容后重读 |
| `RB_LICENSE_REQUIRED` | 受保护 Motion 或普通平台能力需要授权 |
| `RB_OBJECT_CONFLICT` | 草稿修订冲突 |
| `RB_OBJECT_NOT_READY` | 尚未初始化或已关闭 |
| `RB_OBJECT_BUSY` | 任务记录或资源达到上限 |
| `RB_OBJECT_NOT_SUPPORTED` | 当前版本不支持该 key、格式或操作 |
| `RB_ERROR` | 后端失败，读取错误详情或任务结果 |

RGB、SEATBELT_CAN、Wind 和 Haptic 的计算、配置和正常输出按各自合同运行，不应把 `License.OutputAllowed` 当作这些独立产品的总开关。Motion、普通平台输出和受保护人工测试仍按授权状态门控。

## 9. 部署和验收

宿主、头文件、导入库和 DLL 必须来自同一配置和同一次构建。Debug 与 Release 不能混用。VOCORE 需要匹配的 `libusb-1.0.dll`，内置仪表需要同级 `DashboardAssets/RaceBear`。

至少验证：

- Describe 目录可读取，且协议版本符合预期；
- Binary 快照尺寸、`Size`、`Version` 和结构布局匹配；
- 多线程读快照不阻塞计算线程；
- 双写者产生并正确处理修订冲突；
- 短缓冲区不会重复执行命令；
- Shutdown 前任务、回调、测试和设备句柄均已收敛。

真实设备、机械释放、回零、固件升级和长时间运行必须单独记录人工或硬件验收结果。离线对象测试不能替代现场验证。
