# C1ZZL3 Dual Pitch Envelope Lab Experiment

This folder contains the matching web UI for the dual-pitch envelope firmware
experiment. It is separate from the production Envelope Lab.

## What It Tests

- CZ Import Lab decodes separate DCO1 and DCO2 pitch envelopes.
- CZ Import Lab lets the user choose merged, line 1, or line 2 mapping for CZ
  DCA amplitude and DCW phase-distortion envelopes instead of silently averaging
  them.
- The default import mode maps:
  - DCO1 pitch to oscillator 1 pitch
  - DCO2 pitch to oscillator 2 pitch
- Envelope Lab sends protocol v3 envelope payloads with amplitude, PD, pitch 1,
  and pitch 2 lanes.
- Envelope readback accepts dual-pitch pitch responses from the experimental
  firmware.

## Local Test

From the repository root:

```sh
python3 -m http.server 5176 --directory experiments/dual-pitch-envelopes
```

Then open:

```text
http://localhost:5176/import-lab/
```

## Required Firmware

Flash the matching UF2 before testing dual-pitch behaviour:

```text
experimental-firmware/dual-pitch-envelopes/C1ZZL3_DUAL_PITCH.uf2
```

## Current Limitation

This first UI pass sends and reads two pitch lanes, but direct graph editing is
still focused on the main pitch lane. Imported DCO2 pitch is preserved as the
second pitch lane so the firmware behaviour can be tested before expanding the
editor controls.
