#include "Ui.h"
#include "RaceBearSDK.h"
#include <cwchar>
#include <iomanip>
#include <sstream>
#include <dwmapi.h>
#include <uxtheme.h>

namespace {
StatusSink g_status;
HBRUSH g_darkBg_{};
HBRUSH g_darkEditBg_{};
COLORREF g_darkText_ = RGB(220, 220, 220);
COLORREF g_darkDim_ = RGB(160, 160, 160);
bool g_dark_ = false;

struct CanvasState {
  std::function<void(HDC, const RECT &)> paint;
};

LRESULT CALLBACK CanvasProc(HWND hwnd, UINT message, WPARAM wParam,
                            LPARAM lParam) {
  auto *state = reinterpret_cast<CanvasState *>(
      GetWindowLongPtrW(hwnd, GWLP_USERDATA));
  if (message == WM_NCCREATE) {
    state = static_cast<CanvasState *>(
        reinterpret_cast<CREATESTRUCTW *>(lParam)->lpCreateParams);
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
  }
  if (message == WM_PAINT) {
    PAINTSTRUCT ps{};
    HDC dc = BeginPaint(hwnd, &ps);
    RECT rc{};
    GetClientRect(hwnd, &rc);
    FillRect(dc, &rc, g_darkEditBg_ ? g_darkEditBg_ :
                                      reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1));
    if (state && state->paint)
      state->paint(dc, rc);
    HBRUSH frame = CreateSolidBrush(RGB(58, 64, 65));
    FrameRect(dc, &rc, frame);
    DeleteObject(frame);
    EndPaint(hwnd, &ps);
    return 0;
  }
  if (message == WM_ERASEBKGND)
    return 1;
  if (message == WM_NCDESTROY) {
    // CanvasState 随窗口创建，并在窗口最终销毁时释放，回调不得在外部持有它。
    delete state;
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
  }
  return DefWindowProcW(hwnd, message, wParam, lParam);
}

ATOM EnsureCanvasClass() {
  static ATOM atom{};
  if (!atom) {
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = CanvasProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = L"RaceBearMotionStudio.Canvas";
    atom = RegisterClassExW(&wc);
  }
  return atom;
}

HFONT Font() {
  static HFONT font = CreateFontW(-16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                  CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                  DEFAULT_PITCH, L"Microsoft YaHei UI");
  return font;
}
HFONT TitleFont() {
  static HFONT font = CreateFontW(-28, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                  CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                  DEFAULT_PITCH, L"Microsoft YaHei UI");
  return font;
}
HFONT CaptionFont() {
  static HFONT font = CreateFontW(-15, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                  CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                  DEFAULT_PITCH, L"Microsoft YaHei UI");
  return font;
}

void DrawOwnerButton(const DRAWITEMSTRUCT *draw) {
  // 原生按钮使用统一自绘，避免系统主题覆盖深色背景和状态颜色。
  RECT rc = draw->rcItem;
  const bool disabled = (draw->itemState & ODS_DISABLED) != 0;
  const bool pressed = (draw->itemState & ODS_SELECTED) != 0;
  const bool focused = (draw->itemState & ODS_FOCUS) != 0;
  COLORREF fill = pressed ? RGB(0, 126, 121) : RGB(48, 53, 56);
  COLORREF border = focused ? RGB(30, 190, 178) : RGB(74, 80, 83);
  HBRUSH brush = CreateSolidBrush(fill);
  HPEN pen = CreatePen(PS_SOLID, 1, border);
  HGDIOBJ oldBrush = SelectObject(draw->hDC, brush);
  HGDIOBJ oldPen = SelectObject(draw->hDC, pen);
  RoundRect(draw->hDC, rc.left, rc.top, rc.right, rc.bottom, 6, 6);
  SelectObject(draw->hDC, Font());
  SetBkMode(draw->hDC, TRANSPARENT);
  SetTextColor(draw->hDC, disabled ? RGB(112, 117, 119) : RGB(240, 243, 242));
  wchar_t text[256]{};
  GetWindowTextW(draw->hwndItem, text, 256);
  DrawTextW(draw->hDC, text, -1, &rc,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
  SelectObject(draw->hDC, oldPen);
  SelectObject(draw->hDC, oldBrush);
  DeleteObject(pen);
  DeleteObject(brush);
}
LRESULT CALLBACK PageProc(HWND hwnd, UINT message, WPARAM wParam,
                          LPARAM lParam) {
  Page *page = reinterpret_cast<Page *>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
  if (message == WM_NCCREATE) {
    page = static_cast<Page *>(
        reinterpret_cast<CREATESTRUCTW *>(lParam)->lpCreateParams);
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(page));
  }
  if (page && message == WM_COMMAND) {
    page->OnCommand(LOWORD(wParam), HIWORD(wParam),
                    reinterpret_cast<HWND>(lParam));
    return 0;
  }
  if (page && message == WM_HSCROLL && lParam) {
    HWND tb = reinterpret_cast<HWND>(lParam);
    page->OnHScroll(GetDlgCtrlID(tb),
                    static_cast<int>(SendMessageW(tb, TBM_GETPOS, 0, 0)));
    return 0;
  }
  if (message == WM_DRAWITEM) {
    auto *draw = reinterpret_cast<DRAWITEMSTRUCT *>(lParam);
    if (draw && draw->CtlType == ODT_BUTTON) {
      DrawOwnerButton(draw);
      return TRUE;
    }
  }
  if (message == WM_CTLCOLORSTATIC || message == WM_CTLCOLORBTN) {
    if (g_dark_) {
      HWND control = reinterpret_cast<HWND>(lParam);
      SetTextColor((HDC)wParam,
                   GetPropW(control, L"RB_DIM") ? g_darkDim_ : g_darkText_);
      SetBkColor((HDC)wParam, RGB(24, 27, 28));
      return reinterpret_cast<LRESULT>(g_darkBg_);
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
  }
  if (message == WM_CTLCOLOREDIT || message == WM_CTLCOLORLISTBOX) {
    if (g_dark_) {
      SetTextColor((HDC)wParam, g_darkText_);
      SetBkColor((HDC)wParam, RGB(17, 20, 21));
      return reinterpret_cast<LRESULT>(g_darkEditBg_);
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
  }
  if (message == WM_ERASEBKGND && g_dark_) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    FillRect((HDC)wParam, &rc, g_darkBg_);
    return 1;
  }
  if (message == WM_PAINT && g_dark_) {
    PAINTSTRUCT ps{};
    HDC dc = BeginPaint(hwnd, &ps);
    RECT rc{};
    GetClientRect(hwnd, &rc);
    FillRect(dc, &rc, g_darkBg_);
    HPEN divider = CreatePen(PS_SOLID, 1, RGB(55, 60, 62));
    HGDIOBJ old = SelectObject(dc, divider);
    MoveToEx(dc, 28, 76, nullptr);
    LineTo(dc, rc.right - 28, 76);
    SelectObject(dc, old);
    DeleteObject(divider);
    EndPaint(hwnd, &ps);
    return 0;
  }
  return DefWindowProcW(hwnd, message, wParam, lParam);
}
} // namespace

std::wstring Utf8ToWide(const char *text) {
  if (!text || !*text)
    return {};
  int n =
      MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, nullptr, 0);
  if (n <= 0)
    return L"[invalid UTF-8]";
  std::wstring out(static_cast<size_t>(n), L'\0');
  MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, out.data(), n);
  out.pop_back();
  return out;
}
std::string WideToUtf8(const std::wstring &text) {
  if (text.empty())
    return {};
  int n = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, text.c_str(), -1,
                              nullptr, 0, nullptr, nullptr);
  if (n <= 0)
    return {};
  std::string out(static_cast<size_t>(n), '\0');
  WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, text.c_str(), -1,
                      out.data(), n, nullptr, nullptr);
  out.pop_back();
  return out;
}
std::wstring GetWindowString(HWND h) {
  int n = GetWindowTextLengthW(h);
  std::wstring s(static_cast<size_t>(n + 1), L'\0');
  GetWindowTextW(h, s.data(), n + 1);
  s.resize(static_cast<size_t>(n));
  return s;
}
void SetWindowUtf8(HWND h, const char *t) {
  SetWindowTextW(h, Utf8ToWide(t).c_str());
}
bool SetUtf8(char *target, size_t cap, const std::wstring &text) {
  auto s = WideToUtf8(text);
  if (s.size() >= cap)
    return false;
  memcpy(target, s.c_str(), s.size() + 1);
  return true;
}
std::wstring FormatDouble(double v, int p) {
  std::wostringstream s;
  s << std::fixed << std::setprecision(p) << v;
  return s.str();
}
std::wstring ResultText(int rc) {
  switch (rc) {
  case RB_OK:
    return L"成功";
  case RB_ERROR:
    return L"SDK 执行失败";
  case RB_INVALID_ARGUMENT:
    return L"参数无效";
  case RB_BUFFER_TOO_SMALL:
    return L"缓冲区不足";
  case RB_LICENSE_REQUIRED:
    return L"需要有效授权";
  default:
    return L"SDK 返回 " + std::to_wstring(rc);
  }
}

namespace ui {
void SetFont(HWND h) {
  SendMessageW(h, WM_SETFONT, reinterpret_cast<WPARAM>(Font()), TRUE);
}
HWND Make(const wchar_t *c, const wchar_t *t, DWORD s, DWORD ex, HWND p, int id,
          int x, int y, int w, int h) {
  HWND r = CreateWindowExW(ex, c, t, WS_CHILD | WS_VISIBLE | s, x, y, w, h, p,
                           reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
                           GetModuleHandleW(nullptr), nullptr);
  SetFont(r);
  SetWindowTheme(r, L"DarkMode_Explorer", nullptr);
  return r;
}
HWND Label(HWND p, const wchar_t *t, int x, int y, int w, int h) {
  return Make(L"STATIC", t, SS_LEFT, 0, p, 0, x, y, w, h);
}
HWND Caption(HWND p, const wchar_t *t, int x, int y, int w, int h) {
  HWND label = Label(p, t, x, y, w, h);
  SendMessageW(label, WM_SETFONT, reinterpret_cast<WPARAM>(CaptionFont()), TRUE);
  return label;
}
HWND Edit(HWND p, int id, int x, int y, int w, const wchar_t *t, DWORD s) {
  return Make(L"EDIT", t, WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | s,
              0, p, id, x, y, w, 30);
}
HWND Button(HWND p, int id, const wchar_t *t, int x, int y, int w) {
  return Make(L"BUTTON", t, WS_TABSTOP | BS_OWNERDRAW, 0, p, id, x, y, w, 34);
}
HWND Check(HWND p, int id, const wchar_t *t, int x, int y, int w) {
  return Make(L"BUTTON", t, WS_TABSTOP | BS_AUTOCHECKBOX, 0, p, id, x, y, w,
              28);
}
HWND Combo(HWND p, int id, int x, int y, int w) {
  return Make(WC_COMBOBOXW, L"", WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL, 0,
              p, id, x, y, w, 400);
}
HWND List(HWND p, int id, int x, int y, int w, int h, DWORD s) {
  return Make(WC_LISTBOXW, L"",
              WS_TABSTOP | WS_BORDER | LBS_NOTIFY | WS_VSCROLL | s,
              0, p, id, x, y, w, h);
}
HWND Group(HWND p, const wchar_t *t, int x, int y, int w, int h) {
  return Make(L"BUTTON", t, BS_GROUPBOX, 0, p, 0, x, y, w, h);
}
HWND Canvas(HWND p, int id, int x, int y, int w, int h,
            std::function<void(HDC, const RECT &)> paint) {
  if (!EnsureCanvasClass())
    return nullptr;
  auto *state = new CanvasState{std::move(paint)};
  HWND canvas = CreateWindowExW(
      0, L"RaceBearMotionStudio.Canvas", L"",
      WS_CHILD | WS_VISIBLE, x, y, w, h, p,
      reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
      GetModuleHandleW(nullptr), state);
  if (!canvas)
    delete state;
  return canvas;
}
void ComboAdd(HWND c, const std::wstring &t, LPARAM d) {
  int i = static_cast<int>(
      SendMessageW(c, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(t.c_str())));
  SendMessageW(c, CB_SETITEMDATA, i, d);
}
int ComboData(HWND c) {
  int i = static_cast<int>(SendMessageW(c, CB_GETCURSEL, 0, 0));
  return i < 0 ? -1 : static_cast<int>(SendMessageW(c, CB_GETITEMDATA, i, 0));
}
void SelectComboData(HWND c, int d) {
  int n = static_cast<int>(SendMessageW(c, CB_GETCOUNT, 0, 0));
  for (int i = 0; i < n; ++i)
    if (static_cast<int>(SendMessageW(c, CB_GETITEMDATA, i, 0)) == d) {
      SendMessageW(c, CB_SETCURSEL, i, 0);
      return;
    }
}
int Int(HWND h, int f) {
  auto s = GetWindowString(h);
  wchar_t *e = nullptr;
  long v = wcstol(s.c_str(), &e, 10);
  return e == s.c_str() ? f : static_cast<int>(v);
}
double Double(HWND h, double f) {
  auto s = GetWindowString(h);
  wchar_t *e = nullptr;
  double v = wcstod(s.c_str(), &e);
  return e == s.c_str() ? f : v;
}
void SetInt(HWND h, int v) { SetWindowTextW(h, std::to_wstring(v).c_str()); }
void SetDouble(HWND h, double v, int p) {
  SetWindowTextW(h, FormatDouble(v, p).c_str());
}
void SetChecked(HWND h, int v) {
  SendMessageW(h, BM_SETCHECK, v ? BST_CHECKED : BST_UNCHECKED, 0);
}
int Checked(HWND h) {
  return SendMessageW(h, BM_GETCHECK, 0, 0) == BST_CHECKED ? 1 : 0;
}
HWND Slider(HWND p, int id, int x, int y, int w, int min, int max) {
  HWND h = CreateWindowExW(0, TRACKBAR_CLASS, L"",
      WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_HORZ | TBS_FIXEDLENGTH,
      x, y, w, 28, p, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
      GetModuleHandleW(nullptr), nullptr);
  SendMessageW(h, TBM_SETRANGE, TRUE, MAKELPARAM(min, max));
  SendMessageW(h, TBM_SETPAGESIZE, 0, 5);
  SendMessageW(h, TBM_SETTICFREQ, 10, 0);
  SetFont(h);
  return h;
}
} // namespace ui

HBRUSH g_darkBg = nullptr;
HBRUSH g_darkEditBg = nullptr;
COLORREF g_darkText = RGB(220, 220, 220);
COLORREF g_darkDim = RGB(160, 160, 160);

void InitDarkMode(HWND mainWnd) {
  g_dark_ = true;
  g_darkBg_ = CreateSolidBrush(RGB(24, 27, 28));
  g_darkEditBg_ = CreateSolidBrush(RGB(17, 20, 21));
  g_darkBg = g_darkBg_;
  g_darkEditBg = g_darkEditBg_;
  // Dark title bar (Windows 10 20H1+)
  HMODULE dwm = LoadLibraryW(L"dwmapi.dll");
  if (dwm) {
    auto setAttr = (HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD))
        GetProcAddress(dwm, "DwmSetWindowAttribute");
    if (setAttr) {
      BOOL dark = TRUE;
      setAttr(mainWnd, 20, &dark, sizeof(dark)); // DWMWA_USE_IMMERSIVE_DARK_MODE
    }
    FreeLibrary(dwm);
  }
}

void SetStatusSink(StatusSink sink) { g_status = std::move(sink); }
Page::Page(HWND host) : host_(host) {
  static bool registered = false;
  if (!registered) {
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = PageProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = L"RaceBearMotionStudio.Page";
    registered = RegisterClassExW(&wc) != 0 ||
                 GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
  }
  hwnd_ = CreateWindowExW(0, L"RaceBearMotionStudio.Page", L"",
                          WS_CHILD | WS_CLIPCHILDREN, 0, 0, 100, 100, host,
                          nullptr, GetModuleHandleW(nullptr), this);
}
void Page::Show(bool v) {
  ShowWindow(hwnd_, v ? SW_SHOW : SW_HIDE);
  if (v)
    OnShow();
  else
    OnHide();
}
void Page::OnSize(int w, int h) { MoveWindow(hwnd_, 0, 0, w, h, TRUE); }
void Page::Status(const std::wstring &t, bool e) {
  if (g_status)
    g_status(t, e);
}
HWND Page::Header(const wchar_t *t, const wchar_t *s) {
  HWND h = ui::Label(hwnd_, t, 28, 14, 850, 36);
  SendMessageW(h, WM_SETFONT, reinterpret_cast<WPARAM>(TitleFont()), TRUE);
  if (s) {
    HWND subtitle = ui::Label(hwnd_, s, 28, 50, 1120, 22);
    SetPropW(subtitle, L"RB_DIM", reinterpret_cast<HANDLE>(1));
  }
  return h;
}
