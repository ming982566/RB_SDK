#include "Pages.h"
#include <array>

namespace {
std::string J(const std::string &s) {
  std::string o = "\"";
  for (char c : s) {
    if (c == '\"' || c == '\\')
      o += '\\';
    o += c;
  }
  return o + '\"';
}
class GamePage final : public Page {
  SdkRuntime &s_;
  HWND games_{}, telemetry_{}, raw_{}, install_{}, source_{};
  std::array<HWND, 12> e_{};
  RB_GameCatalog gc_{};
  RB_TelemetryCatalog tc_{};
  RB_GameConfig cfg_{};
  int selected_{-1};
  std::string key_;

public:
  GamePage(HWND h, SdkRuntime &s) : Page(h), s_(s) {}
  void Build() override {
    Header(L"游戏和遥测", L"游戏、安装状态和遥测键均由 SDK 动态读取；"
                          L"配置使用完整游戏配置结构体。");
    games_ = ui::List(hwnd_, 100, 20, 85, 280, 260);
    install_ = ui::Label(hwnd_, L"安装状态", 20, 355, 280, 90);
    ui::Button(hwnd_, 101, L"刷新目录/扫描", 20, 455, 130);
    ui::Button(hwnd_, 102, L"选择游戏", 160, 455, 100);
    ui::Button(hwnd_, 103, L"自动检测", 20, 495, 100);
    ui::Button(hwnd_, 104, L"启动", 130, 495, 80);
    ui::Button(hwnd_, 105, L"停止", 220, 495, 80);
    ui::Button(hwnd_, 106, L"自动配置", 20, 535, 100);
    ui::Button(hwnd_, 107, L"检查侧配置", 130, 535, 110);
    ui::Button(hwnd_, 108, L"安装插件", 20, 575, 100);
    source_ = ui::Label(hwnd_, L"数据源：-", 20, 625, 280, 80);
    const wchar_t *l[] = {
        L"数据源端口", L"数据源 IP", L"远程主机 IP", L"转发端口",
        L"转发 IP", L"启用转发", L"直接连接", L"串口",
        L"串口波特率", L"配置内容", L"游戏路径", L"保留"};
    for (int i = 0; i < 11; ++i) {
      ui::Label(hwnd_, l[i], 330, 90 + i * 38, 150);
      e_[i] = ui::Edit(hwnd_, 120 + i, 480, 86 + i * 38, 160);
    }
    ui::Button(hwnd_, 140, L"应用并重连", 330, 525, 130);
    ui::Button(hwnd_, 141, L"保存并回读验证", 475, 525, 150);
    telemetry_ = ui::List(hwnd_, 150, 660, 85, 430, 520, LBS_NOINTEGRALHEIGHT);
    raw_ =
        ui::Label(hwnd_, L"选择遥测项查看原始值和处理值", 660, 620, 430, 30);
    Refresh();
  }
  void Refresh() {
    SendMessageW(games_, LB_RESETCONTENT, 0, 0);
    if (RB_Game_GetCatalog(&gc_) != RB_OK) {
      Status(L"读取游戏目录失败", true);
      return;
    }
    for (int i = 0; i < gc_.Count; ++i) {
      int n = static_cast<int>(SendMessageW(
          games_, LB_ADDSTRING, 0,
          reinterpret_cast<LPARAM>(Utf8ToWide(gc_.Items[i].GameName).c_str())));
      SendMessageW(games_, LB_SETITEMDATA, n, i);
    }
    if (gc_.Count > 0)
      SendMessageW(games_, LB_SETCURSEL, 0, 0);
    const int installCount = RB_Game_RefreshInstallations();
    if (installCount < 0)
      Status(L"游戏安装扫描失败: " + ResultText(installCount), true);
    SendMessageW(telemetry_, LB_RESETCONTENT, 0, 0);
    if (RB_Telemetry_GetCatalog(&tc_) == RB_OK)
      for (int i = 0; i < tc_.Count; ++i) {
        std::wstring t = Utf8ToWide(tc_.Items[i].Name) + L"  [" +
                         Utf8ToWide(tc_.Items[i].Key) + L"] " +
                         Utf8ToWide(tc_.Items[i].Unit);
        int n = static_cast<int>(SendMessageW(
            telemetry_, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(t.c_str())));
        SendMessageW(telemetry_, LB_SETITEMDATA, n, i);
      }
    Status(L"游戏、安装和遥测目录已刷新");
  }
  void Select() {
    int row = static_cast<int>(SendMessageW(games_, LB_GETCURSEL, 0, 0));
    if (row < 0)
      return;
    selected_ = static_cast<int>(SendMessageW(games_, LB_GETITEMDATA, row, 0));
    auto &i = gc_.Items[selected_];
    key_ = i.GameKey;
    int rc = RB_Game_SelectByKey(key_.c_str());
    if (rc != RB_OK) {
      Status(L"选择游戏失败: " + ResultText(rc), true);
      return;
    }
    Read();
    RB_GameInstallInfo inf{};
    if (RB_Game_FindInstallByKey(key_.c_str(), &inf) == RB_OK) {
      std::wstring t = (inf.Found ? L"已安装: " : L"未找到: ") +
                       Utf8ToWide(inf.InstallPath) +
                       (inf.Running ? L"\r\n正在运行" : L"\r\n未运行");
      SetWindowTextW(install_, t.c_str());
    } else
      SetWindowTextW(install_, L"未找到安装信息");
  }
  void Read() {
    int rc = RB_Game_ReadConfig(key_.c_str(), &cfg_);
    if (rc != RB_OK) {
      Status(L"读取游戏配置失败: " + ResultText(rc), true);
      return;
    }
    int vals[] = {cfg_.SourcePort,        0, 0,
                  cfg_.ForwardingPort,    0, cfg_.Forwarding,
                  cfg_.DirectlyConnected, 0, cfg_.SerialBaudRate};
    ui::SetInt(e_[0], vals[0]);
    SetWindowUtf8(e_[1], cfg_.SourceIP);
    SetWindowUtf8(e_[2], cfg_.RemotehostIP);
    ui::SetInt(e_[3], cfg_.ForwardingPort);
    SetWindowUtf8(e_[4], cfg_.ForwardingIP);
    ui::SetInt(e_[5], cfg_.Forwarding);
    ui::SetInt(e_[6], cfg_.DirectlyConnected);
    SetWindowUtf8(e_[7], cfg_.SerialPort);
    ui::SetInt(e_[8], cfg_.SerialBaudRate);
    SetWindowUtf8(e_[9], cfg_.Configuration);
    SetWindowUtf8(e_[10], cfg_.GamePath);
  }
  bool Pull() {
    cfg_.SourcePort = ui::Int(e_[0]);
    cfg_.ForwardingPort = ui::Int(e_[3]);
    cfg_.Forwarding = ui::Int(e_[5]);
    cfg_.DirectlyConnected = ui::Int(e_[6]);
    cfg_.SerialBaudRate = ui::Int(e_[8]);
    return SetUtf8(cfg_.SourceIP, sizeof cfg_.SourceIP,
                   GetWindowString(e_[1])) &&
           SetUtf8(cfg_.RemotehostIP, sizeof cfg_.RemotehostIP,
                   GetWindowString(e_[2])) &&
           SetUtf8(cfg_.ForwardingIP, sizeof cfg_.ForwardingIP,
                   GetWindowString(e_[4])) &&
           SetUtf8(cfg_.SerialPort, sizeof cfg_.SerialPort,
                   GetWindowString(e_[7])) &&
           SetUtf8(cfg_.Configuration, sizeof cfg_.Configuration,
                   GetWindowString(e_[9])) &&
           SetUtf8(cfg_.GamePath, sizeof cfg_.GamePath,
                   GetWindowString(e_[10]));
  }
  void OnShow() override { Refresh(); }
  void OnState(const RB_RuntimeState &st) override {
    SetWindowTextW(source_,
                   (L"数据源：" + Utf8ToWide(st.Source.StatusText) +
                    L"\r\n数据包/活跃：" +
                    std::to_wstring(st.Source.TelemetryActive) +
                    L"  过渡 " +
                    FormatDouble(st.Source.TransitionPercent * 100) + L"%")
                       .c_str());
    int r = static_cast<int>(SendMessageW(telemetry_, LB_GETCURSEL, 0, 0));
    if (r >= 0) {
      int i = static_cast<int>(SendMessageW(telemetry_, LB_GETITEMDATA, r, 0));
      SetWindowTextW(raw_,
                     (L"原始值 " + FormatDouble(RB_Telemetry_GetRawValue(i), 5) +
                      L" / 处理值 " +
                      FormatDouble(RB_Telemetry_GetProcessedValue(i), 5))
                         .c_str());
    }
  }
  void OnCommand(int id, int code, HWND) override {
    if (id == 100 && code == LBN_DBLCLK)
      Select();
    else if (id == 101)
      Refresh();
    else if (id == 102)
      Select();
    else if (id == 103)
      Cmd("game.autoDetect", "{}");
    else if ((id == 104 || id == 105) && !key_.empty())
      Cmd(id == 104 ? "game.launch" : "game.terminate",
          "{\"gameKey\":" + J(key_) + "}");
    else if (id == 106)
      Report(RB_Game_AutoConfigureCurrent(), L"自动配置");
    else if (id == 107) {
      if (Pull())
        Report(RB_Game_CheckCurrentSideConfig(&cfg_), L"侧配置检查");
    } else if (id == 108) {
      RB_GameInstallInfo i{};
      int rc = RB_Game_FindInstallByKey(key_.c_str(), &i);
      Report(rc == RB_OK ? RB_Game_InstallCurrentPlugin(i.InstallPath) : rc,
             L"插件安装");
    } else if (id == 140) {
      if (selected_ < 0 || selected_ >= gc_.Count) {
        Status(L"请先选择游戏", true);
      } else if (Pull())
        Report(RB_Game_AutoConnect(gc_.Items[selected_].CatalogIndex, &cfg_),
               L"应用并重连数据源");
    } else if (id == 141) {
      if (key_.empty()) {
        Status(L"请先选择要保存配置的游戏", true);
        return;
      }
      if (!Pull()) {
        Status(L"文本超出 SDK 固定缓冲区", true);
        return;
      }
      int rc = RB_Game_SaveConfig(key_.c_str(), &cfg_);
      if (rc != RB_OK) {
        Report(rc, L"保存");
        return;
      }
      RB_GameConfig saved{};
      rc = RB_Game_ReadConfig(key_.c_str(), &saved);
      if (rc != RB_OK) {
        Report(rc, L"保存成功，但回读失败");
        return;
      }
      cfg_ = saved;
      Report(RB_OK, L"保存并回读");
    }
  }
  void Report(int rc, const wchar_t *n) {
    Status(std::wstring(n) + L": " + ResultText(rc), rc != RB_OK);
  }
  void Cmd(const char *c, const std::string &a) {
    std::wstring r, e;
    bool ok = s_.Command(c, a, r, e);
    Status(ok ? r : e, !ok);
  }
};
} // namespace
std::unique_ptr<Page> MakeGamePage(HWND h, SdkRuntime &s) {
  return std::make_unique<GamePage>(h, s);
}
