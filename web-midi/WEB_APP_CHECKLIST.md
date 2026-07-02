# Web App Checklist For Browser-Based Hardware Editors

Use this as a review document when checking other web apps, especially ones
that talk to hardware over Web MIDI, WebUSB, or SysEx-style browser APIs.

## Core Browser Support

- Confirm the app only runs in browsers that support the required API.
- Confirm the app clearly reports when the browser does not support the needed
  API.
- Confirm the app is served over HTTPS or another secure context if the API
  requires it.
- Confirm the app behaves correctly on desktop and mobile separately.
- Confirm the app does not assume browser API support is the same across
  Chrome, Android Chrome, Safari, and iOS Safari.

## Device Detection

- Check that device connection succeeds before the UI claims it is ready.
- Check that input ports and output ports are detected independently.
- Check that the app still works if a browser exposes input but not output.
- Check that port enumeration does not rely on only one map iteration style.
- Check fallback paths for reading ports:
  - `forEach`
  - `values()`
  - direct iteration
  - `keys()` plus `get()`
- Check that hot-plugging or reconnecting a device refreshes the UI.
- Check that the selected output survives a refresh where possible.

## Mobile Browser Edge Cases

- Check whether mobile browsers report MIDI access but expose unusable port
  objects.
- Check whether mobile browsers report `size` values that do not match actual
  enumerated ports.
- Check for differences between direct USB connection, OTG adapters, and hubs.
- Check whether mobile requires the device to be connected before the page is
  opened.
- Check whether a powered hub changes detection reliability.

## User-Facing Status And Errors

- Check that every hardware action has a clear success message.
- Check that every failure path produces a useful user-facing message.
- Check that low-level browser or API errors are translated into plain English.
- Check that the app distinguishes between:
  - no browser support
  - permission denied
  - no device found
  - input found but no output found
  - send/build failure

## Developer Diagnostics

- Check that there is a hidden developer mode for troubleshooting.
- Check that developer diagnostics do not clutter the normal UI.
- Check that developer mode can show:
  - detected input ports
  - detected output ports
  - raw browser API object details
  - recent event log entries
- Check that logs can be cleared.
- Check that logs help explain both browser/API failures and app logic failures.

## Send/Build Safety

- Check that payload-building code is wrapped in error handling.
- Check that hardware send operations are wrapped in error handling.
- Check that spread operations do not assume returned values are iterable.
- Check that malformed data fails with an actionable diagnostic, not a vague
  runtime error.
- Check that repeated sends are rate-limited if the device needs cooldown time.

## Editor Interaction

- Check that dragging points feels responsive and does not lag badly.
- Check that stacked points can still be understood visually.
- Check that the active editing lane is obvious.
- Check that only the active lane can be edited when multiple lanes are shown.
- Check that the graph and numeric editor stay in sync while dragging.
- Check that reset returns to a useful demonstration preset, not a flat or
  confusing state.

## Visual Clarity

- Check that the app explains how the graph works without needing outside help.
- Check that stage numbering or labels are understandable.
- Check that hidden or stacked points are explained clearly.
- Check that button labels match actual behavior.
- Check that play/stop notes explain whether they affect browser preview,
  hardware, or both.

## Theme Support

- Check both dark mode and light mode carefully.
- Check that all backgrounds actually switch theme, including:
  - graph canvas
  - side panels
  - performance/settings areas
  - instructions
  - developer tools
- Check that graph lines, points, labels, and legends all follow the theme.
- Check that theme changes apply immediately without needing extra clicks.

## Preset And Data Handling

- Check whether factory presets are protected from accidental overwrite.
- Check whether editing a factory preset creates a custom copy if needed.
- Check whether reset behavior is predictable and documented.
- Check whether names, settings, and presets are stored locally, on device, or
  both.
- Check that local state survives refresh only where intended.

## Control Layout

- Check that only necessary controls are visible in the normal UI.
- Check that advanced or risky controls are hidden behind a developer toggle.
- Check that buttons are placed near the area they affect.
- Check that only the values for the currently edited lane are shown if that is
  less confusing.

## Documentation

- Check that the README matches the current button labels.
- Check that the README explains the normal user workflow from first open to
  successful send.
- Check that the README explains mobile limitations if relevant.
- Check that the README explains any developer-only tools briefly.

## Hosting And Deployment

- Check that the site can be hosted over HTTPS.
- Check that the deployment workflow uses current GitHub Actions versions if
  using Pages.
- Check whether GitHub Pages is configured for `GitHub Actions` instead of
  branch publishing when using a workflow.
- Check that deployment failures are separated into:
  - build failure
  - artifact upload failure
  - Pages deployment queue timeout
- Check whether old queued deployments can block new ones.

## Final Cross-Check

- Test on one desktop browser that is known-good.
- Test on one mobile browser if mobile use matters.
- Test with no device connected.
- Test with input-only detection.
- Test with output-only detection if possible.
- Test a full successful send path.
- Test the developer diagnostics only after a real failure, to make sure they
  are actually useful.
