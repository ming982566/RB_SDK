#include "ExampleServices.h"
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>

// 示例中的每步都检查业务结果，失败后不自动重放保存或导入。
static void Require(int code, const char* operation)
{
    if (code != RB_OK) throw std::runtime_error(std::string(operation) + ": business=" + std::to_string(code));
}

int main(int argc, char** argv) try
{
    if (argc != 3) {
        std::puts("RaceBearProfilePackages --rgb-roundtrip|--motion-roundtrip|--haptic-roundtrip <package.json>");
        return argc == 1 || (argc == 2 && std::strcmp(argv[1], "--help") == 0) ? 0 : 2;
    }
    Require(rb_example::Runtime_SetAppDataName("RaceBearProfileDemo"), "app identity");
    char name[128]{};
    const char* demo = "ExampleProfile";
    if (std::strcmp(argv[1], "--rgb-roundtrip") == 0) {
        RB_RgbEffectProfile profile{};
        Require(rb_example::Rgb_GetDefaultEffectProfile(&profile), "defaults");
        Require(rb_example::Rgb_SaveEffectProfileAs(demo, &profile), "save as");
        const int count = rb_example::Rgb_GetEffectProfileCount();
        if (count < 0) Require(count, "count");
        for (int i = 0; i < count; ++i) {
            Require(rb_example::Rgb_GetEffectProfileName(i, name, sizeof name), "name");
            std::printf("profile[%d]=%s\n", i, name);
        }
        Require(rb_example::Rgb_SaveCurrentEffectProfileName(demo), "select saved config");
        Require(rb_example::Rgb_ReadCurrentEffectProfileName(name, sizeof name), "selection");
        Require(rb_example::Rgb_ExportEffectPackage(demo, argv[2]), "export");
        Require(rb_example::Rgb_ImportEffectPackage(argv[2], name, sizeof name), "import");
        Require(rb_example::Rgb_ReadEffectProfileByName(name, &profile), "read back");
        std::printf("RGB: %s, %d layers; saved only, no hardware or runtime apply.\n", name, profile.LayerCount);
    } else if (std::strcmp(argv[1], "--motion-roundtrip") == 0) {
        RB_MotionEffectProfile profile{};
        for (int i = 0; i < RB_MOTION_EFFECT_COUNT; ++i)
            Require(rb_example::MotionEffect_GetDefaultConfig(i, &profile.Effects[i]), "motion default");
        Require(rb_example::MotionEffect_SaveProfile(demo, &profile), "save motion");
        Require(rb_example::MotionEffect_ExportPackage(demo, argv[2]), "export motion");
        Require(rb_example::MotionEffect_ImportPackage(argv[2], name, sizeof name), "import motion");
        Require(rb_example::MotionEffect_ReadProfile(name, &profile), "read motion");
    } else if (std::strcmp(argv[1], "--haptic-roundtrip") == 0) {
        RB_DynamicV2Profile dynamic{};
        Require(rb_example::Dynamic_SaveProfile(demo, &dynamic), "create parent Dynamic");
        RB_HapticEffectProfile profile{};
        for (int i = 0; i < RB_HAPTIC_EFFECT_COUNT; ++i)
            Require(rb_example::Haptic_GetDefaultConfig(i, &profile.Effects[i]), "haptic default");
        Require(rb_example::Haptic_SaveProfile(demo, &profile), "save haptic");
        Require(rb_example::Haptic_ExportPackage(demo, argv[2]), "export haptic");
        Require(rb_example::Haptic_ImportPackage(argv[2], name, sizeof name), "import haptic");
        Require(rb_example::Haptic_ReadProfile(name, &profile), "read haptic");
    } else return 2;
    return 0;
}
catch (const std::exception& error) {
    std::fprintf(stderr, "%s\n", error.what());
    return 1;
}
