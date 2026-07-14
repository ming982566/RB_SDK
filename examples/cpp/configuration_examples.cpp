#include "configuration_examples.h"

#include "RaceBearSDK.h"

#include <cstring>

int ConfigureDynamicInputExample(int profileIndex, int dofIndex, const char* telemetryKey)
{
	if (dofIndex < 0 || dofIndex >= RB_DOF_COUNT || telemetryKey == nullptr) {
		return RB_INVALID_ARGUMENT;
	}

	// Always read the complete profile first so untouched DOFs and channels survive.
	RB_DynamicV2Profile profile{};
	int rc = RB_Dynamic_ReadProfileByIndex(profileIndex, &profile);
	if (rc != RB_OK) {
		return rc;
	}

	RB_DynamicDofEffect& dof = profile.Dofs[dofIndex];
	if (dof.InputCount >= RB_DYNAMIC_MAX_INPUTS) {
		return RB_ERROR;
	}

	// Persist the stable key and resolve the runtime index for the loaded SDK version.
	const int telemetryIndex = RB_Telemetry_FindIndexByKey(telemetryKey);
	if (telemetryIndex < 0) {
		return RB_INVALID_ARGUMENT;
	}

	RB_DynamicInputEffect& input = dof.Inputs[dof.InputCount];
	input = {};
	input.Enabled = 1;
	input.TelemetryIndex = telemetryIndex;
	strcpy_s(input.Key, sizeof(input.Key), telemetryKey);
	input.Filter.Enabled = 1;
	input.Filter.Dof = dofIndex;
	input.Filter.InMapping = 1;
	input.Filter.InGain = 1.0;
	input.Filter.OutScaling = 100;
	++dof.InputCount;
	dof.Enabled = 1;

	// Apply updates the live model; Save persists the whole profile.
	rc = RB_Dynamic_ApplyProfileToCurrentRig(&profile);
	return rc == RB_OK ? RB_Dynamic_SaveProfileByIndex(profileIndex, &profile) : rc;
}

int ConfigureSerialOutputExample(int instanceIndex, const char* portName)
{
	if (portName == nullptr) {
		return RB_INVALID_ARGUMENT;
	}

	RB_OutputConfig output{};
	int rc = RB_Output_ReadInstanceConfigByIndex(instanceIndex, &output);
	if (rc != RB_OK) {
		return rc;
	}

	// Connection fields must only change while this instance is disconnected.
	if (RB_Output_IsInstanceConnected(instanceIndex)) {
		rc = RB_Output_DisconnectInstanceByIndex(instanceIndex);
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

	rc = RB_Output_SaveInstanceConfigByIndex(instanceIndex, &output);
	return rc == RB_OK ? RB_Output_ApplyInstanceConfigByIndex(instanceIndex, &output) : rc;
}

int ConfigureAdditionalEffectsExample(int profileIndex)
{
	// Motion and haptic profiles use the same profile index as the dynamic profile.
	RB_MotionEffectProfile motion{};
	int rc = RB_MotionEffect_ReadProfileByIndex(profileIndex, &motion);
	if (rc != RB_OK) {
		return rc;
	}
	motion.Effects[0].Enabled = 1;
	motion.Effects[0].Gain = 1.0;
	rc = RB_MotionEffect_ApplyProfileToCurrentRig(&motion);
	if (rc != RB_OK) {
		return rc;
	}
	rc = RB_MotionEffect_SaveProfileByIndex(profileIndex, &motion);
	if (rc != RB_OK) {
		return rc;
	}

	RB_HapticEffectProfile haptic{};
	rc = RB_Haptic_ReadProfileByIndex(profileIndex, &haptic);
	if (rc != RB_OK) {
		return rc;
	}
	haptic.Effects[0].Enabled = 1;
	haptic.Effects[0].OutputChannel = 0; // Haptic1
	haptic.Effects[0].Gain = 1.0;
	rc = RB_Haptic_ApplyProfileToCurrentFunction(&haptic);
	if (rc != RB_OK) {
		return rc;
	}
	rc = RB_Haptic_SaveProfileByIndex(profileIndex, &haptic);
	if (rc != RB_OK) {
		return rc;
	}

	RB_WindEffectConfig wind{};
	rc = RB_Wind_ReadConfig(&wind);
	if (rc != RB_OK) {
		return rc;
	}
	wind.Enabled = 1;
	wind.InputMinKmh = 20.0;
	wind.InputMaxKmh = 180.0;
	rc = RB_Wind_ApplyConfigToCurrentFunction(&wind);
	if (rc != RB_OK) {
		return rc;
	}
	return RB_Wind_SaveConfig(&wind);
}

int ConfigureGameTelemetryExample(const char* gameKey, int catalogIndex, int sourcePort)
{
	if (gameKey == nullptr || sourcePort < 0 || sourcePort > 65535) {
		return RB_INVALID_ARGUMENT;
	}

	// Read the existing game-specific config so fields not shown by this page survive.
	RB_GameConfig config{};
	int rc = RB_Game_ReadConfig(gameKey, &config);
	if (rc != RB_OK) {
		return rc;
	}
	config.SourcePort = sourcePort;
	strcpy_s(config.SourceIP, sizeof(config.SourceIP), "127.0.0.1");

	rc = RB_Game_SaveConfig(gameKey, &config);
	if (rc != RB_OK) {
		return rc;
	}
	return RB_Game_AutoConnect(catalogIndex, &config);
}

int InspectPluginsAndManualPoseExample()
{
	// Plugin indexes are snapshot-local. Refresh first, then use one catalog copy.
	if (RB_Plugin_Refresh() < 0) {
		return RB_ERROR;
	}
	RB_PluginCatalog plugins{};
	int rc = RB_Plugin_GetCatalog(&plugins);
	if (rc != RB_OK) {
		return rc;
	}

	const int platformIndex = RB_Platform_ReadSelectedIndex();
	RB_PlatformConfig platform{};
	if (platformIndex < 0 || RB_Platform_ReadConfigByIndex(platformIndex, &platform) != RB_OK) {
		return RB_ERROR;
	}

	// A value of zero verifies the manual-test path without commanding movement.
	rc = RB_ManualPose_SetTestEnabled(1);
	if (rc == RB_OK) {
		rc = RB_ManualPose_SetDrive(0, 0.0);
	}
	RB_ManualPose_ResetDrive();
	RB_ManualPose_SetTestEnabled(0);
	return rc;
}
