cmake_minimum_required(VERSION 3.15)
project(RaceBearDashboardRender LANGUAGES CXX)
include("../common/ExampleBuild.cmake")
add_executable(RaceBearDashboardRender main.cpp)
racebear_example(RaceBearDashboardRender)
target_link_libraries(RaceBearDashboardRender PRIVATE user32 gdi32 comdlg32 shell32)
add_custom_command(TARGET RaceBearDashboardRender POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${RACEBEAR_SDK_ROOT}/bin/x64/$<CONFIG>/DashboardAssets/RaceBear"
    "$<TARGET_FILE_DIR:RaceBearDashboardRender>/DashboardAssets/RaceBear")
