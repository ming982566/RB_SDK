# RaceBear SDK frontend coverage

Baseline: the repository root `../../include/RaceBearSDK.h`, matching `../../bin/x64` DLL/LIB files, and `../../RaceBearSDK_API.md`.

Status terms: **Implemented** means the UI calls the public API and checks its return value. **Public summary only** means the SDK intentionally exposes read-only/basic capability. **SDK gap** is documented in `SDK_GAPS.md`.

| Page / service | Read APIs and structures | Write / action APIs | Status |
| --- | --- | --- | --- |
| Lifecycle | `RB_Runtime_GetVersion`, loop getters | `RB_Runtime_SetAppDataName`, `Initialize`, start/pause/resume/stop, interval update, `Shutdown` | Implemented |
| Overview | `RB_State_Read` (`RB_RuntimeState`) | Loop pause/resume | Implemented |
| License | `RB_LicenseSnapshot` | `license.check`, `license.activate`, `license.deactivate` | Implemented |
| Games | `RB_Game_GetCatalog`, installation APIs, `RB_GameConfig` | select, auto-detect, launch/terminate, auto-configure, side check, plugin install, Save, AutoConnect | Implemented |
| Telemetry | `RB_Telemetry_GetCatalog`, raw/processed values, `RB_SourceState` | Source creation through game select/auto-connect | Implemented |
| Platform | dynamic platform/name/DOF/parameter APIs, `RB_PlatformConfig`, `RB_Motion6DOFState` | selected index, Apply, Save, manual pose test/reset | Implemented with host-drawn 6DOF limit preview |
| Dynamic filter | dynamic profile directory, `RB_DynamicV2Profile`, per-DOF input candidate catalog | supported channel add/remove, Apply, verified Save, copy, delete, game binding | Implemented with host-drawn transfer preview |
| Motion enhancement | effect/profile catalogs, `RB_MotionEffectProfile` | route add/remove, Apply, Save | Implemented |
| Haptic | effect catalog, `RB_HapticEffectProfile`, aggregate function state | complete effect editing, Apply, Save | Implemented; live per-effect state is SDK gap |
| Wind | `RB_WindEffectConfig`, `RB_WindState` | Apply, Save, guarded test and zero on leave/exit | Implemented |
| Seatbelt | `RB_SeatbeltEffectConfig`, `RB_SeatbeltState` | Apply, Save, guarded test and zero on leave/exit | Implemented; HID injection is SDK gap |
| Outputs | dynamic kinds/catalog/keys, `RB_OutputConfig` | add/delete, Apply, Save, per-instance/all connect/disconnect | Implemented |
| Serial | port enumeration and error APIs | full framing/flow/mode/timeout/reconnect/DTR/RTS configuration, open/read/write/close and callbacks | Implemented |
| Custom games | catalog and `RB_CustomGameState` | MMF/UDP RAW/6DOF registration including Steam ID, sender restriction and DOF options | Implemented |
| External source | `RB_ExternalSourceState`, telemetry catalog | open, submit frames, inspect and close | Implemented |
| Plugins | `RB_Plugin_GetCatalog` | refresh, start runtimes, launch supported frontend | Implemented |
| Logs / diagnostics | `RB_Log_Read`, monitor state | clear, host-file save, open/close monitor | Implemented |
| CAN | complete public `RB_CanState` controller, firmware and motor fields | basic CAN fields through `RB_OutputConfig` | Public read-only capability implemented |
| LAN | none | none | SDK gap |
| System | `RB_SystemConfig`, loop state | logging/telemetry/VR settings and loop controls | Implemented; product-owned language/background settings intentionally omitted |

## Configuration transaction rule

Every configuration page retains the complete structure returned by the SDK. Apply and Save are separate actions. Save performs a second SDK read and byte/field verification before reporting success. Controls never edit SDK JSON files directly, and all directories and selectable keys come from SDK catalog APIs.

## Safety policy

- Initialization always starts the SDK-owned loop with `RB_Runtime_StartLoop(2)`; the UI timer only reads state.
- No output is connected automatically by this host.
- Hardware controls are disabled while `License.OutputAllowed == 0`.
- Manual pose, wind test and seatbelt test are zeroed on page leave and process exit.
- Output connection fields are changed only after the selected instance is disconnected.
- Shutdown stops UI timers/callbacks first, requests output disconnect, destroys serial objects, zeros tests and finally calls `RB_Runtime_Shutdown()`.

## Build and smoke verification

Verified on 2026-07-17 with the installed Visual Studio 2019 Community 16.11 toolchain (`MSVC 19.29`, `v142`) and CMake generator `Visual Studio 16 2019`, platform x64.

| Check | Result |
| --- | --- |
| VS2019 x64 Debug build | Passed without compiler warnings |
| VS2019 x64 Release build | Passed without compiler warnings |
| Debug no-hardware SDK smoke | SDK package synchronized to `0.4.7-vs2019-x64` |
| Release public API smoke | Passed with SDK `0.4.7-vs2019-x64` |
| Extended catalog/config reads | Passed for platform, dynamic, motion, haptic, wind, seatbelt, plugins, outputs and serial enumeration |
| Dynamic arbitrary-channel round trip | Passed; save/read preserved `rb.vehicle.speed`, `InputCount=1` and filter fields |
| Release UI smoke | Main window created, timers ran, `WM_CLOSE` completed cleanup, process exited with code 0 |
| Private/legacy API scan | No `SimRacebear`, `RB_UI_*`, or manual `RB_Runtime_DoCalculations` use in frontend/test sources |
