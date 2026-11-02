#include "RaceBearSDK.h"

#include <WinSock2.h>
#include <Windows.h>
#include <Ws2tcpip.h>

#include <algorithm>
#include <atomic>
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

namespace
{
    constexpr int kDefaultPort = 24740;
    constexpr int kDefaultStaleTimeoutMs = 250;
    constexpr int kDefaultParkRepeatMs = 100;
    constexpr int kMaxPacketBytes = 2048;

    struct BridgeOptions
    {
        int Port = kDefaultPort;                 // 本地 UDP 监听端口，仅接收 SimHub 最终轴值。
        int OutputIndex = -1;                    // -1 表示自动选择第一个运动 CAN 输出。
        int AxisCount = 0;                       // 0 表示从 RB_CAN 输出模板中的 <A1>... 自动读取。
        int StaleTimeoutMs = kDefaultStaleTimeoutMs; // 有效轴值超时后进入停车输出的等待时间。
        int ParkRepeatMs = kDefaultParkRepeatMs; // 停车位置重复提交周期。
        int InputMin = 0;                        // SimHub 输入值下界，默认对应 uint16 全量程。
        int InputMax = 65535;                    // SimHub 输入值上界，默认对应 uint16 全量程。
        int TargetMin = 0;                       // RB_CAN 目标位置下界。
        int TargetMax = 65535;                   // RB_CAN 目标位置上界。
        std::string AppDataName = "SimRacebear"; // 与现有输出配置目录保持一致。
    };

    struct AxisPacket
    {
        std::vector<uint16_t> Values;
    };

    std::atomic_bool g_running{ true };

    bool ParseInt(std::string_view text, int* value)
    {
        if (value == nullptr || text.empty()) return false;
        int parsed = 0;
        const char* begin = text.data();
        const char* end = text.data() + text.size();
        const auto result = std::from_chars(begin, end, parsed, 10);
        if (result.ec != std::errc() || result.ptr != end) return false;
        *value = parsed;
        return true;
    }

    bool ParseUint16(std::string_view text, uint16_t* value)
    {
        if (value == nullptr || text.empty()) return false;
        unsigned int parsed = 0;
        const char* begin = text.data();
        const char* end = text.data() + text.size();
        const auto result = std::from_chars(begin, end, parsed, 10);
        if (result.ec != std::errc() || result.ptr != end || parsed > 65535U) return false;
        *value = static_cast<uint16_t>(parsed);
        return true;
    }

    std::string Trim(std::string_view text)
    {
        size_t first = 0;
        size_t last = text.size();
        while (first < last && (text[first] == ' ' || text[first] == '\t' || text[first] == '\r' || text[first] == '\n')) ++first;
        while (last > first && (text[last - 1] == ' ' || text[last - 1] == '\t' || text[last - 1] == '\r' || text[last - 1] == '\n')) --last;
        return std::string(text.substr(first, last - first));
    }

    std::vector<std::string_view> Split(std::string_view text)
    {
        std::vector<std::string_view> fields;
        size_t start = 0;
        while (start <= text.size()) {
            const size_t comma = text.find(',', start);
            const size_t end = comma == std::string_view::npos ? text.size() : comma;
            fields.push_back(text.substr(start, end - start));
            if (comma == std::string_view::npos) break;
            start = comma + 1;
        }
        return fields;
    }

    // SimHub Generic UDP 公开输出使用 RBH1,<Axis1a>,...；同时兼容带序号和数量的调试包 RBH1,<seq>,<count>,<Axis...>。
    bool ParseAxisPacket(const char* data, int length, int expectedAxisCount, AxisPacket* packet)
    {
        if (data == nullptr || length <= 0 || packet == nullptr || expectedAxisCount <= 0) return false;
        const std::string text = Trim(std::string_view(data, static_cast<size_t>(length)));
        const std::vector<std::string_view> fields = Split(text);
        if (fields.empty() || fields[0] != "RBH1") return false;

        size_t firstValue = 1;
        if (static_cast<int>(fields.size()) == expectedAxisCount + 3) {
            int ignoredSequence = 0;
            int declaredCount = 0;
            if (!ParseInt(fields[1], &ignoredSequence) || !ParseInt(fields[2], &declaredCount) || declaredCount != expectedAxisCount) {
                return false;
            }
            firstValue = 3;
        }
        if (static_cast<int>(fields.size() - firstValue) != expectedAxisCount) return false;

        packet->Values.clear();
        packet->Values.reserve(static_cast<size_t>(expectedAxisCount));
        for (size_t i = firstValue; i < fields.size(); ++i) {
            uint16_t value = 0;
            const std::string field = Trim(fields[i]);
            if (!ParseUint16(field, &value)) return false;
            packet->Values.push_back(value);
        }
        return true;
    }

    int CountAxisTokens(const char* outputString)
    {
        if (outputString == nullptr) return 0;
        const std::string text(outputString);
        int count = 0;
        size_t cursor = 0;
        while ((cursor = text.find("<A", cursor)) != std::string::npos) {
            const size_t end = text.find('>', cursor + 2);
            if (end == std::string::npos) break;
            bool hasDigit = false;
            for (size_t i = cursor + 2; i < end; ++i) {
                if (text[i] >= '0' && text[i] <= '9') {
                    hasDigit = true;
                    break;
                }
            }
            if (hasDigit) ++count;
            cursor = end + 1;
        }
        return count;
    }

    int MapValue(uint16_t raw, const BridgeOptions& options)
    {
        const int inputMin = (std::min)(options.InputMin, options.InputMax);
        const int inputMax = (std::max)(options.InputMin, options.InputMax);
        const int targetMin = (std::min)(options.TargetMin, options.TargetMax);
        const int targetMax = (std::max)(options.TargetMin, options.TargetMax);
        if (inputMax <= inputMin) return targetMin;
        const double normalized = (std::clamp)((static_cast<double>(raw) - inputMin) /
            static_cast<double>(inputMax - inputMin), 0.0, 1.0);
        const double mapped = targetMin + normalized * static_cast<double>(targetMax - targetMin);
        return static_cast<int>((std::llround)(mapped));
    }

    void PrintUsage()
    {
        std::puts("RaceBear SimHub UDP Bridge");
        std::puts("  --port N             localhost UDP port, default 24740");
        std::puts("  --output N           RB_CAN output instance index, default auto");
        std::puts("  --axes N             axis count, default from <A1>... output template");
        std::puts("  --stale-ms N         stale input timeout, default 250");
        std::puts("  --input-min N        input mapping minimum, default 0");
        std::puts("  --input-max N        input mapping maximum, default 65535");
        std::puts("  --target-min N       CAN target minimum, default 0");
        std::puts("  --target-max N       CAN target maximum, default 65535");
        std::puts("  --app-data-name NAME SDK configuration directory name, default SimRacebear");
    }

    bool ParseOptions(int argc, char** argv, BridgeOptions* options)
    {
        if (options == nullptr) return false;
        for (int i = 1; i < argc; ++i) {
            const std::string key(argv[i]);
            if (key == "--help" || key == "-h") {
                PrintUsage();
                return false;
            }
            auto readInt = [&](int* target) {
                return i + 1 < argc && ParseInt(argv[++i], target);
            };
            if (key == "--port") {
                if (!readInt(&options->Port)) return false;
            }
            else if (key == "--output") {
                if (!readInt(&options->OutputIndex)) return false;
            }
            else if (key == "--axes") {
                if (!readInt(&options->AxisCount)) return false;
            }
            else if (key == "--stale-ms") {
                if (!readInt(&options->StaleTimeoutMs)) return false;
            }
            else if (key == "--input-min") {
                if (!readInt(&options->InputMin)) return false;
            }
            else if (key == "--input-max") {
                if (!readInt(&options->InputMax)) return false;
            }
            else if (key == "--target-min") {
                if (!readInt(&options->TargetMin)) return false;
            }
            else if (key == "--target-max") {
                if (!readInt(&options->TargetMax)) return false;
            }
            else if (key == "--app-data-name") {
                if (i + 1 >= argc) return false;
                options->AppDataName = argv[++i];
            }
            else {
                std::fprintf(stderr, "Unknown option: %s\n", key.c_str());
                PrintUsage();
                return false;
            }
        }
        return options->Port > 0 && options->Port <= 65535 &&
            options->AxisCount >= 0 && options->AxisCount <= RB_MAX_CAN_MOTORS &&
            options->StaleTimeoutMs >= 20 && options->StaleTimeoutMs <= 60000 &&
            options->InputMin >= 0 && options->InputMax <= 65535 && options->InputMin < options->InputMax &&
            options->TargetMin < options->TargetMax;
    }

    BOOL WINAPI ConsoleHandler(DWORD controlType)
    {
        if (controlType == CTRL_C_EVENT || controlType == CTRL_CLOSE_EVENT || controlType == CTRL_BREAK_EVENT ||
            controlType == CTRL_LOGOFF_EVENT || controlType == CTRL_SHUTDOWN_EVENT) {
            g_running.store(false);
            return TRUE;
        }
        return FALSE;
    }

    int FindCanOutput(int requestedIndex)
    {
        const int count = RB_Output_GetInstanceCount();
        if (requestedIndex >= 0) {
            if (requestedIndex >= count) return -1;
            RB_OutputConfig config{};
            if (RB_Output_ReadInstanceConfigByIndex(requestedIndex, &config) != RB_OK) return -1;
            return std::strcmp(config.OutputKind, "RB_CAN") == 0 ? requestedIndex : -1;
        }
        for (int i = 0; i < count; ++i) {
            RB_OutputConfig config{};
            if (RB_Output_ReadInstanceConfigByIndex(i, &config) == RB_OK && std::strcmp(config.OutputKind, "RB_CAN") == 0) {
                return i;
            }
        }
        return -1;
    }

    bool ReadOutputConfig(int outputIndex, RB_OutputConfig* config)
    {
        return config != nullptr && RB_Output_ReadInstanceConfigByIndex(outputIndex, config) == RB_OK &&
            std::strcmp(config->OutputKind, "RB_CAN") == 0;
    }

    bool WaitForOutputReady(int outputIndex, int timeoutMs)
    {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);
        RB_Output_ConnectInstanceByIndex(outputIndex);
        while (g_running.load() && std::chrono::steady_clock::now() < deadline) {
            RB_CanOutputHealthState health{};
            health.Size = sizeof(health);
            RB_CanOutputBindingState binding{};
            binding.Size = sizeof(binding);
            if (RB_Can_GetOutputHealthState(outputIndex, &health) == RB_OK &&
                RB_Can_GetOutputBindingState(outputIndex, &binding) == RB_OK &&
                health.DeviceConnected && health.DeviceOpened && binding.Ready && health.Armed && health.OutputActive) {
                return true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        return false;
    }

    bool SendPark(int outputIndex, int axisCount, const BridgeOptions& options, int parkPercent)
    {
        const int targetMin = (std::min)(options.TargetMin, options.TargetMax);
        const int targetMax = (std::max)(options.TargetMin, options.TargetMax);
        const double normalized = (std::clamp)(parkPercent / 100.0, 0.0, 1.0);
        const int park = static_cast<int>((std::llround)(targetMin + normalized * (targetMax - targetMin)));
        std::vector<int32_t> positions(static_cast<size_t>(axisCount), park);
        return RB_Can_SubmitPositions(outputIndex, positions.data(), axisCount) == RB_OK;
    }
}

int main(int argc, char** argv)
{
    BridgeOptions options{};
    if (!ParseOptions(argc, argv, &options)) return 2;
    SetConsoleCtrlHandler(ConsoleHandler, TRUE);

    WSADATA wsaData{};
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::fprintf(stderr, "WSAStartup failed\n");
        return 3;
    }

    int exitCode = 0;
    SOCKET socketHandle = INVALID_SOCKET;
    bool sdkInitialized = false;
    int outputIndex = -1;
    try {
        if (RB_Runtime_SetAppDataName(options.AppDataName.c_str()) != RB_OK || RB_Runtime_Initialize() != RB_OK) {
            std::fprintf(stderr, "RaceBear SDK initialization failed\n");
            exitCode = 4;
            throw 1;
        }
        sdkInitialized = true;
        if (RB_Runtime_StartLoop(1) != RB_OK) {
            std::fprintf(stderr, "RaceBear SDK runtime loop start failed\n");
            exitCode = 5;
            throw 1;
        }

        outputIndex = FindCanOutput(options.OutputIndex);
        if (outputIndex < 0) {
            std::fprintf(stderr, "No RB_CAN output instance was found\n");
            exitCode = 6;
            throw 1;
        }
        RB_OutputConfig outputConfig{};
        if (!ReadOutputConfig(outputIndex, &outputConfig)) {
            std::fprintf(stderr, "Cannot read RB_CAN output configuration\n");
            exitCode = 7;
            throw 1;
        }
        const int axisCount = options.AxisCount > 0 ? options.AxisCount : CountAxisTokens(outputConfig.OutPutString);
        if (axisCount <= 0 || axisCount > RB_MAX_CAN_MOTORS) {
            std::fprintf(stderr, "Cannot determine a valid CAN axis count\n");
            exitCode = 8;
            throw 1;
        }
        if (!WaitForOutputReady(outputIndex, 15000)) {
            std::fprintf(stderr, "RB_CAN output did not become ready\n");
            exitCode = 9;
            throw 1;
        }

        socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (socketHandle == INVALID_SOCKET) {
            std::fprintf(stderr, "UDP socket creation failed: %d\n", WSAGetLastError());
            exitCode = 10;
            throw 1;
        }
        DWORD receiveTimeout = 20;
        setsockopt(socketHandle, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&receiveTimeout), sizeof(receiveTimeout));
        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        address.sin_port = htons(static_cast<u_short>(options.Port));
        if (bind(socketHandle, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) == SOCKET_ERROR) {
            std::fprintf(stderr, "UDP bind failed on 127.0.0.1:%d: %d\n", options.Port, WSAGetLastError());
            exitCode = 11;
            throw 1;
        }

        std::printf("Listening on 127.0.0.1:%d, output OUT_%d, axes=%d\n", options.Port, outputIndex, axisCount);
        std::printf("SimHub packet: RBH1,<Axis1a>,<Axis2a>,...\n");
        const auto startTime = std::chrono::steady_clock::now();
        auto lastPacketTime = startTime;
        auto lastParkTime = startTime - std::chrono::milliseconds(options.ParkRepeatMs);
        auto lastReconnectAttempt = startTime;
        auto lastHealthLog = startTime - std::chrono::seconds(2);
        bool parked = true;
        char receiveBuffer[kMaxPacketBytes]{};

        while (g_running.load()) {
            const int received = recvfrom(socketHandle, receiveBuffer, sizeof(receiveBuffer) - 1, 0, nullptr, nullptr);
            const auto now = std::chrono::steady_clock::now();
            if (received > 0) {
                receiveBuffer[received] = '\0';
                AxisPacket packet{};
                if (ParseAxisPacket(receiveBuffer, received, axisCount, &packet)) {
                    std::vector<int32_t> positions;
                    positions.reserve(packet.Values.size());
                    for (uint16_t value : packet.Values) positions.push_back(MapValue(value, options));
                    if (RB_Can_SubmitPositions(outputIndex, positions.data(), static_cast<int>(positions.size())) == RB_OK) {
                        lastPacketTime = now;
                        parked = false;
                    }
                }
            }

            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastPacketTime).count() >= options.StaleTimeoutMs &&
                std::chrono::duration_cast<std::chrono::milliseconds>(now - lastParkTime).count() >= options.ParkRepeatMs) {
                if (SendPark(outputIndex, axisCount, options, outputConfig.ParkPositionPercent)) parked = true;
                lastParkTime = now;
            }

            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastReconnectAttempt).count() >= 1500) {
                RB_CanOutputHealthState health{};
                health.Size = sizeof(health);
                RB_CanOutputBindingState binding{};
                binding.Size = sizeof(binding);
                if (RB_Can_GetOutputHealthState(outputIndex, &health) == RB_OK &&
                    RB_Can_GetOutputBindingState(outputIndex, &binding) == RB_OK &&
                    !health.RequiresHumanIntervention &&
                    (!health.DeviceConnected || !health.DeviceOpened || !binding.Ready || !health.Armed)) {
                    RB_Output_ConnectInstanceByIndex(outputIndex);
                }
                lastReconnectAttempt = now;
            }

            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastHealthLog).count() >= 1000) {
                RB_CanOutputHealthState health{};
                health.Size = sizeof(health);
                RB_CanOutputBindingState binding{};
                binding.Size = sizeof(binding);
                if (RB_Can_GetOutputHealthState(outputIndex, &health) == RB_OK &&
                    RB_Can_GetOutputBindingState(outputIndex, &binding) == RB_OK) {
                    std::printf("CAN: ready=%d armed=%d active=%d fault=%d retry=%u stale=%d\n",
                        binding.Ready, health.Armed, health.OutputActive, health.FaultReason,
                        health.RetryCount, parked ? 1 : 0);
                }
                lastHealthLog = now;
            }
        }

        // 退出前重复提交停车位置，再由 SDK 按正常输出生命周期清理实时槽。
        for (int i = 0; i < 3; ++i) {
            SendPark(outputIndex, axisCount, options, outputConfig.ParkPositionPercent);
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }
    catch (...) {
        // 上面的分支已经记录具体错误；这里统一执行资源释放。
    }

    if (socketHandle != INVALID_SOCKET) closesocket(socketHandle);
    if (sdkInitialized) {
        if (outputIndex >= 0) RB_Output_DisconnectInstanceByIndex(outputIndex);
        RB_Runtime_Shutdown();
    }
    WSACleanup();
    return exitCode;
}
