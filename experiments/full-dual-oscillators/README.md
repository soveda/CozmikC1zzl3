# C1ZZL3 Full Dual Oscillator Web Experiment

This folder contains the experimental Web MIDI UI for the protocol v9
full-dual-oscillator firmware. It starts from the tested dual-amplitude v6
rollback, then adds separate oscillator wave-family settings.

Do not replace the production Envelope Lab with this page unless the matching
firmware experiment passes hardware testing.

## Current Scope

- Main graph has four selectable lanes:
  - Amp 1 / oscillator 1
  - Amp 2 / oscillator 2
  - PD 1 / oscillator 1
  - PD 2 / oscillator 2
- Pitch graph has two selectable pitch lanes:
  - Pitch 1 / oscillator 1
  - Pitch 2 / oscillator 2
- Settings now include:
  - shared baseline PD amount
  - oscillator 1 wave family
  - oscillator 2 wave family
  - detune, ring, noise, MIDI input channel, Turing range, and Turing MIDI
- Import Lab decodes separate CZ Line 1 and Line 2 oscillator waveform/window
  words and maps them to oscillator 1 and oscillator 2 wave families.
- Protocol v7/v8/v9 settings readback returns a 24-byte response with separate
  oscillator wave-family controls.
- Envelope send/save keeps the protocol v6 lane payload shape and uses
  protocol v9 to identify the full dual-oscillator firmware generation with
  slot-name and per-slot performance setting readback.
- Card slot readback now restores the saved 16-character slot name and its
  saved settings.

## Matching Firmware

Use:

```text
experimental-firmware/full-dual-oscillators/C1ZZL3_FULL_DUAL_OSCILLATORS_PROTOCOL_V9.uf2
```

## Local Test

From the repository root:

```sh
python3 -m http.server 5177 --directory experiments/full-dual-oscillators
```

Then open:

```text
http://localhost:5177/
```

## Hardware-Control Note

This first pass keeps the existing hardware panel behaviour and Turing mode.
The separate oscillator wave-family selection is Web UI only.

If the card later trades away the Turing machine mode, switch up could become
oscillator 1 tone controls and switch middle could become oscillator 2 tone
controls, while switch down remains detune/ring/noise.

## Protocol v9 Test Focus

Protocol v9 saves performance settings with each custom envelope slot,
including PD, detune, ring, noise, MIDI/Turing settings, and oscillator 1/2 wave
families. This is still labelled as an experiment until save/read/load
behaviour passes.
