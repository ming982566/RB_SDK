// 自动生成：tools/Generate-ExampleServices.cjs；仅包含示例使用的公开服务。
#pragma once
#include "ObjectClient.h"

namespace rb_example {
// 同步公开操作 rb.runtime.set_app_data_name；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Runtime_SetAppDataName(const char* name) {
    return Native<int>("rb.service.runtime", "rb.runtime.set_app_data_name", 0x09206df780e497a6ULL, name);
}
// 同步公开操作 rb.runtime.start_loop；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Runtime_StartLoop(int intervalMs) {
    return Native<int>("rb.service.runtime", "rb.runtime.start_loop", 0x3619d7061f665ad6ULL, intervalMs);
}
// 同步公开操作 rb.runtime.pause_loop；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Runtime_PauseLoop() {
    return Native<int>("rb.service.runtime", "rb.runtime.pause_loop", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.runtime.resume_loop；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Runtime_ResumeLoop() {
    return Native<int>("rb.service.runtime", "rb.runtime.resume_loop", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.runtime.stop_loop；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Runtime_StopLoop() {
    return Native<int>("rb.service.runtime", "rb.runtime.stop_loop", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.runtime.set_loop_interval_ms；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Runtime_SetLoopIntervalMs(int intervalMs) {
    return Native<int>("rb.service.runtime", "rb.runtime.set_loop_interval_ms", 0x3619d7061f665ad6ULL, intervalMs);
}
// 同步公开操作 rb.runtime.get_loop_interval_ms；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Runtime_GetLoopIntervalMs() {
    return Native<int>("rb.service.runtime", "rb.runtime.get_loop_interval_ms", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.runtime.is_loop_running；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Runtime_IsLoopRunning() {
    return Native<int>("rb.service.runtime", "rb.runtime.is_loop_running", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.can.get_output_binding_state；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Can_GetOutputBindingState(int outputIndex, RB_CanOutputBindingState* state) {
    return Native<int>("rb.service.can", "rb.can.get_output_binding_state", 0xc1dcd4d58aba6d66ULL, outputIndex, state);
}
// 同步公开操作 rb.can.get_output_health_state；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Can_GetOutputHealthState(int outputIndex, RB_CanOutputHealthState* state) {
    return Native<int>("rb.service.can", "rb.can.get_output_health_state", 0xfad98548142b5681ULL, outputIndex, state);
}
// 同步公开操作 rb.can.submit_positions；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Can_SubmitPositions(int outputIndex, const int32_t* positions, int positionCount) {
    return Native<int>("rb.service.can", "rb.can.submit_positions", 0xee6ab4dd75535abdULL, outputIndex, positions, positionCount);
}
// 同步公开操作 rb.debug.open_monitor；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Debug_OpenMonitor() {
    return Native<int>("rb.service.debug", "rb.debug.open_monitor", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.debug.close_monitor；参数有效期和业务结果见 SERVICE_API 对应条目。
inline void Debug_CloseMonitor() {
    return Native<void>("rb.service.debug", "rb.debug.close_monitor", 0xc4b612d028fa0b14ULL);
}
// 同步公开操作 rb.game.get_catalog；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Game_GetCatalog(RB_GameCatalog* catalog) {
    return Native<int>("rb.service.game", "rb.game.get_catalog", 0xa17f42c66940d939ULL, catalog);
}
// 同步公开操作 rb.game.select_by_key；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Game_SelectByKey(const char* gameKey) {
    return Native<int>("rb.service.game", "rb.game.select_by_key", 0x09206df780e497a6ULL, gameKey);
}
// 同步公开操作 rb.telemetry.find_index_by_key；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Telemetry_FindIndexByKey(const char* telemetryKey) {
    return Native<int>("rb.service.telemetry", "rb.telemetry.find_index_by_key", 0x09206df780e497a6ULL, telemetryKey);
}
// 同步公开操作 rb.telemetry.get_catalog；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Telemetry_GetCatalog(RB_TelemetryCatalog* catalog) {
    return Native<int>("rb.service.telemetry", "rb.telemetry.get_catalog", 0x3457835915293adcULL, catalog);
}
// 同步公开操作 rb.externalsource.open；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int ExternalSource_Open(const RB_ExternalSourceDesc* desc) {
    return Native<int>("rb.service.externalsource", "rb.externalsource.open", 0xd7990fee72140db2ULL, desc);
}
// 同步公开操作 rb.externalsource.submit_frame；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int ExternalSource_SubmitFrame(const RB_ExternalTelemetryFrame* frame) {
    return Native<int>("rb.service.externalsource", "rb.externalsource.submit_frame", 0xac687a6129964a02ULL, frame);
}
// 同步公开操作 rb.externalsource.get_state；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int ExternalSource_GetState(RB_ExternalSourceState* state) {
    return Native<int>("rb.service.externalsource", "rb.externalsource.get_state", 0x111ec6f687b2547fULL, state);
}
// 同步公开操作 rb.externalsource.close；参数有效期和业务结果见 SERVICE_API 对应条目。
inline void ExternalSource_Close() {
    return Native<void>("rb.service.externalsource", "rb.externalsource.close", 0xc4b612d028fa0b14ULL);
}
// 同步公开操作 rb.game.register_custom；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Game_RegisterCustom(const RB_CustomGameRegistration* registration) {
    return Native<int>("rb.service.game", "rb.game.register_custom", 0xc59572a5fd7e76b5ULL, registration);
}
// 同步公开操作 rb.game.unregister_custom；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Game_UnregisterCustom(const char* gameKey) {
    return Native<int>("rb.service.game", "rb.game.unregister_custom", 0x09206df780e497a6ULL, gameKey);
}
// 同步公开操作 rb.game.get_custom_state；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Game_GetCustomState(const char* gameKey, RB_CustomGameState* state) {
    return Native<int>("rb.service.game", "rb.game.get_custom_state", 0xc6fd3ab26bcc16adULL, gameKey, state);
}
// 同步公开操作 rb.game.reload_custom_registry；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Game_ReloadCustomRegistry() {
    return Native<int>("rb.service.game", "rb.game.reload_custom_registry", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.data.read_json；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Data_ReadJson(const char* key, char* buffer, int bufferSize) {
    return Native<int>("rb.service.data", "rb.data.read_json", 0xc4964fc5cc2dd187ULL, key, buffer, bufferSize);
}
// 同步公开操作 rb.game.read_config；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Game_ReadConfig(const char* gameName, RB_GameConfig* config) {
    return Native<int>("rb.service.game", "rb.game.read_config", 0x3fd879acb31ab5b3ULL, gameName, config);
}
// 同步公开操作 rb.game.save_config；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Game_SaveConfig(const char* gameName, const RB_GameConfig* config) {
    return Native<int>("rb.service.game", "rb.game.save_config", 0x32ff9aedc9775ebcULL, gameName, config);
}
// 同步公开操作 rb.game.refresh_installations；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Game_RefreshInstallations() {
    return Native<int>("rb.service.game", "rb.game.refresh_installations", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.game.find_install_by_key；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Game_FindInstallByKey(const char* gameKey, RB_GameInstallInfo* info) {
    return Native<int>("rb.service.game", "rb.game.find_install_by_key", 0x5edc40fffed01bdeULL, gameKey, info);
}
// 同步公开操作 rb.game.auto_connect；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Game_AutoConnect(int catalogIndex, const RB_GameConfig* config) {
    return Native<int>("rb.service.game", "rb.game.auto_connect", 0x738cfaaa18eb1d8cULL, catalogIndex, config);
}
// 同步公开操作 rb.game.auto_configure_current；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Game_AutoConfigureCurrent() {
    return Native<int>("rb.service.game", "rb.game.auto_configure_current", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.game.install_current_plugin；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Game_InstallCurrentPlugin(const char* gamePath) {
    return Native<int>("rb.service.game", "rb.game.install_current_plugin", 0x09206df780e497a6ULL, gamePath);
}
// 同步公开操作 rb.game.check_current_side_config；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Game_CheckCurrentSideConfig(const RB_GameConfig* config) {
    return Native<int>("rb.service.game", "rb.game.check_current_side_config", 0x98b1167043878955ULL, config);
}
// 同步公开操作 rb.plugin.get_catalog；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Plugin_GetCatalog(RB_PluginCatalog* catalog) {
    return Native<int>("rb.service.plugin", "rb.plugin.get_catalog", 0x18d5d81ba0169812ULL, catalog);
}
// 同步公开操作 rb.plugin.refresh；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Plugin_Refresh() {
    return Native<int>("rb.service.plugin", "rb.plugin.refresh", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.plugin.launch；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Plugin_Launch(int pluginIndex) {
    return Native<int>("rb.service.plugin", "rb.plugin.launch", 0x3619d7061f665ad6ULL, pluginIndex);
}
// 同步公开操作 rb.plugin.start_runtimes；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Plugin_StartRuntimes() {
    return Native<int>("rb.service.plugin", "rb.plugin.start_runtimes", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.system.read_config_or_default；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int System_ReadConfigOrDefault(RB_SystemConfig* config) {
    return Native<int>("rb.service.system", "rb.system.read_config_or_default", 0xd1f414bef2bd9c11ULL, config);
}
// 同步公开操作 rb.system.save_config；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int System_SaveConfig(const RB_SystemConfig* config) {
    return Native<int>("rb.service.system", "rb.system.save_config", 0xf7cb43bd85af6cdaULL, config);
}
// 同步公开操作 rb.platform.read_config_by_index；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Platform_ReadConfigByIndex(int platformIndex, RB_PlatformConfig* config) {
    return Native<int>("rb.service.platform", "rb.platform.read_config_by_index", 0xf194d07e9bd13ea2ULL, platformIndex, config);
}
// 同步公开操作 rb.platform.save_config_by_index；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Platform_SaveConfigByIndex(int platformIndex, const RB_PlatformConfig* config) {
    return Native<int>("rb.service.platform", "rb.platform.save_config_by_index", 0x049fcb9c98a0a941ULL, platformIndex, config);
}
// 同步公开操作 rb.platform.set_selected_state；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Platform_SetSelectedState(int platformIndex, const RB_PlatformConfig* config) {
    return Native<int>("rb.service.platform", "rb.platform.set_selected_state", 0x049fcb9c98a0a941ULL, platformIndex, config);
}
// 同步公开操作 rb.platform.apply_runtime_config_to_current_rig；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Platform_ApplyRuntimeConfigToCurrentRig(const RB_PlatformConfig* config) {
    return Native<int>("rb.service.platform", "rb.platform.apply_runtime_config_to_current_rig", 0x52c3db6e5bfa86d4ULL, config);
}
// 同步公开操作 rb.platform.get_count；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Platform_GetCount() {
    return Native<int>("rb.service.platform", "rb.platform.get_count", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.platform.get_name；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Platform_GetName(int platformIndex, char* buffer, int bufferSize) {
    return Native<int>("rb.service.platform", "rb.platform.get_name", 0x551455f08df06637ULL, platformIndex, buffer, bufferSize);
}
// 同步公开操作 rb.platform.is_dof_supported；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Platform_IsDofSupported(int platformIndex, int dofIndex) {
    return Native<int>("rb.service.platform", "rb.platform.is_dof_supported", 0x258290edf43985a5ULL, platformIndex, dofIndex);
}
// 同步公开操作 rb.platform.get_geometry_parameter_name；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Platform_GetGeometryParameterName(int platformIndex, int geometryIndex, char* buffer, int bufferSize) {
    return Native<int>("rb.service.platform", "rb.platform.get_geometry_parameter_name", 0xcf758c1c7ff6b7bcULL, platformIndex, geometryIndex, buffer, bufferSize);
}
// 同步公开操作 rb.platform.get_stroke_parameter_name；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Platform_GetStrokeParameterName(int platformIndex, int strokeIndex, char* buffer, int bufferSize) {
    return Native<int>("rb.service.platform", "rb.platform.get_stroke_parameter_name", 0xcf758c1c7ff6b7bcULL, platformIndex, strokeIndex, buffer, bufferSize);
}
// 同步公开操作 rb.platform.read_selected_index；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Platform_ReadSelectedIndex() {
    return Native<int>("rb.service.platform", "rb.platform.read_selected_index", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.platform.save_selected_index；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Platform_SaveSelectedIndex(int platformIndex) {
    return Native<int>("rb.service.platform", "rb.platform.save_selected_index", 0x3619d7061f665ad6ULL, platformIndex);
}
// 同步公开操作 rb.platform.get_preview_pose_limit；参数有效期和业务结果见 SERVICE_API 对应条目。
inline double Platform_GetPreviewPoseLimit(int platformIndex, const RB_PlatformConfig* config, int dofIndex) {
    return Native<double>("rb.service.platform", "rb.platform.get_preview_pose_limit", 0x557ab84804984f38ULL, platformIndex, config, dofIndex);
}
// 同步公开操作 rb.platform.is_preview_pose_possible；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Platform_IsPreviewPosePossible(int platformIndex, const RB_PlatformConfig* config, int dofIndex) {
    return Native<int>("rb.service.platform", "rb.platform.is_preview_pose_possible", 0xe540292a392dd77eULL, platformIndex, config, dofIndex);
}
// 同步公开操作 rb.manualpose.set_test_enabled；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int ManualPose_SetTestEnabled(int enabled) {
    return Native<int>("rb.service.manualpose", "rb.manualpose.set_test_enabled", 0x3619d7061f665ad6ULL, enabled);
}
// 同步公开操作 rb.manualpose.set_drive；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int ManualPose_SetDrive(int dofIndex, double value) {
    return Native<int>("rb.service.manualpose", "rb.manualpose.set_drive", 0x065cd25b0d0ba623ULL, dofIndex, value);
}
// 同步公开操作 rb.manualpose.reset_drive；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int ManualPose_ResetDrive() {
    return Native<int>("rb.service.manualpose", "rb.manualpose.reset_drive", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.output.read_instance_config_by_index；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Output_ReadInstanceConfigByIndex(int instanceIndex, RB_OutputConfig* config) {
    return Native<int>("rb.service.output", "rb.output.read_instance_config_by_index", 0x410541423fb75b5cULL, instanceIndex, config);
}
// 同步公开操作 rb.output.save_instance_config_by_index；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Output_SaveInstanceConfigByIndex(int instanceIndex, const RB_OutputConfig* config) {
    return Native<int>("rb.service.output", "rb.output.save_instance_config_by_index", 0x86e97c0553b54833ULL, instanceIndex, config);
}
// 同步公开操作 rb.output.apply_instance_config_by_index；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Output_ApplyInstanceConfigByIndex(int instanceIndex, const RB_OutputConfig* config) {
    return Native<int>("rb.service.output", "rb.output.apply_instance_config_by_index", 0x86e97c0553b54833ULL, instanceIndex, config);
}
// 同步公开操作 rb.output.get_catalog；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Output_GetCatalog(RB_OutputCatalog* catalog) {
    return Native<int>("rb.service.output", "rb.output.get_catalog", 0xec9d170da96b3994ULL, catalog);
}
// 同步公开操作 rb.output.get_instance_count；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Output_GetInstanceCount() {
    return Native<int>("rb.service.output", "rb.output.get_instance_count", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.output.get_kind_count；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Output_GetKindCount() {
    return Native<int>("rb.service.output", "rb.output.get_kind_count", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.output.get_kind；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Output_GetKind(int kindIndex, char* buffer, int bufferSize) {
    return Native<int>("rb.service.output", "rb.output.get_kind", 0x551455f08df06637ULL, kindIndex, buffer, bufferSize);
}
// 同步公开操作 rb.output.add_instance；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Output_AddInstance(const char* outputKind) {
    return Native<int>("rb.service.output", "rb.output.add_instance", 0x09206df780e497a6ULL, outputKind);
}
// 同步公开操作 rb.output.delete_instance_by_index；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Output_DeleteInstanceByIndex(int instanceIndex) {
    return Native<int>("rb.service.output", "rb.output.delete_instance_by_index", 0x3619d7061f665ad6ULL, instanceIndex);
}
// 同步公开操作 rb.output.connect_instance_by_index；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Output_ConnectInstanceByIndex(int instanceIndex) {
    return Native<int>("rb.service.output", "rb.output.connect_instance_by_index", 0x3619d7061f665ad6ULL, instanceIndex);
}
// 同步公开操作 rb.output.is_instance_connected；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Output_IsInstanceConnected(int instanceIndex) {
    return Native<int>("rb.service.output", "rb.output.is_instance_connected", 0x3619d7061f665ad6ULL, instanceIndex);
}
// 同步公开操作 rb.output.disconnect_instance_by_index；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Output_DisconnectInstanceByIndex(int instanceIndex) {
    return Native<int>("rb.service.output", "rb.output.disconnect_instance_by_index", 0x3619d7061f665ad6ULL, instanceIndex);
}
// 同步公开操作 rb.output.connect_all；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Output_ConnectAll() {
    return Native<int>("rb.service.output", "rb.output.connect_all", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.output.disconnect_all；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Output_DisconnectAll() {
    return Native<int>("rb.service.output", "rb.output.disconnect_all", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.output.are_all_disconnected；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Output_AreAllDisconnected() {
    return Native<int>("rb.service.output", "rb.output.are_all_disconnected", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.output.request_disconnect_all；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Output_RequestDisconnectAll() {
    return Native<int>("rb.service.output", "rb.output.request_disconnect_all", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.dynamic.read_profile；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Dynamic_ReadProfile(const char* profileName, RB_DynamicV2Profile* profile) {
    return Native<int>("rb.service.dynamic", "rb.dynamic.read_profile", 0xf3de04e9462024e5ULL, profileName, profile);
}
// 同步公开操作 rb.dynamic.read_profile_by_index；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Dynamic_ReadProfileByIndex(int profileIndex, RB_DynamicV2Profile* profile) {
    return Native<int>("rb.service.dynamic", "rb.dynamic.read_profile_by_index", 0xf140936d09ead715ULL, profileIndex, profile);
}
// 同步公开操作 rb.dynamic.save_profile_by_index；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Dynamic_SaveProfileByIndex(int profileIndex, const RB_DynamicV2Profile* profile) {
    return Native<int>("rb.service.dynamic", "rb.dynamic.save_profile_by_index", 0x27c0119d2b62e826ULL, profileIndex, profile);
}
// 同步公开操作 rb.dynamic.save_profile；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Dynamic_SaveProfile(const char* profileName, const RB_DynamicV2Profile* profile) {
    return Native<int>("rb.service.dynamic", "rb.dynamic.save_profile", 0x2ce372a8d8d63db6ULL, profileName, profile);
}
// 同步公开操作 rb.dynamic.apply_profile_to_current_rig；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Dynamic_ApplyProfileToCurrentRig(const RB_DynamicV2Profile* profile) {
    return Native<int>("rb.service.dynamic", "rb.dynamic.apply_profile_to_current_rig", 0xd36f201f42d61aebULL, profile);
}
// 同步公开操作 rb.dynamic.get_profile_count；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Dynamic_GetProfileCount() {
    return Native<int>("rb.service.dynamic", "rb.dynamic.get_profile_count", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.dynamic.get_profile_name；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Dynamic_GetProfileName(int profileIndex, char* buffer, int bufferSize) {
    return Native<int>("rb.service.dynamic", "rb.dynamic.get_profile_name", 0x551455f08df06637ULL, profileIndex, buffer, bufferSize);
}
// 同步公开操作 rb.dynamic.get_input_candidate_catalog；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Dynamic_GetInputCandidateCatalog(int dofIndex, RB_DynamicInputCandidateCatalog* catalog) {
    return Native<int>("rb.service.dynamic", "rb.dynamic.get_input_candidate_catalog", 0x3221a5a2ae94614cULL, dofIndex, catalog);
}
// 同步公开操作 rb.dynamic.read_current_game_profile_index；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Dynamic_ReadCurrentGameProfileIndex() {
    return Native<int>("rb.service.dynamic", "rb.dynamic.read_current_game_profile_index", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.dynamic.save_current_game_profile_index；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Dynamic_SaveCurrentGameProfileIndex(int profileIndex) {
    return Native<int>("rb.service.dynamic", "rb.dynamic.save_current_game_profile_index", 0x3619d7061f665ad6ULL, profileIndex);
}
// 同步公开操作 rb.dynamic.delete_profile_by_index；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Dynamic_DeleteProfileByIndex(int profileIndex) {
    return Native<int>("rb.service.dynamic", "rb.dynamic.delete_profile_by_index", 0x3619d7061f665ad6ULL, profileIndex);
}
// 同步公开操作 rb.motioneffect.get_catalog；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int MotionEffect_GetCatalog(RB_EffectCatalog* catalog) {
    return Native<int>("rb.service.motioneffect", "rb.motioneffect.get_catalog", 0xf72a0eeed4caff7cULL, catalog);
}
// 同步公开操作 rb.motioneffect.get_profile_count；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int MotionEffect_GetProfileCount() {
    return Native<int>("rb.service.motioneffect", "rb.motioneffect.get_profile_count", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.motioneffect.get_profile_name；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int MotionEffect_GetProfileName(int profileIndex, char* buffer, int bufferSize) {
    return Native<int>("rb.service.motioneffect", "rb.motioneffect.get_profile_name", 0x551455f08df06637ULL, profileIndex, buffer, bufferSize);
}
// 同步公开操作 rb.motioneffect.read_profile；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int MotionEffect_ReadProfile(const char* profileName, RB_MotionEffectProfile* profile) {
    return Native<int>("rb.service.motioneffect", "rb.motioneffect.read_profile", 0xa5c3b2b3e6d5feefULL, profileName, profile);
}
// 同步公开操作 rb.motioneffect.read_profile_by_index；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int MotionEffect_ReadProfileByIndex(int profileIndex, RB_MotionEffectProfile* profile) {
    return Native<int>("rb.service.motioneffect", "rb.motioneffect.read_profile_by_index", 0xebe41336260f37dfULL, profileIndex, profile);
}
// 同步公开操作 rb.motioneffect.save_profile；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int MotionEffect_SaveProfile(const char* profileName, const RB_MotionEffectProfile* profile) {
    return Native<int>("rb.service.motioneffect", "rb.motioneffect.save_profile", 0xbb1316d89ef098f2ULL, profileName, profile);
}
// 同步公开操作 rb.motioneffect.save_profile_by_index；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int MotionEffect_SaveProfileByIndex(int profileIndex, const RB_MotionEffectProfile* profile) {
    return Native<int>("rb.service.motioneffect", "rb.motioneffect.save_profile_by_index", 0x0fc66560266233e2ULL, profileIndex, profile);
}
// 同步公开操作 rb.motioneffect.apply_profile_to_current_rig；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int MotionEffect_ApplyProfileToCurrentRig(const RB_MotionEffectProfile* profile) {
    return Native<int>("rb.service.motioneffect", "rb.motioneffect.apply_profile_to_current_rig", 0xcd831df07ae6b419ULL, profile);
}
// 同步公开操作 rb.haptic.get_catalog；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Haptic_GetCatalog(RB_EffectCatalog* catalog) {
    return Native<int>("rb.service.haptic", "rb.haptic.get_catalog", 0xf72a0eeed4caff7cULL, catalog);
}
// 同步公开操作 rb.haptic.read_profile；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Haptic_ReadProfile(const char* profileName, RB_HapticEffectProfile* profile) {
    return Native<int>("rb.service.haptic", "rb.haptic.read_profile", 0x2350fe97c4e1df00ULL, profileName, profile);
}
// 同步公开操作 rb.haptic.read_profile_by_index；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Haptic_ReadProfileByIndex(int profileIndex, RB_HapticEffectProfile* profile) {
    return Native<int>("rb.service.haptic", "rb.haptic.read_profile_by_index", 0x9cc1e8081d0fb790ULL, profileIndex, profile);
}
// 同步公开操作 rb.haptic.save_profile；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Haptic_SaveProfile(const char* profileName, const RB_HapticEffectProfile* profile) {
    return Native<int>("rb.service.haptic", "rb.haptic.save_profile", 0x9c165a0eb9a194cdULL, profileName, profile);
}
// 同步公开操作 rb.haptic.save_profile_by_index；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Haptic_SaveProfileByIndex(int profileIndex, const RB_HapticEffectProfile* profile) {
    return Native<int>("rb.service.haptic", "rb.haptic.save_profile_by_index", 0xb3a647225697495dULL, profileIndex, profile);
}
// 同步公开操作 rb.haptic.apply_profile_to_current_function；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Haptic_ApplyProfileToCurrentFunction(const RB_HapticEffectProfile* profile) {
    return Native<int>("rb.service.haptic", "rb.haptic.apply_profile_to_current_function", 0x51eb5d806955a652ULL, profile);
}
// 同步公开操作 rb.wind.get_default_config；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Wind_GetDefaultConfig(RB_WindEffectConfig* config) {
    return Native<int>("rb.service.wind", "rb.wind.get_default_config", 0xc7a6254c6fac65fbULL, config);
}
// 同步公开操作 rb.wind.read_config；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Wind_ReadConfig(RB_WindEffectConfig* config) {
    return Native<int>("rb.service.wind", "rb.wind.read_config", 0xc7a6254c6fac65fbULL, config);
}
// 同步公开操作 rb.wind.save_config；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Wind_SaveConfig(const RB_WindEffectConfig* config) {
    return Native<int>("rb.service.wind", "rb.wind.save_config", 0x08cb835620509044ULL, config);
}
// 同步公开操作 rb.wind.apply_config_to_current_function；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Wind_ApplyConfigToCurrentFunction(const RB_WindEffectConfig* config) {
    return Native<int>("rb.service.wind", "rb.wind.apply_config_to_current_function", 0x08cb835620509044ULL, config);
}
// 同步公开操作 rb.wind.set_test_output；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Wind_SetTestOutput(int enabled, double outputPercent) {
    return Native<int>("rb.service.wind", "rb.wind.set_test_output", 0x065cd25b0d0ba623ULL, enabled, outputPercent);
}
// 同步公开操作 rb.seatbelt.get_default_config；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Seatbelt_GetDefaultConfig(RB_SeatbeltEffectConfig* config) {
    return Native<int>("rb.service.seatbelt", "rb.seatbelt.get_default_config", 0x94c9ae493fa2e9a1ULL, config);
}
// 同步公开操作 rb.seatbelt.read_config；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Seatbelt_ReadConfig(RB_SeatbeltEffectConfig* config) {
    return Native<int>("rb.service.seatbelt", "rb.seatbelt.read_config", 0x94c9ae493fa2e9a1ULL, config);
}
// 同步公开操作 rb.seatbelt.save_config；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Seatbelt_SaveConfig(const RB_SeatbeltEffectConfig* config) {
    return Native<int>("rb.service.seatbelt", "rb.seatbelt.save_config", 0x5c8528d28a41bf7eULL, config);
}
// 同步公开操作 rb.seatbelt.apply_config_to_current_function；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Seatbelt_ApplyConfigToCurrentFunction(const RB_SeatbeltEffectConfig* config) {
    return Native<int>("rb.service.seatbelt", "rb.seatbelt.apply_config_to_current_function", 0x5c8528d28a41bf7eULL, config);
}
// 同步公开操作 rb.seatbelt.set_test_output；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Seatbelt_SetTestOutput(int enabled, double leftPercent, double rightPercent) {
    return Native<int>("rb.service.seatbelt", "rb.seatbelt.set_test_output", 0xcc6b86899ca3bd88ULL, enabled, leftPercent, rightPercent);
}
// 同步公开操作 rb.rgb.refresh_devices；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Rgb_RefreshDevices() {
    return Native<int>("rb.service.rgb", "rb.rgb.refresh_devices", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.rgb.connect；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Rgb_Connect(const char* uid) {
    return Native<int>("rb.service.rgb", "rb.rgb.connect", 0x09206df780e497a6ULL, uid);
}
// 同步公开操作 rb.rgb.read_device_config；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Rgb_ReadDeviceConfig(RB_RgbDeviceConfig* config) {
    return Native<int>("rb.service.rgb", "rb.rgb.read_device_config", 0x06976bfa813a3503ULL, config);
}
// 同步公开操作 rb.rgb.apply_device_config；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Rgb_ApplyDeviceConfig(const RB_RgbDeviceConfig* config) {
    return Native<int>("rb.service.rgb", "rb.rgb.apply_device_config", 0x550ec8295b82e72aULL, config);
}
// 同步公开操作 rb.rgb.get_default_effect_profile；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Rgb_GetDefaultEffectProfile(RB_RgbEffectProfile* profile) {
    return Native<int>("rb.service.rgb", "rb.rgb.get_default_effect_profile", 0x2d88c1c9206d2315ULL, profile);
}
// 同步公开操作 rb.rgb.save_effect_profile；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Rgb_SaveEffectProfile(const RB_RgbEffectProfile* profile) {
    return Native<int>("rb.service.rgb", "rb.rgb.save_effect_profile", 0x1f330e459cee142aULL, profile);
}
// 同步公开操作 rb.rgb.apply_effect_profile；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Rgb_ApplyEffectProfile(const RB_RgbEffectProfile* profile) {
    return Native<int>("rb.service.rgb", "rb.rgb.apply_effect_profile", 0x1f330e459cee142aULL, profile);
}
// 同步公开操作 rb.command.run；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Command_Run(const char* command, const char* argsJson, char* resultBuffer, int resultBufferSize) {
    return Native<int>("rb.service.command", "rb.command.run", 0x94e57f0a17abece4ULL, command, argsJson, resultBuffer, resultBufferSize);
}
// 同步公开操作 rb.log.read；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Log_Read(char* buffer, int bufferSize) {
    return Native<int>("rb.service.log", "rb.log.read", 0x1edf04ab4bd979c0ULL, buffer, bufferSize);
}
// 同步公开操作 rb.serial.get_available_port_count；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Serial_GetAvailablePortCount() {
    return Native<int>("rb.service.serial", "rb.serial.get_available_port_count", 0xfa00318f42ab6671ULL);
}
// 同步公开操作 rb.serial.get_available_port_info；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Serial_GetAvailablePortInfo(int index, RB_SerialPortInfo* info) {
    return Native<int>("rb.service.serial", "rb.serial.get_available_port_info", 0xc239cdc39cc3f684ULL, index, info);
}
// 同步公开操作 rb.serial.create；参数有效期和业务结果见 SERVICE_API 对应条目。
inline RB_SerialHandle Serial_Create() {
    return Native<RB_SerialHandle>("rb.service.serial", "rb.serial.create", 0xc4c84dc23a98f52fULL);
}
// 同步公开操作 rb.serial.destroy；参数有效期和业务结果见 SERVICE_API 对应条目。
inline void Serial_Destroy(RB_SerialHandle handle) {
    return Native<void>("rb.service.serial", "rb.serial.destroy", 0x7f23dff9f50a67ffULL, handle);
}
// 同步公开操作 rb.serial.init；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Serial_Init(RB_SerialHandle handle, const char* portName, int baudRate) {
    return Native<int>("rb.service.serial", "rb.serial.init", 0xebe1c2d5cebea65cULL, handle, portName, baudRate);
}
// 同步公开操作 rb.serial.init_ex；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Serial_InitEx(RB_SerialHandle handle, const char* portName, int baudRate, int parity, int dataBits, int stopBits, int flowControl, unsigned int readBufferSize) {
    return Native<int>("rb.service.serial", "rb.serial.init_ex", 0x143f275533514c44ULL, handle, portName, baudRate, parity, dataBits, stopBits, flowControl, readBufferSize);
}
// 同步公开操作 rb.serial.open；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Serial_Open(RB_SerialHandle handle) {
    return Native<int>("rb.service.serial", "rb.serial.open", 0x72c599d50a448e38ULL, handle);
}
// 同步公开操作 rb.serial.close；参数有效期和业务结果见 SERVICE_API 对应条目。
inline void Serial_Close(RB_SerialHandle handle) {
    return Native<void>("rb.service.serial", "rb.serial.close", 0x7f23dff9f50a67ffULL, handle);
}
// 同步公开操作 rb.serial.write；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Serial_Write(RB_SerialHandle handle, const void* data, int size) {
    return Native<int>("rb.service.serial", "rb.serial.write", 0x144cd1d5f762efa2ULL, handle, data, size);
}
// 同步公开操作 rb.serial.read；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Serial_Read(RB_SerialHandle handle, void* data, int size) {
    return Native<int>("rb.service.serial", "rb.serial.read", 0x7739b29976189efbULL, handle, data, size);
}
// 同步公开操作 rb.serial.read_all；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Serial_ReadAll(RB_SerialHandle handle, void* data, int size) {
    return Native<int>("rb.service.serial", "rb.serial.read_all", 0x7739b29976189efbULL, handle, data, size);
}
// 同步公开操作 rb.serial.connect_read_event；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Serial_ConnectReadEvent(RB_SerialHandle handle, RB_SerialReadCallback callback, void* userData) {
    return Native<int>("rb.service.serial", "rb.serial.connect_read_event", 0x7dbf121e08f23d12ULL, handle, callback, userData);
}
// 同步公开操作 rb.serial.disconnect_read_event；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Serial_DisconnectReadEvent(RB_SerialHandle handle) {
    return Native<int>("rb.service.serial", "rb.serial.disconnect_read_event", 0x72c599d50a448e38ULL, handle);
}
// 同步公开操作 rb.serial.connect_hot_plug_event；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Serial_ConnectHotPlugEvent(RB_SerialHandle handle, RB_SerialHotPlugCallback callback, void* userData) {
    return Native<int>("rb.service.serial", "rb.serial.connect_hot_plug_event", 0x81984f49f7e45477ULL, handle, callback, userData);
}
// 同步公开操作 rb.serial.disconnect_hot_plug_event；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Serial_DisconnectHotPlugEvent(RB_SerialHandle handle) {
    return Native<int>("rb.service.serial", "rb.serial.disconnect_hot_plug_event", 0x72c599d50a448e38ULL, handle);
}
// 同步公开操作 rb.serial.set_operate_mode；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Serial_SetOperateMode(RB_SerialHandle handle, int operateMode) {
    return Native<int>("rb.service.serial", "rb.serial.set_operate_mode", 0x15ced3077f4d2983ULL, handle, operateMode);
}
// 同步公开操作 rb.serial.set_auto_reconnect_enabled；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Serial_SetAutoReconnectEnabled(RB_SerialHandle handle, int enabled) {
    return Native<int>("rb.service.serial", "rb.serial.set_auto_reconnect_enabled", 0x15ced3077f4d2983ULL, handle, enabled);
}
// 同步公开操作 rb.serial.set_auto_reconnect_interval；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Serial_SetAutoReconnectInterval(RB_SerialHandle handle, unsigned int milliseconds) {
    return Native<int>("rb.service.serial", "rb.serial.set_auto_reconnect_interval", 0xf9bd3e2e933c4f08ULL, handle, milliseconds);
}
// 同步公开操作 rb.serial.set_timeout_config；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Serial_SetTimeoutConfig(RB_SerialHandle handle, const RB_SerialTimeoutConfig* config) {
    return Native<int>("rb.service.serial", "rb.serial.set_timeout_config", 0x223439e8026d9f0dULL, handle, config);
}
// 同步公开操作 rb.serial.set_dtr；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Serial_SetDtr(RB_SerialHandle handle, int set) {
    return Native<int>("rb.service.serial", "rb.serial.set_dtr", 0x15ced3077f4d2983ULL, handle, set);
}
// 同步公开操作 rb.serial.set_rts；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Serial_SetRts(RB_SerialHandle handle, int set) {
    return Native<int>("rb.service.serial", "rb.serial.set_rts", 0x15ced3077f4d2983ULL, handle, set);
}
// 同步公开操作 rb.serial.get_last_error；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Serial_GetLastError(RB_SerialHandle handle) {
    return Native<int>("rb.service.serial", "rb.serial.get_last_error", 0x72c599d50a448e38ULL, handle);
}
// 同步公开操作 rb.serial.get_last_error_msg；参数有效期和业务结果见 SERVICE_API 对应条目。
inline int Serial_GetLastErrorMsg(RB_SerialHandle handle, char* buffer, int bufferSize) {
    return Native<int>("rb.service.serial", "rb.serial.get_last_error_msg", 0xfdc87707cc50a849ULL, handle, buffer, bufferSize);
}
}
