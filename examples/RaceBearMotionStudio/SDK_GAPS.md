# RaceBear SDK gaps

Baseline: the repository root `../../include/RaceBearSDK.h` and `../../RaceBearSDK_API.md`, audited 2026-07-17.

The application does not emulate or invent the following capabilities:

| Area | Public SDK status | Frontend decision |
| --- | --- | --- |
| Haptic live detail | No per-effect runtime state and no manual haptic test API | Configuration, Apply, Save and aggregate function state are implemented. No test button is shown. |
| Seatbelt HID input | The config enum exposes HID/mixed modes, but no public HID sample submission API exists | Existing HID/mixed configuration values are preserved and selectable with a warning; the frontend cannot provide an HID source. |
| CAN management | Only `RB_RuntimeState::Can` and basic CAN output fields are public | Read-only controller/motor summary and public output fields only. No homing, firmware, fault reset or emergency-stop commands are invented. |
| LAN | No public LAN API | No LAN page or controls. |
| Dynamic profile rename | No atomic rename API | Copy and delete are implemented. Rename is omitted to avoid desynchronizing dynamic/motion/haptic profiles. |
