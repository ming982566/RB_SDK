#include "App.h"
#include <commctrl.h>

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR, int show) {
  SetProcessDPIAware();
  INITCOMMONCONTROLSEX icc{sizeof(icc), ICC_STANDARD_CLASSES |
                                            ICC_LISTVIEW_CLASSES |
                                            ICC_BAR_CLASSES};
  InitCommonControlsEx(&icc);
  App app;
  return app.Run(instance, show);
}
