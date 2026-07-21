# C1ZZL3 Stable Dual Pitch Envelope Firmware

This folder is the passing dual-pitch firmware build for separate pitch
envelopes on the two C1ZZL3 oscillators. The production firmware is not changed
by this build, but this folder should now be treated as the stable dual-pitch
reference for future experiments.

Do not use this folder for the next dual-oscillator experiment. Start that work
in `experimental-firmware/dual-oscillator-lanes` so this known-good dual-pitch
firmware remains available as a rollback and comparison point.

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
- SysEx receive buffering is larger than production because protocol v3 envelope
  payloads carry amplitude, PD, pitch 1, and pitch 2 lanes in one RAM/save frame.
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

## Stable Dual Pitch Scope

The firmware applies two independent pitch lanes, and the matching import lab
can send DCO1/DCO2 separately. The Envelope Lab sends, reads, and displays both
pitch lanes. This version is the reference point before testing two-lane PD or
full two-lane oscillator behaviour.
