#pragma once

// 这些示例展示完整的“读取-修改-应用-保存”流程。main.cpp 不会自动调用，
// 避免示例启动后覆盖用户配置或驱动真实硬件。
int ConfigureDynamicInputExample(int profileIndex, int dofIndex, int candidateIndex);
int ConfigureSerialOutputExample(int instanceIndex, const char* portName);
int ConfigureAdditionalEffectsExample(int motionProfileIndex, int dynamicProfileIndex);
int ConfigureGameTelemetryExample(const char* gameKey, int catalogIndex, int sourcePort);
int InspectPluginsAndManualPoseExample();
