# 服务业务合同补充

本文与 [同步服务目录](RaceBearSDK_SERVICE_API.md)、[离线目录](service-api.json) 和
[对象协议](RaceBearSDK_OBJECT_API.md) 配套。操作是 key，不是可导入函数。
逐操作用途、参数、单位和返回值从公开头注释同步到服务目录；复杂结构的字段以头文件为准。

## 两层返回值和缓冲区

`RB_Object_Write` 返回对象分派结果，只有成功后才解释 Result 槽。
Result 是业务原返回值；不能把所有业务负数都解释为 `RB_Result`。
读取/保存配置以及命名方案/配置包采用配置后端编码：

| 值 | 配置后端含义 |
|---|---|
| 0 | 成功 |
| -1 | 参数无效 |
| -2 | 文件打开失败 |
| -3 | 文件写入失败 |
| -4 | JSON 解析失败 |
| -5 | 节点不存在 |
| -6 | 内存分配失败 |
| -7 | 配置后端缓冲区不足 |
| -8 | 内部失败 |
| -9 | 节点已存在 |

例如同步配置服务的 -3 可能是文件写入失败，不能自动当作入口的
`RB_BUFFER_TOO_SMALL` 重试写入。对象草稿和异步命令则将配置失败规范化为
`RB_ERROR`，通过错误详情/任务 JSON 的 backendCode 提供原码。
名称字符串复制入口可返回 `RB_BUFFER_TOO_SMALL`；新独立效果包的 outName 太小时
在写入前返回该值，不执行导入。旧平台/Dynamic 包接口的短缓冲并不保证未发生保存，
应提供 `RB_TEXT_MEDIUM` 容量，不能因短缓冲自动重放写动作。

一般字符串为 UTF-8/NUL；bufferSize 是字节总容量，包含 NUL。
输出结构体先清零，带 Size/Version 的结构必须设为当前公开值。
指针只在同步调用期间借用；返回指针、设备句柄和回调使用对象协议的专门所有权规则。
日志只能一个消费者；设备句柄的使用和关闭由同一宿主协调。

## 方案归属与迁移

| 域 | 方案列表与选择 | 保存/删除 | 便携包 |
|---|---|---|---|
| Dynamic | 独立动态列表，游戏绑定动态方案 | 删除还清理同名 MotionEffect/Haptic；删除后索引变化 | dynamic 包含动态和可选同名 MotionEffect、Haptic |
| MotionEffect | 独立列表，不能复用 Dynamic 索引 | 按名称或本域索引读写；删除不删除 Dynamic | motionEffect 独立包不要求同名 Dynamic |
| Haptic | get_profile_count/name 明确复用 Dynamic；ByIndex 也是 Dynamic 索引 | 同名附属配置；delete_profile 只删震动节点，不删 Dynamic | haptic 独立包要求接收端已有同名 Dynamic；整体迁移用 dynamic 包 |
| RGB | 独立命名库，按 UTF-8 字节排序；活动配置可以未命名 | save_effect_profile_as 只存库副本；选定另做；删除不影响正在输出的灯效 | rgb 独立包不含硬件配置、Flash 或固件 |

Haptic 缺少同名节点时走现有默认回退，不能把它解释成“列表长度不同”的索引错误。
`get_catalog` 列出效果类型，不列用户方案。Haptic 按名字保存仍允许历史独立节点，
但它不会自动进入以 Dynamic 为主的可选列表；第三方应先建立 Dynamic 同名方案。

所有新增方案名称是 1-127 UTF-8 字节，禁止控制字符、`/`、`\`、双引号及 `_Default`。
RGB 的 count/name 是当前目录快照，增删后刷新；持久化名称，不持久化索引。

RGB 的操作分工：

1. `save_effect_profile_as(name, profile)` 保存命名副本，不切换活动配置。
2. `save_current_effect_profile_name(name)` 同一次文件写入更新活动配置和选择名，
   不应用引擎、不连接设备、不改草稿 Revision。
3. 调用 `rb.rgb.reload` 并 Poll 成功，再读取 `rb.rgb.effects` 得到新的基线/Revision。
4. 显式 `rb.rgb.apply` 并 Poll；连接是另外的动作。
5. 原 `save_effect_profile(profile)` 保存未命名活动配置并清空选择名。
   覆盖某命名副本也会清空同名选择，避免名称与活动配置不符；需要时再显式选定。

新独立包为 UTF-8 JSON，最大 4 MiB，schema=1，kind 为 rgb/motionEffect/haptic，
key 为名称，config 是完整配置。导入验证类型、必需字段、布尔/数值类型、有限数值、
范围、重复键和稳定遥测 key；不接受未知绑定，不把运行时输入索引作为便携语义。
同名导入覆盖，不选定、不应用、不连接。导出可覆盖宿主指定的包文件。
旧 dynamic 包没有 haptic 字段时保留接收端的震动配置；新包可携带该字段。
Dynamic 多配置文件导入并非文件系统事务：写盘失败可能已保存部分域，失败后必须
分别读回，不能显示整体成功。逻辑校验失败发生在开始保存之前。

## JSON 数据包

调用 `rb.service.data` 的 `rb.data.read_json`，业务参数 key 是下面这些字符串，
它与对象 Resolve key 不同。例如 `telemetry.catalog` 是 read_json 参数，
`rb.telemetry.catalog` 才是 Read 对象。未知数据 key 返回 `RB_INVALID_ARGUMENT`。

| 数据 key | 读取内容 | 可写 |
|---|---|---|
| runtime.snapshot | 简化运行态 JSON；正式 UI 优先读 rb.runtime.state | 否 |
| game.catalog | count/items；游戏目录 | 否 |
| dashboard.catalog | count/items；内置仪表类型目录，不是用户 .rbdash 列表 | 否 |
| telemetry.catalog | count/items；字段 index/key/name/unit 等元数据 | 否 |
| output.catalog | 输出类型和实例目录 | 否 |
| motionEffect.catalog | count/items；index/name/description/outputText，效果类型 | 否 |
| haptic.catalog | 同上；震动效果类型，不是方案列表 | 否 |
| can.state | statusText/motorCount/firmwareUpdate/motors，CAN 诊断摘要 | 否 |
| plugin.catalog | count/items；名称、版本、状态、图标与可启动状态 | 否 |
| config.telemetry | 遥测配置文档 | 是，整份替换 |
| config.dynamicV2 | 动态配置文档 | 是，整份替换 |
| config.platform | 平台配置文档 | 是，整份替换 |
| config.output | 输出配置文档 | 是，整份替换 |

配置 JSON 是原始整文档兼容入口，不是字段补丁或稳定的跨版本方案格式。第三方优先用
类型化同步服务、修订草稿和便携包；需要编辑原始 JSON 时只修改刚读回的当前版本文档，
保留未知节点，单一写者协调，成功后重新读回。不能自行拼接或拷贝私有配置文件路径。
read_json 不提供 requiredBytes 输出；容量不足时只扩容重读，不写入、不执行动作。
write_json 不隐式 Apply/Connect；读取到文件内容不证明运行态已更新。

## 周期、单位与关闭

`rb.runtime.start_loop/set_loop_interval_ms` 的 intervalMs 是整数 1-1000 ms。
`rb.manualpose.set_drive` 的 dofIndex 0-5 为 Sway/Surge/Heave/Yaw/Roll/Pitch；
线性 DOF 单位 mm，旋转单位度，预览范围从 `rb.platform.get_preview_pose_limit` 获取。
真实手动测试仍受授权和机械保护约束，离开测试必须复位并停用。

Shutdown 前停止宿主线程、回调和新增动作，关闭所有设备与借用句柄。
Shutdown 后不得继续业务访问；Describe/Resolve/GetLastError 及已完成/取消任务的顺序 Poll
按对象协议仍可用。重新 Initialize 会丢弃之前的任务和草稿。
