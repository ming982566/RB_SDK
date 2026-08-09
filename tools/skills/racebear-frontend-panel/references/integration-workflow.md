# Integration Workflow

## Lifecycle

Use one process-wide SDK owner:

1. Call `RB_Runtime_SetAppDataName("StableProductName")` before initialization. Pass a directory name, not a path, and do not borrow `SimRacebear` for another product.
2. Call `RB_Runtime_Initialize()` once.
3. Call `RB_Runtime_StartLoop(intervalMs)` once. A normal frontend does not periodically call `RB_Runtime_DoCalculations()`.
4. Read `RB_RuntimeState` through `RB_State_Read()` on a UI refresh timer, normally 30-60 Hz.
5. Drain logs separately, normally every 100-500 ms. Refresh catalogs only on page entry, explicit refresh, or a known mutation.
6. On exit, stop producers of SDK calls, zero tests, disconnect outputs with a bounded wait, close handles, and call `RB_Runtime_Shutdown()` once.

Keep the backend loop independent from rendering. A blocked UI must not stop motion calculations, and a slow device must not stall the UI.

## Binding And ABI

- Load the DLL matching the header and import library version. Do not mix Debug and Release artifacts.
- Match the C calling convention and exact integer, pointer, array, and structure layout.
- Set versioned structure `Size` fields before calls that require them.
- Allocate writable output buffers and handle documented buffer-too-small behavior.
- Copy fixed character arrays into frontend-owned strings before the next mutation or shutdown.
- Marshal callbacks to the UI thread; never manipulate UI controls directly from SDK or serial callback threads.

## State And Catalogs

Use `RB_State_Read()` for the current runtime snapshot. Do not treat it as a replacement for full catalogs or configurations.

For every selector:

1. Read the owning catalog.
2. Display the SDK-provided name.
3. Keep the provided stable key, ID, or configuration name.
4. Use the returned index only within that catalog snapshot and only for APIs that explicitly require it.
5. Refresh and relocate by the stable value after create, delete, import, device changes, or catalog refresh.

Important distinctions:

- `GameKey` is persistent; `CatalogIndex` is snapshot-local.
- Telemetry `Key` and wire `FieldId` are stable for their documented purposes; runtime `Index` is resolved for the current process only.
- Dynamic, motion effect, platform, output, plugin, dashboard package, and device catalogs own separate index spaces.

## Configuration Transaction

Prefer typed public structures for normal UI pages:

1. Read the complete current structure from the owning API.
2. Preserve fields the page does not edit, including compatibility fields.
3. Modify only fields represented by valid controls and SDK-provided candidates.
4. Validate ranges before calling Apply or Save.
5. Use Apply for supported runtime preview; do not assume it persists.
6. Use Save for persistence.
7. Read back and compare business fields instead of using whole-structure `memcmp`.
8. Refresh affected catalogs after create, delete, or rename.

Use `RB_Data_ReadJson()` and `RB_Data_WriteJson()` only for advanced editors, migration tools, or APIs without a typed equivalent. Write complete documents, not partial fragments, and follow documented reload requirements.

Configuration ownership rules:

- System, wind, and seatbelt are single global configurations unless the current public guide says otherwise.
- Game telemetry is stored per stable game key.
- Platform, Dynamic, Motion Effect, and Output use their own catalogs.
- Haptic structures are independent, while the current profile name directory is associated with Dynamic. Do not use a Motion Effect index for Haptic.

## Commands And Errors

- Use typed APIs for structured state and configuration.
- Use `RB_Command_Run()` for actions documented as commands and pass `{}` for no-argument commands.
- Check both the integer return code and any returned command JSON.
- Surface `RB_INVALID_ARGUMENT` as a validation or stale-selection problem.
- Treat `RB_ERROR` according to the called API's documented state and diagnostics; do not hide it behind tight-loop retries.
- When `RB_LICENSE_REQUIRED` is returned, explain that the operation requires output permission and stop automatic retries.

## Licensing And Tests

Read license presentation state from `RB_RuntimeState::License`. Do not read license files or call the license server directly.

- Keep catalogs, configuration, raw telemetry, activation, disconnect, and reset UI available when permitted by the public contract.
- Disable protected Apply, connection, and manual-test actions when `License.OutputAllowed == 0`.
- Always provide an explicit stop/reset path.
- Zero manual pose, wind, seatbelt, and other active tests on page exit, device loss, license loss, and application shutdown.

## Shutdown Gate

Before `RB_Runtime_Shutdown()`:

- stop UI and dashboard timers;
- stop worker tasks that can call the SDK;
- disconnect serial callbacks before destroying serial handles;
- reset all manual tests;
- call `RB_Output_RequestDisconnectAll()` and poll `RB_Output_AreAllDisconnected()` with a bounded timeout;
- close dashboard package, VOCORE, Dashboard Web, serial, debug monitor, and other owned handles;
- prevent late callbacks from touching destroyed frontend objects.
