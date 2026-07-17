#pragma once
#include "SdkRuntime.h"
#include "Ui.h"
#include <memory>

class App {
public:
  int Run(HINSTANCE instance, int show);

private:
  static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
  LRESULT Handle(HWND, UINT, WPARAM, LPARAM);
  bool Create(HINSTANCE, int, std::wstring &);
  void BuildPages();
  void SelectPage(int index);
  void TickState();
  void TickLogs();
  void Resize(int width, int height);
  void Shutdown();
  HINSTANCE instance_{};
  HWND hwnd_{};
  HWND nav_{};
  HWND brand_{}, brandSub_{};
  HWND host_{};
  HWND status_{};
  SdkRuntime sdk_;
  std::vector<std::unique_ptr<Page>> pages_;
  int selected_{-1};
  bool closing_{};
  RB_RuntimeState state_{};
  HFONT navFont_{};
};
