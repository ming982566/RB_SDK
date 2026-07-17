# RaceBear Motion Studio

Native Windows x64 motion-simulation frontend for RaceBear SDK 0.4.7. It uses Win32/Common Controls, C++17 and the SDK-owned 2 ms runtime loop. No real output is connected automatically.

## Build with Visual Studio 2019

Open `RaceBearMotionStudio.sln`, select `x64` and build the required Debug or
Release configuration. The solution contains only the sample application;
test source files remain under `tests` and do not clutter the main solution.

Run:

```powershell
.\bin-app\Release\RaceBearMotionStudio.exe
```

The build copies the matching Debug or Release `RaceBearSDK.dll` beside the
executable from the repository root `bin` directory. The project also uses the
repository root `include/RaceBearSDK.h`; no SDK files are duplicated inside the
sample directory. Application data is isolated under the SDK app-data name
`RaceBearMotionStudio`.

## Hardware safety

- License-locked output and manual tests stay disabled.
- Output connection requires an explicit user click.
- Platform, wind and seatbelt tests are zeroed on page leave and shutdown.
- Connected output instances must be disconnected before connection settings can be saved or applied.
- Closing the app stops UI timers/callbacks, requests safe output disconnect, then shuts down the SDK.

See [SDK_COVERAGE.md](SDK_COVERAGE.md) for the interface map and [SDK_GAPS.md](SDK_GAPS.md) for capabilities not present in the public SDK.
