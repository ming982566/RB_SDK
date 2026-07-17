#pragma once
#include "RaceBearSDK.h"
#include <string>
#include <vector>

class SdkRuntime {
public:
  ~SdkRuntime();
  bool Start(std::wstring &error);
  void Stop();
  bool ReadState(RB_RuntimeState &state, std::wstring &error) const;
  std::vector<std::wstring> DrainLogs();
  bool Command(const char *command, const std::string &json,
               std::wstring &result, std::wstring &error) const;
  bool Started() const { return started_; }

private:
  bool started_{};
};
