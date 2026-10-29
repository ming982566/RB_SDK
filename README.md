# RaceBear SDK

RaceBear SDK is a Windows x64 C ABI SDK for building telemetry, motion,
haptics, output, and dashboard frontend applications.

## 0.6.1

- The DLL reports runtime version `0.6.1`.
- This maintenance release preserves the public C ABI, structure layouts, and
  public protocols, and remains compatible with `0.6.0` callers.

## 0.6.0

- The DLL reports runtime version `0.6.0`.
- Added platform and DynamicV2 configuration package import/export APIs.
- Added the public SimHub/external-host CAN latest-position submission workflow.
- External realtime mode is opt-in; normal SimRacebear output continues to use
  the SDK-owned position channel by default.
- The public SDK still covers only the documented `RB_CAN` device-management,
  diagnostics, configuration-package, and external-position workflows.
  CAN motor parameter maintenance, `FM_CAN`, `FC_CAN`, `RDS_CAN`,
  `SEATBELT_CAN`, raw SDO/PDO, firmware-internal protocols, and SimRacebear
  private UI adapters remain outside the public handoff boundary.

## 0.5.1

- Fixed repeated configuration-file parsing by caching parsed JSON documents in the SDK.
- DynamicV2, PlatformV2, Output, SystemConfig, and extra-effect reads now reuse the in-memory configuration cache during normal runtime.
- Typed saves update the cache, while `RB_Data_WriteJson()` invalidates the matching cache after raw JSON replacement.
- Output AutoConnect counting, automatic connection, and frontend summaries use the cached output directory.
- Added a configuration-cache regression smoke test. The public C ABI, structure layouts, and firmware protocols are unchanged.

## 0.5.0

- Added the public RB_CAN device-management workflow: CANAPP discovery,
  logical-index assignment, stable UID binding, binding and motor state,
  health diagnostics, and user-triggered recovery.
- Added customer dashboard packages (`.rbdash` V1) with stable `rb.*` data
  bindings, package discovery, validation, page selection, and BGRA8888
  rendering.
- Added the dependency-free Dashboard Designer in `tools/DashboardDesigner`.
  It runs directly as local HTML/CSS/JavaScript and exports a single `.rbdash`
  file.
- Added common vehicle components including circular needle gauges, shift
  lights, warnings, tire state, pedals, text, images, arcs, and bars.
- Added the RaceBear Starter package and retained the four built-in dashboard
  styles: AIM MXS, BOSCH DDU8, SimpleDash, and Ford GT.
- Added the reusable, tool-neutral `racebear-frontend-panel` AI coding Skill
  for frontend integration, review, and debugging.
- Added the RGBAPP WinUSB workflow for four WS2812B outputs, layered telemetry
  effects, system-audio volume effects, device configuration, diagnostics, and
  APP firmware updates.

## Package Contents

| Path | Purpose |
| --- | --- |
| `include/RaceBearSDK.h` | Public C ABI header |
| `bin/x64/Debug` | Matching Debug DLL, import library, optional VOCORE dependency, and dashboard assets |
| `bin/x64/Release` | Matching Release DLL, import library, optional VOCORE dependency, and dashboard assets |
| `docs/RaceBearSDK_FRONTEND_GUIDE.md` | Page-by-page frontend workflow and configuration ownership rules |
| `docs/RaceBearSDK_API.md` | API reference, structures, units, deployment, and examples |
| `docs/RaceBearSDK_RB_CAN_GUIDE.md` | RB_CAN discovery, logical-index assignment, stable UID binding, diagnostics, and recovery |
| `examples/cpp` | Standard C++17 and CMake example |
| `examples/python` | Dependency-free Python `ctypes` binding and example |
| `examples/qt` | Qt 5/Qt 6 integration guide |
| `examples/RaceBearMotionStudio` | Native VS2019 Windows frontend example |
| `tools/DashboardDesigner` | Local browser dashboard editor |
| `tools/skills/racebear-frontend-panel` | Tool-neutral AI frontend integration Skill |

The public package is organized around the stable C ABI and includes the
headers, binaries, resources, documentation, tools, and examples needed for
application integration.

The public package is x64 only. The separate Win32 SDK DLL used by the private
SimHub x86 plugin is built and maintained in `RaceBear_SDK\bin\Win32\Release`;
it is not part of this repository's x64 package. The SimRacebear炫彩 mirror
is maintained in the private frontend directory and is checked against the
public header by the SDK ABI gate before a release.

## Release Workflow

For this repository, an SDK request to "push" means a complete versioned
release unless the request explicitly says to push the branch only. A complete
release must publish all of the following as one version:

- the verified `main` commit;
- the matching `vX.Y.Z` Git tag;
- the GitHub Release for that tag;
- `RaceBearSDK-X.Y.Z-vs2019-x64.zip`; and
- the matching `.sha256` sidecar file.

Run the repository release script from `RB_SDK` after the SDK source, build
outputs, documentation, and public repository status have been checked:

```powershell
pwsh -NoProfile -ExecutionPolicy Bypass -File .\tools\Publish-Release.ps1 -Version 0.6.1
```

The script uses the sibling `RaceBear_SDK` development directory by default.
It rebuilds Debug and Release with VS2019 x64, copies only public package
contents, checks for private source leakage, creates the archive and SHA256,
commits the verified public tree, pushes `main` and the version tag, and then
creates or updates the GitHub Release with both package files attached.

The archive and SHA256 files are intentionally ignored by `.gitignore`; they
must be uploaded as Release assets rather than committed to the source tree.
Never reuse an existing version for a different build. If only a branch push is
needed, state that explicitly so the tag and Release steps are not performed.
See [`docs/RELEASE_PROCESS.md`](docs/RELEASE_PROCESS.md) for the full checklist.

## Quick Start

Use the header, DLL, and LIB from the same Debug or Release configuration.

```cpp
#include "RaceBearSDK.h"

int main()
{
    if (RB_Runtime_SetAppDataName("YourProductName") != RB_OK)
        return 1;
    if (RB_Runtime_Initialize() != RB_OK)
        return 2;
    if (RB_Runtime_StartLoop(2) != RB_OK) {
        RB_Runtime_Shutdown();
        return 3;
    }

    RB_RuntimeState state{};
    RB_State_Read(&state);

    RB_Runtime_Shutdown();
    return 0;
}
```

Normal frontends should let the SDK own the backend calculation loop and read
`RB_RuntimeState` from a separate 30-60 Hz UI timer. Catalogs and complete
configuration structures should be refreshed only when needed.

## Deployment

Place `RaceBearSDK.dll` beside the host executable. The target computer needs
the Microsoft Visual C++ 2015-2022 Redistributable x64.

- VOCORE output: also deploy `libusb-1.0.dll` beside the host executable.
- Ford GT and bundled customer dashboards: keep
  `DashboardAssets/RaceBear` beside `RaceBearSDK.dll` with its directory
  structure unchanged.
- Applications should use a stable product-specific AppData name instead of
  sharing the SimRacebear configuration directory.

The Debug and Release package directories contain matching headers, import
libraries, DLLs, runtime dependencies, and dashboard resources from the same
verified 0.6.1 build.

## Frontend Rules

- Persist stable game, telemetry, output, and dashboard keys or IDs. Runtime
  indexes and catalog row numbers are not persistent identifiers.
- Prefer typed configuration APIs. Use JSON APIs only for advanced editors and
  migration tools.
- Read license state from `RB_RuntimeState::License` and submit license actions
  through the documented command API.
- Stop UI timers and callbacks, reset manual tests, request output disconnect,
  close owned handles, and then call `RB_Runtime_Shutdown()` once.
- Consult the frontend guide before implementing platform, Dynamic, Motion
  Effect, Haptic, Wind, Seatbelt, output, serial, VOCORE, or dashboard pages.

## Dashboard Designer

Open `tools/DashboardDesigner/index.html` directly, or run
`Start-DashboardDesigner.cmd`. Python, Node.js, and a local HTTP server are not
required.

Exported `.rbdash` files can be imported through the dashboard package API.
Templates are declarative and bind to recognized stable `rb.*` keys. The format
contains visual data only, with no script, filesystem, network, process, or SDK
control capabilities.

## AI-Assisted Development

Supply the complete Skill directory to the AI coding tool using that tool's
own Skill or reusable-instruction import mechanism. Installation locations,
invocation syntax, rule filenames, and optional interface metadata vary by
tool, so this SDK does not prescribe them.

After importing it, give the agent a concrete task:

```text
Add RaceBear game selection, license state, configuration pages, and dashboard
output to this Qt application using the provided RaceBear frontend Skill.
```

The Skill guides an AI coding agent through ABI checks, lifecycle ownership,
stable identifiers, configuration transactions, handles, deployment, and
validation. The current public header and documentation remain authoritative.

## Build Compatibility

- Host platform: Windows x64
- SDK toolset: Visual Studio 2019, `v142`
- Public boundary: C ABI with fixed-layout structures and UTF-8 text buffers
- C++ example: C++17
- Python example: 64-bit Python 3.8+ using the standard-library `ctypes` module

See `docs/RaceBearSDK_FRONTEND_GUIDE.md` first, then use
`docs/RaceBearSDK_API.md` for detailed signatures, ranges, units, and failure
behavior. Frontends that manage RB_CAN hardware should also use
`docs/RaceBearSDK_RB_CAN_GUIDE.md`.
