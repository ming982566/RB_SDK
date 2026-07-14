#pragma once

// These examples show complete read-modify-apply-save flows. They are not
// called by main.cpp because running them would overwrite the user's config.
int ConfigureDynamicInputExample(int profileIndex, int dofIndex, const char* telemetryKey);
int ConfigureSerialOutputExample(int instanceIndex, const char* portName);
int ConfigureAdditionalEffectsExample(int profileIndex);
int ConfigureGameTelemetryExample(const char* gameKey, int catalogIndex, int sourcePort);
int InspectPluginsAndManualPoseExample();
