#pragma once
#include <windows.h>

#include <commctrl.h>
#include <functional>
#include <string>
#include <vector>

// Dark theme
extern HBRUSH g_darkBg;
extern HBRUSH g_darkEditBg;
extern COLORREF g_darkText;
extern COLORREF g_darkDim;
void InitDarkMode(HWND mainWnd);

struct RB_RuntimeState;

std::wstring Utf8ToWide(const char *text);
std::string WideToUtf8(const std::wstring &text);
std::wstring GetWindowString(HWND hwnd);
void SetWindowUtf8(HWND hwnd, const char *text);
bool SetUtf8(char *target, size_t capacity, const std::wstring &text);
std::wstring FormatDouble(double value, int precision = 2);
std::wstring ResultText(int rc);

namespace ui {
HWND Label(HWND parent, const wchar_t *text, int x, int y, int w, int h = 22);
HWND Caption(HWND parent, const wchar_t *text, int x, int y, int w, int h = 24);
HWND Edit(HWND parent, int id, int x, int y, int w, const wchar_t *text = L"",
          DWORD style = 0);
HWND Button(HWND parent, int id, const wchar_t *text, int x, int y,
            int w = 100);
HWND Check(HWND parent, int id, const wchar_t *text, int x, int y, int w = 180);
HWND Combo(HWND parent, int id, int x, int y, int w = 220);
HWND List(HWND parent, int id, int x, int y, int w, int h, DWORD style = 0);
HWND Group(HWND parent, const wchar_t *text, int x, int y, int w, int h);
HWND Canvas(HWND parent, int id, int x, int y, int w, int h,
            std::function<void(HDC, const RECT &)> paint);
void SetFont(HWND hwnd);
void ComboAdd(HWND combo, const std::wstring &text, LPARAM data);
int ComboData(HWND combo);
void SelectComboData(HWND combo, int data);
int Int(HWND edit, int fallback = 0);
double Double(HWND edit, double fallback = 0.0);
void SetInt(HWND edit, int value);
void SetDouble(HWND edit, double value, int precision = 3);
void SetChecked(HWND check, int value);
int Checked(HWND check);
HWND Slider(HWND parent, int id, int x, int y, int w, int min, int max);
} // namespace ui

class Page {
public:
  explicit Page(HWND host);
  virtual ~Page() = default;
  virtual void Build() = 0;
  virtual void OnShow() {}
  virtual void OnHide() {}
  virtual void OnCommand(int, int, HWND) {}
  virtual void OnState(const RB_RuntimeState &) {}
  virtual void OnTimer(UINT_PTR) {}
  virtual void OnLog(const std::wstring &) {}
  virtual void OnHScroll(int /*id*/, int /*pos*/) {}
  virtual void OnSize(int width, int height);
  virtual void Shutdown() {}
  HWND Window() const { return hwnd_; }
  void Show(bool value);

protected:
  HWND hwnd_{};
  HWND host_{};
  int contentHeight_{700};
  void Status(const std::wstring &text, bool error = false);
  HWND Header(const wchar_t *title, const wchar_t *subtitle = nullptr);
};

using StatusSink = std::function<void(const std::wstring &, bool)>;
void SetStatusSink(StatusSink sink);
