// 默认仅读协议目录；--runtime 才初始化独立配置，不保存或连接真实输出。
#include "ExampleServices.h"
#include <chrono>
#include <cstdio>
#include <cstring>
#include <thread>
#include <vector>

namespace
{
	// 目录中的公开类型签名：int(RB_WindEffectConfig*)；本机请求不保存、不应用。
	int ReadNativeDefaults()
	{
		int service = 0;
		const int resolved = RB_Object_Resolve("rb.service.wind", &service);
		if (resolved != RB_OK) return resolved;
		RB_WindEffectConfig defaults{};
		RB_ObjectNativeCall call{};
		uint64_t result = 0, revision = 0;
		auto* parameter = &defaults;
		call.Size = sizeof(call); call.Version = 1; call.PointerBytes = sizeof(void*);
		call.Operation = "rb.wind.get_default_config"; call.Signature = 0xc7a6254c6fac65fbULL;
		call.ArgumentCount = 1; call.Result = &result;
		std::memcpy(&call.Arguments[0], &parameter, sizeof(parameter));
		const int transport = RB_Object_Write(service, RB_OBJECT_NATIVE_CALL, &call, sizeof(call), 0, &revision);
		if (transport != RB_OK) return transport;
		int business = RB_ERROR;
		std::memcpy(&business, &result, sizeof(business));
		std::printf("Native defaults business=%d version=%d; no initialization or output\n", business, defaults.Version);
		return business;
	}

	int Wait(uint64_t task)
	{
		// 示例不设置伪取消超时；一直持有同一任务直到消费终态，再允许 Shutdown。
		for (;;) {
			RB_ObjectTaskInfo info{};
			info.Size = sizeof(info); info.Version = 1;
			std::vector<char> result(1024);
			int rc = RB_Object_Poll(task, 1, result.data(), static_cast<int>(result.size()), &info);
			if (rc == RB_BUFFER_TOO_SMALL) {
				result.resize(info.PayloadBytes);
				rc = RB_Object_Poll(task, 1, result.data(), static_cast<int>(result.size()), &info);
			}
			if (rc != RB_OK) return rc;
			if (info.Status >= RB_OBJECT_TASK_COMPLETE) {
				std::printf("Task: %s\n", result.data());
				return info.ResultCode;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	int ReadAndEditDraft()
	{
		int wind = 0;
		int rc = RB_Object_Resolve("rb.wind.config", &wind);
		if (rc != RB_OK) return rc;
		RB_WindEffectConfig config{};
		RB_ObjectReadInfo info{};
		info.Size = sizeof(info); info.Version = 1;
		rc = RB_Object_Read(wind, RB_OBJECT_BINARY, &config, sizeof(config), &info);
		if (rc != RB_OK) return rc;
		const char patch[] = "{\"masterGainPercent\":75}";
		uint64_t next = 0;
		rc = RB_Object_Write(wind, RB_OBJECT_JSON, patch, sizeof(patch) - 1, info.Revision, &next);
		std::printf("Draft update code=%d revision=%llu; no Apply/Save submitted\n", rc,
			static_cast<unsigned long long>(next));
		return rc;
	}

	int ReadTelemetry()
	{
		int telemetry = 0;
		int rc = RB_Object_Resolve("rb.telemetry.snapshot", &telemetry);
		if (rc != RB_OK) return rc;
		for (int i = 0; i < 10; ++i) {
			RB_ObjectTelemetrySnapshot snapshot{};
			RB_ObjectReadInfo info{};
			info.Size = sizeof(info); info.Version = 1;
			rc = RB_Object_Read(telemetry, RB_OBJECT_BINARY, &snapshot, sizeof(snapshot), &info);
			if (rc != RB_OK) return rc;
			std::printf("Snapshot seq=%llu sampled=%d count=%d connected=%d allowed=%d\n",
				static_cast<unsigned long long>(info.Sequence), info.Sampled, snapshot.Count,
				snapshot.SourceConnected, snapshot.OutputAllowed);
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
		return RB_OK;
	}
}

int main(int argc, char** argv) try
{
	std::printf("SDK %s\n", RB_Runtime_GetVersion());
	int bytes = 0;
	if (RB_Object_Describe(0, nullptr, 0, &bytes) != RB_BUFFER_TOO_SMALL) return 1;
	std::vector<char> directory(bytes);
	if (RB_Object_Describe(0, directory.data(), bytes, &bytes) != RB_OK) return 1;
	std::puts(directory.data());
	if (ReadNativeDefaults() != RB_OK) return 1;
	if (argc == 1) return 0;
	if (argc != 2 || std::strcmp(argv[1], "--runtime") != 0) return 2;
	if (rb_example::Runtime_SetAppDataName("RaceBearObjectDemo") != RB_OK) return 1;
	const int initialized = RB_Runtime_Initialize();
	if (initialized != RB_OK) {
		std::printf("Initialize failed: %d\n", initialized);
		return 1;
	}
	uint64_t task = 0;
	int rc = RB_Object_Invoke("rb.runtime.start", "{\"intervalMs\":2}", &task);
	if (rc == RB_OK) rc = Wait(task);
	if (rc == RB_OK) rc = ReadAndEditDraft();
	if (rc == RB_OK) rc = ReadTelemetry();
	// Shutdown 会等待 SDK 自己的在途任务；宿主没有其它调用线程或设备句柄。
	RB_Runtime_Shutdown();
	std::printf("Runtime example code=%d\n", rc);
	return rc == RB_OK ? 0 : 1;
}
catch (const std::exception& error) {
	std::fprintf(stderr, "%s\n", error.what());
	return 1;
}
