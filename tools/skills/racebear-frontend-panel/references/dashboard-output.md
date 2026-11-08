# Dashboard And Output

Treat dashboards as a separate pipeline from backend calculation:

1. Read `rb.runtime.state` or `rb.telemetry.snapshot`.
2. Use `rb.service.dashboard` to enumerate or open a built-in or customer package.
3. Use the dashboard service to render BGRA8888 into a caller-owned buffer.
4. Submit the latest frame through `rb.service.vocore`, `rb.service.dashboardweb`, or the host window.

Dashboard and device operations use `RB_OBJECT_NATIVE_CALL`. The service directory defines the
pixel pointer, stride, dimensions, handle ownership and return values. Native packets are local
to the process and must never be sent to a browser or network peer.

Keep one owned package handle per selected package and close it before replacement or page exit.
Keep one VOCORE handle per screen. Submit frames asynchronously; the SDK keeps the newest pending
frame and does not require the frontend to implement a per-pixel USB loop.

Drive rendering from a 16-33 ms host timer. Do not couple rendering to the backend calculation
cadence. Read service state for connection, client, FPS, replacement/drop counts and errors.

Close in this order: stop the frame timer, stop new service calls, close Web and VOCORE handles,
close the package, then stop the runtime and shut down the SDK. A nonempty pixel buffer, a successful
service dispatch, or a mock screen does not prove that a physical display showed the frame.
