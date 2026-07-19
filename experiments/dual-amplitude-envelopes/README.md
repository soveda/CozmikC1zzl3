# C1ZZL3 Dual Amplitude Envelopes Web Experiment

This folder contains the first experimental web UI for the protocol v5
dual-amplitude firmware. It is based on the dual-oscillator-lanes web app, but
adds separate Amp1 and Amp2 lanes.

Do not replace the production Envelope Lab with this page unless the matching
firmware experiment passes hardware testing.

## Current Scope

- Main graph has four selectable lanes:
  - Amp 1 / oscillator 1
  - Amp 2 / oscillator 2
  - PD 1 / oscillator 1
  - PD 2 / oscillator 2
- Pitch graph keeps two selectable pitch lanes:
  - Pitch 1 / oscillator 1
  - Pitch 2 / oscillator 2
- SysEx envelope send/save uses protocol v5 payload order:
  - Amp 1
  - PD 1
  - Pitch 1
  - Pitch 2
  - PD 2
  - Amp 2
- CZ Import Lab sends DCA1 to Amp1 and DCA2 to Amp2.
- Older imported or locally saved drafts without Amp2 copy Amp1 to Amp2.

## Save / Readback Behaviour

The protocol v5 firmware saves and reads back separate Amp1/Amp2, PD1/PD2, and
Pitch1/Pitch2 lanes using small chained responses. The main envelope readback
frame returns Amp1 and PD1; separate Amp2 and PD2 responses return the second
amplitude and phase-distortion lanes; the pitch response returns Pitch1 and
Pitch2.

Protocol v4 cards are not the target for this page. Use
`experiments/dual-oscillator-lanes/` for protocol v4 rollback testing.

## Local Test

From the repository root:

```sh
python3 -m http.server 5177 --directory experiments/dual-amplitude-envelopes
```

Then open:

```text
http://localhost:5177/import-lab/
```

## Matching Firmware

Use:

```text
experimental-firmware/active-uf2s/C1ZZL3_EXPERIMENT_DUAL_AMPLITUDE_ENVELOPES_PROTOCOL_V5.uf2
```

## Compatibility Note

This experiment is intentionally separate from the single Web UI compatibility
work. The long-term goal is still one Envelope Lab that detects card capability
and adapts, but this draft keeps protocol v5 isolated while the hardware path
is uncertain.
