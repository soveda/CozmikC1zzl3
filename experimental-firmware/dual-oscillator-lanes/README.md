# C1ZZL3 Dual Oscillator Lanes Firmware Experiment

This folder contains the first-pass firmware experiment after the stable
dual-pitch build.

The stable dual-pitch firmware reference remains in:

```text
experimental-firmware/dual-pitch-envelopes
```

Do not move or overwrite that reference while developing this experiment.

## First Pass Scope

- Envelope SysEx protocol is bumped to version 4.
- The custom envelope program now has five lanes:
  - amplitude
  - oscillator 1 phase distortion
  - oscillator 1 pitch
  - oscillator 2 pitch
  - oscillator 2 phase distortion
- Oscillator 1 uses PD lane 1.
- Oscillator 2 uses PD lane 2.
- If a v3 dual-pitch payload is received, PD lane 2 is copied from PD lane 1.
- Factory envelopes and legacy payloads fall back to shared PD behaviour.
- Pitch 1 and pitch 2 behaviour is inherited from the stable dual-pitch build.
- Amplitude, wave family, ring modulation, noise/grit, and detune remain shared.
- Saved envelope readback now uses protocol v4 behaviour with small chained
  responses: the main envelope response returns amplitude and PD lane 1, a
  separate PD2 response returns PD lane 2, and the separate pitch response
  returns pitch 1 and pitch 2.
- Saved envelope flash storage now stores all five lanes: amp, PD lane 1,
  pitch 1, pitch 2, and PD lane 2.
- Older protocol v3 saved custom-envelope data is migrated at startup by
  preserving amp, PD lane 1, pitch 1, and pitch 2, with PD lane 2 inferred from
  PD lane 1. Re-saving from the protocol v4 web UI writes the full five-lane
  format.

## Build

From the repository root:

```sh
cmake -S experimental-firmware/dual-oscillator-lanes -B experimental-firmware/dual-oscillator-lanes/build -DPICO_NO_PICOTOOL=1
cmake --build experimental-firmware/dual-oscillator-lanes/build -j2
./build/_deps/picotool/picotool uf2 convert experimental-firmware/dual-oscillator-lanes/build/C1ZZL3_DUAL_OSCILLATOR_LANES.elf -t elf experimental-firmware/dual-oscillator-lanes/C1ZZL3_DUAL_OSCILLATOR_LANES.uf2 -t uf2
```

## Test Firmware

Expected UF2 path after build:

```text
experimental-firmware/dual-oscillator-lanes/C1ZZL3_DUAL_OSCILLATOR_LANES.uf2
```

## Intended Sequence After This Pass

1. Test v3 dual-pitch payload compatibility.
2. Test v4 two-PD-lane send/save/readback.
3. Only after two-lane PD passes, consider fuller oscillator separation.

## Later Full Two-Lane Behaviour

Potential future signal path:

```text
oscillator 1 pitch envelope -> oscillator 1 phase distortion envelope -> oscillator 1 signal
oscillator 2 pitch envelope -> oscillator 2 phase distortion envelope -> oscillator 2 signal
combined/ring/noise stage -> shared amplitude envelope -> output
```

This is intentionally not implemented yet. The first experiment should be
smaller: two-lane PD envelopes only.

## Hardware Checks Before Promotion

- Boot stability.
- Web MIDI connection.
- Legacy dual-pitch payload compatibility.
- Two-lane PD send/readback.
- CZ Import Lab DCW1/DCW2 mapping.
- Rapid retrigger click test.
- High-PD/high-note harshness check.
- MIDI traffic torture test.
