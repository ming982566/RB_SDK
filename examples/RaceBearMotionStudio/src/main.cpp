#include "App.h"
#include <commctrl.h>

// 启动期间的服务传输异常也必须在宿主边界处理。
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR, int show) try {
  SetProcessDPIAware();
  INITCOMMONCONTROLSEX icc{sizeof(icc), ICC_STANDARD_CLASSES |
                                            ICC_LISTVIEW_CLASSES |
                                            ICC_BAR_CLASSES};
  InitCommonControlsEx(&icc);
  App app;
  return app.Run(instance, show);
}
catch (const std::exception& error) {
  MessageBoxW(nullptr, Utf8ToWide(error.what()).c_str(), L"SDK 启动失败", MB_ICONERROR);
  return 1;
}
