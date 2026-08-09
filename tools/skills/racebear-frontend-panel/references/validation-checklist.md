# Validation Checklist

## Contract And Deployment

- Confirm the adapter uses only `include/RaceBearSDK.h` public exports.
- Confirm header, DLL, LIB, and configuration all come from the same SDK build.
- Confirm fixed structure sizes, field offsets, calling convention, buffers, callbacks, and `Size` fields in the target language.
- Confirm `RaceBearSDK.dll` is beside the host EXE; add `libusb-1.0.dll` only when required by VOCORE.
- Confirm `DashboardAssets/RaceBear` is deployed when the selected templates require it.

## Lifecycle And Responsiveness

- Start with a clean product-specific AppData name and verify defaults are created.
- Initialize and start the backend exactly once.
- Verify UI state refresh continues without driving calculations from the UI timer.
- Exercise rapid page changes, catalog refresh, device removal, and window close without deadlock or use-after-free.
- Shut down with active telemetry, open outputs, dashboard handles, and serial callbacks; verify the process exits and no SDK thread remains.

## Catalog And Configuration

- Refresh every selector from its owning SDK catalog.
- Restart the process and verify persistent choices are restored by stable key, ID, or name rather than runtime index.
- Create, rename/import where supported, delete, refresh, and relocate selections.
- For each editable page: read, modify, apply if supported, save, read back, and compare business fields.
- Verify unrelated fields survive edits and invalid ranges are rejected without corrupting the stored configuration.
- Verify Dynamic, Motion Effect, Haptic, Platform, and Output indexes are never interchanged.

## License And Safety

- Test authorized, checking, missing/expired, and restored license states where fixtures permit.
- Verify protected Apply, test, and connect actions stop when `OutputAllowed == 0`.
- Verify disconnect, reset, configuration, catalogs, raw telemetry, and activation remain available as documented.
- Leave each manual-test page while active and verify outputs return to zero.
- Verify application shutdown requests disconnect and uses a bounded wait.

## Dashboard And Devices

- Render each supported built-in and customer template at the required landscape and portrait sizes; verify nonblank pixels and correct stride.
- Import a `.rbdash`, select it by `PackageId`, switch pages, refresh the catalog, and reopen it.
- Open/close VOCORE repeatedly, test unplug/replug with the selected reconnect policy, and inspect state counters.
- Open/close Dashboard Web, connect/disconnect a client, and verify frame delivery does not block rendering.
- Verify missing `libusb-1.0.dll` or dashboard assets produce actionable diagnostics.

## Evidence

Record exact test commands, configuration, result, and artifact paths. Distinguish automated checks from hardware, visual, and user-manual checks. For SimRacebear frontend changes, state which Debug64/Release64 configurations still require the user's manual build.
