# RaceBear SDK

RaceBear SDK provides the x64 backend runtime, public C API, integration
documentation, and examples required to build a RaceBear-compatible
frontend.

## Package

- `bin/x64/Debug`: Debug DLL and import library with diagnostic output.
- `bin/x64/Release`: Release DLL and import library.
- `include/RaceBearSDK.h`: Public C/C++ API.
- `RaceBearSDK_API.md`: Chinese integration guide.
- `examples`: C++, Python, and Qt integration examples.

The current package is built with Visual Studio 2019 `v142` for
Windows x64. Applications loading the SDK must also be x64.

## Start

Read [RaceBearSDK_API.md](RaceBearSDK_API.md) before integration. The
minimum lifecycle is:

```cpp
RB_Runtime_SetAppDataName("YourCompanyName");
RB_Runtime_Initialize();
RB_Runtime_StartLoop(100); // 100 Hz = about 10 ms per calculation; not 100 ms

// Read state and operate the SDK.

RB_Runtime_Shutdown();
```

Configuration pages must read the existing full structure or JSON
document before editing it. Do not construct partial configuration
documents or hard-code game, platform, effect, or output-key lists.

## License

This repository is not open source. Use and redistribution are governed
by the [RaceBear SDK Proprietary License Agreement](LICENSE).

End-user redistribution is limited to the unmodified Release runtime as
part of a licensed application. Headers, import libraries, Debug builds,
documentation, and examples may not be redistributed as a standalone SDK.
