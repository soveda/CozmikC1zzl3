# C1ZZL3 Stable Dual Pitch Envelope Lab

This folder contains the passing dual-pitch Envelope Lab and CZ Import Lab. It
is separate from the production Envelope Lab, but should now be treated as the
stable dual-pitch reference for future experiments.

Do not use this folder for the next dual-oscillator experiment. Start that work
in `experiments/dual-oscillator-lanes` so this known-good dual-pitch behaviour
remains available for comparison.

## Passing Behaviour

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
- Import handoff does not reload an already-open Envelope Lab tab.
- Imported envelopes are not duplicated during handoff.
- Imported performance settings update the Envelope Lab Settings controls:
  - PD amount
  - detune
  - wave family
  - ring modulation
  - noise/grit

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

This stable dual-pitch reference sends, reads, and displays two pitch lanes.
The next experiment should not add more behaviour here; it should begin with
two-lane PD envelopes in `experiments/dual-oscillator-lanes`, then progress
toward full two-lane oscillator behaviour.
