#include "Pages.h"
#include <array>
#include <mutex>

namespace {
std::wstring OutputKindText(const char *kind) {
  if (strcmp(kind, "Serial") == 0)
    return L"串口";
  if (strcmp(kind, "MMF") == 0)
    return L"MMF";
  return Utf8ToWide(kind);
}

class Output final : public Page {
  SdkRuntime &s_;
  HWND list_{}, kinds_{}, keys_{}, status_{};
  std::array<HWND, 19> e_{};
  RB_OutputCatalog cat_{};
  RB_OutputConfig c_{};
  int idx_{-1};
  bool allowed_{};

public:
  Output(HWND h, SdkRuntime &s) : Page(h), s_(s) {}
  void Build() override {
    Header(L"输出",
           L"输出类型和命令键由 SDK 动态枚举；本软件不会自动连接真实硬件。");
    list_ = ui::List(hwnd_, 800, 20, 85, 260, 220);
    kinds_ = ui::Combo(hwnd_, 801, 20, 320, 260);
    ui::Button(hwnd_, 802, L"刷新", 20, 365);
    ui::Button(hwnd_, 803, L"增加实例", 130, 365);
    ui::Button(hwnd_, 804, L"删除实例", 240, 365);
    ui::Button(hwnd_, 805, L"连接选中", 20, 405);
    ui::Button(hwnd_, 806, L"断开选中", 130, 405);
    ui::Button(hwnd_, 807, L"全部连接", 20, 445);
    ui::Button(hwnd_, 808, L"全部断开", 130, 445);
    status_ = ui::Label(hwnd_, L"状态", 20, 500, 270, 100);
    const wchar_t *n[] = {
        L"自动连接", L"输出位宽 (bit)", L"波特率", L"端口",
        L"IP 地址", L"UDP 远程端口", L"UDP 本地端口", L"MMF 名称",
        L"CAN 索引", L"连接速度 %", L"停放位置 %", L"启动指令",
        L"启动等待 (ms)", L"输出指令", L"输出周期 (ms)", L"结束指令",
        L"结束等待 (ms)"};
    for (int i = 0; i < 17; ++i) {
      int col = i / 9, row = i % 9;
      ui::Label(hwnd_, n[i], 320 + col * 360, 90 + row * 42, 145);
      e_[i] = i == 0 ? ui::Check(hwnd_, 820, L"启用", 470, 86, 100)
                     : ui::Edit(hwnd_, 820 + i, 470 + col * 360, 86 + row * 42,
                                180);
    }
    ui::Button(hwnd_, 850, L"应用", 320, 500);
    ui::Button(hwnd_, 851, L"保存并回读", 430, 500, 130);
    keys_ = ui::List(hwnd_, 852, 1020, 85, 200, 500, LBS_NOINTEGRALHEIGHT);
    ui::Label(hwnd_, L"双击键名插入输出指令", 1020, 595, 200);
    Refresh();
  }
  void Refresh() {
    SendMessageW(list_, LB_RESETCONTENT, 0, 0);
    if (RB_Output_GetCatalog(&cat_) == RB_OK) {
      for (int i = 0; i < cat_.InstanceCount; ++i) {
        std::wstring t = Utf8ToWide(cat_.Instances[i].Key) +
                         (cat_.Instances[i].Connected ? L" [已连接]" : L"");
        int n = static_cast<int>(SendMessageW(
            list_, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(t.c_str())));
        SendMessageW(list_, LB_SETITEMDATA, n, cat_.Instances[i].Index);
      }
      SendMessageW(keys_, LB_RESETCONTENT, 0, 0);
      for (int i = 0; i < cat_.KeyCount; ++i)
        SendMessageW(
            keys_, LB_ADDSTRING, 0,
            reinterpret_cast<LPARAM>(Utf8ToWide(cat_.Keys[i].Key).c_str()));
    }
    SendMessageW(kinds_, CB_RESETCONTENT, 0, 0);
    for (int i = 0; i < RB_Output_GetKindCount(); ++i) {
      char b[64]{};
      if (RB_Output_GetKind(i, b, sizeof b) == RB_OK)
        ui::ComboAdd(kinds_, OutputKindText(b), i);
    }
    if (cat_.InstanceCount) {
      SendMessageW(list_, LB_SETCURSEL, 0, 0);
      Load();
    }
  }
  void Load() {
    int r = static_cast<int>(SendMessageW(list_, LB_GETCURSEL, 0, 0));
    if (r < 0)
      return;
    idx_ = static_cast<int>(SendMessageW(list_, LB_GETITEMDATA, r, 0));
    int rc = RB_Output_ReadInstanceConfigByIndex(idx_, &c_);
    if (rc != RB_OK)
      return;
    ui::SetChecked(e_[0], c_.AutoConnect);
    int ints[] = {c_.OutBit, c_.BaudRate};
    ui::SetInt(e_[1], ints[0]);
    ui::SetInt(e_[2], ints[1]);
    SetWindowUtf8(e_[3], c_.Port);
    SetWindowUtf8(e_[4], c_.IpAddrs);
    ui::SetInt(e_[5], c_.UdpPort);
    ui::SetInt(e_[6], c_.UdpLPort);
    SetWindowUtf8(e_[7], c_.MMFName);
    ui::SetInt(e_[8], c_.CANindex);
    ui::SetInt(e_[9], c_.ConnectionSpeed);
    ui::SetInt(e_[10], c_.ParkPositionPercent);
    SetWindowUtf8(e_[11], c_.StartString);
    SetWindowUtf8(e_[12], c_.StartTime);
    SetWindowUtf8(e_[13], c_.OutPutString);
    SetWindowUtf8(e_[14], c_.OutPutTime);
    SetWindowUtf8(e_[15], c_.EndString);
    SetWindowUtf8(e_[16], c_.EndTime);
    for (int i = 0; i < RB_Output_GetKindCount(); ++i) {
      char b[64]{};
      if (RB_Output_GetKind(i, b, sizeof b) == RB_OK &&
          strcmp(b, c_.OutputKind) == 0) {
        ui::SelectComboData(kinds_, i);
        break;
      }
    }
  }
  bool Pull() {
    c_.AutoConnect = ui::Checked(e_[0]);
    c_.OutBit = ui::Int(e_[1]);
    c_.BaudRate = ui::Int(e_[2]);
    c_.UdpPort = ui::Int(e_[5]);
    c_.UdpLPort = ui::Int(e_[6]);
    c_.CANindex = ui::Int(e_[8]);
    c_.ConnectionSpeed = ui::Int(e_[9]);
    c_.ParkPositionPercent = ui::Int(e_[10]);
    int ki = ui::ComboData(kinds_);
    char k[64]{};
    if (RB_Output_GetKind(ki, k, sizeof k) != RB_OK)
      return false;
    strcpy_s(c_.OutputKind, k);
    return SetUtf8(c_.Port, sizeof c_.Port, GetWindowString(e_[3])) &&
           SetUtf8(c_.IpAddrs, sizeof c_.IpAddrs, GetWindowString(e_[4])) &&
           SetUtf8(c_.MMFName, sizeof c_.MMFName, GetWindowString(e_[7])) &&
           SetUtf8(c_.StartString, sizeof c_.StartString,
                   GetWindowString(e_[11])) &&
           SetUtf8(c_.StartTime, sizeof c_.StartTime,
                   GetWindowString(e_[12])) &&
           SetUtf8(c_.OutPutString, sizeof c_.OutPutString,
                   GetWindowString(e_[13])) &&
           SetUtf8(c_.OutPutTime, sizeof c_.OutPutTime,
                   GetWindowString(e_[14])) &&
           SetUtf8(c_.EndString, sizeof c_.EndString,
                   GetWindowString(e_[15])) &&
           SetUtf8(c_.EndTime, sizeof c_.EndTime, GetWindowString(e_[16]));
  }
  void OnShow() override { Refresh(); }
  void OnState(const RB_RuntimeState &s) override {
    allowed_ = s.License.OutputAllowed;
    SetWindowTextW(status_,
                   (L"实例 " + std::to_wstring(s.Output.Count) + L" / " +
                    (s.Output.AnyRuntimeActive ? L"运行中" : L"空闲") +
                    L"\r\n" + Utf8ToWide(s.LastError))
                       .c_str());
  }
  void OnCommand(int id, int code, HWND) override {
    if (id == 800 && code == LBN_SELCHANGE)
      Load();
    else if (id == 802)
      Refresh();
    else if (id == 803) {
      char k[64]{};
      int x = ui::ComboData(kinds_);
      int rc = RB_Output_GetKind(x, k, sizeof k);
      if (rc == RB_OK) {
        int ni = RB_Output_AddInstance(k);
        rc = ni < 0 ? ni : RB_OK;
      }
      Report(rc, L"增加实例");
      Refresh();
    } else if (id == 804) {
      if (MessageBoxW(hwnd_, L"确定要删除当前输出实例吗？删除前会自动断开连接。",
                       L"确认删除", MB_YESNO | MB_ICONWARNING) != IDYES) return;
      if (idx_ >= 0 && RB_Output_IsInstanceConnected(idx_))
        RB_Output_DisconnectInstanceByIndex(idx_);
      Report(RB_Output_DeleteInstanceByIndex(idx_), L"删除实例");
      Refresh();
    } else if (id == 805) {
      if (!allowed_) {
        Status(L"授权锁定，禁止连接真实输出", true);
        return;
      }
      Report(RB_Output_ConnectInstanceByIndex(idx_), L"连接");
    } else if (id == 806)
      Report(RB_Output_DisconnectInstanceByIndex(idx_), L"断开");
    else if (id == 807) {
      if (!allowed_) {
        Status(L"授权锁定，禁止连接真实输出", true);
        return;
      }
      int n = RB_Output_ConnectAll();
      Report(n < 0 ? n : RB_OK, L"全部连接");
    } else if (id == 808) {
      int n = RB_Output_DisconnectAll();
      Report(n < 0 ? n : RB_OK, L"全部断开");
    } else if (id == 850) {
      if (Pull()) {
        if (RB_Output_IsInstanceConnected(idx_)) {
          Status(L"请先断开实例再修改连接参数", true);
          return;
        }
        Report(RB_Output_ApplyInstanceConfigByIndex(idx_, &c_), L"应用");
      }
    } else if (id == 851) {
      if (Pull()) {
        if (RB_Output_IsInstanceConnected(idx_)) {
          Status(L"已连接实例必须先断开", true);
          return;
        }
        int rc = RB_Output_SaveInstanceConfigByIndex(idx_, &c_);
        if (rc != RB_OK) {
          Report(rc, L"保存");
          return;
        }
        RB_OutputConfig saved{};
        rc = RB_Output_ReadInstanceConfigByIndex(idx_, &saved);
        if (rc != RB_OK) {
          Report(rc, L"保存成功，但回读失败");
          return;
        }
        c_ = saved;
        Report(RB_OK, L"保存并回读");
      }
    } else if (id == 852 && code == LBN_DBLCLK) {
      int r = static_cast<int>(SendMessageW(keys_, LB_GETCURSEL, 0, 0));
      if (r >= 0) {
        wchar_t b[128]{};
        SendMessageW(keys_, LB_GETTEXT, r, reinterpret_cast<LPARAM>(b));
        std::wstring x = GetWindowString(e_[13]) + L"<" + b + L">";
        SetWindowTextW(e_[13], x.c_str());
      }
    }
  }
  void Report(int r, const wchar_t *n) {
    Status(std::wstring(n) + L": " + ResultText(r), r != RB_OK);
  }
};

class Serial final : public Page {
  SdkRuntime &s_;
  HWND ports_{}, baud_{}, send_{}, recv_{}, state_{}, parity_{}, dataBits_{},
       stopBits_{}, flow_{}, mode_{}, autoReconnect_{}, reconnectMs_{}, dtr_{}, rts_{};
  std::array<HWND, 5> timeout_{};
  RB_SerialHandle handle_{};
  std::mutex mu_;
  bool readable_{}, hotplug_{};
  int operateMode_{RB_SERIAL_ASYNC};
  // SDK 可能从工作线程触发回调；这里只置标志，控件更新统一回到 UI 定时器。
  static void ReadCb(RB_SerialHandle, const char *, unsigned int, void *u) {
    auto *p = static_cast<Serial *>(u);
    std::lock_guard<std::mutex> l(p->mu_);
    p->readable_ = true;
  }
  static void HotCb(const char *, int, const RB_SerialPortInfo *, void *u) {
    auto *p = static_cast<Serial *>(u);
    std::lock_guard<std::mutex> l(p->mu_);
    p->hotplug_ = true;
  }

public:
  Serial(HWND h, SdkRuntime &s) : Page(h), s_(s) {}
  void Build() override {
    Header(L"串口",
           L"回调线程只写线程安全标志，数据读取和控件更新始终在 UI 线程完成。");
    ports_ = ui::Combo(hwnd_, 900, 20, 90, 350);
    ui::Button(hwnd_, 901, L"刷新", 385, 88);
    ui::Label(hwnd_, L"波特率", 20, 145, 100);
    baud_ = ui::Edit(hwnd_, 902, 120, 141, 150, L"115200");
    ui::Button(hwnd_, 903, L"打开", 290, 139);
    ui::Button(hwnd_, 904, L"关闭", 400, 139);
    ui::Label(hwnd_, L"校验", 20, 190, 50);
    parity_ = ui::Combo(hwnd_, 910, 75, 185, 140);
    const wchar_t *parityNames[] = {L"无", L"奇校验", L"偶校验", L"标记", L"空格"};
    for (int i = 0; i < 5; ++i) ui::ComboAdd(parity_, parityNames[i], i);
    ui::Label(hwnd_, L"数据位", 235, 190, 55);
    dataBits_ = ui::Combo(hwnd_, 911, 295, 185, 100);
    for (int bits = 5; bits <= 8; ++bits) ui::ComboAdd(dataBits_, std::to_wstring(bits), bits);
    ui::Label(hwnd_, L"停止位", 415, 190, 55);
    stopBits_ = ui::Combo(hwnd_, 912, 475, 185, 120);
    ui::ComboAdd(stopBits_, L"1", RB_SERIAL_STOP_ONE);
    ui::ComboAdd(stopBits_, L"1.5", RB_SERIAL_STOP_ONE_AND_HALF);
    ui::ComboAdd(stopBits_, L"2", RB_SERIAL_STOP_TWO);
    ui::Label(hwnd_, L"流控", 615, 190, 50);
    flow_ = ui::Combo(hwnd_, 913, 670, 185, 150);
    ui::ComboAdd(flow_, L"无", RB_SERIAL_FLOW_NONE);
    ui::ComboAdd(flow_, L"硬件流控", RB_SERIAL_FLOW_HARDWARE);
    ui::ComboAdd(flow_, L"软件流控", RB_SERIAL_FLOW_SOFTWARE);
    ui::Label(hwnd_, L"模式", 840, 190, 50);
    mode_ = ui::Combo(hwnd_, 914, 895, 185, 130);
    ui::ComboAdd(mode_, L"异步", RB_SERIAL_ASYNC);
    ui::ComboAdd(mode_, L"同步", RB_SERIAL_SYNC);
    ui::SelectComboData(parity_, RB_SERIAL_PARITY_NONE);
    ui::SelectComboData(dataBits_, 8);
    ui::SelectComboData(stopBits_, RB_SERIAL_STOP_ONE);
    ui::SelectComboData(flow_, RB_SERIAL_FLOW_NONE);
    ui::SelectComboData(mode_, RB_SERIAL_ASYNC);

    autoReconnect_ = ui::Check(hwnd_, 915, L"自动重连", 20, 235, 120);
    reconnectMs_ = ui::Edit(hwnd_, 916, 145, 231, 100, L"1000");
    ui::Label(hwnd_, L"重连间隔 (ms)", 255, 235, 130);
    dtr_ = ui::Check(hwnd_, 917, L"DTR", 390, 235, 80);
    rts_ = ui::Check(hwnd_, 918, L"RTS", 480, 235, 80);
    const wchar_t *timeoutNames[] = {L"读间隔", L"读乘数", L"读常量", L"写乘数", L"写常量"};
    for (int i = 0; i < 5; ++i) {
      ui::Label(hwnd_, timeoutNames[i], 20 + i * 205, 280, 75);
      timeout_[i] = ui::Edit(hwnd_, 920 + i, 100 + i * 205, 276, 100, L"0");
    }
    send_ = ui::Edit(hwnd_, 905, 20, 335, 820, L"", ES_MULTILINE);
    ui::Button(hwnd_, 906, L"发送 UTF-8", 855, 333, 120);
    recv_ = ui::List(hwnd_, 907, 20, 390, 1080, 270, LBS_NOINTEGRALHEIGHT);
    state_ = ui::Label(hwnd_, L"关闭", 20, 680, 1000);
    Refresh();
  }
  void Refresh() {
    SendMessageW(ports_, CB_RESETCONTENT, 0, 0);
    int n = RB_Serial_GetAvailablePortCount();
    for (int i = 0; i < n; ++i) {
      RB_SerialPortInfo x{};
      if (RB_Serial_GetAvailablePortInfo(i, &x) == RB_OK)
        ui::ComboAdd(
            ports_,
            Utf8ToWide(x.PortName) + L" - " + Utf8ToWide(x.FriendlyName), i);
    }
    if (n)
      SendMessageW(ports_, CB_SETCURSEL, 0, 0);
  }
  void Open() {
    Close();
    int row = ui::ComboData(ports_);
    RB_SerialPortInfo p{};
    if (RB_Serial_GetAvailablePortInfo(row, &p) != RB_OK)
      return;
    handle_ = RB_Serial_Create();
    if (!handle_) {
      Status(L"创建串口句柄失败", true);
      return;
    }
    int rc = RB_Serial_InitEx(handle_, p.PortName, ui::Int(baud_, 115200),
                              ui::ComboData(parity_), ui::ComboData(dataBits_),
                              ui::ComboData(stopBits_), ui::ComboData(flow_), 8192);
    operateMode_ = ui::ComboData(mode_);
    if (rc == RB_OK)
      rc = RB_Serial_SetOperateMode(handle_, operateMode_);
    if (rc == RB_OK)
      rc = RB_Serial_SetAutoReconnectEnabled(handle_, ui::Checked(autoReconnect_));
    if (rc == RB_OK)
      rc = RB_Serial_SetAutoReconnectInterval(handle_, ui::Int(reconnectMs_, 1000));
    RB_SerialTimeoutConfig timeouts{};
    timeouts.ReadIntervalTimeout = ui::Int(timeout_[0]);
    timeouts.ReadTotalTimeoutMultiplier = ui::Int(timeout_[1]);
    timeouts.ReadTotalTimeoutConstant = ui::Int(timeout_[2]);
    timeouts.WriteTotalTimeoutMultiplier = ui::Int(timeout_[3]);
    timeouts.WriteTotalTimeoutConstant = ui::Int(timeout_[4]);
    if (rc == RB_OK)
      rc = RB_Serial_SetTimeoutConfig(handle_, &timeouts);
    if (rc == RB_OK)
      rc = RB_Serial_ConnectReadEvent(handle_, ReadCb, this);
    if (rc == RB_OK)
      rc = RB_Serial_ConnectHotPlugEvent(handle_, HotCb, this);
    if (rc == RB_OK)
      rc = RB_Serial_Open(handle_);
    if (rc == RB_OK)
      rc = RB_Serial_SetDtr(handle_, ui::Checked(dtr_));
    if (rc == RB_OK)
      rc = RB_Serial_SetRts(handle_, ui::Checked(rts_));
    if (rc != RB_OK) {
      Err(L"打开串口", rc);
      Close();
    } else
      SetWindowTextW(state_, L"串口已打开");
  }
  void Close() {
    if (!handle_)
      return;
    RB_Serial_DisconnectReadEvent(handle_);
    RB_Serial_DisconnectHotPlugEvent(handle_);
    RB_Serial_Close(handle_);
    RB_Serial_Destroy(handle_);
    handle_ = nullptr;
    if (state_)
      SetWindowTextW(state_, L"关闭");
  }
  void Shutdown() override { Close(); }
  void OnTimer(UINT_PTR) override {
    bool rd, hp;
    {
      std::lock_guard<std::mutex> l(mu_);
      rd = readable_;
      hp = hotplug_;
      readable_ = hotplug_ = false;
    }
    if (hp)
      Refresh();
    if ((rd || operateMode_ == RB_SERIAL_SYNC) && handle_) {
      char b[4096]{};
      int n = RB_Serial_ReadAll(handle_, b, sizeof b - 1);
      if (n > 0) {
        b[n] = 0;
        SendMessageW(recv_, LB_ADDSTRING, 0,
                     reinterpret_cast<LPARAM>(Utf8ToWide(b).c_str()));
      }
    }
  }
  void OnCommand(int id, int, HWND) override {
    if (id == 901)
      Refresh();
    else if (id == 903)
      Open();
    else if (id == 904)
      Close();
    else if (id == 906 && handle_) {
      auto x = WideToUtf8(GetWindowString(send_));
      int n = RB_Serial_Write(handle_, x.data(), static_cast<int>(x.size()));
      if (n < 0)
        Err(L"发送", n);
      else
        Status(L"已发送 " + std::to_wstring(n) + L" bytes");
    }
  }
  void Err(const wchar_t *n, int rc) {
    char b[512]{};
    if (handle_)
      RB_Serial_GetLastErrorMsg(handle_, b, sizeof b);
    Status(std::wstring(n) + L": " + ResultText(rc) + L" " + Utf8ToWide(b),
           true);
  }
};

class Custom final : public Page {
  SdkRuntime &s_;
  std::array<HWND, 13> e_{};
  HWND transport_{}, mode_{}, state_{};
  std::string key_;

public:
  Custom(HWND h, SdkRuntime &s) : Page(h), s_(s) {}
  void Build() override {
    // MMF 和 UDP 共用同一份游戏描述，传输方式只决定后端从哪里接收数据包。
    Header(L"自定义游戏",
           L"注册 MMF 或 UDP 游戏，支持原始遥测或已计算 6DOF 包协议。");
    const wchar_t *n[] = {L"游戏键", L"游戏名", L"进程名（* 分隔）",
                          L"图片名", L"Steam App ID", L"数据超时 (ms)",
                          L"过渡时间 (ms)", L"MMF 名称", L"MMF 大小 (bytes)",
                          L"UDP 监听 IP", L"UDP 端口", L"UDP 允许的发送端",
                          L"6DOF 叠加运动增强"};
    for (int i = 0; i < 13; ++i) {
      int col = i / 7, row = i % 7;
      ui::Label(hwnd_, n[i], 30 + col * 540, 90 + row * 44, 170);
      e_[i] = i == 12
                  ? ui::Check(hwnd_, 1000 + i, L"启用", 205 + col * 540,
                              86 + row * 44, 120)
                  : ui::Edit(hwnd_, 1000 + i, 205 + col * 540,
                             86 + row * 44, 300);
    }
    transport_ = ui::Combo(hwnd_, 1020, 205, 420, 260);
    ui::ComboAdd(transport_, L"MMF", RB_CUSTOM_TRANSPORT_MMF);
    ui::ComboAdd(transport_, L"UDP", RB_CUSTOM_TRANSPORT_UDP);
    mode_ = ui::Combo(hwnd_, 1021, 745, 420, 260);
    ui::ComboAdd(mode_, L"原始遥测", RB_CUSTOM_INPUT_RAW);
    ui::ComboAdd(mode_, L"6DOF", RB_CUSTOM_INPUT_6DOF);
    ui::Button(hwnd_, 1022, L"注册/更新", 30, 475, 120);
    ui::Button(hwnd_, 1023, L"选择并监听", 165, 475, 120);
    ui::Button(hwnd_, 1024, L"查询状态", 300, 475);
    ui::Button(hwnd_, 1025, L"注销", 410, 475);
    ui::Button(hwnd_, 1026, L"重载注册表", 520, 475, 120);
    state_ = ui::Label(hwnd_, L"状态", 30, 540, 1080, 120);
    ui::SetInt(e_[4], 0);
    ui::SetInt(e_[5], 500);
    ui::SetInt(e_[6], 2000);
    SetWindowTextW(e_[7], L"Local\\RaceBear.CustomGame.sample");
    ui::SetInt(e_[8], 16384);
    SetWindowTextW(e_[9], L"127.0.0.1");
    ui::SetInt(e_[10], 27890);
    SetWindowTextW(e_[11], L"127.0.0.1");
    ui::SelectComboData(transport_, RB_CUSTOM_TRANSPORT_UDP);
    ui::SelectComboData(mode_, RB_CUSTOM_INPUT_RAW);
  }
  void OnCommand(int id, int, HWND) override {
    key_ = WideToUtf8(GetWindowString(e_[0]));
    if (id == 1022) {
      RB_CustomGameRegistration r{};
      r.Size = sizeof r;
      r.Version = 1;
      r.Game.Size = sizeof r.Game;
      r.Game.Version = 1;
      r.Game.Transport = ui::ComboData(transport_);
      r.Game.InputMode = ui::ComboData(mode_);
      r.Game.SteamAppID = ui::Int(e_[4]);
      r.Game.DataTimeoutMs = ui::Int(e_[5]);
      r.Game.TransitionMs = ui::Int(e_[6]);
      r.Game.DofOptions = ui::Checked(e_[12]) ? RB_CUSTOM_DOF_ADD_MOTION_EFFECTS : 0;
      r.Mmf.MappingSize = ui::Int(e_[8]);
      r.Udp.ListenPort = ui::Int(e_[10]);
      bool ok = SetUtf8(r.Game.GameKey, sizeof r.Game.GameKey,
                        GetWindowString(e_[0])) &&
                SetUtf8(r.Game.GameName, sizeof r.Game.GameName,
                        GetWindowString(e_[1])) &&
                SetUtf8(r.Game.ProcessNames, sizeof r.Game.ProcessNames,
                        GetWindowString(e_[2])) &&
                SetUtf8(r.Game.ImageName, sizeof r.Game.ImageName,
                        GetWindowString(e_[3])) &&
                SetUtf8(r.Mmf.MappingName, sizeof r.Mmf.MappingName,
                        GetWindowString(e_[7])) &&
                SetUtf8(r.Udp.ListenAddress, sizeof r.Udp.ListenAddress,
                        GetWindowString(e_[9])) &&
                SetUtf8(r.Udp.AllowedSenderAddress, sizeof r.Udp.AllowedSenderAddress,
                        GetWindowString(e_[11]));
      Report(ok ? RB_Game_RegisterCustom(&r) : RB_INVALID_ARGUMENT, L"注册");
    } else if (id == 1023)
      Report(RB_Game_SelectByKey(key_.c_str()), L"选择/监听");
    else if (id == 1024) {
      RB_CustomGameState st{};
      int rc = RB_Game_GetCustomState(key_.c_str(), &st);
      if (rc == RB_OK)
        SetWindowTextW(state_,
                       (Utf8ToWide(st.StatusText) + L"\r\n包 " +
                        std::to_wstring(st.AcceptedPacketCount) + L"  " +
                        FormatDouble(st.InputFrequencyHz) + L" Hz  距上次接收 " +
                        FormatDouble(st.LastPacketAgeMs) + L" ms\r\n" +
                        Utf8ToWide(st.LastError))
                           .c_str());
      Report(rc, L"状态");
    } else if (id == 1025) {
      if (MessageBoxW(hwnd_, L"确定要注销自定义游戏吗？", L"确认注销",
                       MB_YESNO | MB_ICONWARNING) != IDYES) return;
      Report(RB_Game_UnregisterCustom(key_.c_str()), L"注销");
    } else if (id == 1026)
      Report(RB_Game_ReloadCustomRegistry(), L"重载");
  }
  void Report(int r, const wchar_t *n) {
    Status(std::wstring(n) + L": " + ResultText(r), r != RB_OK);
  }
};

class Plugins final : public Page {
  SdkRuntime &s_;
  HWND list_{}, info_{};
  RB_PluginCatalog cat_{};

public:
  Plugins(HWND h, SdkRuntime &s) : Page(h), s_(s) {}
  void Build() override {
    Header(L"功能插件", L"每次刷新后重新读取目录，旧索引不会复用。");
    list_ = ui::List(hwnd_, 1100, 20, 90, 500, 500);
    info_ = ui::Label(hwnd_, L"选择插件", 550, 100, 550, 150);
    ui::Button(hwnd_, 1101, L"刷新目录", 20, 610);
    ui::Button(hwnd_, 1102, L"启动全部后台运行态", 140, 610, 170);
    ui::Button(hwnd_, 1103, L"启动插件前端", 325, 610, 140);
    Refresh();
  }
  void Refresh() {
    int n = RB_Plugin_Refresh();
    SendMessageW(list_, LB_RESETCONTENT, 0, 0);
    if (n >= 0 && RB_Plugin_GetCatalog(&cat_) == RB_OK)
      for (int i = 0; i < cat_.Count; ++i) {
        std::wstring t = Utf8ToWide(cat_.Items[i].Name) + L"  " +
                         Utf8ToWide(cat_.Items[i].Version);
        int r = static_cast<int>(SendMessageW(
            list_, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(t.c_str())));
        SendMessageW(list_, LB_SETITEMDATA, r, cat_.Items[i].Index);
      }
  }
  void OnShow() override { Refresh(); }
  void OnCommand(int id, int code, HWND) override {
    if (id == 1100 && code == LBN_SELCHANGE) {
      int r = static_cast<int>(SendMessageW(list_, LB_GETCURSEL, 0, 0));
      if (r >= 0)
        SetWindowTextW(info_, (Utf8ToWide(cat_.Items[r].StatusText) +
                               (cat_.Items[r].Launchable ? L"\r\n提供独立前端"
                                                         : L"\r\n仅后台运行"))
                                  .c_str());
    } else if (id == 1101)
      Refresh();
    else if (id == 1102) {
      int n = RB_Plugin_StartRuntimes();
      Status(n < 0 ? L"启动失败"
                   : L"已启动/存在 " + std::to_wstring(n) + L" 个运行态",
             n < 0);
    } else if (id == 1103) {
      int r = static_cast<int>(SendMessageW(list_, LB_GETCURSEL, 0, 0));
      if (r >= 0) {
        int rc = RB_Plugin_Launch(cat_.Items[r].Index);
        Status(ResultText(rc), rc != RB_OK);
      }
    }
  }
};
} // namespace
std::unique_ptr<Page> MakeOutputPage(HWND h, SdkRuntime &s) {
  return std::make_unique<Output>(h, s);
}
std::unique_ptr<Page> MakeSerialPage(HWND h, SdkRuntime &s) {
  return std::make_unique<Serial>(h, s);
}
std::unique_ptr<Page> MakeCustomGamePage(HWND h, SdkRuntime &s) {
  return std::make_unique<Custom>(h, s);
}
std::unique_ptr<Page> MakePluginPage(HWND h, SdkRuntime &s) {
  return std::make_unique<Plugins>(h, s);
}
