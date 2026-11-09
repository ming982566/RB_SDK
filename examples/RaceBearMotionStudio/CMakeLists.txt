cmake_minimum_required(VERSION 3.15)
project(RaceBearMotionStudio LANGUAGES CXX)
include("../common/ExampleBuild.cmake")
add_executable(RaceBearMotionStudio WIN32
    src/main.cpp src/App.cpp src/SdkRuntime.cpp src/Ui.cpp
    src/pages/StatusPages.cpp src/pages/GamePage.cpp src/pages/PlatformPage.cpp
    src/pages/EffectsPages.cpp src/pages/HardwarePages.cpp src/pages/SettingsPages.cpp)
racebear_example(RaceBearMotionStudio)
target_include_directories(RaceBearMotionStudio PRIVATE src)
target_compile_definitions(RaceBearMotionStudio PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX UNICODE _UNICODE)
target_link_libraries(RaceBearMotionStudio PRIVATE
    comctl32 comdlg32 shlwapi ws2_32 uxtheme dwmapi)
