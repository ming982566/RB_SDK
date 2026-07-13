#pragma once

#include "RaceBearSDK.h"

#include <utility>
#include <vector>

// 注册一个使用本机UDP和原始遥测模式的正式自定义游戏。
int RegisterCustomUdpGame();

// 构造一帧可直接通过UDP发送或写入MMF Packet区域的原始遥测包。
std::vector<unsigned char> BuildCustomRawPacket(
	unsigned long long sequence,
	unsigned long long gameTimeUs,
	const std::vector<std::pair<unsigned int, double>>& values);

// 构造直接6DOF包；六个值必须已经归一化到-1到1。
std::vector<unsigned char> BuildCustomDofPacket(
	unsigned long long sequence,
	unsigned long long gameTimeUs,
	const RB_CustomTelemetryDofPayload& dof);

