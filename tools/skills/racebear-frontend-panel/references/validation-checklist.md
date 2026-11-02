# Validation Checklist

## Contract And Deployment

- Confirm the adapter uses only `include/RaceBearSDK.h` public exports.
- Confirm header, DLL, LIB, and configuration all come from the same SDK build.
- Confirm fixed structure sizes, field offsets, calling convention, buffers, callbacks, and `Size` fields in the target language.
- Confirm `RaceBearSDK.dll` is beside the host EXE; add `libusb-1.0.dll` only when required by VOCORE.
- Confirm `DashboardAssets/RaceBear` is deployed when the selected templates require it.
- Record actual loaded DLL path, runtime version and header/LIB/DLL hashes; do not infer identity
  from filenames. Exclude customer configuration, activation files and private SDK sources from delivery.

## Lifecycle And Responsiveness

- Start with a clean product-specific AppData name and verify defaults are created.
- Initialize and start the backend exactly once.
- Verify UI state refresh continues without driving calculations from the UI timer.
- Exercise rapid page changes, catalog refresh, device removal, and window close without deadlock or use-after-free.
- Shut down with active telemetry, open outputs, dashboard handles, and serial callbacks; verify the process exits and no SDK thread remains.
- Inject a long task and a late callback during close: no native call may outlive SDK shutdown.
- Exercise partial initialization and disconnect timeout; distinguish unconfirmed disconnect from success.
- Read a long UTF-8 diagnostic through an undersized buffer and retry with more capacity.
  Verify the complete record appears once, preserves FIFO/type, and that invalid arguments
  do not consume it. Do not interpret a buffer-too-small empty string as an empty queue.

## Catalog And Configuration

- Refresh every selector from its owning SDK catalog.
- Restart the process and verify persistent choices are restored by stable key, ID, or name rather than runtime index.
- Create, rename/import where supported, delete, refresh, and relocate selections.
- For each editable page: read, modify, apply if supported, save, read back, and compare business fields.
- Verify unrelated fields survive edits and invalid ranges are rejected without corrupting the stored configuration.
- Switch A to B, reorder the catalog and delay A's response; saving must write B or refuse a missing
  target, never the stale index or previous draft. A failed read must disable writes.
- Enter empty text, a minus sign, decimals, zero, overflow and invalid pasted text. Valid editing
  must remain possible; invalid drafts must not be written by debounce, blur or window close.
- Exercise saved-but-readback-failed and saved-but-apply-failed results separately.
- Verify Dynamic, Motion Effect, Haptic, Platform, and Output each retain their own catalog index domain.
- Verify RGB effect telemetry bindings survive restart by stable Key and RGBAPP selection survives refresh by UID.
- Verify separate RGB layers can map RPM to LED 1-20 and ABS blinking to LED 21-30, including frequency, duty cycle, fill direction, invert, and priority round trips.
- Verify overlapping RGB layers render in list-derived priority order. Reject segments exceeding
  the selected device's physical capabilities or documented logical mapping bounds; no universal 1-255 limit.
- Verify static effects have no input selector, fixed inputs cannot be rebound, and ordinary inputs
  come from `RB_Rgb_GetEffectInputCount/GetEffectInput`, including unsupported-source presentation.
- Verify Seatbelt CAN selects the correct output instance and remains separate from global seatbelt effects.
- Verify single left/right/center and dual active-node mapping, startup homing wait, old-firmware
  rejection, releasing/locked states, and user-initiated `RequestReenable`. Reconnect or ordinary
  homing must not unlock or replay old tightening targets. Test disconnect waiting and rejection
  while releasing/EStop remains active; report mechanical zero and release timing as hardware checks.
- Verify current effective fields are editable, compatibility fields are preserved, and V2 protection
  is not presented alongside a misleading second V1 control set.

## License And Safety

- Test authorized, checking, missing/expired, and restored license states where fixtures permit.
- Verify protected Apply, test, and connect actions stop when `OutputAllowed == 0`.
- Follow API-specific exceptions, including RGB diagnostic test patterns. Configuration, diagnostics
  and reset tools must not all become inaccessible merely because protected output is locked.
- Verify disconnect, reset, configuration, catalogs, raw telemetry, and activation remain available as documented.
- Leave each manual-test page while active and verify outputs return to zero.
- Verify application shutdown requests disconnect and uses a bounded wait.

## Dashboard And Devices

- Render each supported built-in and customer template at the required landscape and portrait sizes; verify nonblank pixels and correct stride.
- Import a `.rbdash`, select it by `PackageId`, switch pages, refresh the catalog, and reopen it.
- Open/close VOCORE repeatedly, test unplug/replug with the selected reconnect policy, and inspect state counters.
- Open/close Dashboard Web, connect/disconnect a client, and verify frame delivery does not block rendering.
- Verify missing `libusb-1.0.dll` or dashboard assets produce actionable diagnostics.
- With RGBAPP hardware, verify four strip mappings, direction/color order, current limiting, test-pattern stop, unplug fail-dark, latest-only frame counters, and shutdown blackout.
- For firmware tasks, distinguish start accepted, progress 100 and final verified completion; freeze
  conflicting device operations while running. Mark these checks hardware-pending when not performed.

## Product And Visual Acceptance

- Check the scope matrix against all requested workflows and the UI specification's page/container map.
- Verify each ordinary configuration field has a user label, unit, range and disposition; no raw JSON
  substitute for supported typed editors and no placeholder action that only displays a success message.
- Capture all implemented pages at target minimum size and 100/150/200% DPI where supported.
  Check clipped text, overlapping controls, empty content, scrolling, keyboard focus and stop buttons.
- Switch supported languages with unsaved edits and active selection; only text/format should change.
- Test no-game, no-device, unavailable item, failed operation, stale data and unsupported input states.
- Keep demo fixtures visibly separate from actual SDK state.
- Stage the package into a clean directory, run it with documented dependencies, and verify its
  startup, resources, logs and close behavior without private development paths.

## Evidence

Record exact test commands, configuration, result, and artifact paths. Distinguish automated checks from hardware, visual, and user-manual checks. For SimRacebear frontend changes, state which Debug64/Release64 configurations still require the user's manual build.

Use separate status dimensions for planning, implementation, build, offline checks, hardware checks
and delivery. A checked planning row or API count is not evidence. Record actual dates and failure
details. Finish source/build/package/use instructions, dependency hashes and known limitations.
Use `docs/RaceBearSDK_AI_DEVELOPMENT.md` sections 7-9 for the full acceptance and delivery matrix.
