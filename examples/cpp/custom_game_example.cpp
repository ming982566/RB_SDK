#include "custom_game_example.h"

#include <cstring>

namespace
{
	template <size_t Size>
	void CopyText(char (&target)[Size], const char* source)
	{
		strcpy_s(target, source != nullptr ? source : "");
	}

	template <typename Payload>
	std::vector<unsigned char> BuildPacket(unsigned long long sequence, unsigned long long gameTimeUs,
		unsigned int inputMode, unsigned int valueCount, const Payload* payload, size_t payloadSize)
	{
		RB_CustomTelemetryPacketHeader header{};
		header.Magic = RB_CUSTOM_PACKET_MAGIC;
		header.ProtocolVersion = RB_CUSTOM_PROTOCOL_VERSION;
		header.HeaderSize = sizeof(header);
		header.PacketSize = static_cast<unsigned int>(sizeof(header) + payloadSize);
		header.Sequence = sequence;
		header.GameTimeUs = gameTimeUs;
		header.InputMode = inputMode;
		header.ValueCount = valueCount;

		// 协议结构使用pack(1)，仍通过memcpy写字节数组，避免调用方依赖结构体指针别名。
		std::vector<unsigned char> packet(header.PacketSize);
		std::memcpy(packet.data(), &header, sizeof(header));
		if (payload != nullptr && payloadSize > 0) std::memcpy(packet.data() + sizeof(header), payload, payloadSize);
		return packet;
	}
}

int RegisterCustomUdpGame()
{
	RB_CustomGameRegistration registration{};
	registration.Size = sizeof(registration);
	registration.Version = 1;
	registration.Game.Size = sizeof(registration.Game);
	registration.Game.Version = 1;
	CopyText(registration.Game.GameKey, "example.custom_racing");
	CopyText(registration.Game.GameName, "Custom Racing Example");
	CopyText(registration.Game.ProcessNames, "CustomRacing.exe*CustomRacing-Win64.exe");
	CopyText(registration.Game.ImageName, "custom_racing.png");
	registration.Game.Transport = RB_CUSTOM_TRANSPORT_UDP;
	registration.Game.InputMode = RB_CUSTOM_INPUT_RAW;
	registration.Game.DataTimeoutMs = 500;
	registration.Game.TransitionMs = 2000;
	CopyText(registration.Udp.ListenAddress, "127.0.0.1");
	registration.Udp.ListenPort = 27890;
	CopyText(registration.Udp.AllowedSenderAddress, "127.0.0.1");
	return RB_Game_RegisterCustom(&registration);
}

std::vector<unsigned char> BuildCustomRawPacket(unsigned long long sequence, unsigned long long gameTimeUs,
	const std::vector<std::pair<unsigned int, double>>& values)
{
	std::vector<RB_CustomTelemetryRawValue> payload;
	payload.reserve(values.size());
	for (const auto& value : values) payload.push_back({ value.first, value.second });
	return BuildPacket(sequence, gameTimeUs, RB_CUSTOM_INPUT_RAW, static_cast<unsigned int>(payload.size()),
		payload.data(), payload.size() * sizeof(RB_CustomTelemetryRawValue));
}

std::vector<unsigned char> BuildCustomDofPacket(unsigned long long sequence, unsigned long long gameTimeUs,
	const RB_CustomTelemetryDofPayload& dof)
{
	return BuildPacket(sequence, gameTimeUs, RB_CUSTOM_INPUT_6DOF, 6, &dof, sizeof(dof));
}

