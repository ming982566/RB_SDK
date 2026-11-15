# RGB 数据流与交互

RGB 的设备配置、主机灯效草稿、命名方案和运行态是不同对象。
本指南给出完整的第三方实现路线；操作参数见 [服务目录](RaceBearSDK_SERVICE_API.md)，
名称/导入语义见 [方案合同](RaceBearSDK_SERVICE_CONTRACTS.md)。
独立 RGB 不需要 SDK 主授权，设备离线、协议能力和固件维护仍照常限制操作。

## 发现与连接

后台调用 rgb.refresh_devices，按 get_device_count/get_device_info 填充设备列表。
持久化 Uid，不保存设备枚举索引。用户选择后 Invoke `rb.rgb.connect`，参数
`{"uid":"实际设备UID"}`；Poll 到终态才报告完成，并读 `rb.rgb.state` 确认真实连接。
断开也用 Invoke/Poll。失败显示目标设备、步骤、ResultCode 和状态中的错误文本，
保留用户选择；不循环重连、不自动刷固件。

## 设备配置与逻辑映射

连接后后台调用 rgb.read_device_config 读取实际设备配置；平时显示使用
rgb.get_device_config_snapshot，避免每次 UI 定时器访问 USB。
用户编辑完整副本，保留未编辑字段。设备配置应用和 Flash 保存是两个独立按钮。

每个物理输出的 LedCount 不超过设备 MaxLedsPerOutput 和当前结构体上限 150，
输出数量不超过 MaxOutputs 和结构体四路上限；LogicalStart 为 0-600，
LedCount 不超过 600-LogicalStart。600 是共享逻辑地址空间，不是物理灯数。
例如第 2 路 LogicalStart=150、LedCount=75，映射到逻辑像素 150-224；
Reversed 只反转该物理灯带的映射。颜色顺序单独保存，不通过交换效果层颜色代替。

数值框允许中间输入，提交前统一校验并定位字段；越界不静默截断。
应用成功后读回快照，Flash 保存成功后重新读设备验证；失败保留草稿，
显示“应用失败”或“Flash 保存未确认”，不能共用一个“已保存”提示。

## 灯效层

Read `rb.rgb.effects` 获取完整 RB_RgbEffectProfile/3 和 Revision。
图层上限 16，FrameRateHz 为 1-60，总亮度与层亮度为 0-100%。
每层 OutputMask 的四个位对应四个输出；StartPixel/PixelCount 是该路内的零基段，
范围 0-150 且 StartPixel+PixelCount 不超过 150。不要把物理层段范围写成 600。
UI 可显示一基起止 LED，提交时再转换。Priority 按列表顺序赋连续值，
保持相对顺序；不要依赖任意稀疏旧值来表达层级。

添加层先使用默认 Profile/Layer 值。按 EffectType 调用 get_effect_input_count/input，
用 TelemetryKey 关联遥测目录取得 Name/Unit/支持状态；固定来源不能改绑。
静态层不要求遥测，支持的自主闪烁/呼吸也可没有绑定。
不适用字段保留值但禁用编辑，不能改效果类型时把整层清零。

Write 携带 Revision；冲突时重读并由宿主合并，不强制覆盖。
Apply、Save 都使用对应异步命令并消费终态。Apply 不持久化，Save 不表示已应用。
常规刷新只读状态，不能重新加载控件并覆盖正在编辑的草稿。

## 命名方案与迁移

count/name 填充命名库下拉，空库显示空态；“当前未命名配置”是独立状态。
另存为用 save_effect_profile_as；切换前处理未保存草稿，选定后 reload 并 Poll，
再读取草稿，用户显式应用。导入包同名覆盖前由宿主提示目标，
成功后刷新名称列表，不自动切换或连接设备。导出只包含主机灯效，不包含 Flash、
UID 绑定或固件。删除当前命名方案保留已运行的灯效，但选择名变为空。

所有可能访问设备/磁盘的服务放入宿主工作线程；UI 每项动作只有一个所有者。
晚到结果携带 Uid/方案名/草稿版本，只更新原目标；用户切页后不能覆盖新页面。

## 可执行参考与验收

`examples/cpp/profile_packages` 演示命名库另存、枚举、
选定、导出和导入，仅使用十入口 ABI，不触碰设备或正式用户配置。
它是方案服务示例；本指南与 UI_SPEC 一起说明设备和图层编辑链路。
实际产品仍须验证设备发现、读配置失败、映射越界、草稿冲突、固件维护中拒绝操作、
保存成功但应用失败、断开等待以及有真实连接的关闭流程。
