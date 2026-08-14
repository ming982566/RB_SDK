---
name: racebear-frontend-panel
description: Build, extend, review, or debug frontend control panels that integrate the RaceBear SDK public C ABI. Use for React, web, Qt, C#, C++, Python, or other host applications that need RaceBear runtime initialization, state dashboards, game and telemetry selection, license UI, platform and motion configuration, haptics, wind, seatbelt, output management, RB_CAN discovery and stable device binding, public CAN health and recovery diagnostics, serial tools, custom dashboards, VOCORE screens, browser dashboard output, deployment, or shutdown handling. Also use when porting SimRacebear behavior to another frontend or diagnosing stale indexes, unstable keys, duplicate CAN logical indexes, blocked UI threads, leaked handles, config round-trip failures, missing DashboardAssets, or SDK threads that do not exit.
---

# RaceBear Frontend Panel

Use the public SDK contract to build a complete, maintainable frontend without coupling the UI to RaceBear internals.

## Start With The Contract

1. Locate the SDK root that contains `include/RaceBearSDK.h` and `docs/RaceBearSDK_FRONTEND_GUIDE.md`.
2. Read the target repository's root and project-specific instructions before editing, regardless of the filenames or conventions used by the host tool.
3. Treat `include/RaceBearSDK.h` as the signature and ABI authority.
4. Treat `docs/RaceBearSDK_FRONTEND_GUIDE.md` as the page workflow and configuration ownership authority.
5. Use `docs/RaceBearSDK_API.md` for field definitions, units, call examples, deployment, and failure behavior.
6. For RB_CAN device discovery, logical-index assignment, UID binding, motor state, diagnostics, or recovery, read `docs/RaceBearSDK_RB_CAN_GUIDE.md`.
7. Verify that the public header, import library, DLL, runtime dependencies, and resources belong to the same SDK build and configuration.

## Define The Requested Surface

List the requested pages and behaviors before coding. Distinguish:

- read-only state views;
- catalog-backed selectors;
- editable typed configurations;
- commands and one-shot actions;
- owned handles and callbacks;
- real hardware output and manual tests;
- dashboard rendering and frame delivery.

Do not implement unrequested pages or invent configuration fields. Prefer the target application's existing architecture, component library, state management, and FFI pattern.

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

## Treat Dashboards As A Separate Pipeline

Keep these layers independent:

- Render a built-in template or an opened `.rbdash` package to BGRA8888.
- Submit the latest frame to a VOCORE handle, a Dashboard Web handle, or the host window.
- Persist a built-in template choice by its documented stable value and a customer package by `PackageId`, never by refreshed catalog index.

Read [references/dashboard-output.md](references/dashboard-output.md) whenever the request includes a dashboard designer, `.rbdash`, Ford GT assets, VOCORE, browser output, frame timing, or custom instrument UI.

## Enforce Safety Boundaries

- Never include SDK internal headers or instantiate internal backend classes from a third-party frontend.
- Never persist runtime telemetry indexes, catalog row numbers, device enumeration indexes, or refreshed plugin indexes.
- Persist an RB_CAN device by `Uid` and its unique logical index; never persist `CatalogIndex` or `EnumerationIndex`.
- Never reuse an index from a different configuration catalog, even when displayed names match.
- Never run a second motion calculation loop from a UI timer.
- Never bypass `state.License.OutputAllowed` or retry `RB_LICENSE_REQUIRED` in a loop.
- Never allow a page transition or application exit to leave manual pose, wind, seatbelt, haptic, or output tests active.
- Never block the UI thread waiting indefinitely for devices, network clients, callbacks, output disconnect, or frame consumption.
- Never expose raw JSON editing as the normal configuration experience when a typed public API exists.
- Never claim SimRacebear validation unless the user compiled the required frontend configuration manually.

## Verify Before Finishing

Run focused binding tests first, then the page workflow and shutdown tests. Use [references/validation-checklist.md](references/validation-checklist.md) as the completion gate.

Report:

- files and directories changed;
- SDK configuration and binary set used;
- tests actually run and their results;
- hardware or visual checks not performed;
- any SimRacebear Debug64 or Release64 build the user must perform manually.
