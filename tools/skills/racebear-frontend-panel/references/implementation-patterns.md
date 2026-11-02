# Frontend Implementation Patterns

Use these framework-neutral models for selectors, editable pages, asynchronous operations,
and shutdown. The sequences below are pseudocode, not extra SDK exports or a drop-in
language binding. Verify the actual C ABI in the supplied header.

Read `docs/RaceBearSDK_FRONTEND_UI_SPEC.md` for page/container placement and
`docs/RaceBearSDK_AI_DEVELOPMENT.md` for development stages and delivery records.

## 1. Selector And Draft Ownership

Each editor owns this state:

```text
Selection: domain, stableIdentity, catalogGeneration
Editor: loadedIdentity, baseline, draft, draftRevision, fieldText, validationErrors
Operation: id, targetIdentity, capturedRevision, state, error
```

Do not derive identity from displayed text when the SDK supplies a key or UID. Keep separate
identities for the selected CAN device and the device already bound to the selected output.
Where only configuration names are exposed, use those documented names in their owning domain.
Dynamic candidates have an effect `Key` and a distinct `TelemetryKey`; use the latter to
look up telemetry units. Do not invent Name/Unit members on `RB_DynamicInputCandidateItem`
or resolve an effect Key as if it were a stable telemetry key.

Selection change:

```text
cancel pending debounce for the old selection
resolve save/discard/cancel for any old draft
invalidate the old page request generation
set requested stableIdentity; mark loading; disable writes
read the owning catalog and resolve identity to this snapshot's index
read the complete configuration using the actual SDK API
if identity or request generation changed: discard the late UI result
if read failed: show failure for the requested identity; leave writes disabled
otherwise atomically install loadedIdentity + baseline + draft; mark ready
```

Saving:

```text
require requestedIdentity == loadedIdentity
require a complete valid draft and no conflicting operation
capture targetIdentity, draftRevision and the complete draft
resolve targetIdentity against a current owning catalog
if missing: mark unavailable and stop; do not select index 0
serialize writes for this target
call the documented Save API once
on failure: retain draft and show the API result
on success: ReadBack through the owning API
if ReadBack fails: mark saved-but-unconfirmed; retain draft for diagnosis
if target/revision still matches: refresh from ReadBack business fields
otherwise: record result for original target; do not overwrite the new editor
if runtime Apply was requested: report its result separately
```

Disable catalog mutations that could invalidate an index during an in-flight native call,
or route these operations through the same domain queue. A host catalog generation is a
local stale-result guard, not an SDK transaction/version-lock API.
Compare meaningful fields with the documented normalization and units. Do not compare an
entire struct's bytes, padding, reserved data, or unused string tail.

### Selector Test With A Fake Repository

Use two profiles A/B with distinct values. Read A, switch to B, edit, save, then inspect the
repository calls: only B must be written. Repeat after reversing catalog order, removing B,
and delaying A's load until after B is selected. A delayed result must not overwrite B.
These tests exercise host logic without connecting real hardware; they are not ABI tests.

## 2. Numeric Editing And Debounce

Keep `fieldText` separate from parsed numeric values. TextChanged only updates the draft
text, validity, and debounce revision. It must not force the minimum or normalize text.

```text
on debounce/Enter/blur:
    parse the complete input in the chosen locale
    reject empty/intermediate input, overflow, NaN/Inf and trailing junk
    validate the actual field's range, unit and dependent fields
    if invalid: keep text; show field error; do not call SDK
    if valid: update the typed draft and queue the permitted commit policy
```

Allow inputs such as `-`, an empty string, and `0.` while typing, even if they are not yet
committable values. A finite negative InGain is valid; many other fields forbid negatives.
Do not infer a universal 0-100 range from neighboring controls.
Formatting occurs after a successful commit or an explicit revert, not on every keystroke.
Every debounce callback captures target identity and draft revision. Cancel or invalidate it
on selection change, page leave and shutdown.

Auto-save is a product choice for allowed configuration edits. It must not write Flash,
bind a device, start homing, upgrade firmware, connect hardware or replay authorization
requests just because text input became idle.

## 3. Tasks, Callbacks And Closing

A lifecycle service owns workers; widgets do not own detached native calls.
Each task has an immutable target, operation ID, busy state, result and retained lifetime.
SDK callbacks copy payloads into frontend-owned memory and post to the UI dispatcher.
Check the UI request generation or weak owner before presenting results.

```text
Idle -> Running -> Succeeded | Failed
Running -> CancelRequested -> Finished   [only if cancellation exists]
```

If the SDK offers an asynchronous Start/GetState pair, use it directly rather than spawning
a second polling thread per widget. Read state until final success/failure; accepted is not
complete. If a native operation is synchronous, use a tracked host worker and marshal results.
No cancellation API means the UI can request close and wait, but cannot kill the native task.
Never detach a worker merely to keep a close button responsive.

For maintenance operations, serialize target selection, enter/exit maintenance, parameter
writes, synchronization, reboot and upgrade actions. Keep ordinary state presentation alive.
For failures after writes, show possible partial effects and a reread path.

Repeated actions require explicit intent: no tight retry loops on license failure, device
failure, activation, binding, homing or firmware start. UI status polling only reads state.

## 4. Command JSON And Buffer Results

Use a real JSON library from the host stack. Serialize user input as data; parse the command
response and inspect both the native result and documented JSON business result. Retain error
stage/code without logging activation codes, tokens or complete sensitive response bodies.

Read-only data getters can grow buffers and retry within a bounded capacity when documented.
Do not apply that pattern blindly to `RB_Command_Run`: an action may already have run before
the result copy returns `RB_BUFFER_TOO_SMALL`. Preallocate an appropriate result buffer and
send the command once. On a too-small response, show an unconfirmed result and reread the
relevant state; only repeat an action when its idempotence or outcome is known.
Neither retrying with a bigger buffer nor a timeout guarantees the previous action did nothing.

## 5. Lifecycle State Machine

```text
Uninitialized -> Initializing -> Running
Initializing -> Failed
Running -> CloseRequested -> DrainingTasks -> ResettingTests
        -> DisconnectingOutputs -> ClosingHandles -> Shutdown -> Closed
```

On partial initialization failure, release only acquired resources in reverse ownership order.
Never call Shutdown on a runtime that failed to initialize as if initialization had succeeded.

Normal exit:

1. Mark closing. Reject new commands, catalog refresh, tests and task submissions.
2. Stop normal UI/debounce/render producers. Keep a dedicated close coordinator responsive.
3. Request supported cancellation and wait for all SDK-calling host tasks/callback producers.
   If a task is not cancellable, show that it must finish before closing.
4. Reset owned manual-pose, wind, seatbelt and RGB tests through their documented APIs.
   There is no general public Haptic manual-test API; do not invent one.
5. Request all output disconnect and poll with a finite deadline.
6. Close owned package/VOCORE/Web/serial handles and unregister callbacks in the documented
   order; close the debug monitor.
7. Only after no host task can call the SDK, invoke `RB_Runtime_Shutdown()` once.
8. Release bindings/module references, then destroy remaining UI state.

If the disconnect deadline is exceeded, report the still-active state and follow a documented
product close policy. Do not label it safely disconnected merely because the wait ended.
Do not force-terminate workers, unload the DLL, or destroy shared state while calls remain.
The SDK Shutdown is still the lifecycle endpoint; any fallback must preserve its public contract
and report that disconnect was not confirmed.

The short StopBackend example in the frontend guide assumes task quiescence has already been
established; it does not implement the entire asynchronous host close coordinator.

## 6. ABI And Fixture Checks

Seatbelt CAN protection is an output-instance workflow. Single left/center uses only the left
node; single right uses only the right node. Show startup homing wait, releasing, disabled lock,
and fault lock separately. Never request homing or re-enable from a polling/reconnect callback.
After user confirmation, call `RB_SeatbeltCan_RequestReenable` and poll GetState until Ready;
request acceptance is not physical success. A fault lock may still be releasing and reject
the request. Keep diagnostics visible. Account for up to about 16.5 seconds of backend release
waiting during disconnect without blocking UI or unloading the SDK prematurely.

Generate or manually verify bindings from the exact header rather than inferring `Size` and
`Version` conventions from another structure. Some outputs are filled by the SDK; other
structures require caller initialization. Follow each API's specific rule.
Use fixed-width integers, native alignment and exact fixed arrays; do not apply packed layout
globally. Copy SDK-returned strings and retain callback objects for the registration lifetime.

Maintain at least these host fixtures:

| Fixture | Behavior to verify |
|---|---|
| Empty catalog | Empty-state UI, no index 0 access, writes disabled |
| Reordered/removed items | Stable selection or unavailable state |
| Slow read/save | Responsive UI, stale results ignored, target unchanged |
| Invalid numeric draft | No SDK write and original text retained |
| Save success/readback failure | Distinct unconfirmed status, no false rollback |
| License locked | Protected actions blocked, allowed browse/reset remains |
| Device lost | Offline state and test reset request, no endless reconnect commands |
| Task active during close | No Shutdown before task/callback completion |
| RGB input types | Static/no input, fixed input locked, filtered semantic choices |
| Missing assets | Explicit diagnostic and no false successful preview |

These fixtures complement tests against the real DLL and requested hardware; label each
verification level in the delivery report.
