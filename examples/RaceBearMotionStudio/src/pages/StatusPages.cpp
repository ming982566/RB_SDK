#include "Pages.h"
#include <array>
#include <commdlg.h>
#include <fstream>
#include <sstream>

namespace {
std::wstring LocalizeStatusText(const char *text) {
  const std::wstring value = Utf8ToWide(text);
  if (value == L"No Source")
    return L"无数据源";
  if (value == L"CAN output not configured")
    return L"未配置 CAN 输出";
  if (value == L"Ready")
    return L"就绪";
  if (value == L"Idle")
    return L"空闲";
  return value;
}

std::string JsonString(const std::wstring &w) {
  std::string s = WideToUtf8(w), o = "\"";
  for (char c : s) {
    if (c == '\"' || c == '\\')
      o += '\\';
    o += c;
  }
  return o + '\"';
}
class Overview final : public Page {
  SdkRuntime &s_;
  std::vector<HWND> v_;
  RB_RuntimeState state_{};

public:
  Overview(HWND h, SdkRuntime &s) : Page(h), s_(s) {}
  void Build() override {
    Header(L"总览",
           L"高频刷新只读取原子状态快照，后端计算由 SDK 2 ms 内部循环负责。");
    const wchar_t *n[] = {L"SDK 与内部循环",
                          L"授权",
                          L"当前游戏",
                          L"遥测",
                          L"车速 / RPM / 档位",
                          L"平台 / 轴数",
                          L"最终 6DOF",
                          L"输出",
                          L"风感",
                          L"安全带",
                          L"CAN",
                          L"最近动作",
                          L"最近错误"};
    ui::Caption(hwnd_, L"运行与数据源", 28, 96, 240);
    for (int i = 0; i < 5; ++i) {
      ui::Label(hwnd_, n[i], 28, 132 + i * 42, 145);
      v_.push_back(ui::Label(hwnd_, L"-", 190, 132 + i * 42, 400));
    }
    ui::Caption(hwnd_, L"平台与输出", 650, 96, 240);
    for (int i = 5; i < 11; ++i) {
      ui::Label(hwnd_, n[i], 650, 132 + (i - 5) * 42, 130);
      v_.push_back(ui::Label(hwnd_, L"-", 800, 132 + (i - 5) * 42, 410));
    }
    ui::Caption(hwnd_, L"诊断", 28, 380, 240);
    for (int i = 11; i < 13; ++i) {
      ui::Label(hwnd_, n[i], 28, 416 + (i - 11) * 42, 145);
      v_.push_back(ui::Label(hwnd_, L"-", 190, 416 + (i - 11) * 42, 1020));
    }
    ui::Button(hwnd_, 1, L"暂停内部循环", 28, 516, 150);
    ui::Button(hwnd_, 2, L"恢复内部循环", 190, 516, 150);
  }
  void OnState(const RB_RuntimeState &s) override {
    state_ = s;
    std::wstring dof;
    for (double x : s.Rig.MotionState.RigFiltered)
      dof += FormatDouble(x) + L"  ";
    std::wstring can = LocalizeStatusText(s.Can.StatusText) + L"，电机 " +
                       std::to_wstring(s.Can.MotorCount);
    std::wstring vals[] = {
        Utf8ToWide(RB_Runtime_GetVersion()) + L" / " +
            (RB_Runtime_IsLoopRunning() ? L"运行" : L"停止") + L" / " +
            std::to_wstring(RB_Runtime_GetLoopIntervalMs()) + L" ms",
        std::to_wstring(s.License.State) +
            (s.License.OutputAllowed ? L"（输出允许）" : L"（输出锁定）"),
        Utf8ToWide(s.Game.GameName),
        LocalizeStatusText(s.Source.StatusText),
        FormatDouble(s.Source.FinalSpeed * 3.6) + L" km/h / " +
            FormatDouble(s.Source.FinalRPM, 0) + L" rpm / " +
            FormatDouble(s.Source.FinalGear, 0),
        Utf8ToWide(s.Rig.RigName) + L" / " +
            std::to_wstring(s.Rig.NumberOfAxes),
        dof,
        Utf8ToWide(s.Output.OutName) + L" / " +
            (s.Output.AnyRuntimeActive ? L"运行" : L"空闲"),
        FormatDouble(s.Wind.CurrentInputKmh) + L" km/h",
        FormatDouble(s.Seatbelt.LeftOutputPercent) + L"% / " +
            FormatDouble(s.Seatbelt.RightOutputPercent) + L"%",
        can,
        Utf8ToWide(s.LastAction),
        Utf8ToWide(s.LastError)};
    for (size_t i = 0; i < v_.size(); ++i)
      SetWindowTextW(v_[i], vals[i].c_str());
  }
  void OnCommand(int id, int, HWND) override {
    int rc = id == 1   ? RB_Runtime_PauseLoop()
             : id == 2 ? RB_Runtime_ResumeLoop()
                       : RB_INVALID_ARGUMENT;
    Status((id == 1 ? L"暂停: " : L"恢复: ") + ResultText(rc), rc != RB_OK);
  }
};
class License final : public Page {
  SdkRuntime &s_;
  HWND state_{}, problem_{}, remain_{}, code_{}, input_{};

public:
  License(HWND h, SdkRuntime &s) : Page(h), s_(s) {}
  void Build() override {
    Header(L"授权",
           L"未授权时仍可编辑配置，所有真实输出操作由前端和 SDK 双重锁定。");
    state_ = ui::Label(hwnd_, L"-", 210, 100, 600);
    problem_ = ui::Label(hwnd_, L"-", 210, 140, 600);
    remain_ = ui::Label(hwnd_, L"-", 210, 180, 600);
    code_ = ui::Label(hwnd_, L"-", 210, 220, 600);
    ui::Label(hwnd_, L"状态", 30, 100, 160);
    ui::Label(hwnd_, L"异常原因", 30, 140, 160);
    ui::Label(hwnd_, L"剩余时间", 30, 180, 160);
    ui::Label(hwnd_, L"当前激活码", 30, 220, 160);
    ui::Label(hwnd_, L"输入激活码", 30, 280, 160);
    input_ = ui::Edit(hwnd_, 10, 210, 276, 420);
    ui::Button(hwnd_, 11, L"激活", 650, 274);
    ui::Button(hwnd_, 12, L"反激活", 30, 330);
    ui::Button(hwnd_, 13, L"手动检查", 140, 330);
  }
  void OnState(const RB_RuntimeState &s) override {
    const wchar_t *states[] = {L"未知",     L"检查中",   L"未授权 / 输出锁定",
                               L"永久授权", L"限时授权", L"试用"};
    SetWindowTextW(state_, s.License.State >= 0 && s.License.State < 6
                               ? states[s.License.State]
                               : L"异常状态");
    { const wchar_t *problems[] = {L"无异常",      L"未找到本地授权",
                                     L"授权文件无效", L"硬件不匹配",
                                     L"网络检查失败", L"授权服务器错误",
                                     L"授权过期",    L"授权被禁用",
                                     L"授权被撤销",  L"授权解析失败"};
      int p = s.License.Problem;
      SetWindowTextW(problem_, p >= 0 && p < 10 ? problems[p] : L"未知异常"); }
    long long x = s.License.RemainingSeconds;
    std::wstring r = std::to_wstring(x / 86400) + L" 天 " +
                     std::to_wstring(x % 86400 / 3600) + L" 小时 " +
                     std::to_wstring(x % 3600 / 60) + L" 分";
    SetWindowTextW(remain_, r.c_str());
    SetWindowUtf8(code_, s.License.ActivationCode);
  }
  void OnCommand(int id, int, HWND) override {
    if (id == 12 &&
        MessageBoxW(hwnd_, L"确定要反激活当前授权吗？反激活后输出将被锁定。",
                     L"确认反激活", MB_YESNO | MB_ICONWARNING) != IDYES)
      return;
    const char *c = id == 11   ? "license.activate"
                    : id == 12 ? "license.deactivate"
                    : id == 13 ? "license.check"
                               : nullptr;
    if (!c)
      return;
    std::string args =
        id == 11
            ? "{\"activationCode\":" + JsonString(GetWindowString(input_)) + "}"
            : "{}";
    std::wstring r, e;
    bool ok = s_.Command(c, args, r, e);
    Status(ok ? L"授权操作成功: " + r : L"授权操作失败: " + e, !ok);
  }
};
class Logs final : public Page {
  SdkRuntime &s_;
  HWND list_{};
  std::array<HWND, 4> buttons_{};
  std::vector<std::wstring> lines_;

public:
  Logs(HWND h, SdkRuntime &s) : Page(h), s_(s) {}
  void Build() override {
    Header(L"日志和诊断",
           L"SDK 日志队列持续排空；界面最多保留 2000 条，保存文件由宿主管理。");
    list_ = ui::List(hwnd_, 20, 20, 90, 1100, 600,
                     LBS_NOINTEGRALHEIGHT | LBS_USETABSTOPS);
    buttons_[0] = ui::Button(hwnd_, 21, L"清空", 20, 710);
    buttons_[1] = ui::Button(hwnd_, 22, L"保存日志", 130, 710);
    buttons_[2] = ui::Button(hwnd_, 23, L"打开资源监视器", 250, 710, 150);
    buttons_[3] = ui::Button(hwnd_, 24, L"关闭资源监视器", 410, 710, 150);
  }
  void OnSize(int w, int h) override {
    Page::OnSize(w, h);
    if (!list_)
      return;
    const int buttonY = h - 54;
    MoveWindow(list_, 20, 90, w - 40, buttonY - 104, TRUE);
    const int x[] = {20, 130, 250, 410};
    const int widths[] = {100, 100, 150, 150};
    for (int i = 0; i < 4; ++i)
      MoveWindow(buttons_[i], x[i], buttonY, widths[i], 34, TRUE);
  }
  void OnLog(const std::wstring &line) override {
    lines_.push_back(line);
    SendMessageW(list_, LB_ADDSTRING, 0,
                 reinterpret_cast<LPARAM>(line.c_str()));
    while (lines_.size() > 2000) {
      lines_.erase(lines_.begin());
      SendMessageW(list_, LB_DELETESTRING, 0, 0);
    }
    SendMessageW(list_, LB_SETCURSEL, lines_.size() - 1, 0);
  }
  void OnCommand(int id, int, HWND) override {
    if (id == 21) {
      std::wstring r, e;
      s_.Command("logs.clear", "{}", r, e);
      lines_.clear();
      SendMessageW(list_, LB_RESETCONTENT, 0, 0);
      Status(e.empty() ? L"日志已清空" : e, !e.empty());
    } else if (id == 22) {
      wchar_t path[MAX_PATH] = L"RaceBearMotionStudio.log";
      OPENFILENAMEW o{sizeof(o)};
      o.hwndOwner = hwnd_;
      o.lpstrFilter = L"日志文件\0*.log\0";
      o.lpstrFile = path;
      o.nMaxFile = MAX_PATH;
      o.Flags = OFN_OVERWRITEPROMPT;
      if (GetSaveFileNameW(&o)) {
        std::ofstream f(path, std::ios::binary);
        for (auto &l : lines_)
          f << WideToUtf8(l) << "\r\n";
        Status(L"日志已保存");
      }
    } else if (id == 23) {
      int rc = RB_Debug_OpenMonitor();
      Status(ResultText(rc), rc != RB_OK);
    } else if (id == 24) {
      RB_Debug_CloseMonitor();
      Status(L"资源监视器已关闭");
    }
  }
};
} // namespace
std::unique_ptr<Page> MakeOverviewPage(HWND h, SdkRuntime &s) {
  return std::make_unique<Overview>(h, s);
}
std::unique_ptr<Page> MakeLicensePage(HWND h, SdkRuntime &s) {
  return std::make_unique<License>(h, s);
}
std::unique_ptr<Page> MakeLogPage(HWND h, SdkRuntime &s) {
  return std::make_unique<Logs>(h, s);
}
