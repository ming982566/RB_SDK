// RaceBear Dynamic Filter Save/Load comprehensive test
#include "RaceBearSDK.h"
#include <cstdio>
#include <cstring>

static int errors = 0;

static const char* R(int rc) {
  switch (rc) {
    case RB_OK: return "OK";
    case RB_ERROR: return "RB_ERROR";
    case RB_INVALID_ARGUMENT: return "RB_INVALID_ARGUMENT";
    case RB_LICENSE_REQUIRED: return "RB_LICENSE_REQUIRED";
    default: { static char b[32]; sprintf_s(b, "%d", rc); return b; }
  }
}
#define C(expr, msg) do { int r = (expr); if (r != RB_OK) { printf("  FAIL [%s] %s: %s\n", msg, #expr, R(r)); errors++; } else printf("  PASS [%s]\n", msg); } while(0)

int main() {
  printf("=== RaceBear Dynamic Filter Save/Load Test ===\n\n");

  printf("[1] Initialize\n");
  C(RB_Runtime_SetAppDataName("RaceBearTest"), "SetAppDataName");
  C(RB_Runtime_Initialize(), "Initialize");
  C(RB_Runtime_StartLoop(2), "StartLoop");

  printf("\n[2] Telemetry catalog\n");
  RB_TelemetryCatalog tc{};
  C(RB_Telemetry_GetCatalog(&tc), "GetCatalog");
  printf("  %d values\n", tc.Count);

  RB_DynamicInputCandidateCatalog candidates{};
  C(RB_Dynamic_GetInputCandidateCatalog(0, &candidates), "Get Sway candidates");
  if (candidates.Count <= 0) {
    printf("  No Sway candidates\n");
    RB_Runtime_Shutdown();
    return 1;
  }
  const RB_DynamicInputCandidateItem candidate = candidates.Items[0];

  printf("\n[3] Profiles\n");
  int pc = RB_Dynamic_GetProfileCount();
  printf("  %d profiles\n", pc);
  if (pc <= 0) { printf("  No profiles\n"); RB_Runtime_Shutdown(); return 0; }

  printf("\n[4] Read profile 0\n");
  RB_DynamicV2Profile p{};
  C(RB_Dynamic_ReadProfileByIndex(0, &p), "ReadProfileByIndex");
  char pn[128]{};
  RB_Dynamic_GetProfileName(0, pn, 128);
  printf("  Name: %s  DOF[0] Inputs=%d Enabled=%d\n", pn, p.Dofs[0].InputCount, p.Dofs[0].Enabled);

  printf("\n[5] Backup original then modify profile 0 in place\n");
  // Save original for restore
  C(RB_Dynamic_SaveProfile("_BACKUP_ORIG", &p), "Backup original");
  
  // Modify profile 0
  {
    p.Dofs[0].Enabled = 1;
    auto& d = p.Dofs[0];
    d.InputCount = 1;
    auto& in = d.Inputs[0];
    memset(&in, 0, sizeof(in));
    in.Enabled = 1; in.InputType = candidate.InputType;
    in.TelemetryIndex = candidate.TelemetryIndex;
    strcpy_s(in.Key, sizeof(in.Key), candidate.Key);
    in.Filter.Enabled = 1; in.Filter.Dof = candidate.InputType;
    in.Filter.InMapping = 1; in.Filter.InGain = 1.0; in.Filter.OutScaling = 100;
    printf("  Replaced test input: count=%d type=%d key=%s\n", d.InputCount, in.InputType, in.Key);

    printf("\n[6] Save by name then by index\n");
    int sr = RB_Dynamic_SaveProfile("_Default", &p);
    printf("  SaveByName: %s\n", R(sr));
    int sr2 = RB_Dynamic_SaveProfileByIndex(0, &p);
    printf("  SaveByIndex: %s\n", R(sr2));
    
    // Re-find the profile index after save
    int ri = -1;
    for (int i = 0; i < RB_Dynamic_GetProfileCount(); ++i) {
      char b[128]{};
      if (RB_Dynamic_GetProfileName(i, b, 128) == RB_OK && !strcmp(b, "_Default")) { ri = i; break; }
    }
    printf("  _Default now at index: %d\n", ri);
    if (ri < 0) { printf("  FAIL: _Default not found!\n"); errors++; }

    printf("\n[6.5] Reload config before readback\n");
    RB_Command_Run("runtime.reloadConfig", "{}", nullptr, 0);
    
    int readIdx = ri >= 0 ? ri : 0;
    printf("\n[7] Read back index %d\n", readIdx);
    // Try with Size pre-set to sizeof (SDK doc says Size=0 means default, but try both)
    RB_DynamicV2Profile v{};
    v.Size = sizeof(RB_DynamicV2Profile); // Explicitly set Size
    v.Version = 1;
    printf("  v.Size=%d v.Version=%d (before read, preset)\n", v.Size, v.Version);
    C(RB_Dynamic_ReadProfileByIndex(readIdx, &v), "ReadProfileByIndex");
    printf("  v.Size=%d v.Version=%d (after read)\n", v.Size, v.Version);
    printf("  p.Size=%d p.Version=%d (saved data)\n", p.Size, p.Version);
    printf("  sizeof(RB_DynamicV2Profile)=%zu\n", sizeof(RB_DynamicV2Profile));
    printf("  DOF[0] InputCount: %d (expect 1)\n", v.Dofs[0].InputCount);
    printf("  DOF[0] Inputs[0].Key: '%s'\n", v.Dofs[0].InputCount > 0 ? v.Dofs[0].Inputs[0].Key : "(no inputs)");
    printf("  DOF[0] Enabled: %d\n", v.Dofs[0].Enabled);
    
    // Also read raw JSON to see what's on disk
    printf("\n  Raw JSON config.dynamicV2:\n");
    char jbuf[65536]{};
    int jr = RB_Data_ReadJson("config.dynamicV2", jbuf, sizeof(jbuf));
    if (jr == RB_OK) {
      printf("  %.4000s\n", jbuf); // print first 4000 chars
    } else {
      printf("  RB_Data_ReadJson: %s\n", R(jr));
    }
    if (v.Dofs[0].InputCount == 1 && v.Dofs[0].Enabled == 1) {
      printf("  PASS: count and enabled OK\n");
      auto& vi = v.Dofs[0].Inputs[0];
      printf("  [0] En=%d IT=%d Key=%s OS=%d\n", vi.Enabled, vi.InputType, vi.Key, vi.Filter.OutScaling);
      if (vi.Enabled==1 && vi.InputType==candidate.InputType &&
          strcmp(vi.Key, candidate.Key)==0 && vi.Filter.OutScaling==100)
        printf("  PASS: values OK\n");
      else { printf("  FAIL: values mismatch\n"); errors++; }
    } else {
      printf("  FAIL: count=%d enabled=%d\n", v.Dofs[0].InputCount, v.Dofs[0].Enabled);
      errors++;
    }
  }

  printf("\n[8] Cleanup - remove backup profile\n");
  for (int i = 0; i < RB_Dynamic_GetProfileCount(); ++i) {
    char b[128]{};
    if (RB_Dynamic_GetProfileName(i, b, 128) == RB_OK && strcmp(b, "_BACKUP_ORIG") == 0) {
      RB_Dynamic_DeleteProfileByIndex(i);
      printf("  Removed _BACKUP_ORIG at index %d\n", i);
      break;
    }
  }

  printf("\n[9] Shutdown\n");
  RB_Runtime_Shutdown();
  printf("\n=== %s: %d errors ===\n", errors ? "FAILED" : "PASSED", errors);
  return errors;
}
