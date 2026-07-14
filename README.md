# RaceBear SDK

VS2019 x64 DLL SDK package for the RaceBear backend.

## Start Here

Third-party developers should read:

- `include/RaceBearSDK.h`: minimal public C ABI header for normal frontends.
- `RaceBearSDK_API.md`: integration guide, call order, examples, field units, and common failure causes.
- `examples/cpp`: buildable VS2019/CMake C++ console example.
- `examples/python`: dependency-free Python `ctypes` binding and runnable example.
- `examples/qt`: Qt 5/Qt 6 integration guide and adapter code.

The SDK is intended to be usable with only the public header, import library,
DLL, and documentation. Backend source code is not required for normal
integration.

`include/RaceBearSDK.h` is the only public SDK header. Do not add secondary public headers for compatibility layers unless there is a confirmed ABI requirement.

## Package Layout

- `bin/x64/Debug/RaceBearSDK.dll`: Debug runtime with diagnostic output.
- `bin/x64/Debug/RaceBearSDK.lib`: Debug import library.
- `bin/x64/Release/RaceBearSDK.dll`: Release runtime for end-user applications.
- `bin/x64/Release/RaceBearSDK.lib`: Release import library.
- `include/RaceBearSDK.h`: public C/C++ API header.
- `RaceBearSDK_API.md`: Chinese integration guide.
- `examples/`: C++, Python, and Qt integration examples.

The package is built with Visual Studio 2019 `v142` for Windows x64. Host
applications must also be x64.

## Public Header

Third-party code should include:

```cpp
#include "RaceBearSDK.h"
```

The default public API is intentionally C ABI style and does not expose C++
classes, `std::string`, `std::vector`, or UI-framework-specific types across
the DLL boundary.

Minimal runtime order:

```cpp
RB_Runtime_SetAppDataName("YourProductName"); // optional, call before RB_Runtime_Initialize
RB_Runtime_Initialize();
RB_Runtime_StartLoop(2);                      // intervalMs: calculate about once every 2 ms
RB_Runtime_Shutdown();
```

Recommended frontend integration:

```cpp
RB_State_Read(&state);                                      // high-frequency state struct
RB_Game_GetCatalog(&games);                                 // low-frequency catalog struct
RB_Output_ReadInstanceConfigByIndex(0, &output);             // read complete typed config
RB_Output_SaveInstanceConfigByIndex(0, &output);             // persist typed config
RB_Command_Run("output.connectAuto", "{}", buffer, size);   // actions
RB_Log_Read(buffer, size);                                  // runtime log queue
```

Normal C++/Qt/Python frontends use the typed dynamic, output, motion, haptic,
wind, and seatbelt configuration APIs. `RB_Data_ReadJson()` and
`RB_Data_WriteJson()` remain available for scripts, migration tools, and advanced
editors, but end users should never need to edit SDK JSON directly.

Game installation/source configuration, functional plugin catalogs, platform
diagnostics, and limited manual pose testing are also public typed APIs. Complete
CAN device management and LAN control remain product-private in this release.

Third-party games can feed raw telemetry through `RB_ExternalSource_Open()` and
`RB_ExternalSource_SubmitFrame()`. Persist RaceBear telemetry keys such as
`rb.vehicle.speed`; resolve runtime indices with `RB_Telemetry_FindIndexByKey()`
when the source starts. Runtime indices are never a configuration or wire format.

Games developed by SDK customers should use `RB_Game_RegisterCustom()`. Registered
games enter the normal catalog and process-detection flow, persist in
`CustomGamesV1.json`, and receive either the shared MMF/UDP V1 packet protocol in
raw telemetry mode or normalized direct-6DOF mode. Wire packets use stable FieldId
values from `RB_Telemetry_FindFieldIdByKey()`, never runtime indices.

The public header contains no C++ convenience overloads or inline implementations. Product-specific convenience functions belong in the host application's private adapter layer. Stable typed configuration and device-independent operation APIs are public; SimRacebear UI, language, and drawing adapters remain private.

## App Data Name

The SDK stores local config under `%APPDATA%\<AppDataName>\configV2` and related
user files under `Documents\<AppDataName>`. The default app data name is
`Racebear`.

Third-party apps can override it before runtime initialization:

```cpp
RB_Runtime_SetAppDataName("ThirdPartyName");
RB_Runtime_Initialize();
```

The first initialization of a new name creates independent default platform and
dynamic configuration. A third-party app must not reuse or copy the `Racebear`
directory to obtain defaults.

## License Runtime

`RB_Runtime_Initialize()` starts the SDK license runtime. Missing or invalid local
licenses enter a local trial countdown first; when the countdown reaches zero, or
when a timed license expires or is revoked/disabled, the SDK locks real output
capabilities. Perpetual licenses are validated locally and do not run a periodic
online heartbeat.

When output is locked:

- Telemetry/config/query APIs remain available.
- Activation/deactivation commands remain available.
- New output connections are rejected.
- Existing outputs are driven through the normal safe disconnect path when
  possible.

Read license display state from `RB_RuntimeState::License` and submit actions through the command API:

```cpp
RB_RuntimeState state{};
RB_State_Read(&state);

RB_Command_Run("license.check", "{}", buffer, size);
RB_Command_Run("license.activate", "{\"activationCode\":\"...\"}", buffer, size);
RB_Command_Run("license.deactivate", "{}", buffer, size);
```

## Data Packages And Commands

Configuration files are exposed as whole JSON documents. This keeps the C ABI
stable while backend config fields continue to evolve.

```cpp
RB_Data_ReadJson("config.output", buffer, size);
RB_Data_WriteJson("config.output", jsonText);
RB_Command_Run("runtime.reloadConfig", "{}", buffer, size);
```

Readable JSON keys:

- `runtime.snapshot`
- `config.telemetry`
- `config.dynamicV2`
- `config.platform`
- `config.output`
- `game.catalog`
- `telemetry.catalog`
- `output.catalog`
- `motionEffect.catalog`
- `haptic.catalog`
- `can.state`
- `plugin.catalog`

Writable JSON keys:

- `config.telemetry`
- `config.dynamicV2`
- `config.platform`
- `config.output`

Common commands use a string command name and optional JSON args:

```cpp
RB_Command_Run("license.check", "{}", buffer, size);
RB_Command_Run("license.activate", "{\"activationCode\":\"...\"}", buffer, size);
RB_Command_Run("game.launch", "{\"gameKey\":\"Assetto Corsa\"}", buffer, size);
RB_Command_Run("output.connectAuto", "{}", buffer, size);
```

## Distribution Layout

- `include/` public SDK headers.
- `bin/` Debug and Release runtime binaries.
- `examples/` third-party C++, Python, and Qt integration examples.
- `RaceBearSDK_API.md` complete integration guide.

## Notes

- The SDK project uses VS2019 toolset `v142`.
- This package does not include backend source code. Normal integration only needs the header, import library, DLL, documentation, and examples.

