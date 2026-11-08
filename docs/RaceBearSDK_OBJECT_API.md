# RaceBear SDK 统一对象 ABI

本版用于验证“少量通用入口 + 内部业务对象”的接入方式。签名与布局以同包
`include/RaceBearSDK.h` 为准。旧的 268 个公开接口全部保留，新增 7 个
`RB_Object_*`，当前共 275 个公开函数声明。主要新接入面是这 7 个入口加现有
`RB_Runtime_GetVersion/Initialize/Shutdown`，共 10 个；不表示 DLL 已只剩 10 个导出。
独立产品在初始化前设置 AppDataName；可通过同步服务完成，也可沿用原入口。
运行时版本为 `0.6.4`，对象协议版本为 1；不能仅凭运行时版本判断是否支持，
动态加载时须检查新增导出，随后读取 Describe 的 `protocolVersion`。

第二阶段增加 `RB_OBJECT_NATIVE_CALL=3`，在现有 7 个入口内补齐同步业务服务。
目录共 9 个状态/草稿对象、29 个服务域和 20 个异步动作；服务包含 265 个公开操作，
另有 80 个标为 `firstParty=true` 的第一方兼容操作，不属于第三方合同。
CAN、平台、游戏、串口、仪表像素、句柄等可通过对象入口接入，具体参数和业务规则
仍与原类型化接口一致。旧导出保留；这次统一调用通道，不消除各业务数据类型和行为。
不存在任意内存地址/成员路径访问，对象数量不与 ABI 函数数量绑定。

## 0. 第三方从哪里查 key、怎样接入

`rb.*` 是区分大小写的稳定标识，按下表选择调用位置，不能按字符串外观混用。

| key 种类 | 示例 | 传给哪里 | 得到什么 |
|---|---|---|---|
| 状态/草稿对象 | `rb.telemetry.snapshot`、`rb.wind.config` | Resolve 得到 ID，再 Read/Write | 本文第 2 节的结构体或 JSON；草稿还有修订号 |
| 同步服务对象 | `rb.service.wind` | Resolve 得到 ID，再 Write，format=3 | 执行请求中的一个操作，输出数据写入业务参数，返回值写入 Result 槽 |
| 同步服务操作 | `rb.wind.get_default_config` | `RB_ObjectNativeCall.Operation` | 按服务目录的参数顺序和返回类型执行业务；不是 Resolve 的对象 key |
| 异步命令 | `rb.wind.apply` | Invoke 的 command，再 Poll | 任务 ID、终态 ResultCode 和结果 JSON；不是 Resolve 的对象 key |
| 遥测字段 | `rb.vehicle.speed` | 用遥测目录解析字段索引，再访问快照数组 | 某个字段的值和支持信息；不是可单独 Resolve 的对象路径 |

开发时按以下顺序查阅和调用：

1. 使用同包公开头、LIB/DLL；先查本文第 1 节的入口和缓冲区规则。
   需要独立配置目录时，在 Initialize 前通过 `rb.service.runtime` 的
   `rb.runtime.set_app_data_name` 操作设置名称，签名和参数见同步服务目录。
2. Describe(0) 先查询 requiredBytes，再提供缓冲区读取 JSON 总目录。
   检查 protocolVersion、objects 和 commands；服务对象的 operations 包含操作 key、
   signature、result、arguments 和 firstParty。仅使用 `firstParty=false` 的公开操作。
   对象 ID 只在本次 SDK 生命周期内缓存，配置持久化保存稳定 key。
3. Initialize 成功后，按第 2 节读取状态或配置。读取实际遥测前，通过
   `rb.runtime.start` 启动计算循环并用 Poll 确认结果；用 Sampled/Sequence 判断是否有新帧。
4. 配置编辑按 Read → 修改 → 带 Revision 的 Write → Invoke Apply/Save → Poll 终态。
   同步服务按 Resolve 服务域 → 构造封套 → Write(format=3) → 检查 Result；
   两条写入路径的修订与完成规则不同，见第 4 节及文末“同步本机服务”。
5. 退出先停止所有宿主调用、测试、回调和任务，关闭设备句柄，再 Shutdown。
   错误、冲突、任务消费和初始化前可用范围见第 5、6 节。

[同步服务目录](RaceBearSDK_SERVICE_API.md)列出全部 265 项第三方公开操作的
服务对象 key、操作 key、签名、返回类型及有序参数。“对应业务合同”列用于在
[API 手册](RaceBearSDK_API.md)和公开头中定位具体参数范围、输入输出方向、
返回码及所有权；采用对象 ABI 时不需要因此导入对应旧函数。
表中的 C 类型直接对应同包公开头，不能凭名字猜结构体字段。
完整包中的 `examples/object_api/main.cpp` 提供目录查询、同步默认配置、
草稿写入及遥测读取的可编译代码；其运行边界见第 7 节。

### 公开同步服务域索引

当前 29 个服务域中，以下 26 个含第三方公开操作，共 265 项。
运行时可能还列出第一方专用服务域，部分下表服务域也包含第一方操作；
服务域存在不代表其中所有操作都公开，逐操作以 SERVICE_API 和 firstParty 标志为准。
下表数量只统计公开操作，不包含 80 项第一方兼容操作。

| Resolve 的服务对象 key | 公开操作数 | 用途 |
|---|---|---|
| `rb.service.can` | 37 | CAN 设备发现、绑定、健康、电机状态、参数维护和公开升级操作 |
| `rb.service.command` | 1 | 按原 Command 合同执行命令 |
| `rb.service.dashboard` | 12 | 仪表帧渲染、模板包目录与包管理 |
| `rb.service.dashboardweb` | 4 | Web 仪表打开、提交帧、查询状态和关闭 |
| `rb.service.data` | 2 | 按原数据 key 读取或写入 JSON |
| `rb.service.debug` | 3 | 调试监视器打开、关闭与状态 |
| `rb.service.dynamic` | 13 | 动态滤波配置、候选输入、配置包与应用 |
| `rb.service.externalsource` | 4 | 外部遥测源打开、提交帧、状态与关闭 |
| `rb.service.game` | 17 | 游戏目录、选择、配置、安装信息与自定义注册 |
| `rb.service.haptic` | 7 | 震动配置、默认参数、目录与应用 |
| `rb.service.log` | 1 | 读取 SDK 日志，宿主保持单消费者 |
| `rb.service.manualpose` | 4 | 手动平台测试开关、驱动值设置、读取与复位 |
| `rb.service.motioneffect` | 10 | 运动增强配置、目录、默认参数与应用 |
| `rb.service.output` | 25 | 输出类型、实例配置、目录、连接与运行状态 |
| `rb.service.platform` | 22 | 平台配置、选择、能力、保护参数与配置包 |
| `rb.service.plugin` | 4 | 功能插件目录、刷新、启动与状态渲染 |
| `rb.service.rgb` | 23 | RGB 设备、配置、灯效、测试、诊断与公开升级操作 |
| `rb.service.runtime` | 11 | 应用目录名、计算循环、间隔与综合运行状态 |
| `rb.service.seatbelt` | 5 | 安全带默认配置、配置读写、应用与测试 |
| `rb.service.seatbeltcan` | 7 | 安全带 CAN 配置、状态、回零与显式重新启用 |
| `rb.service.serial` | 32 | 串口枚举、句柄、读写、回调、重连和错误 |
| `rb.service.state` | 1 | 按原状态入口读取综合运行状态 |
| `rb.service.system` | 2 | 系统配置读取与保存 |
| `rb.service.telemetry` | 5 | 遥测目录、稳定 key 查找及原类型化值读取 |
| `rb.service.vocore` | 8 | VOCORE 枚举、句柄、像素提交、亮度与状态 |
| `rb.service.wind` | 5 | 风感默认配置、配置读写、应用与测试 |

高频遥测优先 Read(`rb.telemetry.snapshot`)，不逐值调用 telemetry 同步服务。
使用共享配置草稿时优先第 2 节的配置对象，同步配置操作不更新草稿修订号。
服务域概述只用于找目录，各操作是否需要初始化、授权、有效设备或维护状态，
以及指针有效期和阻塞行为，仍遵循对应业务合同。

## 1. 入口和缓冲区

| 入口 | 输入 | 输出与用途 |
|---|---|---|
| `RB_Runtime_GetVersion` | 无 | SDK 持有的版本字符串，调用方不释放 |
| `RB_Runtime_Initialize` | 无 | 初始化结果码；应用目录名在调用前设置 |
| `RB_Runtime_Shutdown` | 无 | 无返回值，停止并释放 SDK |
| `RB_Object_Describe` | objectId、buffer、bufferSize、requiredBytes 指针 | objectId=0 取 JSON 总目录，否则取对象描述；requiredBytes 返回含 NUL 的容量 |
| `RB_Object_Resolve` | key、objectId 指针 | 稳定对象 key 转本次 DLL 的 ID，禁止持久化 ID |
| `RB_Object_Read` | objectId、format、buffer、bufferSize、info 指针 | 快照或草稿复制到 buffer；info 返回负载长度及适用的修订/采样信息 |
| `RB_Object_Write` | objectId、format、data、dataSize、expectedRevision、newRevision 指针 | Binary/JSON 更新草稿并返回修订号；NATIVE_CALL 同步执行业务，业务返回值写 Result 槽 |
| `RB_Object_Invoke` | command、argsJson、taskId 指针 | 验证参数并提交一次动作，返回任务 ID |
| `RB_Object_Poll` | taskId、consume、buffer、bufferSize、info 指针 | 任务状态、实际结果及结果 JSON；成功消费完整终态后才释放记录 |
| `RB_Object_GetLastError` | buffer、bufferSize、requiredBytes 指针 | 当前调用线程的入口错误详情；不会清除所查询的错误 |

精确 C 类型与函数签名以同包 `include/RaceBearSDK.h` 为准。
上述 int 返回值为入口状态码，`RB_OK=0`；GetVersion 返回指针，Shutdown 无返回值。
Invoke 的入口成功不代表任务执行成功，Poll 要检查 info.ResultCode；
同步服务 Write 的入口成功只代表分派成功，业务返回值从 Result 按目录类型恢复。

`RB_ObjectReadInfo` 和 `RB_ObjectTaskInfo` 必须先清零，再设置
`Size=sizeof(结构体)`、`Version=1`。每个并发调用方独占其 buffer/info；
快照和草稿不返回内部对象指针、不保留输入缓冲区；同步服务的句柄/字符串/回调见后文。
二进制使用当前公开结构体的原生 x64
布局，不使用 pack(1)，`uint64_t` 为 64 位。

Describe/GetLastError 的 `requiredBytes` 必填。Read/Poll 用 info 的 `PayloadBytes`
返回容量。`nullptr,0` 查询返回 `RB_BUFFER_TOO_SMALL`；JSON 容量包含末尾 NUL，
二进制容量等于结构体长度。容量不足不写半份负载。Read 两次之间对象可能变化，
最终成功那次的负载、Revision/Sequence 才配套；不要沿用长度查询那次的修订号。
Poll 对尚未完成的任务返回 `RB_OK`、`PayloadBytes=0`，无需结果缓冲区。

Read 的长度查询可能首次加载草稿/目录或启用遥测发布，但不会应用配置或执行设备动作。
成功的对象入口清除本线程旧错误；失败时读取 GetLastError。异步执行错误必须读
Poll 的 `ResultCode` 和结果 JSON，不能跨线程读取工作线程的错误字符串。

## 2. 对象范围

| 稳定 key | 格式与 schema | 返回内容 | 写入 |
|---|---|---|---|
| `rb.telemetry.snapshot` | Binary，`RB_ObjectTelemetrySnapshot/1` | 整帧 Raw/Processed、原始值支持标志、源连接和授权状态；info 提供采样序号与时间 | 只读，高频整帧 |
| `rb.telemetry.catalog` | JSON，与原 telemetry.catalog 合同相同 | 遥测字段 key、索引和字段元数据，用于解释快照数组 | 只读，低频；本版目录内部容量 512 KiB |
| `rb.runtime.state` | Binary，`RB_RuntimeState/2` | 授权、游戏、源、Motion、平台、输出、CAN、风感和安全带等综合状态；精确成员见公开头 | 只读，低频 |
| `rb.wind.state` | Binary，`RB_WindState` | 风感启用、测试、车速与通道输出状态 | 只读，低频 |
| `rb.seatbelt.state` | Binary，`RB_SeatbeltState` | 刹车/横向/碰撞张紧、测试及左右通道输出状态 | 只读，低频 |
| `rb.rgb.state` | Binary，`RB_RgbState/2` | RGB 连接、效果层、帧计数、错误与功耗状态；设备目录与诊断由 rgb 服务获取 | 只读，低频 |
| `rb.wind.config` | Binary，`RB_WindEffectConfig/1`；JSON | 风感曲线、增益、通道补偿等草稿，info 返回 Revision | 草稿；JSON 为补丁，Binary 为整组替换 |
| `rb.seatbelt.config` | Binary，`RB_SeatbeltEffectConfig/2` | 安全带输入、预紧、刹车/横向/碰撞和左右通道等草稿 | 草稿，整组替换 |
| `rb.rgb.effects` | Binary，`RB_RgbEffectProfile/3` | RGB 灯效总开关、帧率、亮度及效果层草稿；不是设备 Flash 配置 | 草稿，整组替换 |

格式常量为 `RB_OBJECT_BINARY=1`、`RB_OBJECT_JSON=2`。二进制配置写入的长度、
Size 和 Version 必须精确匹配本版 schema，不接受零版本兼容写法。业务校验复用原模块
校验器。Wind 有字段元数据；复杂 RGB/安全带仅提供结构体 schema，不承诺能靠 Describe
自动生成完整表单。

## 3. 遥测并发和性能

计算线程在当前计算/输出阶段结束时复制一份 Source 槽位快照，使用 try_lock 发布：
读者正在复制时跳过本次发布，不等待。初始化后的 Resolve/Read 遥测对象启用发布，保持到 Shutdown；
未启用时计算路径只检查标志，不组装整帧。

高频 Read 只复制快照，不获取运行态、命令或配置草稿锁，不读文件/USB，不执行算法。
读者之间有独立的短快照锁；授权复核仍会取得现有授权状态短锁。因此不承诺完全无锁、
零开销或硬实时。其它状态/目录读取和异步旧后端操作仍可能获取运行态锁，应保持低频。
泛型接口不参与算法内部逐字段读写和设备发送，不把 JSON 放入计算热路径。

`Sequence` 是成功发布序号，允许跳号或重复读取；`SampleTimeUs` 来自本进程稳定时钟，
不是 UTC。`Sampled=0`、Count=0 表示尚无实际发布。循环暂停/停止后可继续读最后帧，
必须结合序号、时间、SourceConnected 和运行状态显示陈旧数据，不能把重复帧当新输入。
Raw/Processed 下标通过目录 Index 解析，稳定 `rb.*` key 用于持久化。
RawSupported=0 的原始输入不能显示成“实际测量为零”。

公开目录之外的槽位清零；Count 为槽位上界，目录过滤造成的空洞不是有效公开数据。
沿用旧读取权限：Raw 和 RawSupported 免主授权读取，Processed 未授权时清零。
即使循环已暂停，Read 也会复核授权并屏蔽旧处理值。RGB/安全带/Wind 的内部计算和设备输出
不新增主授权要求；Haptic 既有独立授权行为也不变。急停、释放、连接和人工重新启用
规则原样保留，通用入口不能绕过。

## 4. 配置草稿和冲突

每个可写对象有独立锁、完整草稿和修订号。首次读取从该模块保存配置加载，修订号从 1
开始；读取失败直接返回错误，不伪造默认草稿。运行状态不是草稿，读到保存配置不等于
当前设备已应用。Write 只改变草稿，不保存、不应用、不连接。

写入时传最近成功读取的 `Revision`。先校验整组候选数据，再在短锁内比较版本并替换，
成功递增修订号；0 不表示强制覆盖。两个写者基于同一版本提交，只能一个成功。
`RB_OBJECT_CONFLICT` 时 `newRevision` 返回当前版本，重新读取、由宿主合并后再提交，
不要自动用新版本重放旧的整组覆盖。

草稿由 SDK 所有，同对象的多个页面共享它，不是每个调用源一份。修订号仅管理新入口；
旧 ABI 或外部文件改动不会自动更新草稿。显式 `*.reload` 导入保存配置并递增版本，
执行时若发现提交以来草稿已变，返回冲突，保留新草稿；读取失败同样保留原草稿。
Apply/Save 捕获**提交当时**的完整草稿和修订号，后续编辑不会修改已提交任务。
Save 与 Apply 独立，保存成功不代表运行态已应用；具体执行结果沿用原模块合同。

Wind JSON 是非空对象补丁，只允许下列字段；Binary 始终为完整替换。

| JSON 字段 | 类型 | 范围/单位 |
|---|---|---|
| `enabled` | bool | true/false，不接受 0/1 |
| `inputMinKmh` | number | >=0，km/h |
| `inputMaxKmh` | number | >inputMinKmh，km/h |
| `outputMinWorkPercent`、`masterGainPercent` | number | 0..100，% |
| `gamma` | number | >0 |
| `channelGainPercent`、`channelOffsetPercent` | number[4] | 每项 -100..100，% |

拒绝未知/重复字段、null、NaN/Inf、错误类型、错误数组长度和尾部垃圾。
使用完整候选值进行跨字段校验，不会只写入半份补丁。JSON Write 最多 65536 字节，
可带末尾 NUL，不能有内嵌 NUL；Invoke 参数为 NUL 结尾 UTF-8 JSON 对象，同样最多
65536 字节（不含末尾 NUL）。数值格式不依赖系统区域设置。

## 5. 异步动作

| Invoke 的完整命令 key | JSON 参数 | 用途 |
|---|---|---|
| `rb.wind.apply` | `{}` | 应用提交时的风感草稿 |
| `rb.wind.save` | `{}` | 保存提交时的风感草稿 |
| `rb.wind.reload` | `{}` | 从保存配置重新加载风感草稿 |
| `rb.seatbelt.apply` | `{}` | 应用提交时的安全带草稿 |
| `rb.seatbelt.save` | `{}` | 保存提交时的安全带草稿 |
| `rb.seatbelt.reload` | `{}` | 从保存配置重新加载安全带草稿 |
| `rb.rgb.apply` | `{}` | 应用提交时的 RGB 灯效草稿 |
| `rb.rgb.save` | `{}` | 保存提交时的 RGB 灯效草稿 |
| `rb.rgb.reload` | `{}` | 从保存配置重新加载 RGB 灯效草稿 |
| `rb.wind.test` | `{"enabled":true,"outputPercent":0}`，百分比 0..100 | 开启/停止风感测试 |
| `rb.seatbelt.test` | `{"enabled":true,"leftPercent":0,"rightPercent":0}`，0..100 | 开启/停止左右安全带测试 |
| `rb.rgb.blackout` | `{}` | RGB 黑屏 |
| `rb.rgb.connect` | `{"uid":"设备UID"}`，1..63 UTF-8 字节 | 连接指定 RGB 设备 |
| `rb.rgb.disconnect` | `{}` | 断开 RGB 设备 |
| `rb.output.connect` | `{"instanceIndex":0}`，0..31，仅当前输出目录 | 连接指定输出实例 |
| `rb.output.disconnect` | `{"instanceIndex":0}`，0..31，仅当前输出目录 | 断开指定输出实例 |
| `rb.runtime.start` | `{"intervalMs":2}`，整数 1..1000，ms | 启动内部计算循环 |
| `rb.runtime.stop` | `{}` | 停止内部计算循环 |
| `rb.runtime.pause` | `{}` | 暂停内部计算循环 |
| `rb.runtime.resume` | `{}` | 恢复内部计算循环 |

共 20 个命令，传入完整 key，不拼接斜线缩写。关闭测试传
`{"enabled":false}`；百分比字段可省略，若提供仍会验证。所有命令拒绝未知或重复参数。
输出 Index 来自现有输出目录，不是稳定身份；可用输出服务读取目录，
并串行化会改变索引的配置操作。普通平台连接继续受授权限制。

Invoke 返回 `RB_OK` 只代表已提交一次。单个 SDK 工作线程按队列执行，慢命令会让后续命令
排队；没有逐任务取消/超时终止接口，也不能用该队列承诺紧急停止的响应时限。
已有硬件安全状态机不依赖此队列。宿主不可无界积压测试、连接或保存请求。

任务状态为 QUEUED=0、RUNNING=1、COMPLETE=2、CANCELLED=3。终态 `ResultCode`
才是动作执行结果；Poll 自身返回 OK 不代表动作成功。终态 JSON 包含 code/message，
正常完成还有 revision/backendCode；取消/内部异常结果可能没有这两个字段，应以 info 为准。
配置读取/保存的旧后端错误码与 RB_Result 存在重号，故新入口统一映射为 RB_ERROR，
任务 JSON 的 backendCode 或同步 GetLastError 保留原配置错误码。不能把配置解析失败
解释成需要授权，也不能把写文件失败解释成缓冲区不足。其它动作仍沿用公开 RB_Result。
最多保留 64 条记录，包含未消费的完成任务。达到上限返回 BUSY，taskId=0，不执行动作。

`consume=0` 可反复读取；`consume=1` 只有完整复制终态结果后才删除记录。容量不足保留
结果且不重复动作。正常路径：Invoke 一次，重复 Poll，终态按容量扩容，再 Poll 消费。
多个调用源可查询同一任务，但消费权应归一个所有者；被消费后的 ID 返回 INVALID_ARGUMENT。
任务 ID 仅本次 DLL 生命周期内有效，不持久化。轮询超时不等于动作已取消，不重发 Invoke。

## 6. 生命周期和失败

Describe/Resolve/GetLastError 可在初始化前使用。Read、草稿 Write、Invoke 要求 Initialize 成功。
同步服务 Write 保留各业务原有初始化前能力，不统一放开所有业务；其余须先初始化。
生命周期由宿主一个所有者串行执行。退出时停止宿主调用线程和新的任务提交，等已提交任务
收敛，明确停止测试、断开输出、关闭旧接口所持句柄，再调用 Shutdown。
Shutdown 拒绝新动作、取消未开始任务，等待在途工作线程结束，然后销毁旧后端。
旧后端慢调用没有硬超时保证，Shutdown 也不承诺固定返回时限；禁止强杀线程或提前卸载 DLL。
完成/取消结果可在 Shutdown 返回后顺序查询，重新 Initialize 会清空所有旧任务和草稿。

| 返回值 | 处理 |
|---|---|
| `RB_BUFFER_TOO_SMALL=-3` | 按返回容量扩容，只重读 Read/Describe/Poll/GetLastError |
| `RB_OBJECT_CONFLICT=-100` | 草稿冲突，重读合并；终态任务冲突查看 ResultCode |
| `RB_OBJECT_NOT_READY=-101` | 未初始化或已关闭；不要自动重放动作 |
| `RB_OBJECT_BUSY=-102` | 消费已完成任务后，由用户意图决定是否重新提交 |
| `RB_OBJECT_NOT_SUPPORTED=-103` | key/ID/格式/动作不在本版范围 |
| `RB_INVALID_ARGUMENT=-2` | 修复长度、版本、字段、参数或失效的任务 ID |
| `RB_LICENSE_REQUIRED=-4` | 原后端的受保护能力限制，停止自动重试 |
| `RB_ERROR=-1` | 配置后端失败查看 backendCode/入口错误详情，其它失败结合结果和 SDK 日志诊断 |

## 7. 可编译示例与验收边界

完整 SDK 中的示例为 `examples/object_api/main.cpp` 和该目录 CMakeLists；
模块 Docs 只是文档分发副本，示例与 Skill 应从完整 SDK 获取。
默认只读取目录，不初始化；`--runtime` 使用独立 `RaceBearObjectDemo` 配置目录初始化，
启动循环、改内存 Wind 草稿并读取快照，不保存配置、不连接输出。该模式要求当前账户能
写独立 AppData/Documents 目录。实际产品应实现自己的错误、冲突合并和关闭协调。

内部并发回归编译真实对象服务，以可控后端替身覆盖多读者完整帧、双写冲突、慢任务隔离、
提交快照、队列上限、短缓冲区不消费、不重复执行、reload 冲突/失败、关闭等待及授权屏蔽。
这些离线结果不能替代真实运行态、实际游戏、USB 长稳和机械保护验收。

## 同步本机服务

先通过 Describe 查询实际支持的 `rb.service.*` 对象，再 Resolve 得到本次进程 ID。
逐操作 key、十六进制签名、返回类型和有序参数见
[同步服务目录](RaceBearSDK_SERVICE_API.md)。`firstParty=true` 仅供第一方兼容，
第三方不能据此依赖 SDK 私有接口。Describe 总目录已超过旧版 64 KiB 假设，务必查询长度。

`RB_ObjectNativeCall` 使用本机自然布局，Version=1，PointerBytes=sizeof(void*)，
Size 必须完全匹配。Arguments 有 16 个 uint64_t 槽：按目录顺序用 memcpy 复制
每个参数原类型的字节到先清零的槽；浮点按位复制，不能数值转换；未使用槽为 0。
Signature 填目录签名的 64 位值；Operation 指向 NUL 结尾 key；Result 指向本次调用独占的
uint64_t 槽，void 操作也必须提供。传 dataSize=sizeof(request)、expectedRevision=0，
newRevision 必填并始终返回 0。版本、架构、长度、参数数、签名或归属错误均在执行业务前拒绝。
封套、返回值槽和 newRevision 必须使用互不重叠的有效内存，业务输出也不能覆盖这些控制数据。

Write 的 RB_OK 只表示同步分派成功，业务返回值按原类型的字节从 Result 恢复；
例如业务返回 RB_LICENSE_REQUIRED 时 Write 仍可返回 RB_OK。void 的 Result=0。
服务不统一改写原业务错误码；与旧配置错误码重号时沿用该业务合同，
不能套用异步草稿任务的 backendCode 规范化规则。传输失败读取 GetLastError，不自动重放动作。
宿主类型包装可以让 int 返回传输错误，指针返回 nullptr、浮点/unsigned 返回 0，
void 不返回错误；遇到这些无状态返回类型时须结合 GetLastError 判断传输失败。

这是同进程、同架构、同版本本机调用协议，**不能把封套持久化或直接发到网络/Web**。
签名校验只检查类型合同，不保证任意伪造指针安全；宿主必须提供有效内存与正确容量。
像素、串口和结构体指针在同步调用期间借用，不经 JSON，不复制大块数据，也不保留封套。
若业务原本异步接受像素，仍由原业务在返回前完成必要复制；回调注册例外：
函数指针和 userData 按原 API 保持有效直到注销并停止在途回调。
句柄仍须显式 Close/Destroy，借用字符串按原 API 有效期使用，不自行释放。

服务入口不持有共享分派锁，不排入 64 槽异步队列；每次调用独立封套/结果，
同步绘图回调可重入对象入口。底层业务锁、阻塞时间和单一所有者要求不变：
日志只能一个消费者，设备句柄关闭不能与使用并发，同配置多个写者需串行协调。
同步配置服务操作保存文件/运行态，不更新新草稿修订号；需要共享草稿时仍优先
`rb.wind.config`、`rb.seatbelt.config`、`rb.rgb.effects`，外部更新后显式 reload。
慢服务应放入宿主已有工作线程，停止所有服务调用及回调后才能 Shutdown。
高频遥测继续用整帧快照，不应逐值调用同步兼容服务。
