# C1ZZL3 Dual Pitch Envelope Firmware Experiment

This folder is an experimental firmware build for testing separate pitch envelopes
on the two C1ZZL3 oscillators. The production firmware is not changed by this
experiment.

## What Changes

- Envelope SysEx protocol is bumped to version 3.
- Custom envelopes can carry four lanes:
  - amplitude
  - phase distortion
  - oscillator 1 pitch
  - oscillator 2 pitch
- CZ Import Lab dual mode maps DCO1 pitch to oscillator 1 and DCO2 pitch to
  oscillator 2.
- Older single-pitch envelope payloads are still accepted and copied to both
  oscillators.
- Legacy no-pitch payloads are still accepted and use a neutral pitch envelope.
- Custom envelope flash storage is versioned separately from production, so
  saved production custom envelopes are not reused by this experiment.

## Build

From the repository root:

```sh
cmake -S experimental-firmware/dual-pitch-envelopes -B experimental-firmware/dual-pitch-envelopes/build -DPICO_NO_PICOTOOL=1
cmake --build experimental-firmware/dual-pitch-envelopes/build -j2
./build/_deps/picotool/picotool uf2 convert experimental-firmware/dual-pitch-envelopes/build/C1ZZL3_DUAL_PITCH.elf -t elf experimental-firmware/dual-pitch-envelopes/C1ZZL3_DUAL_PITCH.uf2 -t uf2
```

## Test Firmware

Flash:

```text
experimental-firmware/dual-pitch-envelopes/C1ZZL3_DUAL_PITCH.uf2
```

Use it with:

```text
experiments/dual-pitch-envelopes/
```

## Current Scope

This is the first dual-pitch test pass. The firmware applies two independent
pitch lanes, and the matching import lab can send DCO1/DCO2 separately. The
Envelope Lab currently exposes the main pitch lane for direct editing; imported
DCO2 pitch is preserved, sent, and read back as oscillator 2 pitch.
