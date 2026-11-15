#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include "ExampleServices.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
constexpr int width = 854, height = 480, stride = width * 4;
// 同步窗口线程独占包、DIB 和 SDK 生命周期，关闭不与渲染并发。
struct Preview {
    HWND window{}, packages{}, pages{}, status{};
    HBITMAP bitmap{};
    HDC memory{};
    HGDIOBJ previous{};
    void* pixels{};
    RB_DashboardPackageHandle handle{};
    std::vector<RB_DashboardPackageInfo> catalog;
    bool initialized{}, valid{};
    int selectedPage{};
    ~Preview() {
        if (handle) { try { rb_example::Dashboard_ClosePackage(handle); } catch (...) {} }
        if (memory && previous) SelectObject(memory, previous);
        if (bitmap) DeleteObject(bitmap);
        if (memory) DeleteDC(memory);
        if (initialized) RB_Runtime_Shutdown();
    }
    // 位图使用负高度，第一行就是 SDK 缓冲区第一行。
    void Allocate() {
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = width; info.bmiHeader.biHeight = -height;
        info.bmiHeader.biPlanes = 1; info.bmiHeader.biBitCount = 32;
        memory = CreateCompatibleDC(nullptr);
        bitmap = CreateDIBSection(memory, &info, DIB_RGB_COLORS, &pixels, nullptr, 0);
        if (!memory || !bitmap || !pixels) throw std::runtime_error("DIB allocation failed");
        previous = SelectObject(memory, bitmap);
    }
    static void Check(int rc, const char* operation) {
        if (rc != RB_OK) throw std::runtime_error(std::string(operation) + " business=" + std::to_string(rc));
    }
    static std::wstring Wide(const char* text) {
        const int size = MultiByteToWideChar(CP_UTF8, 0, text, -1, nullptr, 0);
        if (size <= 0) return L"";
        std::wstring result(size, 0);
        MultiByteToWideChar(CP_UTF8, 0, text, -1, result.data(), size);
        result.pop_back(); return result;
    }
    void Refresh(const char* preferred = nullptr) {
        const int refreshed = rb_example::Dashboard_RefreshPackages();
        if (refreshed < 0) Check(refreshed, "refresh packages");
        const int count = rb_example::Dashboard_GetPackageCount();
        if (count < 0) Check(count, "package count");
        catalog.clear();
        if (packages) SendMessageW(packages, CB_RESETCONTENT, 0, 0);
        int selected = 0;
        for (int i = 0; i < count; ++i) {
            RB_DashboardPackageInfo info{}; info.Size = sizeof info; info.Version = 1;
            Check(rb_example::Dashboard_GetPackageInfo(i, &info), "package info");
            if (!info.Valid) continue;
            if (preferred && std::strcmp(preferred, info.PackageId) == 0) selected = static_cast<int>(catalog.size());
            catalog.push_back(info);
            if (packages) SendMessageW(packages, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(Wide(info.Name).c_str()));
        }
        if (catalog.empty()) throw std::runtime_error("No valid packages. Deploy DashboardAssets beside the DLL.");
        if (packages) SendMessageW(packages, CB_SETCURSEL, selected, 0);
        Open(selected);
    }
    void Open(int index) {
        valid = false;
        if (handle) { rb_example::Dashboard_ClosePackage(handle); handle = nullptr; }
        if (index < 0 || index >= static_cast<int>(catalog.size())) throw std::runtime_error("Invalid package selection");
        Check(rb_example::Dashboard_OpenPackage(catalog[index].PackageId, &handle), "open package");
        if (!handle) throw std::runtime_error("Package handle is null");
        if (pages) SendMessageW(pages, CB_RESETCONTENT, 0, 0);
        for (int i = 0; pages && i < catalog[index].PageCount; ++i) {
            const std::wstring label = L"Page " + std::to_wstring(i + 1);
            SendMessageW(pages, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(label.c_str()));
        }
        selectedPage = 0;
        if (pages) SendMessageW(pages, CB_SETCURSEL, 0, 0);
        Check(rb_example::Dashboard_SetPackagePage(handle, 0), "set first page");
    }
    void Render() {
        valid = false;
        RB_RuntimeState state{}; state.Size = sizeof state; state.Version = 2;
        if (initialized) Check(rb_example::ReadBinary("rb.runtime.state", state), "read runtime");
        // 离线模式只使用固定输入；标题与状态明确标记，不伪造实时遥测。
        Check(rb_example::Dashboard_RenderPackageFrame(handle, &state, pixels, width, height, stride), "render frame");
        RB_DashboardPackageState package{}; package.Size = sizeof package; package.Version = 1;
        Check(rb_example::Dashboard_GetPackageState(handle, &package), "package state");
        if (package.CurrentPage != selectedPage) throw std::runtime_error("Rendered package page mismatch");
        valid = true;
        if (status) {
            const std::wstring text = (initialized ? L"Runtime | " : L"Offline preview | ") +
                std::to_wstring(package.RenderedFrames) + L" frames | " +
                std::to_wstring(package.LastRenderTimeMs) + L" ms";
            SetWindowTextW(status, text.c_str());
        }
    }
    void Import() {
        wchar_t path[32768]{};
        OPENFILENAMEW dialog{}; dialog.lStructSize = sizeof dialog; dialog.hwndOwner = window;
        dialog.lpstrFilter = L"RaceBear dashboard\0*.rbdash\0";
        dialog.lpstrFile = path; dialog.nMaxFile = 32768;
        dialog.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
        if (!GetOpenFileNameW(&dialog)) return;
        const auto utf8 = std::filesystem::path(path).u8string();
        char id[RB_TEXT_SMALL]{};
        Check(rb_example::Dashboard_ImportPackage(utf8.c_str(), id, sizeof id), "import package");
        Refresh(id);
    }
    void SaveBmp(const std::filesystem::path& path) {
        BITMAPFILEHEADER file{}; file.bfType = 0x4d42;
        file.bfOffBits = sizeof file + sizeof(BITMAPINFOHEADER); file.bfSize = file.bfOffBits + stride * height;
        BITMAPINFOHEADER info{}; info.biSize = sizeof info; info.biWidth = width;
        info.biHeight = -height; info.biPlanes = 1; info.biBitCount = 32; info.biSizeImage = stride * height;
        std::ofstream output(path, std::ios::binary);
        output.write(reinterpret_cast<const char*>(&file), sizeof file);
        output.write(reinterpret_cast<const char*>(&info), sizeof info);
        output.write(static_cast<const char*>(pixels), stride * height);
        if (!output) throw std::runtime_error("Cannot save snapshot");
    }
};

// 绘制只缩放固定像素缓冲，不把 DPI 或客户区尺寸误当成 stride。
LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM w, LPARAM l)
{
    auto* preview = reinterpret_cast<Preview*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        preview = static_cast<Preview*>(reinterpret_cast<CREATESTRUCTW*>(l)->lpCreateParams);
        preview->window = window;
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(preview));
    }
    try {
        if (message == WM_TIMER) {
            preview->Render(); InvalidateRect(window, nullptr, FALSE); return 0;
        }
        if (message == WM_COMMAND) {
            if (LOWORD(w) == 10 && HIWORD(w) == CBN_SELCHANGE)
                preview->Open(static_cast<int>(SendMessageW(preview->packages, CB_GETCURSEL, 0, 0)));
            else if (LOWORD(w) == 11 && HIWORD(w) == CBN_SELCHANGE) {
                const int page = static_cast<int>(SendMessageW(preview->pages, CB_GETCURSEL, 0, 0));
                Preview::Check(rb_example::Dashboard_SetPackagePage(preview->handle, page), "set page");
                preview->selectedPage = page;
            } else if (LOWORD(w) == 12) preview->Import();
            return 0;
        }
        if (message == WM_PAINT) {
            PAINTSTRUCT ps{}; HDC dc = BeginPaint(window, &ps);
            RECT area{}; GetClientRect(window, &area);
            FillRect(dc, &area, reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
            const int availableW = std::max(1L, area.right), availableH = std::max(1L, area.bottom - 88);
            const double scale = std::min(availableW / double(width), availableH / double(height));
            const int drawnW = static_cast<int>(width * scale), drawnH = static_cast<int>(height * scale);
            const int x = (availableW - drawnW) / 2, y = 56 + (availableH - drawnH) / 2;
            if (preview && preview->valid) {
                SetStretchBltMode(dc, HALFTONE);
                StretchBlt(dc, x, y, drawnW, drawnH, preview->memory, 0, 0, width, height, SRCCOPY);
            }
            EndPaint(window, &ps); return 0;
        }
        if (message == WM_SIZE && preview && preview->status) {
            RECT area{}; GetClientRect(window, &area);
            MoveWindow(preview->status, 12, std::max(56L, area.bottom - 28), std::max(1L, area.right - 24), 24, TRUE);
            InvalidateRect(window, nullptr, FALSE); return 0;
        }
        if (message == WM_GETMINMAXINFO) {
            auto* size = reinterpret_cast<MINMAXINFO*>(l); size->ptMinTrackSize = POINT{760, 520}; return 0;
        }
        if (message == WM_DESTROY) { KillTimer(window, 1); PostQuitMessage(0); return 0; }
    } catch (const std::exception& error) {
        if (preview) {
            preview->valid = false;
            if (preview->status) SetWindowTextW(preview->status, Preview::Wide(error.what()).c_str());
            InvalidateRect(window, nullptr, FALSE);
        }
        return 0;
    }
    return DefWindowProcW(window, message, w, l);
}
}

int main() try
{
    SetProcessDPIAware();
    int argc = 0; LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) return 1;
    const bool runtime = argc == 2 && std::wcscmp(argv[1], L"--runtime") == 0;
    const bool snapshot = argc == 3 && std::wcscmp(argv[1], L"--snapshot") == 0;
    const std::filesystem::path destination = snapshot ? argv[2] : L"";
    const bool help = argc == 2 && std::wcscmp(argv[1], L"--help") == 0;
    LocalFree(argv);
    if (help) { std::puts("RaceBearDashboardRender [--runtime | --snapshot <bmp>]"); return 0; }
    if (argc != 1 && !runtime && !snapshot) return 2;
    Preview preview;
    if (runtime) {
        Preview::Check(rb_example::Runtime_SetAppDataName("RaceBearDashboardDemo"), "app identity");
        Preview::Check(RB_Runtime_Initialize(), "initialize"); preview.initialized = true;
        Preview::Check(rb_example::Runtime_StartLoop(2), "start runtime");
    }
    preview.Allocate();
    if (snapshot) {
        preview.Refresh(); preview.Render(); preview.SaveBmp(destination);
        std::puts("Offline preview snapshot rendered through the SDK."); return 0;
    }
    const HINSTANCE instance = GetModuleHandleW(nullptr);
    WNDCLASSW type{}; type.lpfnWndProc = WindowProc; type.hInstance = instance;
    type.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512)); type.lpszClassName = L"RaceBearDashboardExample";
    if (!RegisterClassW(&type)) throw std::runtime_error("Window registration failed");
    HWND window = CreateWindowW(type.lpszClassName, runtime ? L"RaceBear Dashboard" : L"RaceBear Dashboard - Offline Preview",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, 1000, 680, nullptr, nullptr, instance, &preview);
    if (!window) throw std::runtime_error("Window creation failed");
    preview.packages = CreateWindowW(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
        12, 12, 370, 250, window, reinterpret_cast<HMENU>(10), instance, nullptr);
    preview.pages = CreateWindowW(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        400, 12, 150, 250, window, reinterpret_cast<HMENU>(11), instance, nullptr);
    CreateWindowW(L"BUTTON", L"Import...", WS_CHILD | WS_VISIBLE, 570, 10, 100, 28,
        window, reinterpret_cast<HMENU>(12), instance, nullptr);
    preview.status = CreateWindowW(L"STATIC", L"", WS_CHILD | WS_VISIBLE, 12, 600, 920, 24, window, nullptr, instance, nullptr);
    preview.Refresh(); preview.Render();
    SetTimer(window, 1, 33, nullptr); ShowWindow(window, SW_SHOW);
    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) { TranslateMessage(&message); DispatchMessageW(&message); }
    return 0;
}
catch (const std::exception& error) { std::fprintf(stderr, "%s\n", error.what()); return 1; }
