#include "Pages.h"

#include <algorithm>
#include <array>
#include <utility>

namespace {
class SystemPage final : public Page {
  std::array<HWND, 3> checks_{};
  HWND interval_{};
  RB_SystemConfig config_{};

public:
  SystemPage(HWND host, SdkRuntime &) : Page(host) {}

  void Build() override {
    Header(L"系统与循环",
           L"只管理公共 SDK 运行设置；语言、托盘和后台启动由宿主产品自行实现。");
    checks_[0] = ui::Check(hwnd_, 1200, L"启用运行日志", 30, 100, 220);
    checks_[1] = ui::Check(hwnd_, 1201, L"允许遥测数据显示", 30, 140, 220);
    checks_[2] = ui::Check(hwnd_, 1202, L"启用 VR 补偿", 30, 180, 220);
    ui::Button(hwnd_, 1203, L"读取设置", 30, 230);
    ui::Button(hwnd_, 1204, L"保存设置", 145, 230);

    ui::Group(hwnd_, L"SDK 内部计算循环", 300, 90, 520, 190);
    ui::Label(hwnd_, L"间隔 (ms，1-1000)", 325, 130, 180);
    interval_ = ui::Edit(hwnd_, 1210, 500, 126, 100, L"2");
    ui::Button(hwnd_, 1211, L"应用间隔", 620, 124);
    ui::Button(hwnd_, 1212, L"启动/恢复", 325, 180, 110);
    ui::Button(hwnd_, 1213, L"暂停", 450, 180);
    ui::Button(hwnd_, 1214, L"停止", 565, 180);
    Read();
  }

  void OnShow() override { Read(); }

  void Read() {
    int rc = RB_System_ReadConfigOrDefault(&config_);
    if (rc != RB_OK) {
      Status(L"读取系统设置失败: " + ResultText(rc), true);
      return;
    }
    ui::SetChecked(checks_[0], config_.EnableLogging);
    ui::SetChecked(checks_[1], config_.TelemetryShow);
    ui::SetChecked(checks_[2], config_.VRCompensation);
    ui::SetInt(interval_, RB_Runtime_GetLoopIntervalMs());
  }

  void OnCommand(int id, int, HWND) override {
    if (id == 1203) {
      Read();
    } else if (id == 1204) {
      config_.EnableLogging = ui::Checked(checks_[0]);
      config_.TelemetryShow = ui::Checked(checks_[1]);
      config_.VRCompensation = ui::Checked(checks_[2]);
      const int rc = RB_System_SaveConfig(&config_);
      Status(L"保存系统设置: " + ResultText(rc), rc != RB_OK);
    } else if (id == 1211) {
      const int rc = RB_Runtime_SetLoopIntervalMs(ui::Int(interval_, 2));
      Status(L"设置循环间隔: " + ResultText(rc), rc != RB_OK);
    } else if (id == 1212) {
      int rc = RB_Runtime_IsLoopRunning() ? RB_Runtime_ResumeLoop()
                                         : RB_Runtime_StartLoop(ui::Int(interval_, 2));
      Status(L"启动/恢复循环: " + ResultText(rc), rc != RB_OK);
    } else if (id == 1213) {
      const int rc = RB_Runtime_PauseLoop();
      Status(L"暂停循环: " + ResultText(rc), rc != RB_OK);
    } else if (id == 1214) {
      const int rc = RB_Runtime_StopLoop();
      Status(L"停止循环: " + ResultText(rc), rc != RB_OK);
    }
  }
};

class CanStatusPage final : public Page {
  HWND controller_{}, firmware_{}, motors_{};
  std::wstring motorSnapshot_;

public:
  CanStatusPage(HWND host, SdkRuntime &) : Page(host) {}

  void Build() override {
    Header(L"CAN 状态", L"只读取 SDK 状态包，不提供电机控制或固件升级操作。");
    controller_ = ui::Label(hwnd_, L"控制器", 30, 90, 1120, 55);
    firmware_ = ui::Label(hwnd_, L"固件", 30, 150, 1120, 45);
    motors_ = ui::List(hwnd_, 1250, 30, 215, 1160, 470, LBS_NOINTEGRALHEIGHT);
  }

  void OnState(const RB_RuntimeState &state) override {
    const RB_CanState &can = state.Can;
    SetWindowTextW(controller_,
      (Utf8ToWide(can.StatusText) + L"  电机=" + std::to_wstring(can.MotorCount) +
       L"  固件错误=" + std::to_wstring(can.FirmwareUpdateErrorCode)).c_str());
    SetWindowTextW(firmware_,
      (L"固件更新：运行=" + std::to_wstring(can.FirmwareUpdateRunning) +
       L" 完成=" + std::to_wstring(can.FirmwareUpdateComplete) +
       L" 成功=" + std::to_wstring(can.FirmwareUpdateSuccess) +
       L" 进度=" + std::to_wstring(can.FirmwareUpdateProgress) + L"%  " +
       Utf8ToWide(can.FirmwareUpdateStatusText)).c_str());
    std::wstring snapshot;
    std::array<std::wstring, RB_MAX_CAN_MOTORS> rows{};
    const int motorCount =
        (std::max)(0, (std::min)(can.MotorCount, static_cast<int>(RB_MAX_CAN_MOTORS)));
    for (int i = 0; i < motorCount; ++i) {
      const RB_CanMotorState &m = can.Motors[i];
      rows[i] = Utf8ToWide(m.AxisKey) + L" 节点=" + 
        std::to_wstring(m.NodeId) + L"  在线=" +
        std::to_wstring(m.Online) + L" 就绪=" + std::to_wstring(m.Ready) +
        L" 已回零=" + std::to_wstring(m.Homed) + L" 故障=" +
        std::to_wstring(m.Fault) + L" 模式=" + std::to_wstring(m.OperationMode) +
        L" 状态字=" + std::to_wstring(m.StatusWord) + L" 错误=" +
        std::to_wstring(m.ErrorCode) + L" 温度=" + std::to_wstring(m.Temperature) + L" °C";
      snapshot += rows[i] + L"\n";
    }
    if (snapshot != motorSnapshot_) {
      motorSnapshot_ = std::move(snapshot);
      SendMessageW(motors_, LB_RESETCONTENT, 0, 0);
      for (int i = 0; i < motorCount; ++i)
        SendMessageW(motors_, LB_ADDSTRING, 0,
                     reinterpret_cast<LPARAM>(rows[i].c_str()));
    }
  }
};

class ExternalSourcePage final : public Page {
  HWND sourceKey_{}, sourceName_{}, timeout_{}, transition_{}, telemetry_{},
       value_{}, stateText_{};
  RB_TelemetryCatalog catalog_{};
  unsigned long long sequence_{};

public:
  ExternalSourcePage(HWND host, SdkRuntime &) : Page(host) {}

  void Build() override {
    Header(L"外部数据源",
           L"用于宿主直接采集游戏遥测；MMF 或 UDP 游戏仍在自定义游戏页注册。");
    ui::Label(hwnd_, L"数据源键", 30, 95, 120);
    sourceKey_ = ui::Edit(hwnd_, 1300, 160, 91, 300, L"vendor.external_game");
    ui::Label(hwnd_, L"显示名称", 30, 140, 120);
    sourceName_ = ui::Edit(hwnd_, 1301, 160, 136, 300, L"外部游戏");
    ui::Label(hwnd_, L"超时 (ms)", 30, 185, 120);
    timeout_ = ui::Edit(hwnd_, 1302, 160, 181, 120, L"500");
    ui::Label(hwnd_, L"过渡 (ms)", 300, 185, 120);
    transition_ = ui::Edit(hwnd_, 1303, 410, 181, 120, L"2000");
    ui::Button(hwnd_, 1304, L"打开数据源", 30, 235, 120);
    ui::Button(hwnd_, 1305, L"关闭数据源", 165, 235, 120);

    ui::Label(hwnd_, L"提交遥测项", 30, 310, 120);
    telemetry_ = ui::Combo(hwnd_, 1306, 160, 305, 520);
    ui::Label(hwnd_, L"值", 700, 310, 40);
    value_ = ui::Edit(hwnd_, 1307, 745, 305, 130, L"0");
    ui::Button(hwnd_, 1308, L"提交完整帧", 895, 303, 130);
    stateText_ = ui::Label(hwnd_, L"数据源未打开", 30, 380, 1050, 80);
    RefreshTelemetry();
  }

  void RefreshTelemetry() {
    SendMessageW(telemetry_, CB_RESETCONTENT, 0, 0);
    if (RB_Telemetry_GetCatalog(&catalog_) != RB_OK)
      return;
    for (int i = 0; i < catalog_.Count; ++i)
      ui::ComboAdd(telemetry_, Utf8ToWide(catalog_.Items[i].Name) + L" [" +
                       Utf8ToWide(catalog_.Items[i].Key) + L"]", i);
    if (catalog_.Count)
      SendMessageW(telemetry_, CB_SETCURSEL, 0, 0);
  }

  void OnCommand(int id, int, HWND) override {
    if (id == 1304) {
      RB_ExternalSourceDesc desc{};
      desc.Size = sizeof(desc);
      desc.Version = 1;
      desc.DataTimeoutMs = ui::Int(timeout_, 500);
      desc.TransitionMs = ui::Int(transition_, 2000);
      const bool valid = SetUtf8(desc.SourceKey, sizeof(desc.SourceKey), GetWindowString(sourceKey_)) &&
                         SetUtf8(desc.SourceName, sizeof(desc.SourceName), GetWindowString(sourceName_));
      const int rc = valid ? RB_ExternalSource_Open(&desc) : RB_INVALID_ARGUMENT;
      Status(L"打开外部数据源：" + ResultText(rc), rc != RB_OK);
    } else if (id == 1305) {
      RB_ExternalSource_Close();
      Status(L"外部数据源已关闭");
    } else if (id == 1308) {
      const int item = ui::ComboData(telemetry_);
      if (item < 0 || item >= catalog_.Count) {
        Status(L"请选择有效遥测项", true);
        return;
      }
      RB_ExternalTelemetryFrame frame{};
      frame.Size = sizeof(frame);
      frame.Version = 1;
      frame.Sequence = ++sequence_;
      frame.ValueCount = 1;
      frame.Values[0].Index = catalog_.Items[item].Index;
      frame.Values[0].Value = ui::Double(value_);
      const int rc = RB_ExternalSource_SubmitFrame(&frame);
      Status(L"提交外部遥测帧: " + ResultText(rc), rc != RB_OK);
    }
  }

  void OnTimer(UINT_PTR) override {
    RB_ExternalSourceState state{};
    const int rc = RB_ExternalSource_GetState(&state);
    if (rc == RB_OK)
      SetWindowTextW(stateText_,
        (L"打开=" + std::to_wstring(state.Open) + L" 已连接=" +
         std::to_wstring(state.Connected) + L" 就绪=" +
         std::to_wstring(state.Ready) + L" 序号=" +
         std::to_wstring(state.LastSequence) + L" 帧数=" +
         std::to_wstring(state.SubmittedFrameCount)).c_str());
  }

  void Shutdown() override { RB_ExternalSource_Close(); }
};
} // namespace

std::unique_ptr<Page> MakeSystemPage(HWND h, SdkRuntime &s) {
  return std::make_unique<SystemPage>(h, s);
}
std::unique_ptr<Page> MakeCanStatusPage(HWND h, SdkRuntime &s) {
  return std::make_unique<CanStatusPage>(h, s);
}
std::unique_ptr<Page> MakeExternalSourcePage(HWND h, SdkRuntime &s) {
  return std::make_unique<ExternalSourcePage>(h, s);
}
