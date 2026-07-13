#include "RaceBearSDK.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <string>
#include <thread>
#include <vector>

namespace
{
	// 用 RAII 管理 SDK 生命周期，确保 main() 中途返回时仍会执行 Shutdown。
	// 正式 GUI 程序也应只保留一个全局后端会话，不能由多个页面重复初始化。
	class RuntimeSession
	{
	public:
		// intervalMs 单位为毫秒；当前示例使用2ms。
		bool Start(const char* appDataName, int intervalMs)
		{
			// 应用目录名只能在初始化前设置，不能传入完整路径。
			if (RB_Runtime_SetAppDataName(appDataName) != RB_OK) {
				return false;
			}

			// Initialize 会创建后端运行态并启动 SDK 自己管理的授权服务。
			if (RB_Runtime_Initialize() != RB_OK) {
				return false;
			}
			initialized_ = true;

			// 后端计算由 SDK 的 MultimediaTimer 驱动，前端不再自行调用计算函数。
			if (RB_Runtime_StartLoop(intervalMs) != RB_OK) {
				Stop();
				return false;
			}
			return true;
		}

		void Stop()
		{
			if (!initialized_) {
				return;
			}

			// 调用前必须先停止宿主自己的定时器和工作线程，避免关闭期间并发调用。
			RB_Runtime_Shutdown();
			initialized_ = false;
		}

		~RuntimeSession()
		{
			Stop();
		}

	private:
		bool initialized_ = false;
	};

	// JSON 包大小不固定。SDK 返回 RB_BUFFER_TOO_SMALL 时扩大缓冲区并重试，
	// 不能假设游戏目录或完整配置永远小于某个固定尺寸。
	bool ReadJson(const char* key, std::string& result)
	{
		// 64 KiB 可以覆盖常见配置，16 MiB 上限用于防止异常数据无限分配内存。
		std::vector<char> buffer(64 * 1024);
		while (buffer.size() <= 16 * 1024 * 1024) {
			const int rc = RB_Data_ReadJson(
				key,
				buffer.data(),
				static_cast<int>(buffer.size()));
			if (rc == RB_OK) {
				result.assign(buffer.data());
				return true;
			}
			if (rc != RB_BUFFER_TOO_SMALL) {
				return false;
			}
			buffer.resize(buffer.size() * 2);
		}
		return false;
	}

	// Command 的动作结果通过返回码和 JSON 同时表达。返回码判断调用是否成功，
	// result 中的 ok/count/code 等字段用于更新具体 UI。
	int RunCommand(const char* command, const char* argsJson, std::string& result)
	{
		std::vector<char> buffer(4096);
		for (;;) {
			const int rc = RB_Command_Run(
				command,
				argsJson,
				buffer.data(),
				static_cast<int>(buffer.size()));
			if (rc != RB_BUFFER_TOO_SMALL) {
				result.assign(buffer.data());
				return rc;
			}
			buffer.resize(buffer.size() * 2);
		}
	}

	// RB_Log_Read 每次弹出一条日志；空字符串表示当前队列已经读完。
	void DrainLogs()
	{
		char text[4096]{};
		for (;;) {
			const int rc = RB_Log_Read(text, static_cast<int>(sizeof(text)));
			if (rc != RB_OK || text[0] == '\0') {
				return;
			}
			std::printf("[SDK] %s\n", text);
		}
	}
}

int main()
{
	// GetVersion 不要求先初始化，可用于启动时检查 DLL 与宿主期望版本是否一致。
	std::printf("RaceBear SDK version: %s\n", RB_Runtime_GetVersion());

	// 示例使用独立目录，避免覆盖正式 SimRacebear 或第三方产品配置。
	RuntimeSession session;
	if (!session.Start("RaceBearCppExample", 2)) { // 计算周期间隔2ms。
		std::puts("Failed to initialize RaceBear SDK.");
		return 1;
	}

	// 无参数命令仍需传入合法 JSON 对象 "{}"。
	std::string commandResult;
	const int licenseRc = RunCommand("license.check", "{}", commandResult);
	std::printf("license.check: rc=%d result=%s\n", licenseRc, commandResult.c_str());

	// 目录和配置属于低频 JSON 数据，不应放在 60/100 Hz 状态刷新循环中读取。
	// 推荐用结构体接口枚举游戏：UI 显示 GameName，内部保存和传递 GameKey。
	RB_GameCatalog catalog{};
	if (RB_Game_GetCatalog(&catalog) == RB_OK) {
		std::printf("supported games: %d\n", catalog.Count);
		for (int i = 0; i < (std::min)(catalog.Count, 3); ++i) {
			const RB_GameCatalogItem& item = catalog.Items[i];
			std::printf("  %s -> key=%s source=%d\n",
				item.GameName,
				item.GameKey,
				item.CanCreateSource);
		}
	}

	// JSON 包适合脚本、调试工具或希望自己保留未知字段的高级配置页。
	std::string gameCatalog;
	if (ReadJson("game.catalog", gameCatalog)) {
		const size_t previewLength = (std::min)(gameCatalog.size(), static_cast<size_t>(300));
		std::printf("game.catalog (%zu bytes): %.*s%s\n",
			gameCatalog.size(),
			static_cast<int>(previewLength),
			gameCatalog.c_str(),
			gameCatalog.size() > previewLength ? "..." : "");
	}

	// 外部游戏只能保存稳定key，启动时再解析当前SDK运行时索引。
	const int speedIndex = RB_Telemetry_FindIndexByKey("rb.vehicle.speed");
	const int yawIndex = RB_Telemetry_FindIndexByKey("rb.motion.yaw.position");
	const int packetIndex = RB_Telemetry_FindIndexByKey("rb.stream.packet.id.time");
	if (speedIndex < 0 || yawIndex < 0 || packetIndex < 0) {
		std::puts("Required external telemetry keys are unavailable.");
		return 1;
	}

	RB_ExternalSourceDesc externalDesc{};
	externalDesc.Size = sizeof(externalDesc);
	externalDesc.Version = 1;
	std::snprintf(externalDesc.SourceKey, sizeof(externalDesc.SourceKey), "%s", "example.synthetic_game");
	std::snprintf(externalDesc.SourceName, sizeof(externalDesc.SourceName), "%s", "Synthetic Game Example");
	externalDesc.DataTimeoutMs = 500;
	externalDesc.TransitionMs = 1000;
	if (RB_ExternalSource_Open(&externalDesc) != RB_OK) {
		std::puts("Failed to open external telemetry source.");
		return 1;
	}

	// 示例运行十秒。真实前端通常由 30-100 Hz UI 定时器读取 RB_RuntimeState，
	// 这里只每秒打印一次，避免控制台被高频输出淹没。
	for (int tick = 0; tick < 200; ++tick) {
		// 每帧提交当前游戏支持的完整值集合；未提交的槽位会由SDK清零。
		RB_ExternalTelemetryFrame frame{};
		frame.Size = sizeof(frame);
		frame.Version = 1;
		frame.Sequence = static_cast<unsigned long long>(tick + 1);
		frame.ValueCount = 3;
		frame.Values[0] = { speedIndex, 20.0 }; // m/s
		frame.Values[1] = { yawIndex, std::sin(tick * 0.05) * 10.0 }; // deg
		frame.Values[2] = { packetIndex, static_cast<double>(tick + 1) };
		if (RB_ExternalSource_SubmitFrame(&frame) != RB_OK) {
			std::puts("External frame submission failed.");
			break;
		}

		// SDK 会清零并填写整个结构体，调用方不需要手动设置 Size 或 Version。
		RB_RuntimeState state{};
		if (RB_State_Read(&state) == RB_OK && tick % 20 == 0) {
			std::printf(
				"game=%s telemetry=%d output=%d license=%d allowed=%d\n",
				state.Game.GameName,
				state.Source.TelemetryActive,
				state.Output.AnyRuntimeActive,
				state.License.State,
				state.License.OutputAllowed);
		}

		// 日志采用低成本队列，可循环读取到空字符串为止。
		DrainLogs();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	RB_ExternalSource_Close();

	// 离开 main() 后 RuntimeSession 析构，此时已经没有其它线程继续调用 SDK。
	return 0;
}
