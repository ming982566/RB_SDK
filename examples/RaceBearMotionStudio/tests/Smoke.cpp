#include "SdkRuntime.h"
#include "Ui.h"
#include <iostream>

int wmain() {
  SdkRuntime sdk;
  std::wstring error;
  if (!sdk.Start(error)) {
    std::wcerr << L"START FAIL: " << error << L'\n';
    return 1;
  }
  RB_RuntimeState state{};
  if (!sdk.ReadState(state, error)) {
    std::wcerr << L"STATE FAIL: " << error << L'\n';
    return 2;
  }
  if (!RB_Runtime_IsLoopRunning() || RB_Runtime_GetLoopIntervalMs() != 2) {
    std::wcerr << L"LOOP FAIL\n";
    return 3;
  }
  RB_GameCatalog games{};
  if (RB_Game_GetCatalog(&games) != RB_OK) {
    std::wcerr << L"GAME CATALOG FAIL\n";
    return 4;
  }
  RB_TelemetryCatalog telemetry{};
  if (RB_Telemetry_GetCatalog(&telemetry) != RB_OK) {
    std::wcerr << L"TELEMETRY CATALOG FAIL\n";
    return 5;
  }
  RB_OutputCatalog outputs{};
  if (RB_Output_GetCatalog(&outputs) != RB_OK) {
    std::wcerr << L"OUTPUT CATALOG FAIL\n";
    return 6;
  }
  if (RB_Platform_GetCount() <= 0) {
    std::wcerr << L"PLATFORM CATALOG FAIL\n";
    return 7;
  }
  RB_PlatformConfig platform{};
  if (RB_Platform_ReadConfigByIndex(0, &platform) != RB_OK) {
    std::wcerr << L"PLATFORM CONFIG FAIL\n";
    return 8;
  }
  if (RB_Dynamic_GetProfileCount() <= 0) {
    std::wcerr << L"DYNAMIC CATALOG FAIL\n";
    return 9;
  }
  RB_DynamicV2Profile dynamic{};
  if (RB_Dynamic_ReadProfileByIndex(0, &dynamic) != RB_OK) {
    std::wcerr << L"DYNAMIC CONFIG FAIL\n";
    return 10;
  }
  RB_EffectCatalog motionCatalog{};
  if (RB_MotionEffect_GetCatalog(&motionCatalog) != RB_OK) {
    std::wcerr << L"MOTION CATALOG FAIL\n";
    return 11;
  }
  RB_MotionEffectProfile motion{};
  if (RB_MotionEffect_ReadProfileByIndex(0, &motion) != RB_OK) {
    std::wcerr << L"MOTION CONFIG FAIL\n";
    return 12;
  }
  RB_EffectCatalog hapticCatalog{};
  if (RB_Haptic_GetCatalog(&hapticCatalog) != RB_OK) {
    std::wcerr << L"HAPTIC CATALOG FAIL\n";
    return 13;
  }
  RB_HapticEffectProfile haptic{};
  if (RB_Haptic_ReadProfileByIndex(0, &haptic) != RB_OK) {
    std::wcerr << L"HAPTIC CONFIG FAIL\n";
    return 14;
  }
  RB_WindEffectConfig wind{};
  if (RB_Wind_ReadConfig(&wind) != RB_OK) {
    std::wcerr << L"WIND CONFIG FAIL\n";
    return 15;
  }
  RB_SeatbeltEffectConfig seatbelt{};
  if (RB_Seatbelt_ReadConfig(&seatbelt) != RB_OK) {
    std::wcerr << L"SEATBELT CONFIG FAIL\n";
    return 16;
  }
  RB_PluginCatalog plugins{};
  if (RB_Plugin_GetCatalog(&plugins) != RB_OK) {
    std::wcerr << L"PLUGIN CATALOG FAIL\n";
    return 17;
  }
  if (RB_Serial_GetAvailablePortCount() < 0) {
    std::wcerr << L"SERIAL ENUM FAIL\n";
    return 18;
  }
  std::wcout << L"OK version=" << Utf8ToWide(RB_Runtime_GetVersion())
             << L" games=" << games.Count << L" telemetry=" << telemetry.Count
             << L" outputs=" << outputs.InstanceCount << L'\n';
  sdk.Stop();
  return 0;
}
