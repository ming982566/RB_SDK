# 每种配置只链接和部署同配置 SDK；此文件同时供各示例独立构建使用。
if(NOT WIN32 OR NOT MSVC OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Windows x64 MSVC is required.")
endif()
if(MSVC_TOOLSET_VERSION LESS 142 OR MSVC_TOOLSET_VERSION GREATER 143)
    message(FATAL_ERROR "Examples allow x64 MSVC v142/v143; the official SDK baseline is v142.")
endif()
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE MATCHES "^(Debug|Release)$")
    message(FATAL_ERROR "Single-config builds require -DCMAKE_BUILD_TYPE=Debug or Release.")
endif()
if(NOT RACEBEAR_SDK_ROOT)
    get_filename_component(RACEBEAR_SDK_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
endif()
set(RACEBEAR_EXAMPLE_COMMON "${CMAKE_CURRENT_LIST_DIR}")
function(racebear_example target)
    target_compile_features(${target} PRIVATE cxx_std_17)
    target_compile_options(${target} PRIVATE /utf-8 /EHsc)
    target_include_directories(${target} PRIVATE
        "${RACEBEAR_SDK_ROOT}/include" "${RACEBEAR_EXAMPLE_COMMON}")
    target_link_libraries(${target} PRIVATE
        "${RACEBEAR_SDK_ROOT}/bin/x64/$<CONFIG>/RaceBearSDK.lib")
    foreach(dependency RaceBearSDK.dll libusb-1.0.dll)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${RACEBEAR_SDK_ROOT}/bin/x64/$<CONFIG>/${dependency}"
            "$<TARGET_FILE_DIR:${target}>")
    endforeach()
endfunction()
