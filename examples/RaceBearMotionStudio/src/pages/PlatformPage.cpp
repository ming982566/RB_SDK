#include "Pages.h"
#include <algorithm>
#include <array>
#include <cmath>

namespace {
class PlatformPage final : public Page {
  SdkRuntime &s_;
  HWND combo_{}, summary_{}, test_{}, preview_{};
  std::array<HWND, 21> e_{};
  std::array<HWND, 10> llabels_{};
  std::array<HWND, 6> d_{};
  std::array<HWND, 6> dv_{};
  std::array<bool, 6> dofSupported_{};
  std::array<HWND, 3> ps_{};       // sliders for OutGain, OutSmoothing, Percent
  std::array<HWND, 3> pv_{};       // value display
  RB_PlatformConfig cfg_{};
  RB_Motion6DOFState motion_{};
  int index_{-1};
  bool allowed_{};

  static int PlatSliderIdx(int baseIdx) {
    if (baseIdx == 2) return 0;  // OutGain
    if (baseIdx == 3) return 1;  // OutSmoothing
    if (baseIdx == 8) return 2;  // Percent
    return -1;
  }

  static constexpr int ManualSliderScale = 1000;

  double ManualLimit(int dof) const {
    if (index_ < 0 || dof < 0 || dof >= RB_DOF_COUNT)
      return 0.0;
    return std::abs(RB_Platform_GetPreviewPoseLimit(index_, &cfg_, dof));
  }

  void SetManualSliderUi(int dof, int position) {
    const double value = ManualLimit(dof) * static_cast<double>(position) /
                         static_cast<double>(ManualSliderScale);
    const wchar_t *unit = dof < 3 ? L"mm" : L"deg";
    const std::wstring text = FormatDouble(value, 2) + L" " + unit;
    SetWindowTextW(dv_[dof], text.c_str());
  }

  void UpdateManualControls() {
    const bool enabled = allowed_ && ui::Checked(test_) != 0;
    for (int i = 0; i < RB_DOF_COUNT; ++i)
      EnableWindow(d_[i], enabled && dofSupported_[i]);
  }

public:
  PlatformPage(HWND h, SdkRuntime &s) : Page(h), s_(s) {}
  void Build() override {
    Header(L"动感平台", L"参数名称和 DOF 支持由 SDK "
                        L"返回；手动姿态受授权、可达范围和显式测试开关约束。");
    combo_ = ui::Combo(hwnd_, 200, 20, 85, 300);
    ui::Button(hwnd_, 201, L"读取", 335, 83);
    ui::Button(hwnd_, 202, L"应用", 445, 83);
    ui::Button(hwnd_, 203, L"保存并验证", 555, 83, 130);
    const wchar_t *base[] = {
        L"输出位宽 (bit)", L"默认位置", L"输出增益 %", L"输出平滑 %",
        L"横向比例", L"纵向比例", L"垂直比例", L"减速速度",
        L"总比例 %", L"输出键", L"名称"};
    for (int i = 0; i < 10; ++i) {
      int col = i / 5, row = i % 5;
      const int labelX = col == 0 ? 20 : 350;
      const int editX = col == 0 ? 155 : 520;
      ui::Label(hwnd_, base[i], labelX, 140 + row * 38,
                col == 0 ? 130 : 165);
      int si = PlatSliderIdx(i);
      if (si >= 0) {
        ps_[si] = ui::Slider(hwnd_, 220 + i, editX, 136 + row * 38,
                             100, 0, 100);
        pv_[si] = ui::Label(hwnd_, L"0", editX + 105, 138 + row * 38, 45);
      } else
        e_[10 + i] =
            ui::Edit(hwnd_, 220 + i, editX, 136 + row * 38, 150);
    }
    for (int i = 0; i < 10; ++i) {
      int col = i / 5, row = i % 5;
      const int labelX = col == 0 ? 720 : 1050;
      const int editX = col == 0 ? 920 : 1220;
      llabels_[i] = ui::Label(hwnd_, (L"L" + std::to_wstring(i + 1)).c_str(),
                              labelX, 140 + row * 38, col == 0 ? 195 : 165);
      e_[i] = ui::Edit(hwnd_, 240 + i, editX, 136 + row * 38, 120);
    }
    summary_ = ui::Label(hwnd_, L"平台状态", 20, 360, 1100, 60);
    ui::Group(hwnd_, L"手动姿态测试", 20, 430, 1320, 240);
    test_ = ui::Check(hwnd_, 270, L"我确认测试真实平台", 40, 460, 210);
    const wchar_t *dn[] = {L"Sway", L"Surge", L"Heave",
                           L"Yaw", L"Roll", L"Pitch"};
    for (int i = 0; i < 6; ++i) {
      int col = i / 3, row = i % 3;
      const int baseX = 40 + col * 360;
      ui::Label(hwnd_, dn[i], baseX, 510 + row * 48, 62);
      d_[i] = ui::Slider(hwnd_, 280 + i, baseX + 68, 506 + row * 48,
                         205, -ManualSliderScale, ManualSliderScale);
      SendMessageW(d_[i], TBM_SETPAGESIZE, 0, 100);
      SendMessageW(d_[i], TBM_SETTICFREQ, 100, 0);
      dv_[i] = ui::Label(hwnd_, i < 3 ? L"0.00 mm" : L"0.00 deg",
                         baseX + 278, 510 + row * 48, 78);
    }
    ui::Button(hwnd_, 299, L"复位并退出测试", 40, 634, 170);
    ui::Caption(hwnd_, L"最终 6DOF / 当前限制", 780, 458, 280);
    preview_ = ui::Canvas(hwnd_, 300, 780, 488, 520, 172,
                          [this](HDC dc, const RECT &rc) { PaintMotion(dc, rc); });
    Refresh();
  }
  void Refresh() {
    SendMessageW(combo_, CB_RESETCONTENT, 0, 0);
    int n = RB_Platform_GetCount();
    for (int i = 0; i < n; ++i) {
      char b[RB_TEXT_MEDIUM]{};
      if (RB_Platform_GetName(i, b, sizeof b) == RB_OK)
        ui::ComboAdd(combo_, Utf8ToWide(b), i);
    }
    int sel = RB_Platform_ReadSelectedIndex();
    ui::SelectComboData(combo_, sel);
    Read();
  }
  void Read() {
    index_ = ui::ComboData(combo_);
    if (index_ < 0)
      return;
    int rc = RB_Platform_ReadConfigByIndex(index_, &cfg_);
    if (rc != RB_OK) {
      Status(L"读取平台失败: " + ResultText(rc), true);
      return;
    }
    int *l = &cfg_.L1;
    for (int i = 0; i < 10; ++i) {
      ui::SetInt(e_[i], l[i]);
      char name[RB_TEXT_MEDIUM]{};
      int nameRc = i < 7
                       ? RB_Platform_GetGeometryParameterName(index_, i + 1,
                                                              name, sizeof name)
                       : RB_Platform_GetStrokeParameterName(index_, i + 1, name,
                                                            sizeof name);
      const bool used = nameRc == RB_OK && name[0];
      std::wstring label = used ? Utf8ToWide(name)
                                : L"L" + std::to_wstring(i + 1) + L"（未使用）";
      SetWindowTextW(llabels_[i], label.c_str());
      EnableWindow(e_[i], used);
    }
    int v[] = {cfg_.OutBit,       cfg_.DefaultPos,        cfg_.OutGain,
               cfg_.OutSmoothing, cfg_.Lateral,           cfg_.Longitudinal,
               cfg_.Vertical,     cfg_.DecelerationSpeed, cfg_.Percent};
    for (int i = 0; i < 9; ++i) {
      int si = PlatSliderIdx(i);
      if (si >= 0) {
        SendMessageW(ps_[si], TBM_SETPOS, TRUE, v[i]);
        SetWindowTextW(pv_[si], std::to_wstring(v[i]).c_str());
      } else
        ui::SetInt(e_[10 + i], v[i]);
    }
    SetWindowUtf8(e_[19], cfg_.OutKey);
    for (int i = 0; i < 6; ++i) {
      dofSupported_[i] = RB_Platform_IsDofSupported(index_, i) != 0 &&
                         RB_Platform_IsPreviewPosePossible(index_, &cfg_, i) != 0;
      SendMessageW(d_[i], TBM_SETPOS, TRUE, 0);
      SetManualSliderUi(i, 0);
    }
    UpdateManualControls();
    Status(L"平台完整结构体已读取");
  }
  bool Pull() {
    int *l = &cfg_.L1;
    for (int i = 0; i < 10; ++i)
      l[i] = ui::Int(e_[i]);
    cfg_.OutBit = ui::Int(e_[10]);
    cfg_.DefaultPos = ui::Int(e_[11]);
    cfg_.OutGain = PlatSliderIdx(2) >= 0
                       ? static_cast<int>(SendMessageW(ps_[0], TBM_GETPOS, 0, 0))
                       : ui::Int(e_[12]);
    cfg_.OutSmoothing = PlatSliderIdx(3) >= 0
                            ? static_cast<int>(SendMessageW(ps_[1], TBM_GETPOS, 0, 0))
                            : ui::Int(e_[13]);
    cfg_.Lateral = ui::Int(e_[14]);
    cfg_.Longitudinal = ui::Int(e_[15]);
    cfg_.Vertical = ui::Int(e_[16]);
    cfg_.DecelerationSpeed = ui::Int(e_[17]);
    cfg_.Percent = PlatSliderIdx(8) >= 0
                       ? static_cast<int>(SendMessageW(ps_[2], TBM_GETPOS, 0, 0))
                       : ui::Int(e_[18]);
    return SetUtf8(cfg_.OutKey, sizeof cfg_.OutKey, GetWindowString(e_[19]));
  }
  void OnShow() override { Refresh(); }
  void OnHide() override { Reset(); }
  void Shutdown() override { Reset(); }
  void OnHScroll(int id, int) override {
    for (int si = 0; si < 3; ++si)
      if (ps_[si] && GetDlgCtrlID(ps_[si]) == id) {
        int p = static_cast<int>(SendMessageW(ps_[si], TBM_GETPOS, 0, 0));
        SetWindowTextW(pv_[si], std::to_wstring(p).c_str());
        break;
      }
    if (id >= 280 && id < 286) {
      const int dof = id - 280;
      int position = static_cast<int>(SendMessageW(d_[dof], TBM_GETPOS, 0, 0));
      SetManualSliderUi(dof, position);
      if (!allowed_ || !ui::Checked(test_) || !dofSupported_[dof]) {
        SendMessageW(d_[dof], TBM_SETPOS, TRUE, 0);
        SetManualSliderUi(dof, 0);
        return;
      }

      const double value = ManualLimit(dof) * static_cast<double>(position) /
                           static_cast<double>(ManualSliderScale);
      const int rc = RB_ManualPose_SetDrive(dof, value);
      if (rc != RB_OK)
        Status(L"手动姿态: " + ResultText(rc), true);
    }
  }
  void OnState(const RB_RuntimeState &s) override {
    const bool wasAllowed = allowed_;
    allowed_ = s.License.OutputAllowed != 0;
    if (wasAllowed && !allowed_ && ui::Checked(test_)) {
      ui::SetChecked(test_, 0);
      for (int i = 0; i < RB_DOF_COUNT; ++i) {
        SendMessageW(d_[i], TBM_SETPOS, TRUE, 0);
        SetManualSliderUi(i, 0);
      }
    }
    UpdateManualControls();
    motion_ = s.Rig.MotionState;
    InvalidateRect(preview_, nullptr, FALSE);
    std::wstring t = Utf8ToWide(s.Rig.RigName) + L"  轴数 " +
                     std::to_wstring(s.Rig.NumberOfAxes) + L"  最终 6DOF：";
    for (double x : s.Rig.MotionState.RigFiltered)
      t += FormatDouble(x) + L" ";
    SetWindowTextW(summary_, t.c_str());
  }
  void PaintMotion(HDC dc, const RECT &rc) {
    static const wchar_t *names[] = {L"Sway", L"Surge", L"Heave",
                                     L"Yaw", L"Roll", L"Pitch"};
    const int width = rc.right - rc.left;
    HPEN center = CreatePen(PS_SOLID, 1, RGB(82, 88, 89));
    HBRUSH track = CreateSolidBrush(RGB(36, 40, 41));
    HBRUSH positive = CreateSolidBrush(RGB(0, 140, 210));
    HBRUSH negative = CreateSolidBrush(RGB(220, 110, 40));
    HBRUSH zeroMarker = CreateSolidBrush(RGB(18, 177, 163));
    HGDIOBJ oldPen = SelectObject(dc, center);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(220, 220, 220));
    for (int i = 0; i < 6; ++i) {
      const int top = 8 + i * 26;
      RECT label{10, top, 62, top + 20};
      DrawTextW(dc, names[i], -1, &label, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
      const int left = 68, right = width - 180, middle = (left + right) / 2;
      RECT trackRect{left, top + 5, right, top + 16};
      FillRect(dc, &trackRect, track);
      MoveToEx(dc, middle, top + 3, nullptr);
      LineTo(dc, middle, top + 18);
      double limit = std::abs(motion_.PoseLimit[i]);
      // 运行态尚未给出限制时，用当前平台配置计算预览范围，避免错误显示为 0。
      if (limit <= 0.000001 && index_ >= 0)
        limit = std::abs(RB_Platform_GetPreviewPoseLimit(index_, &cfg_, i));
      const double normalized = limit > 0.000001
          ? (std::max)(-1.0, (std::min)(1.0, motion_.RigFiltered[i] / limit))
          : 0.0;
      const int end = middle + static_cast<int>(normalized * (right - left) / 2);
      if (end != middle) {
        RECT bar{(std::min)(middle, end), top + 5,
                 (std::max)(middle, end), top + 16};
        FillRect(dc, &bar, normalized >= 0 ? positive : negative);
      } else {
        RECT marker{middle - 2, top + 4, middle + 3, top + 17};
        FillRect(dc, &marker, zeroMarker);
      }
      const wchar_t *unit = i < 3 ? L"mm" : L"deg";
      std::wstring valueText = FormatDouble(motion_.RigFiltered[i], 2) +
                               L" / ±" + FormatDouble(limit, 2) + L" " + unit;
      RECT valueRect{right + 8, top, width - 8, top + 20};
      DrawTextW(dc, valueText.c_str(), -1, &valueRect,
                DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
    }
    SelectObject(dc, oldPen);
    DeleteObject(center);
    DeleteObject(track);
    DeleteObject(positive);
    DeleteObject(negative);
    DeleteObject(zeroMarker);
  }
  void OnCommand(int id, int, HWND) override {
    if (id == 201)
      Read();
    else if (id == 202) {
      if (Pull())
        Report(RB_Platform_ApplyRuntimeConfigToCurrentRig(&cfg_), L"应用");
    } else if (id == 203) {
      if (!Pull())
        return;
      int rc = RB_Platform_SaveConfigByIndex(index_, &cfg_);
      if (rc != RB_OK) {
        Report(rc, L"保存平台配置");
        return;
      }
      rc = RB_Platform_SaveSelectedIndex(index_);
      if (rc != RB_OK) {
        Report(rc, L"保存平台选择项");
        return;
      }
      rc = RB_Platform_SetSelectedState(index_, &cfg_);
      if (rc != RB_OK) {
        Report(rc, L"更新平台状态");
        return;
      }
      RB_PlatformConfig saved{};
      rc = RB_Platform_ReadConfigByIndex(index_, &saved);
      if (rc != RB_OK) {
        Report(rc, L"保存成功，但回读失败");
        return;
      }
      cfg_ = saved;
      Report(RB_OK, L"保存并回读");
    } else if (id == 270) {
      if (!allowed_) {
        ui::SetChecked(test_, 0);
        Status(L"授权未允许真实输出，测试被锁定", true);
        return;
      }
      const int rc = RB_ManualPose_SetTestEnabled(ui::Checked(test_));
      Report(rc, L"测试模式");
      if (rc != RB_OK)
        ui::SetChecked(test_, 0);
      if (!ui::Checked(test_)) {
        for (int i = 0; i < RB_DOF_COUNT; ++i) {
          SendMessageW(d_[i], TBM_SETPOS, TRUE, 0);
          SetManualSliderUi(i, 0);
        }
      }
      UpdateManualControls();
    } else if (id == 299)
      Reset();
  }
  void Reset() {
    // 离开页面或退出程序时必须同时归零目标值并关闭手动测试模式。
    const int resetRc = RB_ManualPose_ResetDrive();
    const int disableRc = RB_ManualPose_SetTestEnabled(0);
    if (resetRc != RB_OK && disableRc != RB_OK && allowed_)
      Status(L"手动姿态复位失败", true);
    if (test_)
      ui::SetChecked(test_, 0);
    for (int i = 0; i < RB_DOF_COUNT; ++i) {
      if (d_[i]) {
        SendMessageW(d_[i], TBM_SETPOS, TRUE, 0);
        SetManualSliderUi(i, 0);
      }
    }
    UpdateManualControls();
  }
  void Report(int rc, const wchar_t *n) {
    Status(std::wstring(n) + L": " + ResultText(rc), rc != RB_OK);
  }
};
} // namespace
std::unique_ptr<Page> MakePlatformPage(HWND h, SdkRuntime &s) {
  return std::make_unique<PlatformPage>(h, s);
}
