# C1ZZL3 Dual Oscillator Lanes Experiment

This folder contains the first experimental web UI for the protocol v4
dual-oscillator-lanes firmware. It is based on the stable dual-pitch Envelope
Lab, but adds separate PD1 and PD2 lanes.

The stable dual-pitch reference remains in:

```text
experiments/dual-pitch-envelopes
```

Do not move or overwrite that reference while developing this experiment.

## Current Scope

- Main graph has three selectable lanes:
  - amplitude
  - PD 1 / oscillator 1
  - PD 2 / oscillator 2
- Pitch graph keeps two selectable pitch lanes:
  - pitch 1 / oscillator 1
  - pitch 2 / oscillator 2
- SysEx envelope send/save uses protocol v4 payload order:
  - amplitude
  - PD 1
  - pitch 1
  - pitch 2
  - PD 2
- CZ Import Lab defaults to dual DCW mapping:
  - DCW1 -> PD1
  - DCW2 -> PD2
- Other DCW modes still copy the selected/merged PD shape to both lanes.

## Save / Readback Behaviour

The protocol v4 firmware saves and reads back separate PD1/PD2 lanes. The main
envelope readback frame returns amplitude, PD1, and PD2; pitch1 and pitch2 are
returned by the separate pitch-envelope readback frame.

Older protocol v3 dual-pitch cards still read back as a single PD lane; the UI
copies PD1 to PD2 for display in that case.

## Local Test

From the repository root:

```sh
python3 -m http.server 5177 --directory experiments/dual-oscillator-lanes
```

Then open:

```text
http://localhost:5177/import-lab/
```

## Matching Firmware

Use:

```text
experimental-firmware/active-uf2s/C1ZZL3_EXPERIMENT_DUAL_OSCILLATOR_LANES_PROTOCOL_V4.uf2
```

## Next Sequence

1. Hardware-test two-lane PD with `Load RAM`.
2. Decide whether PD2 persistence/readback is worth adding next.
3. If two-lane PD is useful, expand toward full two-lane oscillator behaviour.

## Full Two-Lane Behaviour To Consider Later

- oscillator 1 pitch envelope
- oscillator 2 pitch envelope
- oscillator 1 PD envelope
- oscillator 2 PD envelope
- possible oscillator-specific amplitude trim
- possible oscillator-specific wave family
- shared final amplitude envelope
- ring/noise position in the signal path

## Compatibility Goal

Keep this in reserve for the next UI pass: the long-term goal is a single
Envelope Lab that can talk to both card versions.

- Protocol v3 / stable dual-pitch cards use one PD envelope lane and two pitch
  lanes. The editor should hide or disable PD2 controls after detecting this
  card version, while keeping imported PD2 data in the browser draft so it is
  not lost.
- Protocol v4 / dual-oscillator-lanes cards use PD1, PD2, pitch1, and pitch2.
  The editor should expose both PD lanes and send the full protocol v4 payload.
- If protocol is not detected yet, the editor should default to the safer
  protocol v3 send path and clearly label PD2 as browser-only until a protocol
  v4 card is confirmed.
- Readback should adapt to the received payload shape rather than treating a
  missing PD2 lane as an error. A protocol v3 read should copy PD1 to PD2 for
  display only and mark that second lane as inferred.
- Import Lab can keep decoding both CZ DCW lanes. The single UI should let the
  user choose whether to send a merged/single-PD version or the full dual-PD
  version once the connected card capability is known.
