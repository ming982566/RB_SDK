import math
import time

from racebear_sdk import RaceBearSDK, decode_utf8


def main() -> None:
    """运行十秒的最小 Python 前端流程。"""
    # with 确保中途发生 Python 异常时仍会调用 sdk.close()。
    with RaceBearSDK() as sdk:
        # GetVersion 不依赖初始化，可先确认当前加载的是哪一版 DLL。
        print("RaceBear SDK version:", sdk.version)

        # 使用示例专属目录，避免读写正式产品的配置和本地授权文件。
        # 2ms是后端计算周期，不是下面Python循环的刷新周期。
        sdk.initialize("RaceBearPythonExample", interval_ms=2)

        # 串口枚举不需要第三方库，返回值已经转换为 Python 字典列表。
        print("Serial ports:")
        for port in sdk.serial_ports():
            print(" ", port["port_name"], port["friendly_name"])

        # 游戏目录属于低频数据，只在页面加载或用户刷新时读取。
        games = sdk.read_json("game.catalog")
        print("Games in catalog:", games.get("count", 0))
        if games.get("items"):
            first_game = games["items"][0]
            print("First game:", first_game.get("gameName"), first_game.get("gameKey"))

        # 无参数命令由封装自动传入 {}，返回值已经解析为 dict。
        print("license.check:", sdk.command("license.check"))

        # 模拟客户自研游戏：配置只保存稳定key，Python封装在提交时解析运行时索引。
        sdk.open_external_source(
            "example.python_game",
            "Python Synthetic Game",
            timeout_ms=500,
            transition_ms=1000,
        )

        # 示例运行十秒并以 10 Hz 读取；真实 UI 可使用 30-100 Hz 定时器。
        for tick in range(100):
            sdk.submit_external_frame(
                tick + 1,
                {
                    "rb.vehicle.speed": 20.0,
                    "rb.motion.yaw.position": math.sin(tick * 0.05) * 10.0,
                    "rb.stream.packet.id.time": float(tick + 1),
                },
            )
            state = sdk.read_state()

            # 控制台每秒打印一次，避免高频状态输出影响阅读。
            if tick % 10 == 0:
                print(
                    "game=",
                    decode_utf8(state.Game.GameName),
                    "telemetry=",
                    state.Source.TelemetryActive,
                    "output=",
                    state.Output.AnyRuntimeActive,
                    "license=",
                    state.License.State,
                    "allowed=",
                    state.License.OutputAllowed,
                )

            # RB_Log_Read 一次只弹出一条，因此循环读取到 None 才算清空队列。
            while True:
                log_text = sdk.read_log()
                if log_text is None:
                    break
                print("[SDK]", log_text)

            # 这只是示例 UI 刷新间隔；不会驱动 SDK 后端计算。
            time.sleep(0.1)

        sdk.close_external_source()


if __name__ == "__main__":
    main()
