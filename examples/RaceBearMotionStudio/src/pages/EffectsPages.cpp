#include "Pages.h"
#include <array>
#include <string>

namespace {
std::wstring DynamicInputDisplayName(const char *key, int inputType) {
  std::wstring name = Utf8ToWide(key ? key : "");
  // InputType 3-5 对应正式项目中的 Yaw/Roll/Pitch 姿态角输入。
  // Roll、Pitch 是历史配置 Key，显示 POS 后缀以免被误认为 DOF 名称。
  if (inputType >= 3 && inputType <= 5)
    name += L" [POS]";
  return name;
}

void FillTelemetry(HWND c, RB_TelemetryCatalog &tc) {
  SendMessageW(c, CB_RESETCONTENT, 0, 0);
  if (RB_Telemetry_GetCatalog(&tc) != RB_OK)
    return;
  for (int i = 0; i < tc.Count; ++i)
    ui::ComboAdd(c,
                 Utf8ToWide(tc.Items[i].Name) + L" [" +
                     Utf8ToWide(tc.Items[i].Key) + L"]",
                 tc.Items[i].Index);
}
class Dynamic final : public Page {
  SdkRuntime &s_;
  HWND profiles_{}, dofs_{}, inputs_{}, tele_{}, copy_{}, dofEnabled_{};
  std::array<HWND, 12> f_{};
  std::array<HWND, 8> sval_{};
  RB_DynamicV2Profile p_{};
  RB_DynamicInputCandidateCatalog candidates_{};
  int pi_{-1}, di_{0}, ii_{-1};

  static int SliderMax(int) { return 100; }

public:
  Dynamic(HWND h, SdkRuntime &s) : Page(h), s_(s) {}
  void Build() override {
    Header(L"动态滤波",
           L"每个 DOF、每个输入通道独立保存完整滤波参数。");
    ui::Label(hwnd_, L"配置方案", 20, 66, 100);
    profiles_ = ui::Combo(hwnd_, 300, 20, 85, 250);
    ui::Button(hwnd_, 301, L"读取", 290, 83);
    ui::Button(hwnd_, 302, L"应用", 402, 83);
    ui::Button(hwnd_, 303, L"保存并验证", 514, 83, 120);
    copy_ = ui::Edit(hwnd_, 304, 660, 85, 190, L"新配置名");
    ui::Button(hwnd_, 305, L"复制", 862, 83, 80);
    ui::Button(hwnd_, 306, L"删除", 954, 83, 80);
    ui::Button(hwnd_, 307, L"绑定当前游戏", 1046, 83, 130);
    ui::Label(hwnd_, L"DOF", 20, 121, 60);
    dofs_ = ui::Combo(hwnd_, 310, 20, 140, 200);
    const wchar_t *dn[] = {L"Sway", L"Surge", L"Heave", L"Yaw",
                           L"Roll", L"Pitch", L"Sway -> Roll", L"Surge -> Pitch"};
    for (int i = 0; i < 8; ++i)
      ui::ComboAdd(dofs_, dn[i], i);
    ui::Label(hwnd_, L"输入通道", 250, 121, 80);
    inputs_ = ui::Combo(hwnd_, 311, 250, 140, 230);
    ui::Button(hwnd_, 312, L"增加通道", 495, 138, 100);
    ui::Button(hwnd_, 313, L"删除通道", 607, 138, 100);
    ui::Label(hwnd_, L"可添加效果", 730, 121, 100);
    tele_ = ui::Combo(hwnd_, 314, 730, 140, 400);
    dofEnabled_ = ui::Check(hwnd_, 315, L"启用当前 DOF", 20, 178, 180);
    const wchar_t *n[] = {L"输入通道启用", L"滤波启用", L"输入范围（+/-）",
                          L"输入增益（+/-）", L"输出比例", L"限幅范围",
                          L"限幅锐度", L"平滑", L"死区",
                          L"冲洗", L"安全阈值", L"保护强度"};
    for (int i = 0; i < 12; ++i) {
      int col = i / 6, row = i % 6;
      const int baseX = 30 + col * 600;
      const int y = 218 + row * 56;
      ui::Label(hwnd_, n[i], baseX, y, 130);
      if (i < 2)
        f_[i] = ui::Check(hwnd_, 330 + i, L"启用", baseX + 140,
                          y - 4, 120);
      else if (i < 4)
        f_[i] = ui::Edit(hwnd_, 330 + i, baseX + 140, y - 4, 220);
      else {
        int smax = SliderMax(i);
        ui::Label(hwnd_, L"0%", baseX + 140, y, 28);
        f_[i] = ui::Slider(hwnd_, 330 + i, baseX + 168, y - 4,
                           250, 0, smax);
        const std::wstring maximum = std::to_wstring(smax) + L"%";
        ui::Label(hwnd_, maximum.c_str(), baseX + 426, y, 45);
        sval_[i - 4] = ui::Label(hwnd_, L"当前：0%", baseX + 485, y, 95);
      }
    }
    Refresh();
  }
  void Refresh() {
    SendMessageW(profiles_, CB_RESETCONTENT, 0, 0);
    int n = RB_Dynamic_GetProfileCount();
    for (int i = 0; i < n; ++i) {
      char b[RB_TEXT_MEDIUM]{};
      if (RB_Dynamic_GetProfileName(i, b, sizeof b) == RB_OK)
        ui::ComboAdd(profiles_, Utf8ToWide(b), i);
    }
    int cur = RB_Dynamic_ReadCurrentGameProfileIndex();
    ui::SelectComboData(profiles_, cur >= 0 ? cur : 0);
    Read();
  }
  void Read() {
    pi_ = ui::ComboData(profiles_);
    if (pi_ < 0)
      return;
    int rc = RB_Dynamic_ReadProfileByIndex(pi_, &p_);
    if (rc != RB_OK) {
      Status(L"读取动态配置失败", true);
      return;
    }
    di_ = 0;
    ui::SelectComboData(dofs_, 0);
    LoadDof();
  }
  void LoadDof() {
    SendMessageW(inputs_, CB_RESETCONTENT, 0, 0);
    auto &d = p_.Dofs[di_];
    ui::SetChecked(dofEnabled_, d.Enabled);
    for (int i = 0; i < d.InputCount; ++i)
      ui::ComboAdd(inputs_,
                   L"通道 " + std::to_wstring(i + 1) + L"  " +
                       DynamicInputDisplayName(d.Inputs[i].Key,
                                               d.Inputs[i].InputType),
                   i);
    ii_ = d.InputCount ? 0 : -1;
    if (ii_ >= 0) {
      ui::SelectComboData(inputs_, ii_);
      LoadInput();
    }
    LoadCandidates();
  }
  void LoadCandidates() {
    SendMessageW(tele_, CB_RESETCONTENT, 0, 0);
    candidates_ = {};
    // 候选目录按当前 DOF 过滤，不能把完整遥测目录直接暴露给用户选择。
    if (RB_Dynamic_GetInputCandidateCatalog(di_, &candidates_) != RB_OK)
      return;
    const auto &dof = p_.Dofs[di_];
    for (int candidateIndex = 0; candidateIndex < candidates_.Count; ++candidateIndex) {
      const auto &candidate = candidates_.Items[candidateIndex];
      bool exists = false;
      for (int inputIndex = 0; inputIndex < dof.InputCount; ++inputIndex)
        if (strcmp(dof.Inputs[inputIndex].Key, candidate.Key) == 0) {
          exists = true;
          break;
      }
      if (!exists)
        ui::ComboAdd(tele_,
                     DynamicInputDisplayName(candidate.Key,
                                             candidate.InputType),
                     candidateIndex);
    }
    if (SendMessageW(tele_, CB_GETCOUNT, 0, 0) > 0)
      SendMessageW(tele_, CB_SETCURSEL, 0, 0);
  }
  void LoadInput() {
    if (ii_ < 0)
      return;
    auto &i = p_.Dofs[di_].Inputs[ii_];
    ui::SetChecked(f_[0], i.Enabled);
    ui::SetChecked(f_[1], i.Filter.Enabled);
    ui::SetInt(f_[2], i.Filter.InMapping);
    ui::SetDouble(f_[3], i.Filter.InGain);
    int v[] = {i.Filter.OutScaling,  i.Filter.Proportion,
               i.Filter.Strength,    i.Filter.Smoothing,
               i.Filter.DeadZone,    i.Filter.Washout,
               i.Filter.Sensitivity, i.Filter.SensitivityStrength};
    for (int x = 0; x < 8; ++x) {
      int fIdx = 4 + x;
      int maxV = SliderMax(fIdx);
      int clamped = v[x] < 0 ? 0 : (v[x] > maxV ? maxV : v[x]);
      if (clamped != v[x]) {
        auto &fi = p_.Dofs[di_].Inputs[ii_].Filter;
        int *fp = &fi.OutScaling;
        fp[x] = clamped;
      }
      SendMessageW(f_[fIdx], TBM_SETPOS, TRUE, clamped);
      const std::wstring value = L"当前：" + std::to_wstring(clamped) + L"%";
      SetWindowTextW(sval_[x], value.c_str());
    }
  }
  void Store() {
    p_.Dofs[di_].Enabled = ui::Checked(dofEnabled_);
    if (ii_ < 0)
      return;
    auto &i = p_.Dofs[di_].Inputs[ii_];
    i.Enabled = ui::Checked(f_[0]);
    i.Filter.Enabled = ui::Checked(f_[1]);
    i.Filter.Dof = i.InputType;
    i.Filter.InMapping = ui::Int(f_[2]);
    i.Filter.InGain = ui::Double(f_[3]);
    int *v = &i.Filter.OutScaling;
    for (int x = 0; x < 8; ++x)
      v[x] = static_cast<int>(SendMessageW(f_[4 + x], TBM_GETPOS, 0, 0));
  }
  void OnShow() override { Refresh(); }
  void OnHScroll(int id, int) override {
    for (int x = 0; x < 8; ++x)
      if (f_[4 + x] && GetDlgCtrlID(f_[4 + x]) == id) {
        int p = static_cast<int>(SendMessageW(f_[4 + x], TBM_GETPOS, 0, 0));
        const std::wstring value = L"当前：" + std::to_wstring(p) + L"%";
        SetWindowTextW(sval_[x], value.c_str());
        Store();
        break;
      }
  }
  void OnCommand(int id, int code, HWND) override {
    if (id == 300 && code == CBN_SELCHANGE)
      Read();
    else if (id == 310 && code == CBN_SELCHANGE) {
      Store();
      di_ = ui::ComboData(dofs_);
      LoadDof();
    } else if (id == 311 && code == CBN_SELCHANGE) {
      Store();
      ii_ = ui::ComboData(inputs_);
      LoadInput();
    } else if (id == 312) {
      Store();
      auto &d = p_.Dofs[di_];
      if (d.InputCount >= RB_DYNAMIC_MAX_INPUTS) {
        Status(L"已达到通道上限", true);
        return;
      }
      int candidateIndex = ui::ComboData(tele_);
      if (candidateIndex < 0 || candidateIndex >= candidates_.Count) {
        Status(L"当前 DOF 没有可添加的通道效果", true);
        return;
      }
      const auto &candidate = candidates_.Items[candidateIndex];
      d.Enabled = 1;  // 新增通道时同步启用 DOF，确保 SDK 保存这组参数。
      auto &i = d.Inputs[d.InputCount++];
      i = {};
      i.Enabled = 1;
      i.InputType = candidate.InputType;
      i.TelemetryIndex = candidate.TelemetryIndex;
      strcpy_s(i.Key, candidate.Key);
      i.Filter.Enabled = 1;
      i.Filter.Dof = i.InputType;
      i.Filter.InMapping = 20;
      i.Filter.InGain = 1;
      i.Filter.OutScaling = 100;
      LoadDof();
      ii_ = d.InputCount - 1;
      ui::SelectComboData(inputs_, ii_);
      LoadInput();
      LoadCandidates();
    } else if (id == 313 && ii_ >= 0) {
      auto &d = p_.Dofs[di_];
      for (int i = ii_; i + 1 < d.InputCount; ++i)
        d.Inputs[i] = d.Inputs[i + 1];
      d.Inputs[--d.InputCount] = {};
      LoadDof();
    } else if (id == 301)
      Read();
    else if (id == 302) {
      Store();
      Report(RB_Dynamic_ApplyProfileToCurrentRig(&p_), L"应用");
    } else if (id == 303) {
      Store();
      // 保存完整结构体前清空未使用槽位，避免删除通道后残留旧参数。
      for (int d = 0; d < 8; ++d) {
        auto &dof = p_.Dofs[d];
        for (int ii = dof.InputCount; ii < RB_DYNAMIC_MAX_INPUTS; ++ii)
          dof.Inputs[ii] = {};
      }
      int rc = RB_Dynamic_SaveProfileByIndex(pi_, &p_);
      if (rc != RB_OK) {
        Report(rc, L"保存");
        return;
      }
      RB_DynamicV2Profile saved{};
      rc = RB_Dynamic_ReadProfileByIndex(pi_, &saved);
      if (rc != RB_OK) {
        Report(rc, L"保存成功，但回读失败");
        return;
      }
      p_ = saved;
      Report(RB_OK, L"保存并回读");
    } else if (id == 305) {
      Store();
      auto n = WideToUtf8(GetWindowString(copy_));
      Report(n.empty() ? RB_INVALID_ARGUMENT
                       : RB_Dynamic_SaveProfile(n.c_str(), &p_),
             L"复制");
      Refresh();
    } else if (id == 306) {
      if (MessageBoxW(hwnd_, L"确定要删除当前动态滤波配置吗？此操作同时删除同名运动增强和震动配置。",
                       L"确认删除", MB_YESNO | MB_ICONWARNING) != IDYES) return;
      Report(RB_Dynamic_DeleteProfileByIndex(pi_), L"删除");
      Refresh();
    } else if (id == 307) {
      Report(RB_Dynamic_SaveCurrentGameProfileIndex(pi_), L"绑定当前游戏");
    }
  }
  void Report(int rc, const wchar_t *n) {
    Status(std::wstring(n) + L": " + ResultText(rc), rc != RB_OK);
  }
};

class Motion final : public Page {
  SdkRuntime &s_;
  HWND profile_{}, effect_{}, input_{}, secondary_{}, route_{};
  std::array<HWND, 10> e_{};
  RB_MotionEffectProfile p_{};
  RB_EffectCatalog cat_{};
  RB_TelemetryCatalog tc_{};
  int pi_{-1}, ei_{0}, ri_{0};

public:
  Motion(HWND h, SdkRuntime &s) : Page(h), s_(s) {}
  void Build() override {
    Header(L"运动增强", L"效果和遥测输入来自 SDK；每条输出路由独立编辑。");
    ui::Label(hwnd_, L"配置方案", 20, 66, 100);
    profile_ = ui::Combo(hwnd_, 400, 20, 85, 260);
    ui::Label(hwnd_, L"效果", 300, 66, 100);
    effect_ = ui::Combo(hwnd_, 401, 300, 85, 300);
    ui::Button(hwnd_, 402, L"读取", 620, 83);
    ui::Button(hwnd_, 403, L"应用", 725, 83);
    ui::Button(hwnd_, 404, L"保存并验证", 830, 83, 120);
    input_ = ui::Combo(hwnd_, 405, 180, 145, 390);
    secondary_ = ui::Combo(hwnd_, 406, 700, 145, 390);
    ui::Label(hwnd_, L"主输入", 20, 150, 150);
    ui::Label(hwnd_, L"辅助输入", 590, 150, 100);
    const wchar_t *n[] = {L"启用", L"增益", L"路由索引",
                          L"路由启用", L"输出 DOF", L"输出比例",
                          L"方向", L"阈值", L"频率 (Hz)",
                          L"持续时间 (s)"};
    for (int i = 0; i < 10; ++i) {
      ui::Label(hwnd_, n[i], 30 + (i / 5) * 450, 220 + (i % 5) * 48, 150);
      if (i == 0 || i == 3) {
        e_[i] = ui::Check(hwnd_, 420 + i, L"启用", 180 + (i / 5) * 450,
                          216 + (i % 5) * 48, 120);
      } else if (i == 4) {
        e_[i] = ui::Combo(hwnd_, 420 + i, 180, 216 + i * 48, 160);
        const wchar_t *dofNames[] = {L"Sway", L"Surge", L"Heave",
                                     L"Yaw", L"Roll", L"Pitch"};
        for (int dof = 0; dof < RB_DOF_COUNT; ++dof)
          ui::ComboAdd(e_[i], dofNames[dof], dof);
      } else {
        e_[i] = ui::Edit(hwnd_, 420 + i, 180 + (i / 5) * 450,
                         216 + (i % 5) * 48, 160);
      }
    }
    ui::Button(hwnd_, 450, L"上一条路由", 30, 485, 110);
    ui::Button(hwnd_, 451, L"下一条路由", 150, 485, 110);
    ui::Button(hwnd_, 452, L"增加路由", 270, 485, 100);
    ui::Button(hwnd_, 453, L"删除路由", 380, 485, 100);
    Refresh();
  }
  void Refresh() {
    RB_MotionEffect_GetCatalog(&cat_);
    SendMessageW(effect_, CB_RESETCONTENT, 0, 0);
    for (int i = 0; i < cat_.Count; ++i)
      ui::ComboAdd(effect_, Utf8ToWide(cat_.Items[i].Name),
                   cat_.Items[i].Index);
    SendMessageW(profile_, CB_RESETCONTENT, 0, 0);
    int n = RB_MotionEffect_GetProfileCount();
    for (int i = 0; i < n; ++i) {
      char b[128]{};
      if (RB_MotionEffect_GetProfileName(i, b, sizeof b) == RB_OK)
        ui::ComboAdd(profile_, Utf8ToWide(b), i);
    }
    FillTelemetry(input_, tc_);
    RB_TelemetryCatalog x{};
    FillTelemetry(secondary_, x);
    ui::SelectComboData(profile_, 0);
    ui::SelectComboData(effect_, cat_.Count ? cat_.Items[0].Index : 0);
    Read();
  }
  void Read() {
    pi_ = ui::ComboData(profile_);
    ei_ = ui::ComboData(effect_);
    if (pi_ < 0 || ei_ < 0)
      return;
    if (RB_MotionEffect_ReadProfileByIndex(pi_, &p_) != RB_OK)
      return;
    ri_ = 0;
    Load();
  }
  void Load() {
    auto &x = p_.Effects[ei_];
    ui::SetChecked(e_[0], x.Enabled);
    ui::SetDouble(e_[1], x.Gain);
    ui::SetInt(e_[2], ri_);
    auto &r = x.Routes[ri_];
    ui::SetChecked(e_[3], r.Enabled);
    ui::SelectComboData(e_[4], r.Dof);
    ui::SetDouble(e_[5], r.OutputRatio);
    ui::SetDouble(e_[6], r.Direction);
    ui::SetDouble(e_[7], r.Threshold);
    ui::SetDouble(e_[8], r.Frequency);
    ui::SetDouble(e_[9], r.Duration);
    ui::SelectComboData(input_, x.InputIndex);
    ui::SelectComboData(secondary_, x.SecondaryInputIndex);
  }
  void Store() {
    auto &x = p_.Effects[ei_];
    x.Enabled = ui::Checked(e_[0]);
    x.Gain = ui::Double(e_[1]);
    x.InputIndex = ui::ComboData(input_);
    x.SecondaryInputIndex = ui::ComboData(secondary_);
    auto &r = x.Routes[ri_];
    r.Enabled = ui::Checked(e_[3]);
    r.Dof = ui::ComboData(e_[4]);
    r.OutputRatio = ui::Double(e_[5]);
    r.Direction = ui::Double(e_[6]);
    r.Threshold = ui::Double(e_[7]);
    r.Frequency = ui::Double(e_[8]);
    r.Duration = ui::Double(e_[9]);
  }
  void OnShow() override { Refresh(); }
  void OnCommand(int id, int code, HWND) override {
    if ((id == 400 || id == 401) && code == CBN_SELCHANGE) {
      Store();
      Read();
    } else if (id == 402)
      Read();
    else if (id == 403) {
      Store();
      Report(RB_MotionEffect_ApplyProfileToCurrentRig(&p_), L"应用");
    } else if (id == 404) {
      Store();
      int rc = RB_MotionEffect_SaveProfileByIndex(pi_, &p_);
      if (rc != RB_OK) {
        Report(rc, L"保存");
        return;
      }
      RB_MotionEffectProfile saved{};
      rc = RB_MotionEffect_ReadProfileByIndex(pi_, &saved);
      if (rc != RB_OK) {
        Report(rc, L"保存成功，但回读失败");
        return;
      }
      p_ = saved;
      Report(RB_OK, L"保存并回读");
    } else if (id == 450 || id == 451) {
      Store();
      int count = std::max(1, p_.Effects[ei_].RouteCount);
      ri_ = id == 450 ? std::max(0, ri_ - 1) : std::min(count - 1, ri_ + 1);
      Load();
    } else if (id == 452) {
      Store();
      auto &x = p_.Effects[ei_];
      if (x.RouteCount < RB_MOTION_ROUTE_COUNT) {
        ri_ = x.RouteCount++;
        x.Routes[ri_] = {};
        x.Routes[ri_].Direction = 1;
        Load();
      }
    } else if (id == 453) {
      Store();
      auto &x = p_.Effects[ei_];
      if (x.RouteCount > 0) {
        for (int i = ri_; i + 1 < x.RouteCount; ++i)
          x.Routes[i] = x.Routes[i + 1];
        x.Routes[--x.RouteCount] = {};
        ri_ = std::max(0, std::min(ri_, x.RouteCount - 1));
        Load();
      }
    }
  }
  void Report(int r, const wchar_t *n) {
    Status(std::wstring(n) + L": " + ResultText(r), r != RB_OK);
  }
};

class Haptic final : public Page {
  SdkRuntime &s_;
  HWND profile_{}, effect_{}, in_{}, sec_{};
  std::array<HWND, 9> e_{};
  RB_HapticEffectProfile p_{};
  RB_EffectCatalog cat_{};
  RB_TelemetryCatalog tc_{};
  int pi_{}, ei_{};

public:
  Haptic(HWND h, SdkRuntime &s) : Page(h), s_(s) {}
  void Build() override {
    Header(L"震动",
           L"公共 SDK 支持完整配置和汇总状态，不提供逐效果运行值或手动测试。");
    ui::Label(hwnd_, L"配置方案", 20, 66, 100);
    profile_ = ui::Combo(hwnd_, 500, 20, 85, 260);
    ui::Label(hwnd_, L"效果", 300, 66, 100);
    effect_ = ui::Combo(hwnd_, 501, 300, 85, 300);
    ui::Button(hwnd_, 502, L"读取", 620, 83);
    ui::Button(hwnd_, 503, L"应用", 725, 83);
    ui::Button(hwnd_, 504, L"保存并验证", 830, 83, 120);
    in_ = ui::Combo(hwnd_, 505, 180, 145, 390);
    sec_ = ui::Combo(hwnd_, 506, 700, 145, 390);
    ui::Label(hwnd_, L"主输入", 20, 150, 150);
    ui::Label(hwnd_, L"辅助输入", 590, 150, 100);
    const wchar_t *n[] = {L"启用", L"输出通道 0-6", L"增益",
                          L"阈值", L"最小输出 (bit)", L"最大输出 (bit)",
                          L"频率 (Hz)", L"方向", L"效果类型"};
    for (int i = 0; i < 9; ++i) {
      ui::Label(hwnd_, n[i], 30 + (i / 5) * 450, 220 + (i % 5) * 48, 150);
      e_[i] = i == 0 ? ui::Check(hwnd_, 520 + i, L"启用", 180, 216, 120)
                     : ui::Edit(hwnd_, 520 + i, 180 + (i / 5) * 450,
                                216 + (i % 5) * 48, 160);
    }
    Refresh();
  }
  void Refresh() {
    RB_Haptic_GetCatalog(&cat_);
    SendMessageW(effect_, CB_RESETCONTENT, 0, 0);
    for (int i = 0; i < cat_.Count; ++i)
      ui::ComboAdd(effect_, Utf8ToWide(cat_.Items[i].Name),
                   cat_.Items[i].Index);
    SendMessageW(profile_, CB_RESETCONTENT, 0, 0);
    int n = RB_Dynamic_GetProfileCount();
    for (int i = 0; i < n; ++i) {
      char b[128]{};
      if (RB_Dynamic_GetProfileName(i, b, sizeof b) == RB_OK)
        ui::ComboAdd(profile_, Utf8ToWide(b), i);
    }
    FillTelemetry(in_, tc_);
    RB_TelemetryCatalog x{};
    FillTelemetry(sec_, x);
    ui::SelectComboData(profile_, 0);
    ui::SelectComboData(effect_, cat_.Count ? cat_.Items[0].Index : 0);
    Read();
  }
  void Read() {
    pi_ = ui::ComboData(profile_);
    ei_ = ui::ComboData(effect_);
    if (pi_ < 0 || ei_ < 0)
      return;
    if (RB_Haptic_ReadProfileByIndex(pi_, &p_) != RB_OK)
      return;
    Load();
  }
  void Load() {
    auto &x = p_.Effects[ei_];
    ui::SetChecked(e_[0], x.Enabled);
    int v[] = {x.OutputChannel};
    ui::SetInt(e_[1], v[0]);
    ui::SetDouble(e_[2], x.Gain);
    ui::SetDouble(e_[3], x.Threshold);
    ui::SetDouble(e_[4], x.MinOutput);
    ui::SetDouble(e_[5], x.MaxOutput);
    ui::SetDouble(e_[6], x.Frequency);
    ui::SetDouble(e_[7], x.Direction);
    ui::SetInt(e_[8], x.EffectType);
    ui::SelectComboData(in_, x.InputIndex);
    ui::SelectComboData(sec_, x.SecondaryInputIndex);
  }
  void Store() {
    auto &x = p_.Effects[ei_];
    x.Enabled = ui::Checked(e_[0]);
    x.OutputChannel = ui::Int(e_[1]);
    x.Gain = ui::Double(e_[2]);
    x.Threshold = ui::Double(e_[3]);
    x.MinOutput = ui::Double(e_[4]);
    x.MaxOutput = ui::Double(e_[5]);
    x.Frequency = ui::Double(e_[6]);
    x.Direction = ui::Double(e_[7]);
    x.EffectType = ui::Int(e_[8]);
    x.InputIndex = ui::ComboData(in_);
    x.SecondaryInputIndex = ui::ComboData(sec_);
  }
  void OnShow() override { Refresh(); }
  void OnCommand(int id, int code, HWND) override {
    if ((id == 500 || id == 501) && code == CBN_SELCHANGE) {
      Store();
      Read();
    } else if (id == 502)
      Read();
    else if (id == 503) {
      Store();
      Report(RB_Haptic_ApplyProfileToCurrentFunction(&p_), L"应用");
    } else if (id == 504) {
      Store();
      int rc = RB_Haptic_SaveProfileByIndex(pi_, &p_);
      if (rc != RB_OK) {
        Report(rc, L"保存");
        return;
      }
      RB_HapticEffectProfile saved{};
      rc = RB_Haptic_ReadProfileByIndex(pi_, &saved);
      if (rc != RB_OK) {
        Report(rc, L"保存成功，但回读失败");
        return;
      }
      p_ = saved;
      Report(RB_OK, L"保存并回读");
    }
  }
  void Report(int r, const wchar_t *n) {
    Status(std::wstring(n) + L": " + ResultText(r), r != RB_OK);
  }
};

class Wind final : public Page {
  SdkRuntime &s_;
  std::array<HWND, 14> e_{};
  std::array<HWND, 10> ws_{};       // sliders for items 3,5-13
  std::array<HWND, 10> wv_{};       // value display for sliders
  HWND state_{};
  RB_WindEffectConfig c_{};
  bool allowed_{};

  static int SliderIdx(int itemIdx) {
    if (itemIdx == 3) return 0;
    if (itemIdx >= 5) return itemIdx - 4; // 5->1, 6->2, ..., 13->9
    return -1;
  }
  static int SliderRangeMin(int itemIdx) {
    return itemIdx >= 6 ? -100 : 0; // ChannelGain/Offset are -100..100
  }

public:
  Wind(HWND h, SdkRuntime &s) : Page(h), s_(s) {}
  void Build() override {
    Header(L"风感", 
           L"速度统一使用 km/h；四通道补偿独立配置，离页与退出自动归零。");
    const wchar_t *n[] = {L"启用", L"起风速度 (km/h)", L"满风速度 (km/h)",
                          L"最低工作 %", L"伽马", L"总增益 %",
                          L"风感 1 增益", L"风感 2 增益", L"风感 3 增益",
                          L"风感 4 增益", L"风感 1 偏移", L"风感 2 偏移",
                          L"风感 3 偏移", L"风感 4 偏移"};
    for (int i = 0; i < 14; ++i) {
      bool sliderItem = (i == 3 || i >= 5);
      ui::Label(hwnd_, n[i], 30 + (i / 7) * 420, 100 + (i % 7) * 44, 150);
      if (i == 0)
        e_[i] = ui::Check(hwnd_, 600, L"", 180, 96, 50);
      else if (sliderItem) {
        int si = SliderIdx(i);
        int col = i / 7, row = i % 7;
        ws_[si] = ui::Slider(hwnd_, 600 + i, 180 + col * 420, 96 + row * 44,
                              100, SliderRangeMin(i), 100);
        wv_[si] = ui::Label(hwnd_, L"0", 285 + col * 420, 98 + row * 44, 42);
      } else
        e_[i] = ui::Edit(hwnd_, 600 + i, 180 + (i / 7) * 420,
                         96 + (i % 7) * 44, 150);
    }
    ui::Button(hwnd_, 620, L"读取", 30, 430);
    ui::Button(hwnd_, 621, L"应用", 140, 430);
    ui::Button(hwnd_, 622, L"保存并验证", 250, 430, 120);
    ui::Label(hwnd_, L"测试输出 %", 400, 435, 100);
    ui::Edit(hwnd_, 623, 510, 431, 100, L"0");
    ui::Button(hwnd_, 624, L"开始测试", 625, 429);
    ui::Button(hwnd_, 625, L"停止/归零", 735, 429);
    state_ = ui::Label(hwnd_, L"状态", 30, 500, 1000, 80);
    Read();
  }
  void Read() {
    int rc = RB_Wind_ReadConfig(&c_);
    if (rc != RB_OK)
      rc = RB_Wind_GetDefaultConfig(&c_);
    if (rc != RB_OK)
      return;
    ui::SetChecked(e_[0], c_.Enabled);
    double v[] = {c_.InputMinKmh,
                  c_.InputMaxKmh,
                  c_.OutputMinWorkPercent,
                  c_.Gamma,
                  c_.MasterGainPercent,
                  c_.ChannelGainPercent[0],
                  c_.ChannelGainPercent[1],
                  c_.ChannelGainPercent[2],
                  c_.ChannelGainPercent[3],
                  c_.ChannelOffsetPercent[0],
                  c_.ChannelOffsetPercent[1],
                  c_.ChannelOffsetPercent[2],
                  c_.ChannelOffsetPercent[3]};
    for (int i = 0; i < 13; ++i) {
      int idx = i + 1; // e_ index
      int si = (idx == 3 || idx >= 5) ? SliderIdx(idx) : -1;
      if (si >= 0) {
        int iv = static_cast<int>(v[i]);
        SendMessageW(ws_[si], TBM_SETPOS, TRUE, iv);
        SetWindowTextW(wv_[si], std::to_wstring(iv).c_str());
      } else
        ui::SetDouble(e_[idx], v[i]);
    }
  }
  void Pull() {
    c_.Enabled = ui::Checked(e_[0]);
    double *v = &c_.InputMinKmh;
    for (int i = 0; i < 5; ++i) {
      int idx = i + 1;
      int si = SliderIdx(idx);
      v[i] = si >= 0 ? static_cast<double>(static_cast<int>(SendMessageW(ws_[si], TBM_GETPOS, 0, 0)))
                     : ui::Double(e_[idx]);
    }
    for (int i = 0; i < 4; ++i) {
      int idx = 6 + i;
      int si = SliderIdx(idx);
      c_.ChannelGainPercent[i] = static_cast<double>(static_cast<int>(SendMessageW(ws_[si], TBM_GETPOS, 0, 0)));
    }
    for (int i = 0; i < 4; ++i) {
      int idx = 10 + i;
      int si = SliderIdx(idx);
      c_.ChannelOffsetPercent[i] = static_cast<double>(static_cast<int>(SendMessageW(ws_[si], TBM_GETPOS, 0, 0)));
    }
  }
  void OnHScroll(int id, int) override {
    for (int si = 0; si < 10; ++si)
      if (ws_[si] && GetDlgCtrlID(ws_[si]) == id) {
        int p = static_cast<int>(SendMessageW(ws_[si], TBM_GETPOS, 0, 0));
        SetWindowTextW(wv_[si], std::to_wstring(p).c_str());
        break;
      }
  }
  void OnShow() override { Read(); }
  void OnHide() override { Zero(); }
  void Shutdown() override { Zero(); }
  void OnState(const RB_RuntimeState &s) override {
    allowed_ = s.License.OutputAllowed;
    SetWindowTextW(state_,
                   (L"输入 " + FormatDouble(s.Wind.CurrentInputKmh) +
                    L" km/h  输出 " +
                    FormatDouble(s.Wind.ChannelNormalizedPercent[0]) + L"% / " +
                    FormatDouble(s.Wind.ChannelNormalizedPercent[1]) + L"% / " +
                    FormatDouble(s.Wind.ChannelNormalizedPercent[2]) + L"% / " +
                    FormatDouble(s.Wind.ChannelNormalizedPercent[3]) + L"%")
                       .c_str());
  }
  void OnCommand(int id, int, HWND) override {
    if (id == 620)
      Read();
    else if (id == 621) {
      Pull();
      Report(RB_Wind_ApplyConfigToCurrentFunction(&c_), L"应用");
    } else if (id == 622) {
      Pull();
      int rc = RB_Wind_SaveConfig(&c_);
      if (rc != RB_OK) {
        Report(rc, L"保存");
        return;
      }
      RB_WindEffectConfig saved{};
      rc = RB_Wind_ReadConfig(&saved);
      if (rc != RB_OK) {
        Report(rc, L"保存成功，但回读失败");
        return;
      }
      c_ = saved;
      Report(RB_OK, L"保存并回读");
    } else if (id == 624) {
      if (!allowed_) {
        Status(L"授权锁定，禁止真实测试输出", true);
        return;
      }
      Report(RB_Wind_SetTestOutput(1, ui::Double(GetDlgItem(hwnd_, 623))),
          L"测试");
    } else if (id == 625)
      Zero();
  }
  void Zero() {
    const int rc = RB_Wind_SetTestOutput(0, 0);
    if (rc != RB_OK && allowed_)
      Status(L"风感测试归零失败: " + ResultText(rc), true);
  }
  void Report(int r, const wchar_t *n) {
    Status(std::wstring(n) + L": " + ResultText(r), r != RB_OK);
  }
};

class Seatbelt final : public Page {
  SdkRuntime &s_;
  std::array<HWND, 23> e_{};
  std::array<HWND, 8> ss_{};       // sliders for items 1,4,5,7,8,10,18,19
  std::array<HWND, 8> sv_{};       // value display for sliders
  HWND mode_{}, state_{};
  RB_SeatbeltEffectConfig c_{};
  bool allowed_{};

  static int SeatSliderIdx(int item) {
    int map[] = {1,4,5,7,8,10,18,19};
    for (int i = 0; i < 8; ++i) if (map[i] == item) return i;
    return -1;
  }
  static int SeatSliderMax(int item) {
    int m[] = {4,8,10}; // items with max 200
    for (int i : m) if (i == item) return 200;
    return 100;
  }
  static bool IsSliderItem(int item) { return SeatSliderIdx(item) >= 0; }

public:
  Seatbelt(HWND h, SdkRuntime &s) : Page(h), s_(s) {}
  void Build() override {
    Header(L"安全带", L"左右通道独立；HID/混合模式可保留配置，但公共 SDK "
                      L"未开放 HID 数据注入。");
    ui::Label(hwnd_, L"输入模式", 30, 95, 140);
    mode_ = ui::Combo(hwnd_, 700, 180, 90, 220);
    ui::ComboAdd(mode_, L"遥测", 0);
    ui::ComboAdd(mode_, L"HID（需外部输入源）", 1);
    ui::ComboAdd(mode_, L"混合（需外部输入源）", 2);
    const wchar_t *n[] = {L"启用",
                          L"基础预紧力 %",
                          L"起始速度 (km/h)",
                          L"满速参数 (km/h，保留)",
                          L"总增益 %",
                          L"释放速度 %",
                          L"启用制动效果",
                          L"制动踏板增益 %",
                          L"制动减速度增益 %",
                          L"启用横向效果",
                          L"横向增益 %",
                          L"横向死区 (g)",
                          L"启用碰撞效果（保留）",
                          L"碰撞阈值 (g，保留)",
                          L"碰撞保持 (ms，保留)",
                          L"碰撞输出（保留）",
                          L"启用左通道",
                          L"启用右通道",
                          L"左通道比例 %",
                          L"右通道比例 %",
                          L"左通道反向",
                          L"右通道反向"};
    for (int i = 0; i < 22; ++i) {
      int col = i / 11, row = i % 11;
      ui::Label(hwnd_, n[i], 30 + col * 480, 135 + row * 36, 180);
      bool ck = i == 0 || i == 6 || i == 9 || i == 12 || i == 16 || i == 17 ||
                i == 20 || i == 21;
      if (ck)
        e_[i] = ui::Check(hwnd_, 710 + i, L"启用", 215 + col * 480,
                          131 + row * 36, 100);
      else if (IsSliderItem(i)) {
        int si = SeatSliderIdx(i);
        ss_[si] = ui::Slider(hwnd_, 710 + i, 215 + col * 480, 131 + row * 36,
                             100, 0, SeatSliderMax(i));
        sv_[si] = ui::Label(hwnd_, L"0", 320 + col * 480, 133 + row * 36, 42);
      } else
        e_[i] = ui::Edit(hwnd_, 710 + i, 215 + col * 480, 131 + row * 36, 150);
    }
    ui::Button(hwnd_, 740, L"读取", 30, 545);
    ui::Button(hwnd_, 741, L"应用", 140, 545);
    ui::Button(hwnd_, 742, L"保存并验证", 250, 545, 120);
    ui::Label(hwnd_, L"测试左/右 %", 400, 550, 100);
    e_[22] = ui::Edit(hwnd_, 743, 510, 546, 100, L"0");
    HWND right = ui::Edit(hwnd_, 744, 620, 546, 100, L"0");
    ui::Button(hwnd_, 745, L"开始测试", 735, 544);
    ui::Button(hwnd_, 746, L"停止/归零", 845, 544);
    state_ = ui::Label(hwnd_, L"状态", 30, 610, 1000, 60);
    SetPropW(hwnd_, L"seatRight", right);
    Read();
  }
  void Read() {
    int rc = RB_Seatbelt_ReadConfig(&c_);
    if (rc != RB_OK)
      rc = RB_Seatbelt_GetDefaultConfig(&c_);
    if (rc != RB_OK)
      return;
    ui::SelectComboData(mode_, c_.InputMode);
    int checks[] = {c_.Enabled,      c_.BrakeEnabled, c_.LateralEnabled,
                    c_.CrashEnabled, c_.LeftEnabled,  c_.RightEnabled,
                    c_.LeftReverse,  c_.RightReverse};
    int ci[] = {0, 6, 9, 12, 16, 17, 20, 21};
    for (int i = 0; i < 8; ++i)
      ui::SetChecked(e_[ci[i]], checks[i]);
    double v[] = {c_.BasePreloadPercent,
                  c_.StartSpeedKmh,
                  c_.FullSpeedKmh,
                  c_.MasterGainPercent,
                  c_.ReleaseSpeedPercent,
                  c_.BrakePedalGainPercent,
                  c_.BrakeDecelGainPercent,
                  c_.LateralGainPercent,
                  c_.LateralDeadzoneG,
                  c_.CrashThresholdPercent,
                  c_.CrashHoldMs,
                  c_.CrashOutputPercent,
                  c_.LeftOutputRatioPercent,
                  c_.RightOutputRatioPercent};
    int vi[] = {1, 2, 3, 4, 5, 7, 8, 10, 11, 13, 14, 15, 18, 19};
    for (int i = 0; i < 14; ++i) {
      int idx = vi[i];
      if (IsSliderItem(idx)) {
        int si = SeatSliderIdx(idx);
        int iv = static_cast<int>(v[i]);
        SendMessageW(ss_[si], TBM_SETPOS, TRUE, iv);
        SetWindowTextW(sv_[si], std::to_wstring(iv).c_str());
      } else
        ui::SetDouble(e_[idx], v[i]);
    }
  }
  void Pull() {
    c_.InputMode = ui::ComboData(mode_);
    int ci[] = {0, 6, 9, 12, 16, 17, 20, 21};
    int *v[] = {&c_.Enabled,      &c_.BrakeEnabled, &c_.LateralEnabled,
                &c_.CrashEnabled, &c_.LeftEnabled,  &c_.RightEnabled,
                &c_.LeftReverse,  &c_.RightReverse};
    for (int i = 0; i < 8; ++i)
      *v[i] = ui::Checked(e_[ci[i]]);
    double *dv[] = {&c_.BasePreloadPercent,
                    &c_.StartSpeedKmh,
                    &c_.FullSpeedKmh,
                    &c_.MasterGainPercent,
                    &c_.ReleaseSpeedPercent,
                    &c_.BrakePedalGainPercent,
                    &c_.BrakeDecelGainPercent,
                    &c_.LateralGainPercent,
                    &c_.LateralDeadzoneG,
                    &c_.CrashThresholdPercent,
                    &c_.CrashHoldMs,
                    &c_.CrashOutputPercent,
                    &c_.LeftOutputRatioPercent,
                    &c_.RightOutputRatioPercent};
    int vi[] = {1, 2, 3, 4, 5, 7, 8, 10, 11, 13, 14, 15, 18, 19};
    for (int i = 0; i < 14; ++i) {
      int idx = vi[i];
      int si = IsSliderItem(idx) ? SeatSliderIdx(idx) : -1;
      *dv[i] = si >= 0 ? static_cast<double>(static_cast<int>(SendMessageW(ss_[si], TBM_GETPOS, 0, 0)))
                       : ui::Double(e_[idx]);
    }
  }
  void OnHScroll(int id, int) override {
    for (int si = 0; si < 8; ++si)
      if (ss_[si] && GetDlgCtrlID(ss_[si]) == id) {
        int p = static_cast<int>(SendMessageW(ss_[si], TBM_GETPOS, 0, 0));
        SetWindowTextW(sv_[si], std::to_wstring(p).c_str());
        break;
      }
  }
  void OnShow() override { Read(); }
  void OnHide() override { Zero(); }
  void Shutdown() override { Zero(); }
  void OnState(const RB_RuntimeState &s) override {
    allowed_ = s.License.OutputAllowed;
    SetWindowTextW(state_,
                   (L"左 " + FormatDouble(s.Seatbelt.LeftOutputPercent) +
                    L"% / 右 " + FormatDouble(s.Seatbelt.RightOutputPercent) +
                    L"% / 制动 " +
                    FormatDouble(s.Seatbelt.BrakeTensionPercent) + L"%")
                       .c_str());
  }
  void OnCommand(int id, int, HWND) override {
    if (id == 740)
      Read();
    else if (id == 741) {
      Pull();
      Report(RB_Seatbelt_ApplyConfigToCurrentFunction(&c_), L"应用");
    } else if (id == 742) {
      Pull();
      int rc = RB_Seatbelt_SaveConfig(&c_);
      if (rc != RB_OK) {
        Report(rc, L"保存");
        return;
      }
      RB_SeatbeltEffectConfig saved{};
      rc = RB_Seatbelt_ReadConfig(&saved);
      if (rc != RB_OK) {
        Report(rc, L"保存成功，但回读失败");
        return;
      }
      c_ = saved;
      Report(RB_OK, L"保存并回读");
    } else if (id == 745) {
      if (!allowed_) {
        Status(L"授权锁定，禁止真实测试输出", true);
        return;
      }
      Report(RB_Seatbelt_SetTestOutput(
              1, ui::Double(e_[22]),
              ui::Double(static_cast<HWND>(GetPropW(hwnd_, L"seatRight")))),
          L"测试");
    } else if (id == 746)
      Zero();
  }
  void Zero() {
    const int rc = RB_Seatbelt_SetTestOutput(0, 0, 0);
    if (rc != RB_OK && allowed_)
      Status(L"安全带测试归零失败: " + ResultText(rc), true);
  }
  void Report(int r, const wchar_t *n) {
    Status(std::wstring(n) + L": " + ResultText(r), r != RB_OK);
  }
};
} // namespace
std::unique_ptr<Page> MakeDynamicPage(HWND h, SdkRuntime &s) {
  return std::make_unique<Dynamic>(h, s);
}
std::unique_ptr<Page> MakeMotionPage(HWND h, SdkRuntime &s) {
  return std::make_unique<Motion>(h, s);
}
std::unique_ptr<Page> MakeHapticPage(HWND h, SdkRuntime &s) {
  return std::make_unique<Haptic>(h, s);
}
std::unique_ptr<Page> MakeWindPage(HWND h, SdkRuntime &s) {
  return std::make_unique<Wind>(h, s);
}
std::unique_ptr<Page> MakeSeatbeltPage(HWND h, SdkRuntime &s) {
  return std::make_unique<Seatbelt>(h, s);
}
