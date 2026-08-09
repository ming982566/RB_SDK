# Dashboard And Output

Use `docs/RaceBearSDK_FRONTEND_GUIDE.md` section 15 for the integration workflow and the dashboard chapters in `docs/RaceBearSDK_API.md` for the package format, renderer, designer, VOCORE, Web output, and deployment contract.

## Pipeline

Treat rendering and delivery as separate operations:

1. Read the latest `RB_RuntimeState`.
2. Render an SDK built-in template with `RB_Dashboard_RenderFrame()`, or open a package by stable `PackageId` and use `RB_Dashboard_RenderPackageFrame()`.
3. Present BGRA8888 in the host window and/or submit the same latest frame to VOCORE or Dashboard Web.
4. Read each output handle's state on a slower status timer.
5. Close every handle before runtime shutdown.

Use a dedicated 16-33 ms render timer for typical dashboards. VOCORE and Web accept latest-frame replacement; do not wait for a pending frame to clear before rendering the next current frame.

## Customer Packages

- `.rbdash` V1 is a UTF-8 without BOM JSON file with PNG/JPEG Data URI images.
- Bind elements only to SDK-recognized stable `rb.*` data keys.
- Persist `PackageId`, not the package catalog index.
- Refresh the package catalog after import or delete, then relocate the selection by `PackageId`.
- Close the previous `RB_DashboardPackageHandle` when selection changes or the page exits.
- Treat the format as declarative data. Do not add script execution, arbitrary filesystem access, networking, process control, or SDK control calls to a template.

## Designer

The bundled designer lives at `tools/DashboardDesigner/index.html`. It runs as local HTML/CSS/JavaScript; `Start-DashboardDesigner.cmd` opens the file in the default browser and does not require Python, Node, an HTTP server, or a third-party runtime.

For designer UI work:

- provide catalog-backed data-binding dropdowns instead of free-text keys;
- make selection state visible and support multi-selection, alignment, distribution, layer order, duplication, deletion, undo/redo, zoom, and responsive canvas sizing where the current format supports them;
- ensure every palette component renders immediately after insertion and remains selectable;
- include common vehicle widgets such as circular needle gauges, digital values, shift lights, warning lamps, gear, lap/time, fuel, temperatures, tire state, pedals, steering, bars, arcs, text, and images;
- export a valid single-file `.rbdash` and clearly show the SDK import/user package destination described by the current API.

Do not promise pixel-perfect reproduction of a complex production cluster unless the current component model, font/assets, animations, masks, conditional visibility, and test data support it. State format limitations precisely.

## VOCORE

- Enumerate devices and open by `DeviceId`.
- Keep one `RB_VoCoreHandle` per screen and close it explicitly.
- Use `RB_VoCore_SetAutoReconnect()` for the selected policy.
- Do not destroy a handle merely because a device was unplugged when automatic reconnect should continue.
- Read `RB_VoCore_GetState()` for connection, reconnect, actual FPS, dropped/replaced frames, and last libusb error.
- Deploy `libusb-1.0.dll` beside the host EXE when VOCORE is used.

## Browser Dashboard

- Store the returned `RB_DashboardWebHandle`.
- Submit current frames asynchronously and read `RB_DashboardWeb_GetState()` for service/client/encoding status.
- Bind only to the intended interface and port; treat LAN exposure as a deployment decision.
- Close the Web handle before SDK shutdown.

## Assets

Deploy `DashboardAssets/RaceBear` with its directory structure beside `RaceBearSDK.dll` when using Ford GT or bundled customer assets. Missing assets must produce a visible diagnostic; do not silently substitute a misleading complete state.
