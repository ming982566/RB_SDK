#include "configuration_examples.h"

#include "ExampleServices.h"

#include <cstring>

int ConfigureDynamicInputExample(int profileIndex, int dofIndex, int candidateIndex)
{
	if (dofIndex < 0 || dofIndex >= RB_DYNAMIC_DOF_COUNT) {
		return RB_INVALID_ARGUMENT;
	}
	RB_DynamicInputCandidateCatalog candidates{};
	int rc = rb_example::Dynamic_GetInputCandidateCatalog(dofIndex, &candidates);
	if (rc != RB_OK || candidateIndex < 0 || candidateIndex >= candidates.Count) {
		return rc == RB_OK ? RB_INVALID_ARGUMENT : rc;
	}
	const RB_DynamicInputCandidateItem& candidate = candidates.Items[candidateIndex];

	// Always read the complete profile first so untouched DOFs and channels survive.
	RB_DynamicV2Profile profile{};
	rc = rb_example::Dynamic_ReadProfileByIndex(profileIndex, &profile);
	if (rc != RB_OK) {
		return rc;
	}

	RB_DynamicDofEffect& dof = profile.Dofs[dofIndex];
	if (dof.InputCount >= RB_DYNAMIC_MAX_INPUTS) {
		return RB_ERROR;
	}

	RB_DynamicInputEffect& input = dof.Inputs[dof.InputCount];
	input = {};
	input.Enabled = 1;
	input.InputType = candidate.InputType;
	input.TelemetryIndex = candidate.TelemetryIndex;
	strcpy_s(input.Key, sizeof(input.Key), candidate.Key);
	input.Filter.Enabled = 1;
	input.Filter.Dof = input.InputType;
	input.Filter.InMapping = 20;
	input.Filter.InGain = 1.0;
	input.Filter.OutScaling = 100;
	++dof.InputCount;
	dof.Enabled = 1;

	// Apply updates the live model; Save persists the whole profile.
	rc = rb_example::Dynamic_ApplyProfileToCurrentRig(&profile);
	return rc == RB_OK ? rb_example::Dynamic_SaveProfileByIndex(profileIndex, &profile) : rc;
}

int ConfigureSerialOutputExample(int instanceIndex, const char* portName)
{
	if (portName == nullptr) {
		return RB_INVALID_ARGUMENT;
	}

	RB_OutputConfig output{};
	int rc = rb_example::Output_ReadInstanceConfigByIndex(instanceIndex, &output);
	if (rc != RB_OK) {
		return rc;
	}

	// Connection fields must only change while this instance is disconnected.
	if (rb_example::Output_IsInstanceConnected(instanceIndex)) {
		rc = rb_example::Output_DisconnectInstanceByIndex(instanceIndex);
		if (rc != RB_OK) {
			return rc;
		}
	}

	strcpy_s(output.OutputKind, sizeof(output.OutputKind), "Serial");
	strcpy_s(output.Port, sizeof(output.Port), portName);
	output.BaudRate = 115200;
	output.OutBit = 16;
	output.AutoConnect = 1;
	strcpy_s(output.OutPutString, sizeof(output.OutPutString), "<A1><A2><A3><A4>");
	strcpy_s(output.OutPutTime, sizeof(output.OutPutTime), "2"); // milliseconds

	rc = rb_example::Output_SaveInstanceConfigByIndex(instanceIndex, &output);
	return rc == RB_OK ? rb_example::Output_ApplyInstanceConfigByIndex(instanceIndex, &output) : rc;
}

int ConfigureAdditionalEffectsExample(int motionProfileIndex, int dynamicProfileIndex)
{
	// 运动增强拥有独立目录，motionProfileIndex 必须来自运动增强配置选择框。
	RB_MotionEffectProfile motion{};
	int rc = rb_example::MotionEffect_ReadProfileByIndex(motionProfileIndex, &motion);
	if (rc != RB_OK) {
		return rc;
	}
	motion.Effects[0].Enabled = 1;
	motion.Effects[0].Gain = 1.0;
	rc = rb_example::MotionEffect_ApplyProfileToCurrentRig(&motion);
	if (rc != RB_OK) {
		return rc;
	}
	rc = rb_example::MotionEffect_SaveProfileByIndex(motionProfileIndex, &motion);
	if (rc != RB_OK) {
		return rc;
	}

	// 震动内容独立，但当前 ByIndex 接口按动态配置索引查找同名震动节点。
	RB_HapticEffectProfile haptic{};
	rc = rb_example::Haptic_ReadProfileByIndex(dynamicProfileIndex, &haptic);
	if (rc != RB_OK) {
		return rc;
	}
	haptic.Effects[0].Enabled = 1;
	haptic.Effects[0].OutputChannel = 0; // Haptic1
	haptic.Effects[0].Gain = 1.0;
	rc = rb_example::Haptic_ApplyProfileToCurrentFunction(&haptic);
	if (rc != RB_OK) {
		return rc;
	}
	rc = rb_example::Haptic_SaveProfileByIndex(dynamicProfileIndex, &haptic);
	if (rc != RB_OK) {
		return rc;
	}

	RB_WindEffectConfig wind{};
	rc = rb_example::Wind_ReadConfig(&wind);
	if (rc != RB_OK) {
		return rc;
	}
	wind.Enabled = 1;
	wind.InputMinKmh = 20.0;
	wind.InputMaxKmh = 180.0;
	rc = rb_example::Wind_ApplyConfigToCurrentFunction(&wind);
	if (rc != RB_OK) {
		return rc;
	}
	return rb_example::Wind_SaveConfig(&wind);
}

int ConfigureGameTelemetryExample(const char* gameKey, int catalogIndex, int sourcePort)
{
	if (gameKey == nullptr || sourcePort < 0 || sourcePort > 65535) {
		return RB_INVALID_ARGUMENT;
	}

	// Read the existing game-specific config so fields not shown by this page survive.
	RB_GameConfig config{};
	int rc = rb_example::Game_ReadConfig(gameKey, &config);
	if (rc != RB_OK) {
		return rc;
	}
	config.SourcePort = sourcePort;
	strcpy_s(config.SourceIP, sizeof(config.SourceIP), "127.0.0.1");

	rc = rb_example::Game_SaveConfig(gameKey, &config);
	if (rc != RB_OK) {
		return rc;
	}
	return rb_example::Game_AutoConnect(catalogIndex, &config);
}

int InspectPluginsAndManualPoseExample()
{
	// Plugin indexes are snapshot-local. Refresh first, then use one catalog copy.
	if (rb_example::Plugin_Refresh() < 0) {
		return RB_ERROR;
	}
	RB_PluginCatalog plugins{};
	int rc = rb_example::Plugin_GetCatalog(&plugins);
	if (rc != RB_OK) {
		return rc;
	}

	const int platformIndex = rb_example::Platform_ReadSelectedIndex();
	RB_PlatformConfig platform{};
	if (platformIndex < 0 || rb_example::Platform_ReadConfigByIndex(platformIndex, &platform) != RB_OK) {
		return RB_ERROR;
	}

	// A value of zero verifies the manual-test path without commanding movement.
	rc = rb_example::ManualPose_SetTestEnabled(1);
	if (rc == RB_OK) {
		rc = rb_example::ManualPose_SetDrive(0, 0.0);
	}
	rb_example::ManualPose_ResetDrive();
	rb_example::ManualPose_SetTestEnabled(0);
	return rc;
}

int ConfigureRgbEffectExample(const char* optionalDeviceUid)
{
	RB_RgbEffectProfile profile{};
	int rc = rb_example::Rgb_GetDefaultEffectProfile(&profile);
	if (rc != RB_OK) return rc;

	profile.Enabled = 1;
	profile.FrameRateHz = 30;
	profile.MasterBrightnessPercent = 80.0;
	profile.LayerCount = 3;
	// 第1路的 LED 1-20 显示 RPM，公开接口使用从0开始的 StartPixel 和数量。
	profile.Layers[0].Enabled = 1;
	profile.Layers[0].EffectType = RB_RGB_EFFECT_RPM_BAR;
	strcpy_s(profile.Layers[0].TelemetryKey, sizeof(profile.Layers[0].TelemetryKey), "rb.vehicle.engine.rpm");
	profile.Layers[0].OutputMask = 0x01;
	profile.Layers[0].StartPixel = 0;
	profile.Layers[0].PixelCount = 20;
	profile.Layers[0].MinValue = 0.0;
	profile.Layers[0].MaxValue = 8000.0;
	profile.Layers[0].ColorA = 0x0000FF00U;
	profile.Layers[0].ColorB = 0x00FF0000U;
	profile.Layers[0].FillMode = RB_RGB_FILL_FORWARD;
	// 换挡闪烁覆盖同一区段，较高优先级使它在触发时覆盖 RPM 条。
	profile.Layers[1].Enabled = 1;
	profile.Layers[1].EffectType = RB_RGB_EFFECT_SHIFT_FLASH;
	strcpy_s(profile.Layers[1].TelemetryKey, sizeof(profile.Layers[1].TelemetryKey), "rb.vehicle.engine.rpm");
	profile.Layers[1].OutputMask = 0x01;
	profile.Layers[1].StartPixel = 0;
	profile.Layers[1].PixelCount = 20;
	profile.Layers[1].Threshold = 7200.0;
	profile.Layers[1].ColorA = 0x00FFFFFFU;
	profile.Layers[1].SpeedHz = 8.0;
	profile.Layers[1].BlinkDutyPercent = 50.0;
	profile.Layers[1].Priority = 20;
	// 第1路的 LED 21-30 在 ABS 激活时独立闪烁，不影响前20颗 RPM 灯。
	profile.Layers[2].Enabled = 1;
	profile.Layers[2].EffectType = RB_RGB_EFFECT_TELEMETRY_BLINK;
	strcpy_s(profile.Layers[2].TelemetryKey, sizeof(profile.Layers[2].TelemetryKey), "rb.driver_assist.abs.active");
	profile.Layers[2].OutputMask = 0x01;
	profile.Layers[2].StartPixel = 20;
	profile.Layers[2].PixelCount = 10;
	profile.Layers[2].Threshold = 0.5;
	profile.Layers[2].ColorA = 0x00FFFF00U;
	profile.Layers[2].SpeedHz = 6.0;
	profile.Layers[2].BlinkDutyPercent = 35.0;
	profile.Layers[2].Priority = 30;

	// Apply only updates the runtime copy. Save is the explicit disk write.
	rc = rb_example::Rgb_ApplyEffectProfile(&profile);
	if (rc != RB_OK) return rc;
	rc = rb_example::Rgb_SaveEffectProfile(&profile);
	if (rc != RB_OK || optionalDeviceUid == nullptr || optionalDeviceUid[0] == '\0') return rc;

	// Device discovery is low frequency and UID based; directory indexes are not persisted.
	if (rb_example::Rgb_RefreshDevices() < 0) return RB_ERROR;
	rc = rb_example::Rgb_Connect(optionalDeviceUid);
	if (rc != RB_OK) return rc;

	RB_RgbDeviceConfig device{};
	rc = rb_example::Rgb_ReadDeviceConfig(&device);
	if (rc != RB_OK) return rc;
	device.GlobalBrightness = 204;
	device.CurrentLimitMa = 5000;
	// Applying does not write MCU Flash. Call SaveDeviceConfig only from an explicit Save action.
	return rb_example::Rgb_ApplyDeviceConfig(&device);
}
