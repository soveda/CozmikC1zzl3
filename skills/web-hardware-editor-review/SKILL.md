---
name: web-hardware-editor-review
description: Use when reviewing, debugging, or extending browser-based hardware editors that use Web MIDI, WebUSB, or SysEx-style browser APIs, especially when checking browser support, device detection, mobile edge cases, diagnostics, theming, interaction design, and deployment.
---

# Web Hardware Editor Review

Use this skill when working on browser-based editors that communicate with
hardware.

## Review Order

Work through these areas in order:

1. Browser and API support
2. Device detection and enumeration
3. Mobile browser edge cases
4. User-facing status and failure messages
5. Developer diagnostics
6. Send/build robustness
7. Editor interaction and visual clarity
8. Theme integrity
9. Data handling and reset behavior
10. Hosting and deployment

## Key Rules

- Treat desktop and mobile as different platforms.
- Check MIDI inputs and outputs independently.
- Do not assume browser MIDI port maps are fully iterable in all browsers.
- Keep advanced troubleshooting behind a developer mode when possible.
- Prefer actionable diagnostics over vague runtime errors.
- Verify theme changes affect the graph, controls, panels, and labels.
- Verify documentation matches the actual UI labels and workflow.

## Reference

For the full reusable checklist, read `references/checklist.md`.
