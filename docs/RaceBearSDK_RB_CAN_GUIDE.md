# RaceBear SDK RB_CAN 接入指南

本文说明第三方 Windows x64 前端如何通过 RaceBear SDK 管理 `RB_CAN` 输出和 RaceBear CANAPP 设备。接口以 `include/RaceBearSDK.h` 为准。

当前公开范围只覆盖 `ProtocolType == 1` 的 `RB_CAN`：

- 设备发现和身份读取；
- 逻辑索引设置；
- 输出实例与设备 UID 的稳定绑定；
- 绑定、运行、电机和健康状态；
- 用户确认安全后的受控恢复。

本文不适用于 `FM_CAN`、`FC_CAN` 或 `RDS_CAN`。电机参数维护、固件升级、底层 SDO/PDO 和急停控制不属于当前第三方公开接口。

## 1. 四种索引和一个稳定身份

RB_CAN 页面必须区分以下标识：

| 标识 | 用途 | 能否持久化 |
|---|---|---|
| `outputIndex` | 当前 `RB_Output` 目录中的 `OUT_n` 零基索引 | 否，增删输出后重新取得 |
| `CatalogIndex` | 最近一次 CAN 设备刷新中的目录行号 | 否，下次刷新后失效 |
| `EnumerationIndex` | SDK 当前设备快照中的运行时访问索引 | 否，禁止保存 |
| `LogicalIndex` | 写入 CANAPP 的逻辑索引，范围 `1-255` | 是，但多卡必须唯一 |
| `Uid` | CANAPP MCU 的 24 位大写十六进制 UID | 是，物理设备稳定身份 |

输出配置最终使用 `{Uid, LogicalIndex}` 绑定物理设备。不要只保存 `CatalogIndex`、`EnumerationIndex` 或某次 USB 枚举顺序。

## 2. 初始化和线程边界

```cpp
if (RB_Runtime_SetAppDataName("YourProductName") != RB_OK)
    return false;
if (RB_Runtime_Initialize() != RB_OK)
    return false;
if (RB_Runtime_StartLoop(2) != RB_OK) {
    RB_Runtime_Shutdown();
    return false;
}
```

- SDK 初始化、输出连接和设备刷新不能与应用退出并发执行。
- UI 状态定时器可以读取绑定、健康和电机状态。
- USB 设备刷新只在进入设备页、用户点击刷新或收到热插拔提示后执行，不要放进高频定时器。
- 设置索引和绑定是用户明确发起的维护操作，不得自动循环重试。

## 3. 创建或定位 RB_CAN 输出

输出类型必须来自 SDK 目录。不要让用户手写类型文本。

```cpp
int rbCanOutputIndex = -1;
const int outputCount = RB_Output_GetInstanceCount();
for (int i = 0; i < outputCount; ++i) {
    RB_OutputConfig config{};
    if (RB_Output_ReadInstanceConfigByIndex(i, &config) == RB_OK &&
        std::strcmp(config.OutputKind, "RB_CAN") == 0) {
        rbCanOutputIndex = i;
        break;
    }
}

if (rbCanOutputIndex < 0) {
    rbCanOutputIndex = RB_Output_AddInstance("RB_CAN");
}
```

保存 `outputIndex` 只适合当前输出目录快照。添加或删除任意输出后，应重新读取输出目录并重新定位目标实例。

## 4. 刷新和显示设备目录

```cpp
const int count = RB_Can_RefreshDevices();
if (count < 0) {
    // 显示刷新失败并读取 SDK 日志。
    return;
}

for (int i = 0; i < count; ++i) {
    RB_CanDeviceInfo info{};
    info.Size = sizeof(info);
    if (RB_Can_GetDeviceInfo(i, &info) != RB_OK)
        continue;

    if (info.ProtocolType != 1) {
        // 当前页面只接入 RB_CAN，忽略其它协议身份。
        continue;
    }

    // 显示 Uid、LogicalIndex、FirmwareVersion、IapMode、
    // IdentityValid、DuplicateLogicalIndex 和 StatusText。
}
```

`RB_Can_GetDeviceCount()` 只返回最近一次刷新得到的数量，不重新扫描 USB。刷新后必须丢弃旧目录项对象和选中行号，再按 UID 恢复选择。

设备只有满足下列条件时才适合绑定：

- `ProtocolType == 1`；
- `IapMode == 0`；
- `IdentityValid != 0`；
- `Uid` 非空；
- 目标逻辑索引没有被另一张设备占用。

`DuplicateLogicalIndex != 0` 不应让整个页面失去操作能力。用户仍需能够按 UID 选中其中一张设备，并为它分配新的唯一索引。

## 5. 单独修改设备逻辑索引

只修改设备索引、不绑定输出时使用：

```cpp
const char* uid = selectedDevice.Uid;
const int newLogicalIndex = 2;

const int rc = RB_Can_SetDeviceLogicalIndex(uid, newLogicalIndex);
if (rc == 0) {
    RB_Can_RefreshDevices();
}
```

调用前必须确认：

1. 所有可能使用该设备的输出已经断开；
2. 新索引在当前在线设备中唯一；
3. 用户按 UID 确认了目标设备；
4. 当前没有退出、连接或其它维护动作并发执行。

返回 `0` 表示写入并读回成功。负数属于设备维护错误，不能统一解释成 `RB_Result`；前端应显示 SDK 日志和刷新后的 `StatusText`。

## 6. 将输出稳定绑定到设备

通常应直接使用绑定接口。它把目标逻辑索引写入设备，并将稳定 UID 和逻辑索引持久化到对应输出配置。

```cpp
if (RB_Output_IsInstanceRuntimeActive(rbCanOutputIndex)) {
    // 等待输出完全停止后再允许绑定。
    return;
}

const int rc = RB_Can_BindOutputDevice(
    rbCanOutputIndex,
    selectedDevice.Uid,
    requestedLogicalIndex);

if (rc == 0) {
    RB_Can_RefreshDevices();

    RB_CanOutputBindingState binding{};
    binding.Size = sizeof(binding);
    if (RB_Can_GetOutputBindingState(rbCanOutputIndex, &binding) == RB_OK &&
        binding.BindingStatus == RB_CAN_BINDING_READY) {
        // 绑定确认完成。
    }
}
```

不要通过修改 `RB_OutputConfig::CANindex` 代替 UID 绑定。`CANindex` 是公开兼容字段，完整绑定必须由 `RB_Can_BindOutputDevice()` 完成。

多张设备拥有相同旧逻辑索引时，仍然按以下顺序处理：

1. 刷新设备目录；
2. 用户按 UID 选中目标设备；
3. 输入当前未被占用的新逻辑索引；
4. 调用 `RB_Can_BindOutputDevice()`；
5. 再次刷新并读取绑定状态。

不需要拔掉另一张重复索引的设备，也不能根据设备目录行号猜测目标。

## 7. 绑定状态

```cpp
RB_CanOutputBindingState binding{};
binding.Size = sizeof(binding);
if (RB_Can_GetOutputBindingState(rbCanOutputIndex, &binding) == RB_OK) {
    switch (binding.BindingStatus) {
    case RB_CAN_BINDING_UNBOUND:
        break;
    case RB_CAN_BINDING_READY:
        break;
    case RB_CAN_BINDING_OFFLINE:
        break;
    case RB_CAN_BINDING_CONFLICT:
        break;
    case RB_CAN_BINDING_IAP:
        break;
    case RB_CAN_BINDING_ERROR:
        break;
    }
}
```

页面应同时显示：

- `Uid`、`LogicalIndex` 和 `ProtocolType`；
- `DeviceOpened` 和 `Ready`；
- `EStopLatched`、`EStopConfirmed` 和 `EStopCommandStatus`；
- `FirmwareVersion`、`Capabilities` 和 `StatusText`。

只有 `BindingStatus == RB_CAN_BINDING_READY`、协议身份为 `1` 且安全状态允许时，才向用户提供连接动作。最终连接许可仍由 SDK 判断。

## 8. 连接和断开

```cpp
const int connectRc = RB_Output_ConnectInstanceByIndex(rbCanOutputIndex);
if (connectRc != RB_OK && connectRc != RB_LICENSE_REQUIRED) {
    // 显示连接失败和 SDK 日志。
}

// 页面刷新只读取状态。
const int connected = RB_Output_IsInstanceConnected(rbCanOutputIndex);
const int active = RB_Output_IsInstanceRuntimeActive(rbCanOutputIndex);

// 用户断开。
RB_Output_DisconnectInstanceByIndex(rbCanOutputIndex);
```

连接调用成功表示流程已经启动，不保证电机立即进入运行。继续读取绑定、健康和电机状态，直到 SDK 报告就绪、运行或明确故障。

## 9. 读取单输出电机状态

```cpp
const int motorCount = RB_Can_GetOutputMotorCount(rbCanOutputIndex);
for (int i = 0; i < motorCount; ++i) {
    RB_CanMotorState motor{};
    if (RB_Can_GetOutputMotorState(rbCanOutputIndex, i, &motor) == RB_OK) {
        // 显示 NodeId、AxisKey、Online、Homed、Ready、Fault、
        // StatusWord、ErrorCode、Temperature 和 OperationMode。
    }
}
```

`motorIndex` 是当前输出会话中的行号，不是 CAN Node-ID。重连、重新绑定或设备拓扑变化后，重新取得电机数量并重建列表。

## 10. 健康诊断和人工恢复

```cpp
RB_CanOutputHealthState health{};
health.Size = sizeof(health);
if (RB_Can_GetOutputHealthState(rbCanOutputIndex, &health) == RB_OK) {
    // RuntimeStatus 和 FaultReason 用于机器判断。
    // StatusText、USB/CAN 计数和延迟用于诊断显示。
}
```

`RequiresHumanIntervention != 0` 时，前端不得自动无限重试。只有用户确认以下条件后才能提供恢复按钮：

- 物理急停已经释放；
- 设备 UID、逻辑索引和协议身份正确；
- USB/CAN 连接恢复；
- 电机和机械结构处于安全状态；
- 当前没有其它维护动作。

```cpp
const int rc = RB_Can_RequestOutputRecovery(rbCanOutputIndex);
```

`RB_OK` 只表示 SDK 接受了一次恢复请求。页面继续轮询健康状态，不能在定时器中重复调用恢复接口。

## 11. 推荐页面状态机

1. 页面打开或热插拔提示后刷新设备目录。
2. 使用 UID 恢复设备选择，不保存目录行号。
3. 读取当前 RB_CAN 输出的绑定状态。
4. 未绑定或冲突时允许用户选择 UID 和唯一逻辑索引。
5. 绑定成功后刷新设备和绑定状态。
6. 用户明确点击连接后启动输出连接流程。
7. 运行期间只读取连接、绑定、电机和健康状态。
8. 故障需要人工处理时停止自动操作并显示恢复按钮。
9. 页面关闭不应断开正在运行的输出；应用退出必须执行安全断开。

## 12. 安全退出

```cpp
RB_Output_RequestDisconnectAll();

// 使用宿主定时器等待，不能无限阻塞 UI 线程。
for (int i = 0; i < 300 && !RB_Output_AreAllDisconnected(); ++i) {
    Sleep(10);
}

RB_Runtime_Shutdown();
```

退出期间停止设备刷新、绑定、恢复和其它新命令。不得通过直接卸载 DLL 或终止工作线程代替 SDK 关闭流程。

## 13. 最低验收清单

- 单张 RB_CAN 设备可以刷新并显示 UID、索引、固件和身份状态。
- 两张设备同时在线且旧逻辑索引相同时，可以按 UID 为其中一张设置新的唯一索引，不要求拔出另一张。
- 输出绑定后重启应用，仍能按 UID 和逻辑索引恢复绑定。
- 拔出和重新插入设备后，旧目录索引不会被继续使用。
- 连接、回零、运行、断开和退出状态能够完整显示。
- 设备离线、身份冲突、Bus-Off、节点离线、驱动故障和急停能够区分。
- 人工恢复只由用户触发，不会形成无限重试。
- 应用退出前全部输出完成安全断开。
