# Integration Workflow

## Lifecycle

Use one process-wide SDK owner:

1. Resolve `rb.service.runtime` and call `rb.runtime.set_app_data_name` through `RB_OBJECT_NATIVE_CALL` before initialization.
2. Call `RB_Runtime_Initialize()` once.
3. Invoke `rb.runtime.start` once and Poll its task to a terminal result.
4. Read `rb.runtime.state` and `rb.telemetry.snapshot` on UI timers, normally 30-60 Hz.
5. Drain `rb.service.log` with one consumer, normally every 100-500 ms. Grow the buffer on `RB_BUFFER_TOO_SMALL`; the record remains queued.
6. Refresh catalogs only on page entry, explicit refresh, or a known mutation.
7. On exit, stop producers, reset tests, disconnect outputs, close handles, invoke `rb.runtime.stop`, wait for its result, then call `RB_Runtime_Shutdown()`.

Keep backend calculation, UI rendering, logs, catalogs and slow devices on separate host timers.

## Binding And ABI

- Load matching header, import library, DLL and configuration.
- Initialize every versioned structure's `Size` and `Version` exactly as documented.
- Resolve stable keys on every process start; never persist object IDs or task IDs.
- Use caller-owned buffers and per-call `RB_ObjectReadInfo`/`RB_ObjectTaskInfo`.
- Use native service packets only in-process and only with signatures from the generated service directory.
- Keep all pointer, string, handle and callback targets alive for the complete synchronous service call.
- Marshal callbacks to the UI thread before touching controls.

## State And Catalogs

Use `rb.runtime.state` for a low-frequency runtime summary and `rb.telemetry.snapshot` for
high-frequency whole-frame telemetry. Use service domains for catalogs and device operations.

For every selector:

1. Read the owning catalog.
2. Display the SDK-provided name.
3. Keep the stable key, UID or package ID.
4. Use indexes only within that catalog snapshot.
5. Refresh and relocate by stable identity after mutation or hot-plug.

## Configuration Transaction

Read the complete object, preserve fields the page does not edit, and keep the returned Revision.
Write with that Revision, then Invoke the matching `*.apply` or `*.save` command and Poll it.
`RB_OBJECT_CONFLICT` requires a fresh Read and an explicit merge; do not replay stale data.
JSON is reserved for objects that document JSON support. Complex RGB and seatbelt settings use
the complete Binary schema.

## Commands And Errors

Invoke once, retain the task ID, Poll to a terminal state, inspect `ResultCode`, then consume.
A short Poll buffer retains the result; grow it and Poll again. Never resubmit because copying
the result was short. A synchronous service's transport result and its business result are separate.

Gate Motion and ordinary platform output by the license snapshot. Keep RGBAPP, seatbelt, Wind and
Haptic workflows available under their own service and safety contracts. Do not create automatic
retry loops for `RB_LICENSE_REQUIRED`.

## Shutdown Gate

Before `RB_Runtime_Shutdown()`:

- stop UI and dashboard timers;
- stop worker tasks that can call the SDK;
- unregister callbacks before destroying their handles;
- reset manual tests and request output disconnect;
- close dashboard packages, VOCORE, Web, serial, debug and other owned handles;
- wait for owned object tasks to reach a terminal state;
- prevent late callbacks from touching destroyed frontend objects.
