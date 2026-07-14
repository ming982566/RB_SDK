cmake_minimum_required(VERSION 3.15)
project(RaceBearSDKCppExample LANGUAGES CXX)

# RaceBearSDK.lib 由 VS2019 生成；本示例直接链接导入库，因此要求 MSVC。
if(NOT WIN32 OR NOT MSVC)
    message(FATAL_ERROR "This example requires Windows and the MSVC toolchain.")
endif()

# SDK 只发布 x64 版本，避免误生成 Win32 工程后在运行时无法加载 DLL。
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "RaceBear SDK requires an x64 target.")
endif()

# 示例位于 SDK_ROOT/examples/cpp，因此向上两级就是 SDK 根目录。
get_filename_component(RACEBEAR_SDK_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/../.."
    ABSOLUTE)

# 公开头只需要 RaceBearSDK.h，不应包含 src 下的任何私有头文件。
add_executable(RaceBearSDKCppExample
    main.cpp
    custom_game_example.cpp
    custom_game_example.h
    configuration_examples.cpp
    configuration_examples.h)
target_compile_features(RaceBearSDKCppExample PRIVATE cxx_std_17)

# 示例源码使用 UTF-8 无 BOM 中文注释；VS2019 必须显式按 UTF-8 解析源码。
target_compile_options(RaceBearSDKCppExample PRIVATE /utf-8)

target_include_directories(RaceBearSDKCppExample PRIVATE
    "${RACEBEAR_SDK_ROOT}/include")
target_link_libraries(RaceBearSDKCppExample PRIVATE
    "${RACEBEAR_SDK_ROOT}/bin/x64/Release/RaceBearSDK.lib")

# Windows 默认从 EXE 同目录查找 DLL，构建后自动复制可直接运行示例。
add_custom_command(TARGET RaceBearSDKCppExample POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${RACEBEAR_SDK_ROOT}/bin/x64/Release/RaceBearSDK.dll"
        "$<TARGET_FILE_DIR:RaceBearSDKCppExample>")
