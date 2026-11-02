---
name: racebear-frontend-panel
description: Build, extend, review, or debug third-party control panels using the RaceBear SDK public C ABI. Use for complete frontend delivery or individual runtime, motion, device, RGB, CAN, dashboard, configuration, and lifecycle workflows. Includes page/container design, stable selection, editable drafts, asynchronous tasks, offline validation, deployment, and delivery evidence across C++, C#, Qt, web/native bridges, and Python hosts.
---

# RaceBear Frontend Panel

Use the public SDK contract to build a complete, maintainable frontend around stable RaceBear interfaces.

## Start With The Contract

1. Locate the SDK root that contains `include/RaceBearSDK.h` and `docs/RaceBearSDK_FRONTEND_GUIDE.md`.
2. Read the target repository's root and project-specific instructions before editing, regardless of the filenames or conventions used by the host tool.
3. Treat `include/RaceBearSDK.h` as the signature and ABI authority.
4. Treat `docs/RaceBearSDK_FRONTEND_GUIDE.md` as the page workflow and configuration ownership authority.
5. Use `docs/RaceBearSDK_API.md` for field definitions, units, call examples, deployment, and failure behavior.
6. For RB_CAN device discovery, logical-index assignment, UID binding, motor state, diagnostics, or recovery, read `docs/RaceBearSDK_RB_CAN_GUIDE.md`.
7. Verify that the public header, import library, DLL, runtime dependencies, and resources belong to the same SDK build and configuration.

For a new or complete frontend, read `docs/RaceBearSDK_AI_DEVELOPMENT.md` first and
use its staged workflow through local delivery. For page layout, controls, interactions,
localization, or visual acceptance, read `docs/RaceBearSDK_FRONTEND_UI_SPEC.md`.
These documents are relative to the SDK root; they are also usable without installing a Skill.
Do not require SDK private sources or SimRacebear's UI helpers.

## Define The Requested Surface

List the requested pages and behaviors before coding. Distinguish:

- read-only state views;
- catalog-backed selectors;
- editable typed configurations;
- commands and one-shot actions;
- owned handles and callbacks;
- real hardware output and manual tests;
- dashboard rendering and frame delivery.

Keep the implementation scoped to the requested pages and SDK-defined configuration fields. Prefer the target application's existing architecture, component library, state management, and FFI pattern.

For each requested workflow, record the page/container, public APIs, writable and preserved
fields, dependencies, implementation state, and actual validation evidence. Treat planning,
code complete, build passed, offline passed, and hardware passed as distinct facts.
Do not turn every public helper or legacy export into a button or equate API counts with completion.

Read [references/feature-routing.md](references/feature-routing.md) to map each page to the correct public API family and authoritative documentation section.

## Implement In This Order

1. Add the language binding or adapter around the C ABI. Preserve fixed-size structures, integer widths, calling convention, buffer ownership, and `Size` initialization.
2. Set a stable product AppData name before initialization. Initialize once for the process, then start the SDK-owned backend loop.
3. Separate the backend loop, UI state refresh, log drain, catalog refresh, and dashboard frame timers.
4. Build read-only runtime and license state before adding editable configuration pages.
5. Populate selectors from their owning SDK catalogs. Keep stable keys or names in application state; treat catalog indexes as snapshot-local.
6. For edits, read the complete typed structure, modify requested fields, validate UI ranges, apply for preview only when supported, save through the owning API, then read back.
7. Add output connections, manual tests, callbacks, and hardware handles only after the configuration flow is stable.
8. Stop UI timers and callbacks, zero manual tests, request output disconnect, close owned handles, then shut down the SDK once.

Read [references/integration-workflow.md](references/integration-workflow.md) for lifecycle, threading, keys, configuration transactions, licensing, and error handling.
Read [references/implementation-patterns.md](references/implementation-patterns.md)
when implementing selectors, editable forms, debounce, background work, command results, or close handling.
It provides state models, task ownership, stale-result protection and reproducible host fixtures.

## Treat Dashboards As A Separate Pipeline

Keep these layers independent:

- Render a built-in template or an opened `.rbdash` package to BGRA8888.
- Submit the latest frame to a VOCORE handle, a Dashboard Web handle, or the host window.
- Persist a built-in template choice by its documented stable value and a customer package by `PackageId`; use refreshed catalog indexes only for the current snapshot.

Read [references/dashboard-output.md](references/dashboard-output.md) whenever the request includes a dashboard designer, `.rbdash`, Ford GT assets, VOCORE, browser output, frame timing, or custom instrument UI.

## Preserve Integration Safeguards

- Build the adapter from `include/RaceBearSDK.h` and the documented public exports.
- Persist stable keys and IDs; treat runtime telemetry indexes, catalog rows, device enumeration indexes, and refreshed plugin indexes as snapshot-local values.
- Identify an RB_CAN device by `Uid` and its unique logical index. `CatalogIndex` and `EnumerationIndex` remain valid only for the current device snapshot.
- Identify an RGBAPP by `RB_RgbDeviceInfo::Uid`; keep device-directory indexes snapshot-local. Populate ordinary effect inputs through `RB_Rgb_GetEffectInputCount/GetEffectInput` for the selected effect type; reserve the full telemetry catalog for expert diagnostics.
- Model RGB lighting as independent segmented layers. Show 1-based inclusive start/end LED controls, convert them to `StartPixel/PixelCount`, and expose the effect's applicable fields. Reordering layers determines priority. Respect both device physical capabilities and documented logical bounds.
- Keep each configuration catalog's index in its own domain, even when displayed names match.
- Let the SDK-owned loop perform motion calculations; UI timers read state and update presentation.
- Gate protected actions with `state.License.OutputAllowed`, and present `RB_LICENSE_REQUIRED` without automatic retry loops. Follow API-specific exceptions such as RGB hardware diagnostic patterns; do not disable all tools indiscriminately.
- Reset manual pose, wind, seatbelt, RGB patterns and any host-owned test mode on leaving the relevant test page and on exit. Do not invent a public Haptic test API or disconnect normal running outputs simply because navigation changed.
- Use bounded waits or asynchronous state updates for devices, network clients, callbacks, output disconnect, and frame consumption so the UI remains responsive.
- Present typed configuration controls when a public typed API is available; reserve raw JSON for advanced tooling.
- Preserve numeric text while editing, separate drafts from snapshots, resolve targets by stable identity before writing, and ignore stale UI results. No detached SDK workers or automatic replay of side-effecting commands after a short result buffer.
- Report SimRacebear validation only after the user has manually built the required frontend configuration.

## Verify Before Finishing

Run focused binding tests first, then the page workflow and shutdown tests. Use [references/validation-checklist.md](references/validation-checklist.md) as the completion gate.

For implementation requests, continue through the next uncompleted in-scope stage after each
successful check. Complete independently testable work while hardware or credentials are unavailable.
Respect the user's pause/cancel requests and actual execution permissions. Publish or operate hardware
only within the user's authorization; never claim mock, visual, or offline checks prove real device behavior.
Stop only at actual local delivery or a concrete external blocker, recording what remains.

Report:

- files and directories changed;
- SDK configuration and binary set used;
- tests actually run and their results;
- hardware or visual checks not performed;
- any SimRacebear Debug64 or Release64 build the user must perform manually.
