#pragma once
#include "SdkRuntime.h"
#include "Ui.h"
#include <memory>

std::unique_ptr<Page> MakeOverviewPage(HWND host, SdkRuntime &sdk);
std::unique_ptr<Page> MakeLicensePage(HWND host, SdkRuntime &sdk);
std::unique_ptr<Page> MakeGamePage(HWND host, SdkRuntime &sdk);
std::unique_ptr<Page> MakePlatformPage(HWND host, SdkRuntime &sdk);
std::unique_ptr<Page> MakeDynamicPage(HWND host, SdkRuntime &sdk);
std::unique_ptr<Page> MakeMotionPage(HWND host, SdkRuntime &sdk);
std::unique_ptr<Page> MakeHapticPage(HWND host, SdkRuntime &sdk);
std::unique_ptr<Page> MakeWindPage(HWND host, SdkRuntime &sdk);
std::unique_ptr<Page> MakeSeatbeltPage(HWND host, SdkRuntime &sdk);
std::unique_ptr<Page> MakeOutputPage(HWND host, SdkRuntime &sdk);
std::unique_ptr<Page> MakeSerialPage(HWND host, SdkRuntime &sdk);
std::unique_ptr<Page> MakeCustomGamePage(HWND host, SdkRuntime &sdk);
std::unique_ptr<Page> MakePluginPage(HWND host, SdkRuntime &sdk);
std::unique_ptr<Page> MakeLogPage(HWND host, SdkRuntime &sdk);
std::unique_ptr<Page> MakeSystemPage(HWND host, SdkRuntime &sdk);
std::unique_ptr<Page> MakeCanStatusPage(HWND host, SdkRuntime &sdk);
std::unique_ptr<Page> MakeExternalSourcePage(HWND host, SdkRuntime &sdk);
