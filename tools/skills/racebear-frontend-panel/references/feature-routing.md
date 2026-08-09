# Feature Routing

Load only the sections needed for the requested page. Confirm every exact signature in `include/RaceBearSDK.h` before implementation.

| Frontend surface | Start with | Read in the frontend guide |
| --- | --- | --- |
| Runtime summary | `RB_State_Read` | Sections 2 and 5 |
| License | `state.License`, `RB_Command_Run` license commands | Section 6 |
| Games and installs | `RB_Game_*` | Section 7 |
| Telemetry catalog and values | `RB_Telemetry_*` | Sections 7 and 14 |
| Platform configuration and tests | `RB_Platform_*`, `RB_ManualPose_*` | Section 8 |
| Dynamic filters | `RB_Dynamic_*` | Section 9 |
| Motion effects | `RB_MotionEffect_*` | Section 10 |
| Haptics | `RB_Haptic_*` | Section 11 |
| Wind and seatbelt | `RB_Wind_*`, `RB_Seatbelt_*` | Section 12 |
| Output instances | `RB_Output_*` | Section 13 |
| Custom games and external sources | `RB_Game_RegisterCustom`, `RB_ExternalSource_*`, telemetry FieldId APIs | Section 14 |
| Dashboards and screens | `RB_Dashboard_*`, `RB_VoCore_*`, `RB_DashboardWeb_*` | Section 15 and `references/dashboard-output.md` |
| Serial | `RB_Serial_*` | Section 16 |
| Plugins, logs, diagnostics | `RB_Plugin_*`, `RB_Log_*`, `RB_Debug_*` | Section 17 |
| Commands and advanced JSON | `RB_Command_Run`, `RB_Data_*Json` | Section 18 |

## Page Design Rules

- Use a dropdown or searchable selector for catalog values. Do not require users to type SDK keys, data bindings, output kinds, profile names, or package IDs when the SDK can enumerate them.
- Keep the selected stable key/ID in page state. If a catalog refresh removes it, show an unavailable state instead of silently selecting a different entry.
- Disable commands that are invalid for the current snapshot, but keep disconnect, reset, and recovery actions reachable.
- Show SDK error results beside the action that caused them. Do not replace all failures with a generic toast.
- Use typed numeric controls with documented ranges and units. Do not expose raw structure memory, reserved fields, or runtime indexes.
- Keep read, apply, save, connect, test, and delete as distinct user actions when their semantics differ.

## Framework Adapter Shape

Keep the SDK adapter independent of visual components:

- one lifecycle service owns initialize/start/shutdown;
- one state store publishes immutable runtime snapshots;
- feature repositories wrap catalog and typed config APIs;
- owned-handle services wrap serial, VOCORE, Web, and dashboard package lifetimes;
- UI components consume view models and issue explicit commands.

Use the target framework's normal FFI:

- C/C++: include the public header and link the matching import library.
- C#: use explicit `DllImport`, fixed-layout structures, fixed buffers or safe marshaling helpers, and retained callback delegates.
- Qt: keep the C ABI adapter outside widgets and emit queued signals to the UI thread.
- Web/Electron: call the native SDK through a narrow local/native bridge; do not expose arbitrary SDK calls to untrusted page content.
- Python: use dependency-free `ctypes` where appropriate and keep callback objects alive for the registration lifetime.

Do not introduce a new UI framework or state library solely for the integration when the host already has an established pattern.
