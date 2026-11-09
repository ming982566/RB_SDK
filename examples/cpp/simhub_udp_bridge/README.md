# RaceBear SimHub UDP Bridge

这个桥接程序使用 SimHub 的公开 Generic UDP 输出接收最终轴值，再通过 `RaceBearSDK.dll` 的公开 C ABI 提交到现有 `RB_CAN` 输出实例。

当前只导入 10 个统一 ABI 的子集；CAN 操作由 `../../common/ExampleServices.h`
中的宿主包装经 Resolve/Write(NATIVE_CALL) 分派，不导入旧业务函数。
`--help` 在初始化前退出，不打开网络或设备；实际启动将按配置连接真实输出。

## SimHub 输出文本

在 SimHub Motion 中使用 Generic UDP 输出，目标地址填写 `127.0.0.1`，端口填写 `24740`，周期建议 `2 ms`。输出模板为：

```text
RBH1,<Axis1a>,<Axis2a>,<Axis3a>,<Axis4a>
```

轴数量必须和 RB_CAN 输出配置中的 `<A1>...<An>` 一致。`Axis1a` 等值按 SimHub 的 16 位轴值解释为 `0..65535`。程序也接受带序号的调试包：

```text
RBH1,<sequence>,<axisCount>,<Axis1a>,<Axis2a>,<Axis3a>,<Axis4a>
```

## 启动

```text
RaceBearSimHubUdpBridge.exe --port 24740 --output 0
```

程序会读取当前 SDK 配置目录中第一个 `RB_CAN` 输出，启动 SDK 内部循环并等待该输出 Ready。默认 250 ms 未收到有效包后，所有轴回到输出配置的停车位置。桥接程序退出前会重复提交停车位置，然后正常断开输出和关闭 SDK。

## 设计边界

- SimHub 负责游戏源、平台几何、滤波、Motion Effect 和最终轴值。
- 桥接程序只负责协议解析、值域映射、超时停车、健康显示和连接请求。
- CAN 协议、电机状态、故障处理和自动恢复继续由 RaceBear SDK 统一负责。
- 不使用 SimHub 私有、反编译或隐藏接口。
