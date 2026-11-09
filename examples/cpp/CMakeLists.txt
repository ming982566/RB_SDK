cmake_minimum_required(VERSION 3.15)
project(RaceBearSDKCppExample LANGUAGES CXX)
include("../common/ExampleBuild.cmake")
add_executable(RaceBearSDKCppExample
    main.cpp custom_game_example.cpp custom_game_example.h
    configuration_examples.cpp configuration_examples.h)
racebear_example(RaceBearSDKCppExample)
add_executable(RaceBearSimHubUdpBridge simhub_udp_bridge/main.cpp)
racebear_example(RaceBearSimHubUdpBridge)
target_link_libraries(RaceBearSimHubUdpBridge PRIVATE ws2_32)
