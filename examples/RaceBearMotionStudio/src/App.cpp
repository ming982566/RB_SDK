#include "App.h"
#include "pages/Pages.h"
#include <array>
#include <uxtheme.h>
#include <vssym32.h>

namespace {
constexpr UINT_PTR STATE_TIMER = 1, LOG_TIMER = 2;
constexpr int ID_NAV = 100;
WNDPROC g_origHostProc{};

LRESULT CALLBACK HostProc(HWND h, UINT m, WPARAM w, LPARAM l) {
  if (m == WM_ERASEBKGND) {
    RECT rc;
    GetClientRect(h, &rc);
    FillRect((HDC)w, &rc, g_darkBg);
    return 1;
  }
  return CallWindowProcW(g_origHostProc, h, m, w, l);
}
} // namespace
int App::Run(HINSTANCE i, int show) {
  std::wstring e;
  // SDK 必须先于界面启动，页面创建时会立即读取目录和运行态数据。
  if (!sdk_.Start(e)) {
    MessageBoxW(nullptr, e.c_str(), L"RaceBear Motion Studio", MB_ICONERROR);
    return 1;
  }
  if (!Create(i, show, e)) {
    MessageBoxW(nullptr, e.c_str(), L"RaceBear Motion Studio", MB_ICONERROR);
    sdk_.Stop();
    return 2;
  }
  MSG m{};
  while (GetMessageW(&m, nullptr, 0, 0) > 0) {
    TranslateMessage(&m);
    DispatchMessageW(&m);
  }
  return static_cast<int>(m.wParam);
}
bool App::Create(HINSTANCE i, int show, std::wstring &e) {
  instance_ = i;
  WNDCLASSEXW wc{sizeof(wc)};
  wc.lpfnWndProc = WndProc;
  wc.hInstance = i;
  wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
  wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
  wc.lpszClassName = L"RaceBearMotionStudio.Main";
  wc.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
  if (!RegisterClassExW(&wc)) {
    e = L"无法注册主窗口";
    return false;
  }
  hwnd_ = CreateWindowExW(0, wc.lpszClassName, L"RaceBear Motion Studio",
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT,
                          CW_USEDEFAULT, 1600, 950, nullptr, nullptr, i, this);
  if (!hwnd_) {
    e = L"无法创建主窗口";
    return false;
  }
  InitDarkMode(hwnd_);
  brand_ = ui::Label(hwnd_, L"RACEBEAR", 24, 18, 180, 30);
  brandSub_ = ui::Label(hwnd_, L"SDK 示例前端", 24, 50, 180, 22);
  HFONT brandFont = CreateFontW(-22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                DEFAULT_PITCH, L"Microsoft YaHei UI");
  SendMessageW(brand_, WM_SETFONT, reinterpret_cast<WPARAM>(brandFont), TRUE);
  SetPropW(brandSub_, L"RB_DIM", reinterpret_cast<HANDLE>(1));
  nav_ = ui::List(hwnd_, ID_NAV, 0, 84, 228, 716,
                  LBS_OWNERDRAWFIXED | LBS_HASSTRINGS |
                  LBS_NOINTEGRALHEIGHT | WS_VSCROLL);
  host_ = CreateWindowExW(0, L"STATIC", L"",
                          WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 228, 0, 1000,
                          800, hwnd_, nullptr, i, nullptr);
  status_ = ui::Label(hwnd_, L"就绪", 244, 800, 1000, 28);
  // Subclass host_ for dark background
  g_origHostProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(
      host_, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HostProc)));
  // Navigation font — larger, bold
  navFont_ = CreateFontW(-16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                               DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                               CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                               DEFAULT_PITCH, L"Microsoft YaHei UI");
  SendMessageW(nav_, WM_SETFONT, reinterpret_cast<WPARAM>(navFont_), TRUE);
  SetWindowTheme(host_, L"DarkMode_Explorer", nullptr);
  BuildPages();
  SetStatusSink([this](const std::wstring &t, bool err) {
    SetWindowTextW(status_, t.c_str());
    if (err)
      SetPropW(status_, L"RB_ERROR", reinterpret_cast<HANDLE>(1));
    else
      RemovePropW(status_, L"RB_ERROR");
    InvalidateRect(status_, nullptr, TRUE);
    if (err)
      MessageBeep(MB_ICONWARNING);
  });
  SendMessageW(nav_, LB_SETCURSEL, 0, 0);
  SelectPage(0);
  // 界面定时器只读取 SDK 快照和日志，后端计算由 SDK 内部循环独立驱动。
  SetTimer(hwnd_, STATE_TIMER, 33, nullptr);
  SetTimer(hwnd_, LOG_TIMER, 250, nullptr);
  ShowWindow(hwnd_, show);
  UpdateWindow(hwnd_);
  return true;
}
void App::BuildPages() {
  const wchar_t *names[] = {
      L"总览",     L"授权",       L"游戏和遥测", L"动感平台",  L"动态滤波",
      L"运动增强", L"震动",       L"风感",       L"安全带",    L"输出",
      L"串口",     L"自定义游戏", L"外部数据源", L"CAN 状态",
      L"功能插件", L"系统与循环", L"日志和诊断"};
  for (auto n : names)
    SendMessageW(nav_, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(n));
  pages_.push_back(MakeOverviewPage(host_, sdk_));
  pages_.push_back(MakeLicensePage(host_, sdk_));
  pages_.push_back(MakeGamePage(host_, sdk_));
  pages_.push_back(MakePlatformPage(host_, sdk_));
  pages_.push_back(MakeDynamicPage(host_, sdk_));
  pages_.push_back(MakeMotionPage(host_, sdk_));
  pages_.push_back(MakeHapticPage(host_, sdk_));
  pages_.push_back(MakeWindPage(host_, sdk_));
  pages_.push_back(MakeSeatbeltPage(host_, sdk_));
  pages_.push_back(MakeOutputPage(host_, sdk_));
  pages_.push_back(MakeSerialPage(host_, sdk_));
  pages_.push_back(MakeCustomGamePage(host_, sdk_));
  pages_.push_back(MakeExternalSourcePage(host_, sdk_));
  pages_.push_back(MakeCanStatusPage(host_, sdk_));
  pages_.push_back(MakePluginPage(host_, sdk_));
  pages_.push_back(MakeSystemPage(host_, sdk_));
  pages_.push_back(MakeLogPage(host_, sdk_));
  for (auto &p : pages_) {
    p->Build();
    p->Show(false);
  }
}
void App::SelectPage(int i) {
  if (i < 0 || i >= static_cast<int>(pages_.size()) || i == selected_)
    return;
  // 先触发旧页面的隐藏清理，再让新页面读取并展示当前状态。
  if (selected_ >= 0)
    pages_[selected_]->Show(false);
  selected_ = i;
  pages_[selected_]->Show(true);
  RECT r{};
  GetClientRect(host_, &r);
  pages_[selected_]->OnSize(r.right, r.bottom);
}
void App::TickState() {
  std::wstring e;
  RB_RuntimeState next{};
  if (!sdk_.ReadState(next, e)) {
    SetWindowTextW(status_, e.c_str());
    return;
  }
  state_ = next;
  for (auto &p : pages_)
    p->OnState(state_);
}
void App::TickLogs() {
  for (const auto &line : sdk_.DrainLogs())
    for (auto &p : pages_)
      p->OnLog(line);
}
void App::Resize(int w, int h) {
  constexpr int navWidth = 228;
  constexpr int brandHeight = 84;
  constexpr int statusHeight = 34;
  MoveWindow(nav_, 0, brandHeight, navWidth, h - brandHeight, TRUE);
  MoveWindow(host_, navWidth, 0, w - navWidth, h - statusHeight, TRUE);
  MoveWindow(status_, navWidth + 20, h - statusHeight + 6,
             w - navWidth - 40, statusHeight - 8, TRUE);
  if (selected_ >= 0)
    pages_[selected_]->OnSize(w - navWidth, h - statusHeight);
}
void App::Shutdown() {
  if (closing_)
    return;
  closing_ = true;
  KillTimer(hwnd_, STATE_TIMER);
  KillTimer(hwnd_, LOG_TIMER);
  for (auto &p : pages_)
    p->Shutdown();
  // SDK 退出前先清零测试输出并断开设备，避免硬件停在最后一次测试值。
  const int poseResetRc = RB_ManualPose_ResetDrive();
  const int poseDisableRc = RB_ManualPose_SetTestEnabled(0);
  const int windRc = RB_Wind_SetTestOutput(0, 0);
  const int seatbeltRc = RB_Seatbelt_SetTestOutput(0, 0, 0);
  const int disconnectRc = RB_Output_RequestDisconnectAll();
  const bool cleanupFailed =
      (poseResetRc != RB_OK && state_.Rig.Count > 0) ||
      (poseDisableRc != RB_OK && state_.Rig.Count > 0) ||
      (windRc != RB_OK && state_.Function.Count > 0) ||
      (seatbeltRc != RB_OK && state_.Function.Count > 0) ||
      disconnectRc != RB_OK;
  if (cleanupFailed)
    OutputDebugStringW(
        L"RaceBear Motion Studio: one or more shutdown safety calls failed.\n");
  for (int i = 0; i < 100 && !RB_Output_AreAllDisconnected(); ++i)
    Sleep(10);
  RB_Debug_CloseMonitor();
  sdk_.Stop();
}
LRESULT CALLBACK App::WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
  App *a = m == WM_NCCREATE
               ? static_cast<App *>(
                     reinterpret_cast<CREATESTRUCTW *>(l)->lpCreateParams)
               : reinterpret_cast<App *>(GetWindowLongPtrW(h, GWLP_USERDATA));
  if (m == WM_NCCREATE)
    SetWindowLongPtrW(h, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(a));
  return a ? a->Handle(h, m, w, l) : DefWindowProcW(h, m, w, l);
}
LRESULT App::Handle(HWND h, UINT m, WPARAM w, LPARAM l) {
  switch (m) {
  case WM_GETMINMAXINFO: {
    auto *info = reinterpret_cast<MINMAXINFO *>(l);
    info->ptMinTrackSize.x = 1450;
    info->ptMinTrackSize.y = 800;
    return 0;
  }
  case WM_SIZE:
    Resize(LOWORD(l), HIWORD(l));
    return 0;
  case WM_MEASUREITEM:
    if (reinterpret_cast<MEASUREITEMSTRUCT *>(l)->CtlID == ID_NAV) {
      reinterpret_cast<MEASUREITEMSTRUCT *>(l)->itemHeight = 46;
      return TRUE;
    }
    return 0;
  case WM_DRAWITEM: {
    auto *dis = reinterpret_cast<DRAWITEMSTRUCT *>(l);
    if (dis->CtlID != ID_NAV) break;
    // Background
    HBRUSH selectedBrush = CreateSolidBrush(RGB(37, 43, 43));
    FillRect(dis->hDC, &dis->rcItem,
             (dis->itemState & ODS_SELECTED) ? selectedBrush : g_darkBg);
    // Accent bar (3px blue on left edge)
    if (dis->itemState & ODS_SELECTED) {
      RECT bar = {dis->rcItem.left, dis->rcItem.top,
                  dis->rcItem.left + 3, dis->rcItem.bottom};
      HBRUSH accent = CreateSolidBrush(RGB(18, 177, 163));
      FillRect(dis->hDC, &bar, accent);
      DeleteObject(accent);
    }
    DeleteObject(selectedBrush);
    RECT marker = {dis->rcItem.left + 18, dis->rcItem.top + 19,
                   dis->rcItem.left + 24, dis->rcItem.top + 25};
    HBRUSH markerBrush = CreateSolidBrush(
        (dis->itemState & ODS_SELECTED) ? RGB(18, 177, 163) : RGB(92, 99, 99));
    FillRect(dis->hDC, &marker, markerBrush);
    DeleteObject(markerBrush);
    // Text
    wchar_t buf[128] = {};
    SendMessageW(nav_, LB_GETTEXT, dis->itemID, reinterpret_cast<LPARAM>(buf));
    SelectObject(dis->hDC, navFont_);
    SetBkMode(dis->hDC, TRANSPARENT);
    SetTextColor(dis->hDC, (dis->itemState & ODS_SELECTED) ? RGB(245,248,247) : g_darkText);
    RECT tr = {dis->rcItem.left + 36, dis->rcItem.top,
               dis->rcItem.right, dis->rcItem.bottom};
    DrawTextW(dis->hDC, buf, -1, &tr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    return TRUE;
  }
  case WM_TIMER:
    if (w == STATE_TIMER)
      TickState();
    else if (w == LOG_TIMER)
      TickLogs();
    if (selected_ >= 0)
      pages_[selected_]->OnTimer(w);
    return 0;
  case WM_COMMAND: {
    int id = LOWORD(w), code = HIWORD(w);
    HWND ctl = reinterpret_cast<HWND>(l);
    if (id == ID_NAV && code == LBN_SELCHANGE)
      SelectPage(static_cast<int>(SendMessageW(nav_, LB_GETCURSEL, 0, 0)));
    else if (selected_ >= 0)
      pages_[selected_]->OnCommand(id, code, ctl);
    return 0;
  }
  case WM_CLOSE:
    Shutdown();
    DestroyWindow(h);
    return 0;
  case WM_DESTROY:
    Shutdown();
    PostQuitMessage(0);
    return 0;
  case WM_ERASEBKGND:
    {
      RECT rc{};
      GetClientRect(h, &rc);
      FillRect(reinterpret_cast<HDC>(w), &rc, g_darkBg);
    }
    return 1;
  case WM_CTLCOLORSTATIC:
  case WM_CTLCOLORBTN:
    SetTextColor((HDC)w, GetPropW(reinterpret_cast<HWND>(l), L"RB_ERROR")
                                ? RGB(242, 112, 112)
                                : GetPropW(reinterpret_cast<HWND>(l), L"RB_DIM")
                                      ? RGB(140, 148, 148)
                                      : g_darkText);
    SetBkColor((HDC)w, RGB(24, 27, 28));
    return reinterpret_cast<LRESULT>(g_darkBg);
  case WM_CTLCOLORLISTBOX:
    SetTextColor((HDC)w, g_darkText);
    SetBkColor((HDC)w, RGB(17, 20, 21));
    return reinterpret_cast<LRESULT>(g_darkEditBg);
  }
  return DefWindowProcW(h, m, w, l);
}
