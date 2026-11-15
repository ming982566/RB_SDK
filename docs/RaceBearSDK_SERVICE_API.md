# 同步对象服务目录

此目录由 SDK 真源声明生成，仅列第三方公开操作；第一方兼容操作不属于公开合同。
请求格式、错误和生命周期参见 [对象协议](RaceBearSDK_OBJECT_API.md#同步本机服务)。签名是十六进制字符串。
第三方只依赖对象 key、操作 key、签名、返回类型和参数顺序；不要导入或拼接其它函数名。

离线生成绑定使用 [service-api.json](service-api.json)，不要解析 Markdown。逐操作业务说明在表后；
配置后端返回码、方案归属和 JSON 数据包 key 见 [业务合同补充](RaceBearSDK_SERVICE_CONTRACTS.md)。

| 对象 key | 操作 key | 签名 | 返回类型 | 参数（顺序即槽位） |
|---|---|---|---|---|
| rb.service.rgb | rb.rgb.get_effect_profile_count | fa00318f42ab6671 | int | 无 |
| rb.service.rgb | rb.rgb.get_effect_profile_name | 551455f08df06637 | int | int index, char* buffer, int bufferSize |
| rb.service.rgb | rb.rgb.read_effect_profile_by_name | 0810bf557997ab14 | int | const char* name, RB_RgbEffectProfile* profile |
| rb.service.rgb | rb.rgb.save_effect_profile_as | 71c2e734572824df | int | const char* name, const RB_RgbEffectProfile* profile |
| rb.service.rgb | rb.rgb.delete_effect_profile | 09206df780e497a6 | int | const char* name |
| rb.service.rgb | rb.rgb.export_effect_package | 8b20cb572c544fad | int | const char* name, const char* packagePath |
| rb.service.rgb | rb.rgb.import_effect_package | c4964fc5cc2dd187 | int | const char* packagePath, char* outName, int bufferSize |
| rb.service.rgb | rb.rgb.read_current_effect_profile_name | 1edf04ab4bd979c0 | int | char* buffer, int bufferSize |
| rb.service.rgb | rb.rgb.save_current_effect_profile_name | 09206df780e497a6 | int | const char* name |
| rb.service.motioneffect | rb.motioneffect.export_package | 8b20cb572c544fad | int | const char* name, const char* packagePath |
| rb.service.motioneffect | rb.motioneffect.import_package | c4964fc5cc2dd187 | int | const char* packagePath, char* outName, int bufferSize |
| rb.service.haptic | rb.haptic.get_profile_count | fa00318f42ab6671 | int | 无 |
| rb.service.haptic | rb.haptic.get_profile_name | 551455f08df06637 | int | int index, char* buffer, int bufferSize |
| rb.service.haptic | rb.haptic.delete_profile | 09206df780e497a6 | int | const char* name |
| rb.service.haptic | rb.haptic.export_package | 8b20cb572c544fad | int | const char* name, const char* packagePath |
| rb.service.haptic | rb.haptic.import_package | c4964fc5cc2dd187 | int | const char* packagePath, char* outName, int bufferSize |
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

## 逐操作业务说明

以下说明从标准公开头注释生成；参数顺序严格遵守上表。
除非条目另有说明，字符串为 UTF-8/NUL，输出缓冲由调用方分配，容量以字节计；
同步调用期间不得释放或并发改写参数，返回后不保留普通输入指针。

### rb.rgb.get_effect_profile_count

返回 RGB 命名方案数量；空库为 0，不把未命名活动配置计入。
- 返回：非负数量；配置读取失败返回配置后端负数码。

### rb.rgb.get_effect_profile_name

读取按 UTF-8 字节排序的 RGB 方案名；增删后须刷新列表。
- 参数 `index`：当前库的零基索引，不可持久化。
- 参数 `buffer`：输出 UTF-8/NUL 名称，调用方拥有。
- 参数 `bufferSize`：输出容量字节，最大合法名称需 128 字节。
- 返回：RB_OK、RB_INVALID_ARGUMENT、RB_BUFFER_TOO_SMALL 或配置后端负数码。

### rb.rgb.read_effect_profile_by_name

精确读取命名灯效；不改变当前选择、草稿或运行态。
- 参数 `name`：UTF-8 名称，1-127 字节，不能含路径分隔符、双引号、控制字符或为 _Default。
- 参数 `profile`：输出完整 RB_RgbEffectProfile/3，调用前设置 Size/Version。
- 返回：0=成功；-5=不存在；其它配置后端负数码或 RB_INVALID_ARGUMENT。

### rb.rgb.save_effect_profile_as

创建或覆盖命名灯效副本；不选定、不应用、不连接设备。
- 参数 `name`：UTF-8 名称，1-127 字节，规则同 read_effect_profile_by_name。
- 参数 `profile`：输入完整 RB_RgbEffectProfile/3，所有字段按公开头校验。
- 返回：0=成功；其它配置后端负数码或 RB_INVALID_ARGUMENT。

### rb.rgb.delete_effect_profile

删除命名灯效；若为当前选择则清空名称，保留活动配置和运行态。
- 参数 `name`：要删除的精确名称；不支持 _Default。
- 返回：0=成功；-5=不存在；其它配置后端负数码。

### rb.rgb.export_effect_package

导出命名 RGB 方案为 schema=1、kind=rgb 的独立 JSON 包。
- 参数 `name`：已存在的命名方案。
- 参数 `packagePath`：UTF-8 目标路径；由宿主文件保存对话框选择，可覆盖已有文件。
- 返回：0=成功；其它配置后端负数码。包不包含设备固件或 Flash 配置。

### rb.rgb.import_effect_package

验证并导入 RGB 包为命名副本；同名覆盖，不选定或应用。
- 参数 `packagePath`：UTF-8 包路径，文件最大 4 MiB。
- 参数 `outName`：输出导入的 UTF-8/NUL 名称；容量不足时不写配置。
- 参数 `bufferSize`：outName 容量字节，建议 128。
- 返回：0=成功；RB_BUFFER_TOO_SMALL=输出不足且无副作用；其它配置后端负数码。

### rb.rgb.read_current_effect_profile_name

读取已选 RGB 命名方案名称；空字符串表示未命名活动配置。
- 参数 `buffer`：输出 UTF-8/NUL 名称，调用方拥有。
- 参数 `bufferSize`：输出容量字节，建议 128。
- 返回：RB_OK、RB_BUFFER_TOO_SMALL 或配置后端负数码。

### rb.rgb.save_current_effect_profile_name

选定命名 RGB 方案，同一次文件写入更新活动配置与选择名；不应用运行态。
- 参数 `name`：已存在的精确名称；之后显式 reload 草稿和 apply，不能再次 save 当前配置覆盖它。
- 返回：0=成功；-5=不存在；其它配置后端负数码。
- 说明：原 save_effect_profile 保存未命名活动配置，会清空选择名；save_effect_profile_as 只保存库副本。

### rb.motioneffect.export_package

导出独立 MotionEffect 方案包，不要求同名 Dynamic 方案。
- 参数 `name`：已存在的运动增强方案名，1-127 UTF-8 字节。
- 参数 `packagePath`：UTF-8 目标路径，可覆盖已有文件。
- 返回：0=成功；其它配置后端负数码；不改变运行态。

### rb.motioneffect.import_package

验证并导入独立 MotionEffect 包；同名覆盖，不应用。
- 参数 `packagePath`：schema=1、kind=motionEffect，最大 4 MiB 的 UTF-8 JSON 文件。
- 参数 `outName`：输出 UTF-8/NUL 方案名，容量不足时不保存。
- 参数 `bufferSize`：outName 容量字节，建议 128。
- 返回：0=成功；RB_BUFFER_TOO_SMALL=输出不足且无副作用；其它配置后端负数码。

### rb.haptic.get_profile_count

返回 Haptic 可选方案数量，明确复用 Dynamic 方案目录，不枚举孤立的震动节点。
- 返回：非负数量；失败为负数；配合 get_profile_name 和 read_profile_by_index 使用。

### rb.haptic.get_profile_name

读取与 Dynamic 同一顺序的 Haptic 可选方案名。
- 参数 `index`：Dynamic 当前零基索引，不可持久化。
- 参数 `buffer`：输出 UTF-8/NUL 名称。
- 参数 `bufferSize`：输出容量字节。
- 返回：RB_OK、RB_BUFFER_TOO_SMALL 或错误码。

### rb.haptic.delete_profile

删除同名震动附属配置，不删除 Dynamic，也不改变当前 Haptic 运行态。
- 参数 `name`：非 _Default 的震动配置名称。
- 返回：0=成功；其它配置后端负数码；之后按索引读取将走既有默认回退。

### rb.haptic.export_package

导出已经保存的同名 Haptic 节点为独立包，不导出隐式默认回退值。
- 参数 `name`：已保存的震动方案名，1-127 UTF-8 字节。
- 参数 `packagePath`：UTF-8 目标路径，可覆盖已有文件。
- 返回：0=成功；-5=未保存该节点；其它配置后端负数码。

### rb.haptic.import_package

验证并导入 Haptic 包；要求接收端已有同名 Dynamic 方案，不应用。
- 参数 `packagePath`：schema=1、kind=haptic，最大 4 MiB 的 UTF-8 JSON 文件。
- 参数 `outName`：输出 UTF-8/NUL 名称，容量不足时不保存。
- 参数 `bufferSize`：outName 容量字节，建议 128。
- 返回：0=成功；-5=无同名 Dynamic；RB_BUFFER_TOO_SMALL=输出不足且无副作用；其它配置后端负数码。
- 说明：跨机整体迁移可直接使用包含震动节点的 Dynamic 配置包。

### rb.runtime.set_app_data_name

设置配置目录使用的应用名称，省略调用时默认为 "SimRacebear"。
- 参数 `name`：UTF-8、NUL 结尾的单个合法路径段；不能为空，也不能包含 Windows 路径非法字符。
- 返回：RB_OK 表示设置成功；RB_INVALID_ARGUMENT 表示名称无效；RB_ERROR 表示内部失败。
- 说明：必须在 RB_Runtime_Initialize 之前调用。

### rb.runtime.get_app_data_name

读取当前配置目录使用的应用名称。
- 参数 `buffer`：接收 UTF-8、NUL 结尾文本的调用方缓冲区。
- 参数 `bufferSize`：buffer 的总容量，单位为字节且必须大于 0。
- 返回：RB_OK 表示成功；RB_INVALID_ARGUMENT 表示缓冲区无效；RB_BUFFER_TOO_SMALL 表示容量不足；RB_ERROR 表示内部失败。

### rb.runtime.start_loop

启动由 SDK 专用高分辨率调度线程驱动的后端计算循环；已启动时会按新周期间隔重建循环。
- 参数 `intervalMs`：计算周期间隔，单位毫秒；有效范围 1-1000 ms，推荐初始值为 2 ms。
- 返回：RB_OK 表示启动成功；RB_INVALID_ARGUMENT 表示间隔不合法；RB_ERROR 表示未初始化或调度线程启动失败。
- 说明：传入 2 表示约每 2 ms 计算一次；传入 1 表示约每 1 ms 计算一次。调度迟到时跳过旧周期，不连续补跑历史计算。

### rb.runtime.pause_loop

暂停已启动的内部计算循环，但保留调度线程和当前频率。
- 返回：RB_OK 表示已暂停；RB_ERROR 表示循环未启动或内部失败。

### rb.runtime.resume_loop

恢复已暂停的内部计算循环。
- 返回：RB_OK 表示已恢复；RB_ERROR 表示循环未启动或内部失败。

### rb.runtime.stop_loop

停止并释放内部计算调度线程；未启动时调用也视为成功。
- 返回：RB_OK 表示停止完成；RB_ERROR 表示内部失败。

### rb.runtime.set_loop_interval_ms

修改内部计算循环周期；循环未启动时只保存间隔，已启动时立即重建定时器。
- 参数 `intervalMs`：计算周期间隔，单位毫秒；有效范围 1-1000 ms。
- 返回：RB_OK 表示设置成功；RB_INVALID_ARGUMENT 表示间隔不合法；RB_ERROR 表示重建定时器失败。

### rb.runtime.get_loop_interval_ms

获取当前保存的内部计算循环周期间隔。
- 返回：成功时返回 1-1000 范围内的间隔，单位毫秒；内部失败时返回 RB_ERROR。

### rb.runtime.is_loop_running

查询内部计算调度线程是否已经启动。
- 返回：1 表示已启动，0 表示未启动；内部异常也返回 0。

### rb.runtime.is_loop_paused

查询已启动的内部计算循环是否处于暂停状态。
- 返回：1 表示已暂停，0 表示未暂停或循环未启动；内部异常也返回 0。

### rb.runtime.do_calculations

手动执行一次完整后端计算，仅供不使用内部循环的高级或调试场景。
- 返回：RB_OK 表示计算成功；RB_ERROR 表示内部循环正在运行、已有计算尚未结束或本次计算失败。
- 说明：常规前端应调用 rb.runtime.start_loop，不要自行定时调用本函数。

### rb.state.read

原子读取前端高频刷新所需的完整运行态快照。
- 参数 `state`：接收状态的结构体指针；SDK 会清零并填写 Size、Version 和全部成员。
- 返回：RB_OK 表示读取成功；RB_INVALID_ARGUMENT 表示 state 为空；RB_ERROR 表示内部失败。

### rb.can.refresh_devices

重新枚举当前连接的 RaceBear CANAPP 设备并刷新设备目录。
- 返回：成功时返回当前目录数量；失败返回负数。
- 说明：本阶段第三方只接入 ProtocolType=1 的 RB_CAN 设备。刷新后旧 CatalogIndex 和 EnumerationIndex 全部失效。

### rb.can.get_device_count

返回最近一次 rb.can.refresh_devices 建立的设备目录数量。
- 返回：大于等于 0 的设备数量；失败返回负数。
- 说明：本函数不重新扫描 USB。设备热插拔后应先显式刷新。

### rb.can.get_device_info

读取最近一次设备目录中的一张 CANAPP 身份快照。
- 参数 `catalogIndex`：最近一次刷新返回目录中的零基索引。
- 参数 `info`：接收设备信息；调用前必须把 Size 设为 sizeof(RB_CanDeviceInfo)。
- 返回：RB_OK 表示成功；RB_INVALID_ARGUMENT 表示参数或结构大小无效；RB_ERROR 表示目录项已失效或读取失败。
- 说明：CatalogIndex 和 EnumerationIndex 只能用于当前目录快照，持久绑定必须保存 Uid。

### rb.can.set_device_logical_index

按稳定 MCU UID 设置一张 RB_CAN 设备的卡内逻辑索引。
- 参数 `uid`：RB_CanDeviceInfo::Uid 返回的 24 位大写十六进制 UID。
- 参数 `logicalIndex`：新逻辑索引，范围 1-255，并且必须与其它在线设备唯一。
- 返回：0 表示写入并读回成功；负数表示设备离线、索引冲突、输出运行中或底层访问失败。
- 说明：这是明确的设备维护动作。请先断开相关输出，并由用户操作触发单次调用。

### rb.can.bind_output_device

将一个 RB_CAN 输出实例稳定绑定到指定 UID 和逻辑索引。
- 参数 `outputIndex`：当前输出目录中的零基索引；对应实例的 OutputKind 必须为 RB_CAN。
- 参数 `uid`：最近一次设备目录返回的稳定 UID。
- 参数 `logicalIndex`：要写入设备并保存到输出配置的逻辑索引，范围 1-255。
- 返回：0 表示设备索引和输出绑定均已持久化；负数表示身份、协议、冲突、安全状态或写入失败。
- 说明：绑定完成后应重新刷新设备目录并读取 rb.can.get_output_binding_state 确认，不要直接修改运行时枚举索引。

### rb.can.get_output_binding_state

读取一个 CAN 输出与物理控制器的稳定绑定状态，包括独立 SEATBELT_CAN。
- 参数 `outputIndex`：当前输出目录中的零基索引。
- 参数 `state`：接收绑定状态；调用前必须把 Size 设为 sizeof(RB_CanOutputBindingState)。
- 返回：RB_OK 表示成功；RB_INVALID_ARGUMENT 表示参数或结构大小无效；RB_ERROR 表示输出不存在或不是有效 CAN 输出。
- 说明：SEATBELT_CAN 按保存的 UID 查询缓存目录；绑定就绪不代表电机就绪或满足最低运行固件版本。

### rb.can.sync_controller_from_output_config

根据当前输出配置同步 CAN 控制器会话和绑定状态。

### rb.can.get_controller_status_text

读取 CAN 控制器状态文本。

### rb.can.get_motor_count

读取当前 CAN 控制器管理的电机数量。

### rb.can.get_motor_state

读取当前 CAN 控制器管理的单个电机状态。

### rb.can.get_motor_parameter_count

读取全局 CAN 参数目录数量。

### rb.can.set_motor_parameter_can_baud_rate

设置参数维护使用的 CAN 波特率，支持 500 或 1000 Kbit/s。

### rb.can.begin_motor_parameter_maintenance

进入 CAN 电机参数维护模式，停止实时初始化/监控并保留参数 SDO 通道。

### rb.can.end_motor_parameter_maintenance

退出 CAN 电机参数维护模式并请求重新初始化和回零。

### rb.can.get_motor_parameter_name

读取指定参数的显示名称。

### rb.can.get_motor_parameter_count_for_motor

读取指定电机按协议支持的参数数量。

### rb.can.get_motor_parameter_profile_type

返回当前电机的参数文件类型 RB_CAN/FM_CAN/FC_CAN/RDS_CAN；只查询定义，不访问设备。

### rb.can.export_motor_parameter_package

导出调用方提供的当前窗口原始参数值为 UTF-8 JSON .rbmotor 文件，不读取或写入设备。
- 参数 `values`：按 SDK 参数索引排列，count 必须等于该电机参数数量；版本号以 uint32 原始位放入 int。
- 参数 `packagePath`：UTF-8 文件路径；验证全部数值后以临时文件替换目标。
- 参数 `errorParameterIndex`：可空；失败的 SDK 参数索引，无具体参数时为 -1。
- 返回：RB_OK 成功；RB_INVALID_ARGUMENT 表示类型/数量/数值无效；RB_ERROR 表示文件操作失败。

### rb.can.import_motor_parameter_package

完整校验同类型文件后返回参数草稿，不写设备；失败时两个输出数组保持不变。
- 参数 `values`：SDK 参数索引排列的草稿值；仅 selected[i]==1 的值有效。
- 参数 `selected`：同等长度的选择数组；Node-ID、波特率、只读项始终为 0。
- 参数 `count`：两个数组长度，必须等于该电机参数数量。
- 参数 `errorParameterIndex`：可空；具体参数错误时返回 SDK 索引，其余为 -1。

### rb.can.sync_motor_parameters

同步窗口提供的勾选值到另一同类型电机；必须已进入参数维护模式。
- 参数 `values`：SDK 参数索引排列；count 必须等于源/目标参数数量。
- 参数 `selected`：同长度的 0/1 标记；地址、波特率和只读项始终忽略。
- 参数 `result`：必填。完整校验/目标预读后仅写差异，逐项回读，最后调用厂家保存/重启。
- 返回：RB_OK 成功，校验失败为 RB_INVALID_ARGUMENT，其它为设备或 SDK 错误。
- 说明：同步调用可能等待设备超时；失败可能已部分生效，不提供事务回滚。源电机不读写。

### rb.can.get_motor_parameter_capabilities_for_motor

读取指定电机的参数能力位。

### rb.can.is_motor_parameter_writable

查询指定电机参数是否允许写入。

### rb.can.get_motor_parameter_name_for_motor

读取指定电机参数名称。

### rb.can.read_motor_parameter

读取指定电机参数值。

### rb.can.write_motor_parameter

写入指定电机参数值。

### rb.can.save_motor_parameters

保存指定电机参数，可选择保存后重启电机。

### rb.can.restore_motor_parameters

恢复指定电机参数默认值。

### rb.can.reboot_motor

重启指定电机。

### rb.can.clear_motor_fault

清除指定电机故障。

### rb.can.start_servo_firmware_update

在电机参数维护模式中异步升级指定 RB 伺服驱动器固件。

### rb.can.get_output_motor_count

返回指定 RB_CAN 输出当前管理的电机数量。
- 参数 `outputIndex`：当前输出目录中的零基索引。
- 返回：大于等于 0 的电机数量；参数无效或状态不可用时返回负数。
- 说明：电机目录属于该输出当前会话，重连或重新绑定后应重新取得数量。

### rb.can.get_output_motor_state

读取指定 RB_CAN 输出中的一个电机状态。
- 参数 `outputIndex`：当前输出目录中的零基索引。
- 参数 `motorIndex`：该输出当前电机目录中的零基索引。
- 参数 `state`：接收电机状态的结构体指针。
- 返回：RB_OK 表示成功；RB_INVALID_ARGUMENT 表示索引或指针无效；RB_ERROR 表示状态读取失败。

### rb.can.get_output_health_state

读取指定 RB_CAN 输出的机器可读健康与恢复状态。
- 参数 `outputIndex`：当前输出目录中的零基索引。
- 参数 `state`：接收健康快照；调用前把 Size 设为 sizeof(RB_CanOutputHealthState)。
- 返回：RB_OK 表示成功；RB_INVALID_ARGUMENT 表示参数或结构大小无效；RB_ERROR 表示读取失败。

### rb.can.submit_positions

向已连接并已武装的 CAN 运动输出提交一帧最新目标位置。
- 参数 `outputIndex`：当前输出目录中的零基索引；输出类型必须是 RB_CAN、FM_CAN、FC_CAN 或 RDS_CAN。
- 参数 `positions`：按输出配置 OutPutString 中的轴顺序排列的位置数组；位置使用 SDK 现有的 int32 目标位置单位。
- 参数 `positionCount`：位置数量，必须与输出配置中的 CAN 轴数量一致，范围 1-16。
- 返回：RB_OK 表示已提交到现有 latest-only 实时位置通道；RB_INVALID_ARGUMENT 表示参数无效；RB_ERROR 表示输出尚未 Ready、被安全状态阻断或提交失败。
- 说明：该接口只提交最新位置，不复制 CAN 协议，也不绕过现有急停、故障和自动恢复状态机。外部桥接程序必须持续发送，并在输入超时后主动发送停车位置或断开输出。

### rb.can.set_external_realtime_mode

设置指定 CAN 输出由外部宿主接管实时位置帧。
- 参数 `outputIndex`：当前输出目录中的零基索引。
- 参数 `enabled`：非零表示启用；启用后 SDK 内部 CAN 输出模板不再重复发送位置。
- 返回：RB_OK 表示设置成功；RB_INVALID_ARGUMENT 表示索引无效；RB_ERROR 表示会话不存在。
- 说明：仅用于 SimHub 等外部运动宿主；普通 SimRacebear 前端不应启用。

### rb.can.is_external_realtime_mode

查询指定 CAN 输出是否由外部宿主接管实时位置帧。

### rb.can.request_output_recovery

人工排除故障后显式清除指定输出的恢复限制并启动一次受控恢复。
- 参数 `outputIndex`：当前输出目录中的零基索引。
- 返回：RB_OK 表示恢复请求已接受；RB_INVALID_ARGUMENT 表示索引无效；RB_ERROR 表示安全条件或当前状态不允许恢复。
- 说明：仅由用户明确触发。返回成功不表示已经恢复运行，随后继续轮询健康快照。

### rb.debug.open_monitor

打开 SDK 内置资源监视器窗口；重复调用会激活已经打开的窗口。
- 返回：RB_OK 表示窗口已打开；RB_ERROR 表示窗口线程或窗口创建失败。
- 说明：可在 Debug 或 Release DLL 中使用。窗口只读取状态，不修改运行参数。

### rb.debug.close_monitor

关闭资源监视器窗口并等待其 UI 线程退出。
- 返回：无返回值。
- 说明：RB_Runtime_Shutdown 会自动调用本函数，宿主也可以提前关闭窗口。

### rb.debug.is_monitor_open

查询资源监视器窗口是否已经打开。
- 返回：1 表示窗口已打开，0 表示窗口未打开。

### rb.game.get_catalog

读取 SDK 支持的完整游戏目录。
- 参数 `catalog`：接收游戏目录的结构体指针；SDK 会清零并填写 Size、Version、Count 和 Items。
- 返回：RB_OK 表示成功；RB_INVALID_ARGUMENT 表示 catalog 为空；RB_ERROR 表示内部失败。
- 说明：第三方创建或选择游戏源时应使用 Items[i].GameKey，不要让用户手动输入完整游戏名。

### rb.game.select_by_key

按稳定游戏 key 选择当前游戏并创建对应遥测源。
- 参数 `gameKey`：来自 RB_GameCatalogItem::GameKey 的 UTF-8、NUL 结尾字符串。
- 返回：RB_OK 表示已选择；RB_INVALID_ARGUMENT 表示 gameKey 为空或不存在；RB_ERROR 表示创建失败。

### rb.telemetry.find_index_by_key

按RaceBear稳定遥测key解析当前SDK运行时索引。
- 参数 `telemetryKey`：来自RB_TelemetryCatalogItem::Key的UTF-8字符串。
- 返回：成功返回0到目录数量减1的索引；key不存在或参数无效时返回RB_INVALID_ARGUMENT。
- 说明：返回索引用于当前进程调用；持久化时保存 telemetryKey。

### rb.telemetry.find_field_id_by_key

按稳定遥测key取得MMF/UDP线协议FieldId。
- 返回：成功返回非负FieldId；key不存在时返回RB_INVALID_ARGUMENT。
- 说明：FieldId可以写入客户游戏协议；运行时Index不能写入协议。

### rb.telemetry.get_catalog

读取RaceBear遥测目录、稳定key、显示名、单位和当前源支持状态。

### rb.telemetry.get_raw_value

读取指定运行时索引的原始输入遥测值；索引应通过rb.telemetry.find_index_by_key解析。

### rb.telemetry.get_processed_value

读取指定运行时索引经过数据源过渡处理后的处理值。

### rb.externalsource.open

创建并连接SDK外部游戏源，替换当前活动内置游戏源。
- 返回：RB_OK表示成功；RB_INVALID_ARGUMENT表示结构版本或字段无效；RB_ERROR表示创建失败。

### rb.externalsource.submit_frame

提交一帧完整外部遥测数据；函数只复制数据，不在调用线程执行平台计算。
- 返回：RB_OK表示接收成功；RB_INVALID_ARGUMENT表示帧或值无效；RB_ERROR表示外部源未打开。

### rb.externalsource.get_state

读取当前外部游戏源状态。

### rb.externalsource.close

关闭并释放外部游戏源；未打开时调用也安全。

### rb.game.register_custom

注册或更新自定义游戏并持久化到CustomGamesV1.json。

### rb.game.unregister_custom

注销自定义游戏；内置游戏不能通过本接口删除。

### rb.game.get_custom_state

读取指定自定义游戏的注册和接收状态。

### rb.game.reload_custom_registry

重新读取CustomGamesV1.json并刷新游戏目录。

### rb.data.read_json

按固定数据 key 读取 UTF-8 JSON 包。
- 参数 `key`：数据包 key；支持范围见 RaceBearSDK_SERVICE_CONTRACTS.md 的“JSON 数据包”章节。
- 参数 `buffer`：接收 UTF-8、NUL 结尾 JSON 的调用方缓冲区。
- 参数 `bufferSize`：buffer 的总容量，单位为字节且必须大于 0。
- 返回：RB_OK 表示成功；RB_INVALID_ARGUMENT 表示 key 或缓冲区无效；RB_BUFFER_TOO_SMALL 表示容量不足；RB_ERROR 表示读取失败。

### rb.data.write_json

按固定配置 key 写入完整 UTF-8 JSON 配置文档。
- 参数 `key`：可写配置包 key；支持范围见 RaceBearSDK_SERVICE_CONTRACTS.md 的“JSON 数据包”章节。
- 参数 `jsonText`：UTF-8、NUL 结尾的完整 JSON 文本，不能为空。
- 返回：RB_OK 表示写入成功；RB_INVALID_ARGUMENT 表示 key 或文本无效；RB_ERROR 表示保存失败。
- 说明：保存后是否需要执行 runtime.reloadConfig 命令，取决于对应配置的使用场景。

### rb.game.read_config

读取指定游戏的遥测配置。
- 参数 `gameName`：游戏配置名，通常使用 RB_GameCatalogItem::GameKey。
- 参数 `config`：接收配置的结构体指针。
- 返回：RB_OK/CONFIG_SUCCESS 表示成功；负数表示失败或参数无效。

### rb.game.save_config

保存指定游戏的遥测配置。
- 参数 `gameName`：游戏配置名，通常使用 RB_GameCatalogItem::GameKey。
- 参数 `config`：完整游戏配置结构体。
- 返回：RB_OK/CONFIG_SUCCESS 表示成功；负数表示失败或参数无效。

### rb.game.refresh_installations

重新扫描受支持游戏的安装目录和运行进程。
- 返回：大于等于 0 的安装信息数量；失败返回负数。
- 说明：扫描属于低频操作，只在启动、页面刷新或用户明确要求时调用。

### rb.game.get_install_count

返回最近一次安装扫描的条目数量；失败返回负数。

### rb.game.get_install_info

按扫描结果索引读取安装和运行状态。
- 参数 `index`：零基扫描结果索引。
- 参数 `info`：接收完整安装信息的结构体指针。
- 返回：RB_OK 表示成功；RB_INVALID_ARGUMENT 表示索引或指针无效。

### rb.game.find_install_by_key

按稳定 gameKey 查找安装和运行状态。
- 参数 `gameKey`：来自 RB_GameCatalogItem::GameKey。
- 参数 `info`：接收安装信息的结构体指针。
- 返回：RB_OK 表示找到安装目录或运行进程；RB_ERROR 表示未找到。

### rb.game.find_install_path_by_key

按稳定 gameKey 读取游戏安装路径。
- 返回：RB_OK、RB_BUFFER_TOO_SMALL、RB_INVALID_ARGUMENT 或 RB_ERROR。

### rb.game.auto_connect

按游戏目录索引和完整遥测配置创建或重新连接当前 Source。
- 参数 `catalogIndex`：RB_GameCatalogItem::CatalogIndex。
- 参数 `config`：先由 rb.game.read_config 读取并修改的完整配置。
- 返回：RB_OK 表示 Source 已成功创建或连接；其他值表示配置或连接失败。

### rb.game.auto_configure_current

对当前游戏执行 SDK 内置的一键遥测配置。
- 返回：RB_OK 表示游戏侧配置成功；RB_ERROR 表示当前游戏不支持或配置失败。

### rb.game.install_current_plugin

为当前游戏安装 SDK 内置的游戏侧遥测插件。
- 参数 `gamePath`：UTF-8 游戏安装目录；应来自 RB_GameInstallInfo::InstallPath。
- 返回：RB_OK 表示安装成功；其他值表示路径无效、不支持或安装失败。

### rb.game.check_current_side_config

检查当前游戏侧配置是否与给定遥测配置匹配。
- 返回：RB_OK 表示配置可用；RB_ERROR 表示缺失、不匹配或当前游戏不支持检查。

### rb.plugin.get_catalog

一次读取功能插件目录；目录刷新后旧索引失效。
- 参数 `catalog`：接收插件目录的结构体指针；调用前零初始化。
- 返回：RB_OK 表示成功；RB_INVALID_ARGUMENT 表示 catalog 为空。

### rb.plugin.refresh

重新扫描功能插件并返回插件数量；失败返回负数。

### rb.plugin.launch

启动指定插件提供的前端程序。
- 参数 `pluginIndex`：最近一次插件目录中的零基索引。
- 返回：RB_OK 表示已启动；RB_ERROR 表示插件不可启动或启动失败。

### rb.plugin.start_runtimes

启动全部功能插件后台运行态。
- 返回：大于等于 0 的已启动/已存在运行态数量；失败返回负数。

### rb.system.read_config_or_default

读取系统配置；配置不存在时返回 SDK 默认值。

### rb.system.save_config

保存系统配置。

### rb.platform.read_config_by_index

按平台索引读取平台结构和运行参数。

### rb.platform.save_config_by_index

按平台索引保存平台结构和运行参数。

### rb.platform.export_package

导出指定平台的完整配置包。

### rb.platform.import_package

导入平台配置包并覆盖包内 platformKey 对应的平台节点。

### rb.platform.set_selected_state

设置当前选中的平台状态缓存。

### rb.platform.apply_runtime_config_to_current_rig

将平台配置应用到当前运行中的平台模型。

### rb.platform.read_motion_protection_config_by_index

按平台索引读取 Motion 碰撞/异常保护配置。

### rb.platform.save_motion_protection_config_by_index

按平台索引保存 Motion 碰撞/异常保护配置。

### rb.platform.apply_motion_protection_config_to_current_rig

将 Motion 碰撞/异常保护配置热应用到当前运行中的 Rig。

### rb.platform.read_motion_protection_config_v2_by_index

读取六自由度结果保护 V2 配置，SDK 写入 Size/Version 和全部字段。

### rb.platform.save_motion_protection_config_v2_by_index

保存 V2 配置，Size 和 Version 必须正确，失败不部分保存。

### rb.platform.apply_motion_protection_config_v2_to_current_rig

热应用 V2 配置，需有效授权和当前 Rig。

### rb.platform.apply_config_to_current_rig

保存并应用指定平台配置到当前运行中的平台模型。

### rb.platform.get_count

返回 SDK 支持的平台数量。
- 返回：返回大于等于 0 的平台数量；初始化失败时返回 0。
- 说明：有效平台索引范围为 [0, 返回值)，索引仅用于当前 SDK 版本的目录访问。

### rb.platform.get_name

按目录索引读取平台名称。
- 参数 `platformIndex`：平台零基目录索引。
- 参数 `buffer`：接收 UTF-8、NUL 结尾平台名称的缓冲区。
- 参数 `bufferSize`：buffer 的总字节数。
- 返回：RB_OK 表示成功；RB_INVALID_ARGUMENT 表示索引或缓冲区无效。

### rb.platform.is_dof_supported

查询平台是否支持指定 6DOF。
- 参数 `platformIndex`：平台零基目录索引。
- 参数 `dofIndex`：DOF 索引，0-5 依次为 Sway/Surge/Heave/Yaw/Roll/Pitch。
- 返回：1 表示支持，0 表示不支持或参数无效。

### rb.platform.get_geometry_parameter_name

读取 L1-L7 几何参数的显示名称。
- 参数 `platformIndex`：平台零基目录索引。
- 参数 `geometryIndex`：参数编号，范围 1-7。
- 参数 `buffer`：接收 UTF-8、NUL 结尾名称的缓冲区；未使用参数返回空字符串。
- 参数 `bufferSize`：buffer 的总字节数。
- 返回：RB_OK 表示调用成功；RB_INVALID_ARGUMENT 表示参数无效。

### rb.platform.get_stroke_parameter_name

读取 L8-L10 执行器行程参数的显示名称。
- 参数 `platformIndex`：平台零基目录索引。
- 参数 `strokeIndex`：参数编号，范围 8-10。
- 参数 `buffer`：接收 UTF-8、NUL 结尾名称的缓冲区；未使用参数返回空字符串。
- 参数 `bufferSize`：buffer 的总字节数。
- 返回：RB_OK 表示调用成功；RB_INVALID_ARGUMENT 表示参数无效。

### rb.platform.read_selected_index

读取当前持久化选择的平台零基索引；失败返回负数。

### rb.platform.save_selected_index

保存当前平台零基索引；返回 RB_OK 或错误码。

### rb.platform.get_preview_pose_limit

计算指定平台配置下某个 DOF 的正负预览范围。
- 返回：线性 DOF 返回毫米，旋转 DOF 返回度；不可达或参数无效时返回 0。

### rb.platform.is_preview_pose_possible

判断指定平台配置是否可以预览该 DOF；1=可以，0=不可达或参数无效。

### rb.manualpose.set_test_enabled

启用或关闭真实运行平台的手动姿态测试输入。
- 参数 `enabled`：1=启用，0=关闭并恢复正常遥测输入。
- 返回：RB_OK 表示状态已更新；RB_ERROR 表示运行平台尚未创建。

### rb.manualpose.set_drive

设置单个 DOF 的手动测试值。
- 参数 `dofIndex`：0-5 依次为 Sway/Surge/Heave/Yaw/Roll/Pitch。
- 参数 `value`：线性 DOF 单位 mm，旋转 DOF 单位度；应限制在 rb.platform.get_preview_pose_limit 返回范围内。
- 返回：RB_OK 表示已设置；RB_INVALID_ARGUMENT 或 RB_ERROR 表示输入无效或平台不可用。

### rb.manualpose.reset_drive

将全部手动姿态输入复位为 0；返回 RB_OK 或 RB_ERROR。

### rb.manualpose.get_drive

读取单个 DOF 当前手动测试值；参数无效或平台不可用时返回 0。

### rb.output.read_instance_config_by_index

按输出实例索引读取输出配置。
- 参数 `instanceIndex`：输出实例零基索引，范围 [0, rb.output.get_instance_count)。
- 参数 `config`：接收完整配置的结构体指针；调用前零初始化。
- 返回：RB_OK 表示成功；RB_INVALID_ARGUMENT 表示索引或指针无效；RB_ERROR 表示读取失败。
- 说明：编辑页面先读取完整结构体，再修改目标字段并保存，以保留未展示字段。

### rb.output.get_instance_config_snapshot_by_index

读取指定输出实例的运行态配置缓存，不访问配置文件；适合状态栏和高频 UI 刷新。

### rb.output.save_instance_config_by_index

按输出实例索引保存输出配置。
- 参数 `instanceIndex`：输出实例零基索引。
- 参数 `config`：要保存的完整配置；字符串均为 UTF-8、NUL 结尾。
- 返回：RB_OK 表示已保存；其他值表示参数无效或写入失败。
- 说明：修改 Port、IP、端口、CAN 索引等连接参数前，应先断开该实例。

### rb.output.apply_instance_config_by_index

按输出实例索引将输出配置应用到运行态。
- 参数 `instanceIndex`：输出实例零基索引。
- 参数 `config`：要应用的完整配置。
- 返回：RB_OK 表示运行态已接受配置；其他值表示实例不存在或应用失败。
- 说明：Apply 不表示用户要求立即连接，也不能替代 Save。

### rb.output.get_idle_disable_timeout_seconds

读取或保存 CAN 输出空闲下使能超时，单位秒；0 禁止自动下使能。
- 说明：不扩展 RB_OutputConfig；只有 RB_CAN/FM_CAN/FC_CAN 支持该设置，
并且仅在输出配置的 AutoConnect 为 1 时执行。

### rb.output.set_idle_disable_timeout_seconds

设置指定 CAN 输出空闲下使能超时，单位秒；0 禁止自动下使能。

### rb.output.get_catalog

一次读取输出实例目录和当前可用于命令模板的输出 Key。
- 参数 `catalog`：接收目录的结构体指针；调用前零初始化。
- 返回：RB_OK 表示成功；RB_INVALID_ARGUMENT 表示 catalog 为空。
- 说明：Key 列表会随当前平台轴数和已创建功能变化，前端不要写死。

### rb.output.get_instance_count

返回当前输出实例数量。
- 返回：返回大于等于 0 的实例数量；初始化失败时返回 0。
- 说明：有效实例索引范围为 [0, 返回值)。

### rb.output.get_kind_count

返回 SDK 支持的输出类型数量。
- 返回：大于等于 0 的类型数量；初始化前也可以调用。

### rb.output.get_kind

按索引读取可传给 rb.output.add_instance 的输出类型 key。
- 参数 `kindIndex`：输出类型零基索引，范围 [0, rb.output.get_kind_count)。
- 参数 `buffer`：接收 UTF-8、NUL 结尾类型 key 的缓冲区。
- 参数 `bufferSize`：buffer 总字节数。
- 返回：RB_OK、RB_BUFFER_TOO_SMALL 或 RB_INVALID_ARGUMENT。

### rb.seatbeltcan.get_default_config

读取安全带 CAN 默认配置。

### rb.seatbeltcan.read_config

读取指定输出实例的安全带 CAN 配置。

### rb.seatbeltcan.save_config

保存指定输出实例的安全带 CAN 配置。

### rb.seatbeltcan.apply_config

将安全带 CAN 配置应用到已创建但未连接的输出实例；安全带产品不依赖 SDK 主授权。

### rb.seatbeltcan.get_state

读取指定安全带 CAN 输出实例的连接、回零和目标位置状态。

### rb.seatbeltcan.request_homing

请求指定安全带 CAN 输出实例执行一次回零。

### rb.seatbeltcan.request_reenable

显式重新启用已完成释放/停机的安全带会话。仍释放时拒绝提交，急停仍按下时固件拒绝恢复。
仅提交异步请求，随后轮询rb.seatbeltcan.get_state；RB_OK不表示已进入Ready。
旧遥测/测试目标会作废，恢复后等待新的输入。

### rb.output.ensure_default_config

确保至少存在一个默认 Serial 输出实例。
- 返回：RB_OK 表示配置已经存在或创建成功；负数表示初始化或写入失败。

### rb.output.get_auto_connect_instance_count

返回启用 AutoConnect 的输出实例数量。
- 返回：大于等于 0 的实例数量；失败按 0 处理并读取 SDK 日志。

### rb.output.get_instance_key

读取指定输出实例的稳定 key。
- 返回：RB_OK、RB_BUFFER_TOO_SMALL 或 RB_INVALID_ARGUMENT。

### rb.output.add_instance

添加输出实例。
- 参数 `outputKind`："Serial"、"UDP"、"MMF"、"RB_CAN"、"FM_CAN"、"FC_CAN"、"RDS_CAN"、"SEATBELT_CAN"、"Sim-FAN" 或 "Vibration"。
- 返回：成功时返回新实例的零基索引；失败返回负数错误码。

### rb.output.delete_instance_by_index

删除指定输出实例及其配置。
- 说明：删除前必须先断开实例；删除后后续实例索引可能变化，应刷新目录。

### rb.output.connect_instance_by_index

连接指定输出实例。
- 参数 `instanceIndex`：输出实例零基索引。
- 返回：RB_OK 表示连接流程已成功启动；其他值表示索引无效或启动失败。

### rb.output.is_instance_connected

查询指定输出实例是否已经连接。
- 参数 `instanceIndex`：输出实例零基索引。
- 返回：1 表示已连接，0 表示未连接或索引无效。

### rb.output.is_instance_runtime_active

查询指定实例是否已进入周期输出运行态。
- 参数 `instanceIndex`：输出实例零基索引。
- 返回：1 表示正在周期输出；0 表示未运行或索引无效。

### rb.output.is_any_runtime_active

查询是否存在任意周期输出运行态。
- 返回：1 表示至少一个实例正在输出；0 表示全部未运行。

### rb.output.disconnect_instance_by_index

断开指定输出实例。
- 参数 `instanceIndex`：输出实例零基索引。
- 返回：RB_OK 表示成功，其他值表示索引无效或断开失败。

### rb.output.connect_all

连接全部输出实例。
- 返回：大于等于 0 表示成功发起连接的实例数量；负数表示整体操作失败。
- 说明：未授权时仅连接 `SEATBELT_CAN`、`Sim-FAN` 和 `Vibration` 独立输出；普通平台输出仍保持断开。

### rb.output.connect_auto

只连接 AutoConnect=1 的输出实例。
- 返回：大于等于 0 表示成功发起连接的实例数量；负数表示整体操作失败。
- 说明：未授权时仅连接 `SEATBELT_CAN`、`Sim-FAN` 和 `Vibration` 独立输出；普通平台输出仍保持断开。

### rb.output.disconnect_all

断开全部输出实例。
- 返回：大于等于 0 表示成功发起断开的实例数量；负数表示整体操作失败。

### rb.output.are_all_disconnected

查询全部输出是否已完成断开。
- 返回：1 表示全部断开；0 表示仍有连接或正在执行断开状态机。

### rb.output.request_disconnect_all

请求全部输出按各自结束命令和状态机安全断开。
- 返回：RB_OK 表示请求已提交；调用方应继续轮询 rb.output.are_all_disconnected。

### rb.dynamic.read_profile

按配置名读取 6DOF 动态配置。
- 参数 `profileName`：UTF-8 配置名，例如 "_Default"。
- 参数 `profile`：接收完整配置的结构体指针。

### rb.dynamic.read_profile_by_index

按动态配置索引读取 6DOF 动态配置。

### rb.dynamic.save_profile_by_index

按动态配置索引保存 6DOF 动态配置。

### rb.dynamic.save_profile

按配置名保存 6DOF 动态配置。

### rb.dynamic.export_package

导出指定动态 profile 的 DynamicV2 与同名 MotionEffect 配置包。

### rb.dynamic.import_package

导入动态配置包；profile 不存在时自动创建，存在时完整覆盖。

### rb.dynamic.apply_profile_to_current_rig

将动态配置应用到当前运行中的平台模型。

### rb.dynamic.get_profile_count

返回动态滤波配置数量。
- 返回：返回大于等于 0 的配置数量；初始化失败时返回 0。
- 说明：有效配置索引范围为 [0, 返回值)。

### rb.dynamic.get_profile_name

按索引读取动态滤波配置显示名称。
- 参数 `profileIndex`：动态配置零基索引。
- 参数 `buffer`：接收 UTF-8、NUL 结尾名称的缓冲区。
- 参数 `bufferSize`：buffer 的总字节数。
- 返回：RB_OK 表示成功；RB_INVALID_ARGUMENT 表示索引或缓冲区无效。

### rb.dynamic.get_input_candidate_catalog

读取指定目标 DOF 可以添加的输入通道效果目录。
- 参数 `dofIndex`：目标 DOF：0=Sway、1=Surge、2=Heave、3=Yaw、4=Roll、5=Pitch、6=SwayToRoll、7=SurgeToPitch。
- 参数 `catalog`：接收候选目录；调用前零初始化。
- 返回：RB_OK 表示成功；RB_INVALID_ARGUMENT 表示索引或指针无效；RB_ERROR 表示目录读取失败。
- 说明：前端显示 Items[].Key，使用目录候选，并通过 Key 保持同一 DOF 中的输入唯一。

### rb.dynamic.read_current_game_profile_index

返回当前游戏绑定的动态配置索引。
- 返回：有效零基索引；无当前游戏、无绑定或读取失败时返回负数。

### rb.dynamic.save_current_game_profile_index

把指定动态配置索引绑定到当前游戏。
- 参数 `profileIndex`：动态配置零基索引。
- 返回：RB_OK 表示保存成功；其他值表示无当前游戏、索引无效或写入失败。

### rb.dynamic.delete_profile_by_index

删除指定动态配置及同名运动增强、震动配置。
- 参数 `profileIndex`：动态配置零基索引。
- 返回：RB_OK 表示删除成功；其他值表示索引无效或删除失败。
- 说明：删除后配置索引会变化，前端必须刷新配置目录。

### rb.motioneffect.get_catalog

一次读取运动增强效果目录。
- 参数 `catalog`：接收效果名称、说明和输出说明；调用前零初始化。
- 返回：RB_OK 表示成功；RB_INVALID_ARGUMENT 表示 catalog 为空。

### rb.motioneffect.get_profile_count

返回运动增强配置数量。
- 返回：大于等于 0 的配置数量；失败返回负数。
- 说明：运动增强拥有独立配置目录；索引只能与 rb.motioneffect.get_profile_name 配套使用，不能复用动态滤波索引。

### rb.motioneffect.get_profile_name

按索引读取运动增强配置名。
- 参数 `profileIndex`：配置零基索引。
- 参数 `buffer`：接收 UTF-8、NUL 结尾名称的缓冲区。
- 参数 `bufferSize`：buffer 总字节数。
- 返回：RB_OK、RB_BUFFER_TOO_SMALL 或 RB_INVALID_ARGUMENT。

### rb.motioneffect.delete_profile

删除指定名称的运动增强配置。
- 参数 `profileName`：UTF-8 配置名。
- 返回：RB_OK 表示删除成功；其他值表示名称无效或删除失败。
- 说明：本函数只删除运动增强配置；动态滤波和震动配置不受影响。

### rb.motioneffect.read_profile

按配置名读取运动增强配置。

### rb.motioneffect.read_profile_by_index

按配置索引读取运动增强配置。

### rb.motioneffect.save_profile

按配置名保存运动增强配置。

### rb.motioneffect.save_profile_by_index

按配置索引保存运动增强配置。

### rb.motioneffect.apply_profile_to_current_rig

将运动增强配置应用到当前运行中的平台模型。

### rb.motioneffect.get_default_config

读取指定运动增强效果的默认参数。

### rb.haptic.get_catalog

一次读取震动效果目录。
- 参数 `catalog`：接收目录的结构体指针；调用前零初始化。
- 返回：RB_OK 表示成功；RB_INVALID_ARGUMENT 表示 catalog 为空。

### rb.haptic.read_profile

按配置名读取震动效果配置。

### rb.haptic.read_profile_by_index

按动态配置索引读取同名震动配置。
- 参数 `profileIndex`：动态配置零基索引。
- 参数 `profile`：接收完整震动配置的结构体指针。
- 返回：RB_OK 表示成功；其他值表示索引无效或读取失败。
- 说明：缺少同名配置时读取默认配置。

### rb.haptic.save_profile

按配置名保存震动效果配置。

### rb.haptic.save_profile_by_index

按动态配置索引保存同名震动配置。
- 参数 `profileIndex`：动态配置零基索引。
- 参数 `profile`：要保存的完整震动配置。
- 返回：RB_OK 表示保存成功；其他值表示参数无效或写入失败。

### rb.haptic.apply_profile_to_current_function

将震动配置应用到当前运行态。
- 参数 `profile`：要应用的完整震动配置。
- 返回：RB_OK 表示应用成功；Haptic 不依赖 SDK 主授权。
- 说明：Apply 不写配置文件。

### rb.haptic.get_default_config

读取指定震动效果的默认参数。
- 参数 `effectIndex`：rb.haptic.get_catalog 返回的效果索引。
- 参数 `config`：接收默认参数的结构体指针。
- 返回：RB_OK 表示成功；RB_INVALID_ARGUMENT 表示索引或指针无效。

### rb.wind.get_default_config

读取默认风感配置。

### rb.wind.read_config

读取当前风感配置。

### rb.wind.save_config

保存风感配置。

### rb.wind.apply_config_to_current_function

将风感配置应用到当前运行中的风感功能；Wind 不依赖 SDK 主授权。

### rb.wind.set_test_output

启用或关闭风感测试输出。
- 参数 `enabled`：1=测试，0=恢复当前运行配置。
- 参数 `outputPercent`：四通道统一测试输出，范围 0-100%。
- 返回：RB_OK 表示测试状态已更新；Wind 不依赖 SDK 主授权。

### rb.seatbelt.get_default_config

读取默认安全带配置。

### rb.seatbelt.read_config

读取当前安全带配置。

### rb.seatbelt.save_config

保存安全带配置。

### rb.seatbelt.apply_config_to_current_function

将安全带配置应用到当前运行中的安全带功能；安全带产品不依赖 SDK 主授权。

### rb.seatbelt.set_test_output

启用或关闭左右安全带测试输出；安全带产品不依赖 SDK 主授权。
- 参数 `enabled`：1=测试，0=恢复当前运行配置。
- 参数 `leftPercent`：左通道测试输出，范围 0-100%。
- 参数 `rightPercent`：右通道测试输出，范围 0-100%。
- 返回：RB_OK 表示测试状态已更新；其他值表示运行态不可用。

### rb.rgb.refresh_devices

刷新 RGBAPP WinUSB 设备目录并返回设备数量。

### rb.rgb.get_device_count

返回最近一次刷新后的 RGBAPP 设备数量。

### rb.rgb.get_device_info

读取设备目录项；index 为零基目录索引。

### rb.rgb.connect

按稳定 MCU UID 连接 RGBAPP；RGB 产品不依赖 SDK 主授权。

### rb.rgb.disconnect

熄灭并断开当前 RGBAPP。

### rb.rgb.get_device_config_snapshot

读取连接或应用配置时更新的内存快照，不访问 USB；适合 UI 页面刷新。

### rb.rgb.read_device_config

读取设备当前四路灯带与功率保护配置。

### rb.rgb.apply_device_config

将灯带配置应用到设备运行态，不写 Flash；RGB 产品不依赖 SDK 主授权。

### rb.rgb.save_device_config

将设备当前灯带配置明确保存到 Flash。

### rb.rgb.get_effect_input_count

返回指定效果可选择的语义输入数量；静态色返回 0。

### rb.rgb.get_effect_input

按效果类型和候选索引读取语义输入；RPM和换挡效果返回固定输入。

### rb.rgb.get_default_effect_profile

读取默认遥测 RGB 效果配置。

### rb.rgb.read_effect_profile

从 ExtraEffectsV2.json 读取 RGB 效果配置。

### rb.rgb.save_effect_profile

明确保存 RGB 效果配置；不会自动连接设备。

### rb.rgb.apply_effect_profile

将 RGB 效果配置应用到当前运行态；Apply 不读取或写入配置文件，且不依赖 SDK 主授权。

### rb.rgb.get_state

读取内存中的 RGB 运行状态，不访问配置文件或设备。

### rb.rgb.set_test_pattern

设置固件测试图案和 RGB888 颜色；RGB 产品的诊断和正常输出均不依赖 SDK 主授权。

### rb.rgb.blackout

立即请求设备熄灭。

### rb.rgb.read_diagnostics

主动向设备读取一次诊断快照。

### rb.rgb.start_app_firmware_update

按稳定 UID 异步更新 RGBAPP 的 APP 固件。
- 参数 `uid`：24 个十六进制字符的 MCU UID；设备可处于 APP 或 IAP 模式。
- 参数 `binFilePath`：RGBAPP APP `.bin` 文件路径。SDK 在擦除前校验大小、向量地址和 CRC。
- 返回：RB_OK 表示后台任务已启动；RB_INVALID_ARGUMENT 表示参数无效；RB_ERROR 表示已有任务或运行态不可用。

### rb.rgb.start_iap_firmware_update

按稳定 UID 异步更新 RGBAPP 的 Stage-1/IAP 固件。
- 参数 `uid`：24 个十六进制字符的 MCU UID；目标必须当前处于 APP 模式。
- 参数 `binFilePath`：RGBAPP_IAP `.bin` 文件路径。SDK 会校验 Stage-1 向量、清单、大小和 CRC。
- 返回：RB_OK 表示后台任务已启动；RB_INVALID_ARGUMENT 表示参数或镜像无效；RB_ERROR 表示设备或运行态不可用。

### rb.rgb.get_firmware_update_state

读取 RGBAPP APP 固件更新内存状态。

### rb.rgb.is_firmware_update_running

返回 RGBAPP APP 固件更新线程当前是否运行。

### rb.command.run

执行统一动作命令，并可返回命令结果 JSON。
- 参数 `command`：UTF-8、NUL 结尾的命令 key；支持范围见 RaceBearSDK_API.md 的“Command”章节。
- 参数 `argsJson`：UTF-8、NUL 结尾的参数对象；无参数命令应传 "{}"。
- 参数 `resultBuffer`：可选的结果缓冲区；不需要结果时可传 nullptr。
- 参数 `resultBufferSize`：resultBuffer 的总容量，单位为字节；传入缓冲区时必须大于 0。
- 返回：RB_OK 表示命令执行成功；RB_INVALID_ARGUMENT 表示命令或参数无效/不支持；RB_BUFFER_TOO_SMALL 表示结果缓冲区不足；RB_ERROR 表示命令执行失败。

### rb.log.read

从 SDK 运行日志队列中弹出一条日志文本。
- 参数 `buffer`：接收 UTF-8、NUL 结尾日志的调用方缓冲区；队列为空时写入空字符串。
- 参数 `bufferSize`：buffer 的总容量，单位为字节且必须大于 0。
- 说明：容量不足时保留队首及最近日志类型，写入空字符串；调用方应扩容后重试。
无效参数不消耗日志。日志队列应由单一消费者读取，成功后再取对应类型。
- 返回：RB_OK 表示读取完成；RB_INVALID_ARGUMENT 表示缓冲区无效；RB_BUFFER_TOO_SMALL 表示容量不足；RB_ERROR 表示内部失败。

### rb.vocore.get_device_count

枚举当前可访问的 VOCORE USB 屏幕数量。
- 返回：大于或等于 0 表示设备数量；libusb 运行库不可用或枚举失败时返回 RB_ERROR。
- 说明：SDK 目录必须同时存在 VS2019 x64 版 libusb-1.0.dll，设备需安装 VOCORE 官方 LibUSBK 驱动。

### rb.vocore.get_device_info

按当前枚举索引读取 VOCORE 屏幕信息。
- 参数 `index`：设备索引，范围为 0 到 rb.vocore.get_device_count-1。
- 参数 `info`：调用方结构体；调用前必须将 Size 设为 sizeof(RB_VoCoreDeviceInfo)。
- 返回：RB_OK 表示成功；RB_INVALID_ARGUMENT 表示参数或索引无效；RB_ERROR 表示设备无法访问。

### rb.vocore.open

打开一块 VOCORE 屏幕并启动独立发送线程。
- 参数 `deviceId`：RB_VoCoreDeviceInfo.DeviceId；传 nullptr 或空字符串时打开第一块可用屏幕。
- 参数 `autoReconnect`：1=拔插或 USB 错误后每 1000ms 自动重连，0=连接丢失后保持断开。
- 参数 `handle`：接收新句柄的指针；成功后由调用方负责调用 rb.vocore.close。
- 返回：RB_OK 表示线程已启动；RB_INVALID_ARGUMENT 表示 handle 为空；RB_ERROR 表示设备打开失败且未启用自动重连。

### rb.vocore.close

停止发送线程、释放 USB 接口并销毁 VOCORE 句柄。
- 参数 `handle`：rb.vocore.open 返回的句柄；nullptr 会被忽略。
- 说明：调用后句柄立即失效；不得与同一句柄的其他接口并发调用。

### rb.vocore.submit_frame

异步提交一帧完整画面，尚未发送的旧帧会被最新帧覆盖。
- 参数 `handle`：已打开的 VOCORE 句柄。
- 参数 `pixels`：指向输入像素首字节，函数返回前会完成复制，调用方随后可复用该内存。
- 参数 `dataSize`：pixels 可读总字节数，至少为 stride*height。
- 参数 `width`：输入画面宽度。旋转 0/180 时等于屏幕宽度，旋转 90/270 时等于屏幕高度。
- 参数 `height`：输入画面高度。旋转 0/180 时等于屏幕高度，旋转 90/270 时等于屏幕宽度。
- 参数 `stride`：每行字节数，允许包含行尾填充。
- 参数 `pixelFormat`：RB_VoCorePixelFormat 中的值。
- 参数 `rotation`：顺时针旋转角度，只允许 0、90、180、270。
- 返回：RB_OK 表示画面已进入最新帧队列；RB_INVALID_ARGUMENT 表示尺寸、格式或句柄无效；RB_LICENSE_REQUIRED 表示授权输出已锁定。

### rb.vocore.set_brightness

设置 VOCORE 背光亮度。
- 参数 `handle`：已打开的 VOCORE 句柄。
- 参数 `brightness`：亮度，范围 0-255；命令由发送线程异步执行。
- 返回：RB_OK 表示设置已排队；RB_INVALID_ARGUMENT 表示句柄或亮度无效。

### rb.vocore.set_auto_reconnect

运行时启用或关闭设备自动重连。
- 参数 `handle`：已打开的 VOCORE 句柄。
- 参数 `enabled`：1=启用，0=关闭。
- 返回：RB_OK 表示设置完成；RB_INVALID_ARGUMENT 表示句柄无效。

### rb.vocore.get_state

读取 VOCORE 连接状态、实际 FPS 和帧统计。
- 参数 `handle`：已打开的 VOCORE 句柄。
- 参数 `state`：调用方结构体；调用前必须将 Size 设为 sizeof(RB_VoCoreState)。
- 返回：RB_OK 表示成功；RB_INVALID_ARGUMENT 表示句柄、结构体或 Size 无效。

### rb.dashboardweb.open

启动浏览器仪表 HTTP 服务。

### rb.dashboardweb.close

停止浏览器仪表服务并释放所有客户端连接。

### rb.dashboardweb.submit_frame

异步提交一帧 BGRA8888 画面；尚未编码的旧帧会被覆盖。

### rb.dashboardweb.get_state

读取 Web 服务、客户端和编码统计。

### rb.dashboard.render_frame

使用SDK内置模板生成一帧BGRA8888仪表画面。
- 参数 `state`：rb.state.read读取到的当前运行态。
- 参数 `pixels`：接收画面的调用方缓冲区，容量至少为stride*height字节。
- 参数 `width`：输出画面宽度；竖屏通常为480，横屏通常为854。
- 参数 `height`：输出画面高度；竖屏通常为854，横屏通常为480。
- 参数 `stride`：每行字节数，不能小于width*4。
- 参数 `templateIndex`：RB_DashboardTemplate中的模板值。
- 返回：RB_OK表示生成成功；RB_INVALID_ARGUMENT表示参数、尺寸或模板无效；RB_ERROR表示GDI资源创建失败。
- 说明：本函数只生成画面，不执行USB发送；生成结果可直接交给rb.vocore.submit_frame。

### rb.dashboard.refresh_packages

重新扫描内置和用户 .rbdash 模板；返回模板数量或负数错误码。

### rb.dashboard.get_package_count

返回最近一次扫描得到的客户仪表模板数量。

### rb.dashboard.get_package_info

按目录索引读取客户仪表模板信息。

### rb.dashboard.open_package

按稳定模板 ID 打开客户仪表并创建缓存渲染句柄。

### rb.dashboard.close_package

关闭客户仪表句柄并释放图片、字体和绑定缓存。

### rb.dashboard.set_package_page

设置客户仪表当前页面。

### rb.dashboard.render_package_frame

使用客户仪表模板生成一帧 BGRA8888 画面。

### rb.dashboard.get_package_state

读取客户仪表句柄状态和渲染统计。

### rb.dashboard.import_package

将外部 .rbdash 文件导入当前 AppDataName 的用户仪表目录。

### rb.dashboard.delete_package

删除当前 AppDataName 用户目录中的指定模板；内置模板不能删除。

### rb.dashboard.get_user_package_directory

返回当前 AppDataName 的用户仪表目录 UTF-8 路径。

### rb.serial.get_available_port_count

枚举当前系统可用串口的数量。
- 返回：成功时返回大于或等于 0 的端口数量；内部失败时返回 RB_ERROR。

### rb.serial.get_available_port_info

按枚举索引读取一个串口的完整设备信息。
- 参数 `index`：端口索引，范围为 0 到 rb.serial.get_available_port_count-1。
- 参数 `info`：接收信息的结构体指针；SDK 会先清零整个结构体。
- 返回：RB_OK 表示成功；RB_INVALID_ARGUMENT 表示 info 为空；RB_ERROR 表示索引无效或枚举失败。

### rb.serial.get_version

获取 SDK 内部串口库版本文本。
- 返回：串口库持有的 NUL 结尾字符串；调用方不得修改或释放。

### rb.serial.create

创建一个独立串口对象。
- 返回：成功时返回有效 RB_SerialHandle；内存分配或内部初始化失败时返回 nullptr。
- 说明：每个成功创建的句柄最终都必须交给 rb.serial.destroy。

### rb.serial.destroy

断开事件、关闭端口并销毁串口对象。
- 参数 `handle`：rb.serial.create 返回的句柄；nullptr 会被忽略。
- 返回：无返回值。
- 说明：调用后句柄立即失效，不得再次使用；销毁时不得有并发读写或回调注册操作。

### rb.serial.init

使用 8 数据位、无校验、1 停止位、无流控和 4096 字节读缓冲初始化串口参数。
- 参数 `handle`：有效串口句柄。
- 参数 `portName`：UTF-8、NUL 结尾的端口名，例如 "COM3"。
- 参数 `baudRate`：波特率，例如 115200，必须大于 0。
- 返回：RB_OK 表示参数已设置；RB_INVALID_ARGUMENT 表示句柄或端口名无效；RB_ERROR 表示内部失败。
- 说明：本函数只配置对象，不会打开串口；随后调用 rb.serial.open。

### rb.serial.init_ex

使用完整通信参数初始化串口对象。
- 参数 `handle`：有效串口句柄。
- 参数 `portName`：UTF-8、NUL 结尾的端口名，例如 "COM3"。
- 参数 `baudRate`：波特率，必须大于 0。
- 参数 `parity`：RB_SerialParity 中的值。
- 参数 `dataBits`：数据位数，通常为 5、6、7 或 8。
- 参数 `stopBits`：RB_SerialStopBits 中的值。
- 参数 `flowControl`：RB_SerialFlowControl 中的值。
- 参数 `readBufferSize`：异步模式内部读缓冲容量，单位为字节且必须大于 0。
- 返回：RB_OK 表示参数已设置；RB_INVALID_ARGUMENT 表示句柄或端口名无效；RB_ERROR 表示参数设置失败。
- 说明：本函数只配置对象，不会打开串口；随后调用 rb.serial.open。

### rb.serial.open

按已初始化的参数打开串口。
- 参数 `handle`：已通过 rb.serial.init 或 rb.serial.init_ex 配置的有效句柄。
- 返回：RB_OK 表示打开成功；RB_INVALID_ARGUMENT 表示句柄无效；RB_ERROR 表示设备打开失败。

### rb.serial.close

关闭串口连接但保留对象及其配置。
- 参数 `handle`：有效串口句柄；nullptr 会被忽略。
- 返回：无返回值。

### rb.serial.is_open

查询串口当前是否已打开。
- 参数 `handle`：有效串口句柄。
- 返回：1 表示已打开，0 表示未打开、句柄无效或内部失败。

### rb.serial.write

向串口写入二进制数据。
- 参数 `handle`：已打开的有效串口句柄。
- 参数 `data`：待发送数据首地址。
- 参数 `size`：请求写入的字节数，必须大于 0。
- 返回：大于或等于 0 表示实际写入字节数；负数表示参数、连接或写入失败，可调用 rb.serial.get_last_error 查询原因。

### rb.serial.read

按当前超时配置读取最多 size 字节；异步模式下从内部队列读取并等待所需数据。
- 参数 `handle`：已打开的有效串口句柄。
- 参数 `data`：接收数据的缓冲区。
- 参数 `size`：缓冲区容量及请求字节数，必须大于 0。
- 返回：大于或等于 0 表示实际读取字节数；负数表示参数、连接或读取失败。

### rb.serial.read_some

非强制满量读取当前可用数据，最多读取 size 字节。
- 参数 `handle`：已打开的有效串口句柄。
- 参数 `data`：接收数据的缓冲区。
- 参数 `size`：缓冲区容量，必须大于 0。
- 返回：正数表示读取字节数，0 表示当前无数据，负数表示参数、连接或读取失败。

### rb.serial.read_all

读取当前已经可用的全部数据，但最多写入 size 字节。
- 参数 `handle`：已打开的有效串口句柄。
- 参数 `data`：接收数据的缓冲区。
- 参数 `size`：缓冲区容量，必须大于 0。
- 返回：正数表示读取字节数，0 表示当前无数据，负数表示参数、连接或读取失败。

### rb.serial.read_line

读取到指定分隔符或缓冲区上限，并在结果末尾写入 NUL；分隔符不写入结果。
- 参数 `handle`：已打开的有效串口句柄。
- 参数 `data`：接收文本的缓冲区。
- 参数 `size`：缓冲区总容量，单位为字节且必须大于 0，其中包含末尾 NUL。
- 参数 `delimiter`：行结束分隔符，例如 '\n'。
- 返回：大于或等于 0 表示结果文本字节数，不包含 NUL；负数表示参数、连接或读取失败。

### rb.serial.get_read_buffer_used_len

获取当前可立即读取的字节数；异步模式下返回内部队列长度。
- 参数 `handle`：已打开的有效串口句柄。
- 返回：当前可读字节数；0 可能表示暂无数据，也可能表示句柄无效、端口未打开或查询失败。

### rb.serial.connect_read_event

注册串口可读事件回调。
- 参数 `handle`：有效串口句柄。
- 参数 `callback`：非空回调函数；回调可能运行在 SDK 工作线程中。
- 参数 `userData`：调用方上下文指针，SDK 不解析并在回调时原样传回，可为 nullptr。
- 返回：RB_OK 表示注册成功；RB_INVALID_ARGUMENT 表示句柄或回调为空；RB_ERROR 表示注册失败。
- 说明：userData 指向的对象必须至少存活到断开回调或销毁串口对象。

### rb.serial.disconnect_read_event

断开串口可读事件回调并释放内部适配器。
- 参数 `handle`：有效串口句柄。
- 返回：RB_OK 表示断开成功；RB_INVALID_ARGUMENT 表示句柄无效；RB_ERROR 表示断开失败。

### rb.serial.connect_hot_plug_event

注册系统串口热插拔事件回调。
- 参数 `handle`：有效串口句柄；热插拔监听与该对象生命周期绑定。
- 参数 `callback`：非空回调函数；回调可能运行在 SDK 工作线程中。
- 参数 `userData`：调用方上下文指针，SDK 不解析并在回调时原样传回，可为 nullptr。
- 返回：RB_OK 表示注册成功；RB_INVALID_ARGUMENT 表示句柄或回调为空；RB_ERROR 表示注册失败。

### rb.serial.disconnect_hot_plug_event

断开系统串口热插拔事件回调并释放内部适配器。
- 参数 `handle`：有效串口句柄。
- 返回：RB_OK 表示断开成功；RB_INVALID_ARGUMENT 表示句柄无效；RB_ERROR 表示断开失败。

### rb.serial.set_operate_mode

设置串口同步或异步操作模式。
- 参数 `handle`：有效串口句柄。
- 参数 `operateMode`：RB_SerialOperateMode 中的值。
- 返回：RB_OK 表示设置成功；RB_INVALID_ARGUMENT 表示句柄无效；RB_ERROR 表示内部失败。

### rb.serial.set_auto_reconnect_enabled

启用或关闭串口断线自动重连。
- 参数 `handle`：有效串口句柄。
- 参数 `enabled`：0 表示关闭，非 0 表示启用。
- 返回：RB_OK 表示设置成功；RB_INVALID_ARGUMENT 表示句柄无效；RB_ERROR 表示内部失败。

### rb.serial.is_auto_reconnect_enabled

查询串口自动重连是否启用。
- 参数 `handle`：有效串口句柄。
- 返回：1 表示启用，0 表示关闭、句柄无效或内部失败。

### rb.serial.set_auto_reconnect_interval

设置自动重连尝试间隔。
- 参数 `handle`：有效串口句柄。
- 参数 `milliseconds`：重连间隔，单位毫秒。
- 返回：RB_OK 表示设置成功；RB_INVALID_ARGUMENT 表示句柄无效；RB_ERROR 表示内部失败。

### rb.serial.get_auto_reconnect_interval

获取自动重连尝试间隔。
- 参数 `handle`：有效串口句柄。
- 返回：重连间隔，单位毫秒；句柄无效或内部失败时返回 0。

### rb.serial.try_reconnect_now

不等待下一次自动重连周期，立即执行一次重连尝试。
- 参数 `handle`：有效串口句柄。
- 返回：RB_OK 表示重连成功；RB_INVALID_ARGUMENT 表示句柄无效；RB_ERROR 表示重连失败。

### rb.serial.set_timeout_config

设置串口读写超时参数。
- 参数 `handle`：有效串口句柄。
- 参数 `config`：非空超时配置指针，各字段语义与 Windows COMMTIMEOUTS 对应。
- 返回：RB_OK 表示设置成功；RB_INVALID_ARGUMENT 表示句柄或 config 为空；RB_ERROR 表示内部失败。

### rb.serial.get_timeout_config

读取串口当前读写超时参数。
- 参数 `handle`：有效串口句柄。
- 参数 `config`：接收超时配置的非空结构体指针。
- 返回：RB_OK 表示读取成功；RB_INVALID_ARGUMENT 表示句柄或 config 为空；RB_ERROR 表示内部失败。

### rb.serial.set_dtr

设置串口 DTR 控制线状态。
- 参数 `handle`：有效串口句柄。
- 参数 `set`：0 表示清除 DTR，非 0 表示置位 DTR。
- 返回：RB_OK 表示操作已提交；RB_INVALID_ARGUMENT 表示句柄无效；RB_ERROR 表示内部失败。

### rb.serial.set_rts

设置串口 RTS 控制线状态。
- 参数 `handle`：有效串口句柄。
- 参数 `set`：0 表示清除 RTS，非 0 表示置位 RTS。
- 返回：RB_OK 表示操作已提交；RB_INVALID_ARGUMENT 表示句柄无效；RB_ERROR 表示内部失败。

### rb.serial.get_last_error

获取该串口对象最近一次底层操作的错误码。
- 参数 `handle`：有效串口句柄。
- 返回：有效句柄时返回串口库错误码，0 通常表示无错误；句柄无效返回 RB_INVALID_ARGUMENT，内部异常返回 RB_ERROR。
- 说明：返回值属于串口库错误码域，不应按 RB_Result 解释。

### rb.serial.get_last_error_msg

获取该串口对象最近一次底层操作的错误说明。
- 参数 `handle`：有效串口句柄。
- 参数 `buffer`：接收 UTF-8、NUL 结尾错误文本的调用方缓冲区。
- 参数 `bufferSize`：buffer 的总容量，单位为字节且必须大于 0。
- 返回：RB_OK 表示读取成功；RB_INVALID_ARGUMENT 表示句柄或缓冲区无效；RB_BUFFER_TOO_SMALL 表示容量不足；RB_ERROR 表示内部失败。
