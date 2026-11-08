# 同步对象服务目录

此目录由 SDK 真源声明生成，仅列第三方公开操作；第一方兼容操作不属于公开合同。
请求格式、错误和生命周期参见 [对象协议](RaceBearSDK_OBJECT_API.md#同步本机服务)。签名是十六进制字符串。
第三方只依赖对象 key、操作 key、签名、返回类型和参数顺序；不要导入或拼接其它函数名。

| 对象 key | 操作 key | 签名 | 返回类型 | 参数（顺序即槽位） |
|---|---|---|---|---|
| rb.service.runtime | rb.runtime.set_app_data_name | 09206df780e497a6 | int | const char* name |
| rb.service.runtime | rb.runtime.get_app_data_name | 1edf04ab4bd979c0 | int | char* buffer, int bufferSize |
| rb.service.runtime | rb.runtime.start_loop | 3619d7061f665ad6 | int | int intervalMs |
| rb.service.runtime | rb.runtime.pause_loop | fa00318f42ab6671 | int | 无 |
| rb.service.runtime | rb.runtime.resume_loop | fa00318f42ab6671 | int | 无 |
| rb.service.runtime | rb.runtime.stop_loop | fa00318f42ab6671 | int | 无 |
| rb.service.runtime | rb.runtime.set_loop_interval_ms | 3619d7061f665ad6 | int | int intervalMs |
| rb.service.runtime | rb.runtime.get_loop_interval_ms | fa00318f42ab6671 | int | 无 |
| rb.service.runtime | rb.runtime.is_loop_running | fa00318f42ab6671 | int | 无 |
| rb.service.runtime | rb.runtime.is_loop_paused | fa00318f42ab6671 | int | 无 |
| rb.service.runtime | rb.runtime.do_calculations | fa00318f42ab6671 | int | 无 |
| rb.service.state | rb.state.read | 98a5d088fbeba1ff | int | RB_RuntimeState* state |
| rb.service.can | rb.can.refresh_devices | fa00318f42ab6671 | int | 无 |
| rb.service.can | rb.can.get_device_count | fa00318f42ab6671 | int | 无 |
| rb.service.can | rb.can.get_device_info | 336d1f5b6fcea2ff | int | int catalogIndex, RB_CanDeviceInfo* info |
| rb.service.can | rb.can.set_device_logical_index | 9cba53675984cc15 | int | const char* uid, int logicalIndex |
| rb.service.can | rb.can.bind_output_device | 448995d2bceffb92 | int | int outputIndex, const char* uid, int logicalIndex |
| rb.service.can | rb.can.get_output_binding_state | c1dcd4d58aba6d66 | int | int outputIndex, RB_CanOutputBindingState* state |
| rb.service.can | rb.can.sync_controller_from_output_config | fa00318f42ab6671 | int | 无 |
| rb.service.can | rb.can.get_controller_status_text | 1edf04ab4bd979c0 | int | char* buffer, int bufferSize |
| rb.service.can | rb.can.get_motor_count | fa00318f42ab6671 | int | 无 |
| rb.service.can | rb.can.get_motor_state | 4e26119faa3966d7 | int | int motorIndex, RB_CanMotorState* state |
| rb.service.can | rb.can.get_motor_parameter_count | fa00318f42ab6671 | int | 无 |
| rb.service.can | rb.can.set_motor_parameter_can_baud_rate | 3619d7061f665ad6 | int | int baudRateK |
| rb.service.can | rb.can.begin_motor_parameter_maintenance | fa00318f42ab6671 | int | 无 |
| rb.service.can | rb.can.end_motor_parameter_maintenance | fa00318f42ab6671 | int | 无 |
| rb.service.can | rb.can.get_motor_parameter_name | 551455f08df06637 | int | int parameterIndex, char* buffer, int bufferSize |
| rb.service.can | rb.can.get_motor_parameter_count_for_motor | 3619d7061f665ad6 | int | int motorIndex |
| rb.service.can | rb.can.get_motor_parameter_profile_type | 551455f08df06637 | int | int motorIndex, char* buffer, int bufferSize |
| rb.service.can | rb.can.export_motor_parameter_package | c807be4fa1a422fd | int | int motorIndex, const int* values, int count, const char* packagePath, int* errorParameterIndex |
| rb.service.can | rb.can.import_motor_parameter_package | 3a4b4efb89e901f1 | int | int motorIndex, const char* packagePath, int* values, int* selected, int count, int* errorParameterIndex |
| rb.service.can | rb.can.sync_motor_parameters | a8ecf4d0772357e7 | int | int sourceMotorIndex, int targetMotorIndex, const int* values, const int* selected, int count, RB_CanMotorParameterSyncResult* result |
| rb.service.can | rb.can.get_motor_parameter_capabilities_for_motor | 3619d7061f665ad6 | int | int motorIndex |
| rb.service.can | rb.can.is_motor_parameter_writable | 258290edf43985a5 | int | int motorIndex, int parameterIndex |
| rb.service.can | rb.can.get_motor_parameter_name_for_motor | cf758c1c7ff6b7bc | int | int motorIndex, int parameterIndex, char* buffer, int bufferSize |
| rb.service.can | rb.can.read_motor_parameter | 8556ffc21ff1f59a | int | int motorIndex, int parameterIndex, int* outValue |
| rb.service.can | rb.can.write_motor_parameter | 78409099752fa48a | int | int motorIndex, int parameterIndex, int value |
| rb.service.can | rb.can.save_motor_parameters | 258290edf43985a5 | int | int motorIndex, int rebootAfterSave |
| rb.service.can | rb.can.restore_motor_parameters | 3619d7061f665ad6 | int | int motorIndex |
| rb.service.can | rb.can.reboot_motor | 3619d7061f665ad6 | int | int motorIndex |
| rb.service.can | rb.can.clear_motor_fault | 3619d7061f665ad6 | int | int motorIndex |
| rb.service.can | rb.can.start_servo_firmware_update | 9c698b7b50d6633d | int | int motorIndex, const char* binFilePath |
| rb.service.can | rb.can.get_output_motor_count | 3619d7061f665ad6 | int | int outputIndex |
| rb.service.can | rb.can.get_output_motor_state | 0d24e8602857a224 | int | int outputIndex, int motorIndex, RB_CanMotorState* state |
| rb.service.can | rb.can.get_output_health_state | fad98548142b5681 | int | int outputIndex, RB_CanOutputHealthState* state |
| rb.service.can | rb.can.submit_positions | ee6ab4dd75535abd | int | int outputIndex, const int32_t* positions, int positionCount |
| rb.service.can | rb.can.set_external_realtime_mode | 258290edf43985a5 | int | int outputIndex, int enabled |
| rb.service.can | rb.can.is_external_realtime_mode | 3619d7061f665ad6 | int | int outputIndex |
| rb.service.can | rb.can.request_output_recovery | 3619d7061f665ad6 | int | int outputIndex |
| rb.service.debug | rb.debug.open_monitor | fa00318f42ab6671 | int | 无 |
| rb.service.debug | rb.debug.close_monitor | c4b612d028fa0b14 | void | 无 |
| rb.service.debug | rb.debug.is_monitor_open | fa00318f42ab6671 | int | 无 |
| rb.service.game | rb.game.get_catalog | a17f42c66940d939 | int | RB_GameCatalog* catalog |
| rb.service.game | rb.game.select_by_key | 09206df780e497a6 | int | const char* gameKey |
| rb.service.telemetry | rb.telemetry.find_index_by_key | 09206df780e497a6 | int | const char* telemetryKey |
| rb.service.telemetry | rb.telemetry.find_field_id_by_key | 09206df780e497a6 | int | const char* telemetryKey |
| rb.service.telemetry | rb.telemetry.get_catalog | 3457835915293adc | int | RB_TelemetryCatalog* catalog |
| rb.service.telemetry | rb.telemetry.get_raw_value | d799255b4c35d1c8 | double | int telemetryIndex |
| rb.service.telemetry | rb.telemetry.get_processed_value | d799255b4c35d1c8 | double | int telemetryIndex |
| rb.service.externalsource | rb.externalsource.open | d7990fee72140db2 | int | const RB_ExternalSourceDesc* desc |
| rb.service.externalsource | rb.externalsource.submit_frame | ac687a6129964a02 | int | const RB_ExternalTelemetryFrame* frame |
| rb.service.externalsource | rb.externalsource.get_state | 111ec6f687b2547f | int | RB_ExternalSourceState* state |
| rb.service.externalsource | rb.externalsource.close | c4b612d028fa0b14 | void | 无 |
| rb.service.game | rb.game.register_custom | c59572a5fd7e76b5 | int | const RB_CustomGameRegistration* registration |
| rb.service.game | rb.game.unregister_custom | 09206df780e497a6 | int | const char* gameKey |
| rb.service.game | rb.game.get_custom_state | c6fd3ab26bcc16ad | int | const char* gameKey, RB_CustomGameState* state |
| rb.service.game | rb.game.reload_custom_registry | fa00318f42ab6671 | int | 无 |
| rb.service.data | rb.data.read_json | c4964fc5cc2dd187 | int | const char* key, char* buffer, int bufferSize |
| rb.service.data | rb.data.write_json | 8b20cb572c544fad | int | const char* key, const char* jsonText |
| rb.service.game | rb.game.read_config | 3fd879acb31ab5b3 | int | const char* gameName, RB_GameConfig* config |
| rb.service.game | rb.game.save_config | 32ff9aedc9775ebc | int | const char* gameName, const RB_GameConfig* config |
| rb.service.game | rb.game.refresh_installations | fa00318f42ab6671 | int | 无 |
| rb.service.game | rb.game.get_install_count | fa00318f42ab6671 | int | 无 |
| rb.service.game | rb.game.get_install_info | 736e2ed22285698e | int | int index, RB_GameInstallInfo* info |
| rb.service.game | rb.game.find_install_by_key | 5edc40fffed01bde | int | const char* gameKey, RB_GameInstallInfo* info |
| rb.service.game | rb.game.find_install_path_by_key | c4964fc5cc2dd187 | int | const char* gameKey, char* buffer, int bufferSize |
| rb.service.game | rb.game.auto_connect | 738cfaaa18eb1d8c | int | int catalogIndex, const RB_GameConfig* config |
| rb.service.game | rb.game.auto_configure_current | fa00318f42ab6671 | int | 无 |
| rb.service.game | rb.game.install_current_plugin | 09206df780e497a6 | int | const char* gamePath |
| rb.service.game | rb.game.check_current_side_config | 98b1167043878955 | int | const RB_GameConfig* config |
| rb.service.plugin | rb.plugin.get_catalog | 18d5d81ba0169812 | int | RB_PluginCatalog* catalog |
| rb.service.plugin | rb.plugin.refresh | fa00318f42ab6671 | int | 无 |
| rb.service.plugin | rb.plugin.launch | 3619d7061f665ad6 | int | int pluginIndex |
| rb.service.plugin | rb.plugin.start_runtimes | fa00318f42ab6671 | int | 无 |
| rb.service.system | rb.system.read_config_or_default | d1f414bef2bd9c11 | int | RB_SystemConfig* config |
| rb.service.system | rb.system.save_config | f7cb43bd85af6cda | int | const RB_SystemConfig* config |
| rb.service.platform | rb.platform.read_config_by_index | f194d07e9bd13ea2 | int | int platformIndex, RB_PlatformConfig* config |
| rb.service.platform | rb.platform.save_config_by_index | 049fcb9c98a0a941 | int | int platformIndex, const RB_PlatformConfig* config |
| rb.service.platform | rb.platform.export_package | 9c698b7b50d6633d | int | int platformIndex, const char* packagePath |
| rb.service.platform | rb.platform.import_package | c4964fc5cc2dd187 | int | const char* packagePath, char* outPlatformKey, int bufferSize |
| rb.service.platform | rb.platform.set_selected_state | 049fcb9c98a0a941 | int | int platformIndex, const RB_PlatformConfig* config |
| rb.service.platform | rb.platform.apply_runtime_config_to_current_rig | 52c3db6e5bfa86d4 | int | const RB_PlatformConfig* config |
| rb.service.platform | rb.platform.read_motion_protection_config_by_index | cb9589661482ed7c | int | int platformIndex, RB_MotionProtectionConfig* config |
| rb.service.platform | rb.platform.save_motion_protection_config_by_index | fd99657ac0390597 | int | int platformIndex, const RB_MotionProtectionConfig* config |
| rb.service.platform | rb.platform.apply_motion_protection_config_to_current_rig | 1aee46c8e0d0051a | int | const RB_MotionProtectionConfig* config |
| rb.service.platform | rb.platform.read_motion_protection_config_v2_by_index | a3d261a652dc8908 | int | int platformIndex, RB_MotionProtectionConfigV2* config |
| rb.service.platform | rb.platform.save_motion_protection_config_v2_by_index | 1abcf2a4bff9f177 | int | int platformIndex, const RB_MotionProtectionConfigV2* config |
| rb.service.platform | rb.platform.apply_motion_protection_config_v2_to_current_rig | d58e1a3b34efa866 | int | const RB_MotionProtectionConfigV2* config |
| rb.service.platform | rb.platform.apply_config_to_current_rig | 049fcb9c98a0a941 | int | int platformIndex, const RB_PlatformConfig* config |
| rb.service.platform | rb.platform.get_count | fa00318f42ab6671 | int | 无 |
| rb.service.platform | rb.platform.get_name | 551455f08df06637 | int | int platformIndex, char* buffer, int bufferSize |
| rb.service.platform | rb.platform.is_dof_supported | 258290edf43985a5 | int | int platformIndex, int dofIndex |
| rb.service.platform | rb.platform.get_geometry_parameter_name | cf758c1c7ff6b7bc | int | int platformIndex, int geometryIndex, char* buffer, int bufferSize |
| rb.service.platform | rb.platform.get_stroke_parameter_name | cf758c1c7ff6b7bc | int | int platformIndex, int strokeIndex, char* buffer, int bufferSize |
| rb.service.platform | rb.platform.read_selected_index | fa00318f42ab6671 | int | 无 |
| rb.service.platform | rb.platform.save_selected_index | 3619d7061f665ad6 | int | int platformIndex |
| rb.service.platform | rb.platform.get_preview_pose_limit | 557ab84804984f38 | double | int platformIndex, const RB_PlatformConfig* config, int dofIndex |
| rb.service.platform | rb.platform.is_preview_pose_possible | e540292a392dd77e | int | int platformIndex, const RB_PlatformConfig* config, int dofIndex |
| rb.service.manualpose | rb.manualpose.set_test_enabled | 3619d7061f665ad6 | int | int enabled |
| rb.service.manualpose | rb.manualpose.set_drive | 065cd25b0d0ba623 | int | int dofIndex, double value |
| rb.service.manualpose | rb.manualpose.reset_drive | fa00318f42ab6671 | int | 无 |
| rb.service.manualpose | rb.manualpose.get_drive | d799255b4c35d1c8 | double | int dofIndex |
| rb.service.output | rb.output.read_instance_config_by_index | 410541423fb75b5c | int | int instanceIndex, RB_OutputConfig* config |
| rb.service.output | rb.output.get_instance_config_snapshot_by_index | 410541423fb75b5c | int | int instanceIndex, RB_OutputConfig* config |
| rb.service.output | rb.output.save_instance_config_by_index | 86e97c0553b54833 | int | int instanceIndex, const RB_OutputConfig* config |
| rb.service.output | rb.output.apply_instance_config_by_index | 86e97c0553b54833 | int | int instanceIndex, const RB_OutputConfig* config |
| rb.service.output | rb.output.get_idle_disable_timeout_seconds | f657d055fdb5985f | int | int instanceIndex, int* timeoutSeconds |
| rb.service.output | rb.output.set_idle_disable_timeout_seconds | 258290edf43985a5 | int | int instanceIndex, int timeoutSeconds |
| rb.service.output | rb.output.get_catalog | ec9d170da96b3994 | int | RB_OutputCatalog* catalog |
| rb.service.output | rb.output.get_instance_count | fa00318f42ab6671 | int | 无 |
| rb.service.output | rb.output.get_kind_count | fa00318f42ab6671 | int | 无 |
| rb.service.output | rb.output.get_kind | 551455f08df06637 | int | int kindIndex, char* buffer, int bufferSize |
| rb.service.seatbeltcan | rb.seatbeltcan.get_default_config | ba23385f2359e5f0 | int | RB_SeatbeltCanConfig* config |
| rb.service.seatbeltcan | rb.seatbeltcan.read_config | f4d35ed4c140417f | int | int outputInstanceIndex, RB_SeatbeltCanConfig* config |
| rb.service.seatbeltcan | rb.seatbeltcan.save_config | 784ff9a1f217e38e | int | int outputInstanceIndex, const RB_SeatbeltCanConfig* config |
| rb.service.seatbeltcan | rb.seatbeltcan.apply_config | 784ff9a1f217e38e | int | int outputInstanceIndex, const RB_SeatbeltCanConfig* config |
| rb.service.seatbeltcan | rb.seatbeltcan.get_state | b986895ea431c7ac | int | int outputInstanceIndex, RB_SeatbeltCanState* state |
| rb.service.seatbeltcan | rb.seatbeltcan.request_homing | 3619d7061f665ad6 | int | int outputInstanceIndex |
| rb.service.seatbeltcan | rb.seatbeltcan.request_reenable | 3619d7061f665ad6 | int | int outputInstanceIndex |
| rb.service.output | rb.output.ensure_default_config | fa00318f42ab6671 | int | 无 |
| rb.service.output | rb.output.get_auto_connect_instance_count | fa00318f42ab6671 | int | 无 |
| rb.service.output | rb.output.get_instance_key | 551455f08df06637 | int | int instanceIndex, char* buffer, int bufferSize |
| rb.service.output | rb.output.add_instance | 09206df780e497a6 | int | const char* outputKind |
| rb.service.output | rb.output.delete_instance_by_index | 3619d7061f665ad6 | int | int instanceIndex |
| rb.service.output | rb.output.connect_instance_by_index | 3619d7061f665ad6 | int | int instanceIndex |
| rb.service.output | rb.output.is_instance_connected | 3619d7061f665ad6 | int | int instanceIndex |
| rb.service.output | rb.output.is_instance_runtime_active | 3619d7061f665ad6 | int | int instanceIndex |
| rb.service.output | rb.output.is_any_runtime_active | fa00318f42ab6671 | int | 无 |
| rb.service.output | rb.output.disconnect_instance_by_index | 3619d7061f665ad6 | int | int instanceIndex |
| rb.service.output | rb.output.connect_all | fa00318f42ab6671 | int | 无 |
| rb.service.output | rb.output.connect_auto | fa00318f42ab6671 | int | 无 |
| rb.service.output | rb.output.disconnect_all | fa00318f42ab6671 | int | 无 |
| rb.service.output | rb.output.are_all_disconnected | fa00318f42ab6671 | int | 无 |
| rb.service.output | rb.output.request_disconnect_all | fa00318f42ab6671 | int | 无 |
| rb.service.dynamic | rb.dynamic.read_profile | f3de04e9462024e5 | int | const char* profileName, RB_DynamicV2Profile* profile |
| rb.service.dynamic | rb.dynamic.read_profile_by_index | f140936d09ead715 | int | int profileIndex, RB_DynamicV2Profile* profile |
| rb.service.dynamic | rb.dynamic.save_profile_by_index | 27c0119d2b62e826 | int | int profileIndex, const RB_DynamicV2Profile* profile |
| rb.service.dynamic | rb.dynamic.save_profile | 2ce372a8d8d63db6 | int | const char* profileName, const RB_DynamicV2Profile* profile |
| rb.service.dynamic | rb.dynamic.export_package | 9c698b7b50d6633d | int | int profileIndex, const char* packagePath |
| rb.service.dynamic | rb.dynamic.import_package | c4964fc5cc2dd187 | int | const char* packagePath, char* outProfileName, int bufferSize |
| rb.service.dynamic | rb.dynamic.apply_profile_to_current_rig | d36f201f42d61aeb | int | const RB_DynamicV2Profile* profile |
| rb.service.dynamic | rb.dynamic.get_profile_count | fa00318f42ab6671 | int | 无 |
| rb.service.dynamic | rb.dynamic.get_profile_name | 551455f08df06637 | int | int profileIndex, char* buffer, int bufferSize |
| rb.service.dynamic | rb.dynamic.get_input_candidate_catalog | 3221a5a2ae94614c | int | int dofIndex, RB_DynamicInputCandidateCatalog* catalog |
| rb.service.dynamic | rb.dynamic.read_current_game_profile_index | fa00318f42ab6671 | int | 无 |
| rb.service.dynamic | rb.dynamic.save_current_game_profile_index | 3619d7061f665ad6 | int | int profileIndex |
| rb.service.dynamic | rb.dynamic.delete_profile_by_index | 3619d7061f665ad6 | int | int profileIndex |
| rb.service.motioneffect | rb.motioneffect.get_catalog | f72a0eeed4caff7c | int | RB_EffectCatalog* catalog |
| rb.service.motioneffect | rb.motioneffect.get_profile_count | fa00318f42ab6671 | int | 无 |
| rb.service.motioneffect | rb.motioneffect.get_profile_name | 551455f08df06637 | int | int profileIndex, char* buffer, int bufferSize |
| rb.service.motioneffect | rb.motioneffect.delete_profile | 09206df780e497a6 | int | const char* profileName |
| rb.service.motioneffect | rb.motioneffect.read_profile | a5c3b2b3e6d5feef | int | const char* profileName, RB_MotionEffectProfile* profile |
| rb.service.motioneffect | rb.motioneffect.read_profile_by_index | ebe41336260f37df | int | int profileIndex, RB_MotionEffectProfile* profile |
| rb.service.motioneffect | rb.motioneffect.save_profile | bb1316d89ef098f2 | int | const char* profileName, const RB_MotionEffectProfile* profile |
| rb.service.motioneffect | rb.motioneffect.save_profile_by_index | 0fc66560266233e2 | int | int profileIndex, const RB_MotionEffectProfile* profile |
| rb.service.motioneffect | rb.motioneffect.apply_profile_to_current_rig | cd831df07ae6b419 | int | const RB_MotionEffectProfile* profile |
| rb.service.motioneffect | rb.motioneffect.get_default_config | 4ae7f0f0b3de6b3a | int | int effectIndex, RB_MotionEffectConfig* config |
| rb.service.haptic | rb.haptic.get_catalog | f72a0eeed4caff7c | int | RB_EffectCatalog* catalog |
| rb.service.haptic | rb.haptic.read_profile | 2350fe97c4e1df00 | int | const char* profileName, RB_HapticEffectProfile* profile |
| rb.service.haptic | rb.haptic.read_profile_by_index | 9cc1e8081d0fb790 | int | int profileIndex, RB_HapticEffectProfile* profile |
| rb.service.haptic | rb.haptic.save_profile | 9c165a0eb9a194cd | int | const char* profileName, const RB_HapticEffectProfile* profile |
| rb.service.haptic | rb.haptic.save_profile_by_index | b3a647225697495d | int | int profileIndex, const RB_HapticEffectProfile* profile |
| rb.service.haptic | rb.haptic.apply_profile_to_current_function | 51eb5d806955a652 | int | const RB_HapticEffectProfile* profile |
| rb.service.haptic | rb.haptic.get_default_config | 1201390abac4df2b | int | int effectIndex, RB_HapticEffectConfig* config |
| rb.service.wind | rb.wind.get_default_config | c7a6254c6fac65fb | int | RB_WindEffectConfig* config |
| rb.service.wind | rb.wind.read_config | c7a6254c6fac65fb | int | RB_WindEffectConfig* config |
| rb.service.wind | rb.wind.save_config | 08cb835620509044 | int | const RB_WindEffectConfig* config |
| rb.service.wind | rb.wind.apply_config_to_current_function | 08cb835620509044 | int | const RB_WindEffectConfig* config |
| rb.service.wind | rb.wind.set_test_output | 065cd25b0d0ba623 | int | int enabled, double outputPercent |
| rb.service.seatbelt | rb.seatbelt.get_default_config | 94c9ae493fa2e9a1 | int | RB_SeatbeltEffectConfig* config |
| rb.service.seatbelt | rb.seatbelt.read_config | 94c9ae493fa2e9a1 | int | RB_SeatbeltEffectConfig* config |
| rb.service.seatbelt | rb.seatbelt.save_config | 5c8528d28a41bf7e | int | const RB_SeatbeltEffectConfig* config |
| rb.service.seatbelt | rb.seatbelt.apply_config_to_current_function | 5c8528d28a41bf7e | int | const RB_SeatbeltEffectConfig* config |
| rb.service.seatbelt | rb.seatbelt.set_test_output | cc6b86899ca3bd88 | int | int enabled, double leftPercent, double rightPercent |
| rb.service.rgb | rb.rgb.refresh_devices | fa00318f42ab6671 | int | 无 |
| rb.service.rgb | rb.rgb.get_device_count | fa00318f42ab6671 | int | 无 |
| rb.service.rgb | rb.rgb.get_device_info | d00ba5745b9fa12e | int | int index, RB_RgbDeviceInfo* info |
| rb.service.rgb | rb.rgb.connect | 09206df780e497a6 | int | const char* uid |
| rb.service.rgb | rb.rgb.disconnect | c4b612d028fa0b14 | void | 无 |
| rb.service.rgb | rb.rgb.get_device_config_snapshot | 06976bfa813a3503 | int | RB_RgbDeviceConfig* config |
| rb.service.rgb | rb.rgb.read_device_config | 06976bfa813a3503 | int | RB_RgbDeviceConfig* config |
| rb.service.rgb | rb.rgb.apply_device_config | 550ec8295b82e72a | int | const RB_RgbDeviceConfig* config |
| rb.service.rgb | rb.rgb.save_device_config | fa00318f42ab6671 | int | 无 |
| rb.service.rgb | rb.rgb.get_effect_input_count | 3619d7061f665ad6 | int | int effectType |
| rb.service.rgb | rb.rgb.get_effect_input | cb89ab3cbf622e13 | int | int effectType, int inputIndex, RB_RgbEffectInputItem* item |
| rb.service.rgb | rb.rgb.get_default_effect_profile | 2d88c1c9206d2315 | int | RB_RgbEffectProfile* profile |
| rb.service.rgb | rb.rgb.read_effect_profile | 2d88c1c9206d2315 | int | RB_RgbEffectProfile* profile |
| rb.service.rgb | rb.rgb.save_effect_profile | 1f330e459cee142a | int | const RB_RgbEffectProfile* profile |
| rb.service.rgb | rb.rgb.apply_effect_profile | 1f330e459cee142a | int | const RB_RgbEffectProfile* profile |
| rb.service.rgb | rb.rgb.get_state | b317ebe8b1d210fa | int | RB_RgbState* state |
| rb.service.rgb | rb.rgb.set_test_pattern | 2b1c8c07b2e9b901 | int | int pattern, int red, int green, int blue |
| rb.service.rgb | rb.rgb.blackout | fa00318f42ab6671 | int | 无 |
| rb.service.rgb | rb.rgb.read_diagnostics | 7b907bf550bdb92b | int | RB_RgbDiagnostics* diagnostics |
| rb.service.rgb | rb.rgb.start_app_firmware_update | 8b20cb572c544fad | int | const char* uid, const char* binFilePath |
| rb.service.rgb | rb.rgb.start_iap_firmware_update | 8b20cb572c544fad | int | const char* uid, const char* binFilePath |
| rb.service.rgb | rb.rgb.get_firmware_update_state | 502832dba157267a | int | RB_RgbFirmwareUpdateState* state |
| rb.service.rgb | rb.rgb.is_firmware_update_running | fa00318f42ab6671 | int | 无 |
| rb.service.command | rb.command.run | 94e57f0a17abece4 | int | const char* command, const char* argsJson, char* resultBuffer, int resultBufferSize |
| rb.service.log | rb.log.read | 1edf04ab4bd979c0 | int | char* buffer, int bufferSize |
| rb.service.vocore | rb.vocore.get_device_count | fa00318f42ab6671 | int | 无 |
| rb.service.vocore | rb.vocore.get_device_info | 6c07eb252f2012b1 | int | int index, RB_VoCoreDeviceInfo* info |
| rb.service.vocore | rb.vocore.open | 5a79d7ba9a17d184 | int | const char* deviceId, int autoReconnect, RB_VoCoreHandle* handle |
| rb.service.vocore | rb.vocore.close | 25396c174230e013 | void | RB_VoCoreHandle handle |
| rb.service.vocore | rb.vocore.submit_frame | 05f9e2a4fe630e59 | int | RB_VoCoreHandle handle, const void* pixels, int dataSize, int width, int height, int stride, int pixelFormat, int rotation |
| rb.service.vocore | rb.vocore.set_brightness | 73ce31672059324b | int | RB_VoCoreHandle handle, int brightness |
| rb.service.vocore | rb.vocore.set_auto_reconnect | 73ce31672059324b | int | RB_VoCoreHandle handle, int enabled |
| rb.service.vocore | rb.vocore.get_state | 1bbdecc72df305b8 | int | RB_VoCoreHandle handle, RB_VoCoreState* state |
| rb.service.dashboardweb | rb.dashboardweb.open | 102ac4fc4c02552e | int | const RB_DashboardWebConfig* config, RB_DashboardWebHandle* handle |
| rb.service.dashboardweb | rb.dashboardweb.close | 26824625632c4eb5 | void | RB_DashboardWebHandle handle |
| rb.service.dashboardweb | rb.dashboardweb.submit_frame | 56e00b5106f444bf | int | RB_DashboardWebHandle handle, const void* pixels, int dataSize, int width, int height, int stride, int pixelFormat, int rotation |
| rb.service.dashboardweb | rb.dashboardweb.get_state | 7b75b9ec27e1b4e0 | int | RB_DashboardWebHandle handle, RB_DashboardWebState* state |
| rb.service.dashboard | rb.dashboard.render_frame | 74f6695e4623889c | int | const RB_RuntimeState* state, void* pixels, int width, int height, int stride, int templateIndex |
| rb.service.dashboard | rb.dashboard.refresh_packages | fa00318f42ab6671 | int | 无 |
| rb.service.dashboard | rb.dashboard.get_package_count | fa00318f42ab6671 | int | 无 |
| rb.service.dashboard | rb.dashboard.get_package_info | 59ec21b307d007df | int | int index, RB_DashboardPackageInfo* info |
| rb.service.dashboard | rb.dashboard.open_package | 075b3bbefdf21afd | int | const char* packageId, RB_DashboardPackageHandle* handle |
| rb.service.dashboard | rb.dashboard.close_package | 745299af411f38a3 | void | RB_DashboardPackageHandle handle |
| rb.service.dashboard | rb.dashboard.set_package_page | 83a59d2a6c6e91ff | int | RB_DashboardPackageHandle handle, int pageIndex |
| rb.service.dashboard | rb.dashboard.render_package_frame | f9dbabb6326815e0 | int | RB_DashboardPackageHandle handle, const RB_RuntimeState* state, void* pixels, int width, int height, int stride |
| rb.service.dashboard | rb.dashboard.get_package_state | 1bede66f8f624f84 | int | RB_DashboardPackageHandle handle, RB_DashboardPackageState* state |
| rb.service.dashboard | rb.dashboard.import_package | c4964fc5cc2dd187 | int | const char* sourcePath, char* packageIdBuffer, int packageIdBufferSize |
| rb.service.dashboard | rb.dashboard.delete_package | 09206df780e497a6 | int | const char* packageId |
| rb.service.dashboard | rb.dashboard.get_user_package_directory | 1edf04ab4bd979c0 | int | char* buffer, int bufferSize |
| rb.service.serial | rb.serial.get_available_port_count | fa00318f42ab6671 | int | 无 |
| rb.service.serial | rb.serial.get_available_port_info | c239cdc39cc3f684 | int | int index, RB_SerialPortInfo* info |
| rb.service.serial | rb.serial.get_version | 5055621638170689 | const char* | 无 |
| rb.service.serial | rb.serial.create | c4c84dc23a98f52f | RB_SerialHandle | 无 |
| rb.service.serial | rb.serial.destroy | 7f23dff9f50a67ff | void | RB_SerialHandle handle |
| rb.service.serial | rb.serial.init | ebe1c2d5cebea65c | int | RB_SerialHandle handle, const char* portName, int baudRate |
| rb.service.serial | rb.serial.init_ex | 143f275533514c44 | int | RB_SerialHandle handle, const char* portName, int baudRate, int parity, int dataBits, int stopBits, int flowControl, unsigned int readBufferSize |
| rb.service.serial | rb.serial.open | 72c599d50a448e38 | int | RB_SerialHandle handle |
| rb.service.serial | rb.serial.close | 7f23dff9f50a67ff | void | RB_SerialHandle handle |
| rb.service.serial | rb.serial.is_open | 72c599d50a448e38 | int | RB_SerialHandle handle |
| rb.service.serial | rb.serial.write | 144cd1d5f762efa2 | int | RB_SerialHandle handle, const void* data, int size |
| rb.service.serial | rb.serial.read | 7739b29976189efb | int | RB_SerialHandle handle, void* data, int size |
| rb.service.serial | rb.serial.read_some | 7739b29976189efb | int | RB_SerialHandle handle, void* data, int size |
| rb.service.serial | rb.serial.read_all | 7739b29976189efb | int | RB_SerialHandle handle, void* data, int size |
| rb.service.serial | rb.serial.read_line | c5b042fd0546530b | int | RB_SerialHandle handle, void* data, int size, char delimiter |
| rb.service.serial | rb.serial.get_read_buffer_used_len | c5578f978021a9bb | unsigned int | RB_SerialHandle handle |
| rb.service.serial | rb.serial.connect_read_event | 7dbf121e08f23d12 | int | RB_SerialHandle handle, RB_SerialReadCallback callback, void* userData |
| rb.service.serial | rb.serial.disconnect_read_event | 72c599d50a448e38 | int | RB_SerialHandle handle |
| rb.service.serial | rb.serial.connect_hot_plug_event | 81984f49f7e45477 | int | RB_SerialHandle handle, RB_SerialHotPlugCallback callback, void* userData |
| rb.service.serial | rb.serial.disconnect_hot_plug_event | 72c599d50a448e38 | int | RB_SerialHandle handle |
| rb.service.serial | rb.serial.set_operate_mode | 15ced3077f4d2983 | int | RB_SerialHandle handle, int operateMode |
| rb.service.serial | rb.serial.set_auto_reconnect_enabled | 15ced3077f4d2983 | int | RB_SerialHandle handle, int enabled |
| rb.service.serial | rb.serial.is_auto_reconnect_enabled | 72c599d50a448e38 | int | RB_SerialHandle handle |
| rb.service.serial | rb.serial.set_auto_reconnect_interval | f9bd3e2e933c4f08 | int | RB_SerialHandle handle, unsigned int milliseconds |
| rb.service.serial | rb.serial.get_auto_reconnect_interval | c5578f978021a9bb | unsigned int | RB_SerialHandle handle |
| rb.service.serial | rb.serial.try_reconnect_now | 72c599d50a448e38 | int | RB_SerialHandle handle |
| rb.service.serial | rb.serial.set_timeout_config | 223439e8026d9f0d | int | RB_SerialHandle handle, const RB_SerialTimeoutConfig* config |
| rb.service.serial | rb.serial.get_timeout_config | 48ad4b9b43ab362c | int | RB_SerialHandle handle, RB_SerialTimeoutConfig* config |
| rb.service.serial | rb.serial.set_dtr | 15ced3077f4d2983 | int | RB_SerialHandle handle, int set |
| rb.service.serial | rb.serial.set_rts | 15ced3077f4d2983 | int | RB_SerialHandle handle, int set |
| rb.service.serial | rb.serial.get_last_error | 72c599d50a448e38 | int | RB_SerialHandle handle |
| rb.service.serial | rb.serial.get_last_error_msg | fdc87707cc50a849 | int | RB_SerialHandle handle, char* buffer, int bufferSize |
