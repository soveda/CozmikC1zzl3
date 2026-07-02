# AGENTS

## Purpose

Use this guidance when reviewing, debugging, or extending browser-based
hardware editors, especially apps that use Web MIDI, WebUSB, or SysEx-like
browser APIs.

## Primary Checks

### Browser Support

- Verify the target API is supported in the current browser.
- Verify the app is running in a secure context when required.
- Treat desktop and mobile browser support as separate test surfaces.
- Do not assume Chrome desktop behavior matches Android Chrome behavior.
- Do not assume Safari or iOS Safari supports the same browser APIs.

### Device Detection

- Verify device connection before claiming the app is ready.
- Check inputs and outputs independently.
- Assume some browsers may expose input without output.
- Do not rely on a single MIDI map enumeration path.
- When reading browser MIDI maps, support:
  - `forEach`
  - `values()`
  - direct iteration
  - `keys()` plus `get()`
- Re-test after reconnecting or hot-plugging devices.

### Mobile Web MIDI Edge Cases

- Expect some mobile browsers to report MIDI access while exposing incomplete
  or unusable port objects.
- Watch for mismatches between reported `size` and actually enumerated ports.
- Consider OTG adapters, hubs, and power delivery as likely causes of
  inconsistent behavior.
- If mobile detection is flaky, test whether connecting the device before page
  load changes behavior.

### User-Facing Errors

- Surface clear status for success and failure paths.
- Distinguish between:
  - unsupported browser
  - permission denied
  - no device
  - input-only detection
  - output missing
  - send/build failure
- Translate low-level browser errors into plain language where possible.

### Developer Diagnostics

- Prefer a hidden developer mode over exposing advanced troubleshooting in the
  main UI.
- Include developer readouts for:
  - detected inputs
  - detected outputs
  - raw browser API details
  - recent event log entries
- Make logs clearable.
- Use diagnostics to explain both browser API failures and application logic
  failures.

### Send/Build Robustness

- Wrap payload-building code in error handling.
- Wrap send operations in error handling.
- Do not assume spread inputs are iterable without validation.
- Prefer explicit diagnostics when payload construction fails.
- Respect device cooldowns or throttling where needed.

### Editor UX

- Check that dragging is responsive.
- Keep graph and numeric editors synchronized.
- Make stacked points understandable.
- Ensure only the active lane can be edited when multiple lanes are visible.
- Reset should return to a useful, explanatory preset rather than a visually
  flat state.

### Theme Integrity

- Test light and dark mode separately.
- Verify all backgrounds, panels, controls, and graph surfaces switch themes.
- Verify graph lines, points, and labels follow the active theme.
- Theme changes should apply immediately without extra focus or clicks.

### Data Handling

- Protect factory presets or provide an explicit copy-on-edit flow.
- Document where names, settings, and preset data are stored.
- Verify reset behavior is predictable and documented.
- Verify local persistence only where intended.

### Documentation

- Keep button names in documentation aligned with the UI.
- Document normal user workflow from open to successful send.
- Document mobile browser limitations when relevant.
- Briefly document developer-only troubleshooting tools.

### Hosting And Deployment

- Prefer HTTPS-hosted deployment.
- If using GitHub Pages workflows, use current Actions versions.
- If using a Pages workflow, verify repository Pages source is `GitHub Actions`.
- Distinguish between:
  - build failure
  - artifact upload failure
  - Pages queue/deployment timeout
- Remember old queued Pages deployments can block new ones.

## Final Validation Pass

- Test with no hardware connected.
- Test on at least one known-good desktop browser.
- Test on mobile if mobile use matters.
- Test full successful send flow.
- Test failure states intentionally to confirm diagnostics are useful.
