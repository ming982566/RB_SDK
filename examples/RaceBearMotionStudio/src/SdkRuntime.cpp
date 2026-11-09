#include "SdkRuntime.h"
#include "Ui.h"
#include <array>

SdkRuntime::~SdkRuntime() { Stop(); }
bool SdkRuntime::Start(std::wstring &error) {
  // AppData 名称只在初始化前生效，用于隔离示例程序和正式软件的配置。
  if (rb_example::Runtime_SetAppDataName("RaceBearMotionStudio") != RB_OK) {
    error = L"无法设置独立应用数据目录名称";
    return false;
  }
  int rc = RB_Runtime_Initialize();
  if (rc != RB_OK) {
    error = L"SDK 初始化失败: " + ResultText(rc);
    return false;
  }
  started_ = true;
  // 参数单位是 ms；2 表示由 SDK 内部以约 500 Hz 驱动后端循环。
  rc = rb_example::Runtime_StartLoop(2);
  if (rc != RB_OK) {
    error = L"SDK 2 ms 内部循环启动失败：" + ResultText(rc);
    Stop();
    return false;
  }
  return true;
}
void SdkRuntime::Stop() {
  if (!started_)
    return;
  RB_Runtime_Shutdown();
  started_ = false;
}
bool SdkRuntime::ReadState(RB_RuntimeState &s, std::wstring &error) const {
  // 高频 UI 读取一份完整运行态，不逐字段同步分派。
  int rc = rb_example::ReadBinary("rb.runtime.state", s);
  if (rc == RB_OK)
    return true;
  error = ResultText(rc);
  return false;
}
std::vector<std::wstring> SdkRuntime::DrainLogs() {
  std::vector<std::wstring> out;
  std::vector<char> b(4096);
  // 单次最多取 500 条，防止大量历史日志长时间占用 UI 线程。
  for (int i = 0; i < 500; ++i) {
    int rc = rb_example::Log_Read(b.data(), static_cast<int>(b.size()));
    if (rc == RB_BUFFER_TOO_SMALL && b.size() < 16 * 1024 * 1024) {
      b.resize(b.size() * 2);
      --i; // 日志尚未出队，扩容只重读同一条记录。
      continue;
    }
    if (rc != RB_OK) {
      out.push_back(L"RB_Log_Read: " + ResultText(rc));
      break;
    }
    if (!b[0])
      break;
    out.push_back(Utf8ToWide(b.data()));
  }
  return out;
}
bool SdkRuntime::Command(const char *c, const std::string &j, std::wstring &r,
                         std::wstring &e) const {
  // 动作只调用一次；容量不足报告错误，不以扩容为理由重发动作。
  std::vector<char> b(64 * 1024);
  int rc = rb_example::Command_Run(c, j.c_str(), b.data(), static_cast<int>(b.size()));
  r = Utf8ToWide(b.data());
  if (rc == RB_OK)
    return true;
  e = Utf8ToWide(b.data());
  if (e.empty())
    e = ResultText(rc);
  return false;
}
