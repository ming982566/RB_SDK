#include "RaceBearSDK.h"
#include <cstdio>
#include <cstring>

namespace {
int failures = 0;

template <typename T, typename Read, typename Save>
void Check(const char *name, Read read, Save save) {
  T before{};
  int rc = read(&before);
  if (rc != RB_OK) {
    std::printf("SKIP %-18s read=%d\n", name, rc);
    return;
  }

  rc = save(&before);
  if (rc != RB_OK) {
    std::printf("FAIL %-18s save=%d\n", name, rc);
    ++failures;
    return;
  }

  T after{};
  rc = read(&after);
  if (rc != RB_OK) {
    std::printf("FAIL %-18s reread=%d\n", name, rc);
    ++failures;
    return;
  }

  if (std::memcmp(&before, &after, sizeof(T)) == 0) {
    std::printf("PASS %-18s exact round trip\n", name);
    return;
  }

  const auto *left = reinterpret_cast<const unsigned char *>(&before);
  const auto *right = reinterpret_cast<const unsigned char *>(&after);
  size_t first = 0;
  while (first < sizeof(T) && left[first] == right[first])
    ++first;
  std::printf("DIFF %-18s firstByte=%zu before=%u after=%u size=%zu\n",
              name, first, left[first], right[first], sizeof(T));
}
} // namespace

int main() {
  if (RB_Runtime_SetAppDataName("RaceBearMotionStudioSaveTest") != RB_OK ||
      RB_Runtime_Initialize() != RB_OK) {
    std::puts("SDK initialization failed");
    return 2;
  }

  RB_GameCatalog games{};
  if (RB_Game_GetCatalog(&games) == RB_OK && games.Count > 0) {
    const char *key = nullptr;
    RB_GameConfig probe{};
    for (int i = 0; i < games.Count; ++i) {
      if (RB_Game_ReadConfig(games.Items[i].GameKey, &probe) == RB_OK) {
        key = games.Items[i].GameKey;
        break;
      }
    }
    if (!key) {
      RB_GameConfig initial{};
      initial.Size = sizeof(initial);
      initial.Version = 1;
      initial.SourcePort = 32500;
      initial.ForwardingPort = 32501;
      initial.SerialBaudRate = 115200;
      strcpy_s(initial.SourceIP, "127.0.0.1");
      strcpy_s(initial.ForwardingIP, "127.0.0.1");
      if (RB_Game_SaveConfig("__roundtrip_test__", &initial) == RB_OK)
        key = "__roundtrip_test__";
    }
    if (key) {
      Check<RB_GameConfig>(
          "game",
          [key](RB_GameConfig *value) { return RB_Game_ReadConfig(key, value); },
          [key](const RB_GameConfig *value) { return RB_Game_SaveConfig(key, value); });
    } else {
      std::puts("FAIL game               cannot create test config");
      ++failures;
    }
  }

  if (RB_Platform_GetCount() > 0) {
    Check<RB_PlatformConfig>(
        "platform",
        [](RB_PlatformConfig *value) { return RB_Platform_ReadConfigByIndex(0, value); },
        [](const RB_PlatformConfig *value) { return RB_Platform_SaveConfigByIndex(0, value); });
  }

  RB_OutputCatalog outputs{};
  if (RB_Output_GetCatalog(&outputs) == RB_OK && outputs.InstanceCount > 0) {
    const int index = outputs.Instances[0].Index;
    Check<RB_OutputConfig>(
        "output",
        [index](RB_OutputConfig *value) { return RB_Output_ReadInstanceConfigByIndex(index, value); },
        [index](const RB_OutputConfig *value) { return RB_Output_SaveInstanceConfigByIndex(index, value); });
  }

  if (RB_Dynamic_GetProfileCount() > 0) {
    Check<RB_DynamicV2Profile>(
        "dynamic",
        [](RB_DynamicV2Profile *value) { return RB_Dynamic_ReadProfileByIndex(0, value); },
        [](const RB_DynamicV2Profile *value) { return RB_Dynamic_SaveProfileByIndex(0, value); });
    Check<RB_MotionEffectProfile>(
        "motion effect",
        [](RB_MotionEffectProfile *value) { return RB_MotionEffect_ReadProfileByIndex(0, value); },
        [](const RB_MotionEffectProfile *value) { return RB_MotionEffect_SaveProfileByIndex(0, value); });
    Check<RB_HapticEffectProfile>(
        "haptic",
        [](RB_HapticEffectProfile *value) { return RB_Haptic_ReadProfileByIndex(0, value); },
        [](const RB_HapticEffectProfile *value) { return RB_Haptic_SaveProfileByIndex(0, value); });
  }

  Check<RB_WindEffectConfig>("wind", RB_Wind_ReadConfig, RB_Wind_SaveConfig);
  Check<RB_SeatbeltEffectConfig>("seatbelt", RB_Seatbelt_ReadConfig,
                                 RB_Seatbelt_SaveConfig);
  Check<RB_SystemConfig>("system", RB_System_ReadConfigOrDefault,
                         RB_System_SaveConfig);

  RB_Runtime_Shutdown();
  return failures == 0 ? 0 : 1;
}
