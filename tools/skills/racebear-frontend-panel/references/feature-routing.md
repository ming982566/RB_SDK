# Feature Routing

Load only the sections needed for the requested page. Confirm every exact structure, enum,
signature and service argument in `include/RaceBearSDK.h` and the generated service directory.
The table routes documentation reading; it does not imply that every operation needs a page.

| Frontend surface | Start with | Read next |
| --- | --- | --- |
| Unified object ABI | `RB_Object_*`, Describe registry | `docs/RaceBearSDK_OBJECT_API.md`, `docs/RaceBearSDK_SERVICE_API.md` |
| Runtime summary and license | `rb.runtime.state` | `RaceBearSDK_FRONTEND_GUIDE.md` sections 2 and 3 |
| Games and installs | `rb.service.game` | Service directory entries beginning with `rb.game.` |
| Telemetry catalog and values | `rb.telemetry.catalog`, `rb.telemetry.snapshot` | Object API sections 2 and 3 |
| Platform configuration and tests | `rb.service.platform`, `rb.service.manualpose` | Service directory and authorization rules |
| Dynamic filters | `rb.service.dynamic` | Service directory entries beginning with `rb.dynamic.` |
| Motion effects | `rb.service.motioneffect` | Service directory entries beginning with `rb.motioneffect.` |
| Haptics | `rb.service.haptic` | Service directory entries beginning with `rb.haptic.` |
| Wind and seatbelt algorithms | `rb.wind.config`, `rb.seatbelt.config`, `rb.service.wind`, `rb.service.seatbelt` | Object API sections 2, 4 and 5 |
| Seatbelt CAN hardware | `rb.service.seatbeltcan` | CAN guide safety and explicit re-enable workflow |
| RGBAPP lighting | `rb.rgb.effects`, `rb.rgb.state`, `rb.service.rgb` | RGB effect input and device operations in the service directory |
| Output instances | `rb.service.output` | Output catalog, connection and disconnect operations |
| RB_CAN devices and binding | `rb.service.can` | `docs/RaceBearSDK_RB_CAN_GUIDE.md` |
| Custom games and external sources | `rb.service.game`, `rb.service.externalsource`, `rb.service.telemetry` | Stable key and frame contracts |
| Dashboards and screens | `rb.service.dashboard`, `rb.service.vocore`, `rb.service.dashboardweb` | `docs/RaceBearSDK_DASHBOARD_DEVELOPMENT.md` and `references/dashboard-output.md` |
| Serial | `rb.service.serial` | Handle, callback and close rules in the service directory |
| Plugins, logs and diagnostics | `rb.service.plugin`, `rb.service.log`, `rb.service.debug` | Single-consumer and handle ownership rules |
| Commands and advanced JSON | `RB_Object_Invoke`, `RB_Object_Read/Write` | Object API sections 4 and 5 |
| System settings | `rb.service.system`, `rb.service.runtime` | Service directory and lifecycle rules |

## Page Design Rules

- Use a selector for SDK-enumerated keys, data bindings, output kinds, profile names and package IDs.
- Keep stable key, UID or package ID in page state; catalog indexes are snapshot-local.
- Disable operations invalid for the current snapshot while keeping disconnect, reset and recovery reachable.
- Show the returned error beside the action that caused it.
- Keep read, apply, save, connect, test and delete as distinct actions.
- Restore CAN selection by UID after every refresh and keep duplicate logical indexes visible for repair.
- Separate host RGB effect Save from device Flash Save; effect keys and device UIDs have different lifetimes.
- Treat unsupported fields as read-only or out of scope; never render them as working controls.

## Adapter Shape

Keep the SDK adapter independent from visual components:

- one lifecycle owner for initialize, runtime commands and shutdown;
- one immutable state store for snapshots;
- one repository per catalog/configuration domain;
- one owned-handle service for serial, VOCORE, Web and dashboard packages;
- UI components consume view models and issue explicit object commands.

Use the host framework's normal FFI and marshal callbacks to the UI thread. Native service packets
are in-process only. Do not serialize pointer slots or let untrusted web content call arbitrary
service keys.
