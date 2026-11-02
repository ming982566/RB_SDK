# Feature Routing

Load only the sections needed for the requested page. Confirm every exact signature in `include/RaceBearSDK.h` before implementation.

Use `docs/RaceBearSDK_FRONTEND_UI_SPEC.md` section 2 for containers and required actions.
The table below routes API reading, not a promise that every export needs an independent page.

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
| Wind and seatbelt algorithms | `RB_Wind_*`, `RB_Seatbelt_*` | Section 12 |
| Seatbelt CAN hardware | `RB_SeatbeltCan_*` with the selected output instance; explicit `RequestReenable` after protection | API Seatbelt CAN section; CAN guide dedicated seatbelt section |
| RGBAPP lighting | `RB_Rgb_*`, effect-specific `RB_Rgb_GetEffectInputCount/GetEffectInput` | Section 12.1 and API section 8.1.6 |
| Output instances | `RB_Output_*` | Section 13 |
| RB_CAN devices and binding | `RB_Can_RefreshDevices`, `RB_Can_GetDeviceInfo`, `RB_Can_BindOutputDevice`, binding/motor/health APIs | Section 13 and `docs/RaceBearSDK_RB_CAN_GUIDE.md` |
| Custom games and external sources | `RB_Game_RegisterCustom`, `RB_ExternalSource_*`, telemetry FieldId APIs | Section 14 |
| Dashboards and screens | `RB_Dashboard_*`, `RB_VoCore_*`, `RB_DashboardWeb_*` | Section 15 and `references/dashboard-output.md` |
| Serial | `RB_Serial_*` | Section 16 |
| Plugins, logs, diagnostics | `RB_Plugin_*`, `RB_Log_*`, `RB_Debug_*` | Section 17 |
| Commands and advanced JSON | `RB_Command_Run`, `RB_Data_*Json` | Section 18 |
| System settings and loop diagnostics | `RB_System_*`, `RB_Runtime_*` | Sections 1-2 and API system configuration |

For CAN maintenance and firmware tasks, use the CAN guide section 12 and frontend guide
section 19. Keep lifecycle/supporting exports in the service layer. New MotionProtection
editors use V2; V1 is compatibility support, not a second active set of ordinary controls.

## Page Design Rules

- Use a dropdown or searchable selector for SDK-enumerated keys, data bindings, output kinds, profile names, and package IDs.
- Keep the selected stable key/ID in page state. If a catalog refresh removes it, show an unavailable state instead of silently selecting a different entry.
- Disable commands that are invalid for the current snapshot, but keep disconnect, reset, and recovery actions reachable.
- Show SDK error results beside the action that caused them so developers and users retain the relevant diagnostic context.
- Use typed numeric controls with documented ranges and units, while keeping reserved fields and runtime-only indexes in the adapter layer.
- Keep read, apply, save, connect, test, and delete as distinct user actions when their semantics differ.
- In an RB_CAN page, restore selection by stable UID after every device refresh. Keep duplicate-index devices selectable so the user can assign one target UID a new unique index without unplugging the other device.
- In an RGB page, separate host effect Save from device Flash Save. Effects retain telemetry `Key`; devices retain UID; Apply operations must not imply persistence.
- Drafts retain their original target. Switching profiles or outputs reloads the complete detail
  view; failed loading disables writes instead of leaving an old configuration active.
- Handle loading, empty, unavailable, busy, invalid, failed and stale states, not just the
  successful case. Use user-facing labels rather than raw field/algorithm names.
- Map every field to editable, read-only, compatibility-preserved, reserved, or out-of-scope;
  unsupported/legacy fields must not look like functional controls.

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
- Web/Electron: call the native SDK through a narrow local/native bridge that exposes only the operations needed by trusted page content.
- Python: use dependency-free `ctypes` where appropriate and keep callback objects alive for the registration lifetime.

Follow the host application's established UI framework and state-management pattern unless the requested work explicitly calls for a change.
