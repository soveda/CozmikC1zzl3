# C1ZZL3 Full Dual Oscillator Firmware Experiment

This folder contains the first full-dual-oscillator firmware draft.

It starts from the tested protocol v6 dual-amplitude rollback and keeps the
same oscillator lookup tables. The new experiment adds independent oscillator
wave-family controls through Web MIDI settings while preserving the existing
hardware panel and Turing machine behaviour.

## Current Scope

- Envelope protocol reports version 8.
- Envelope payload remains the tested six-lane shape:
  - Amp1
  - PD1
  - Pitch1
  - Pitch2
  - PD2
  - Amp2
  - CZ sustain markers
- Oscillator 1 uses Amp1, PD1, Pitch1, and oscillator 1 wave family.
- Oscillator 2 uses Amp2, PD2, Pitch2, and oscillator 2 wave family.
- Both oscillators reuse the same `pdWaveLUT`, `phaseWarpLUT`, and oscillator
  render functions.
- Settings protocol adds a 16-byte payload / 24-byte response that carries
  separate wave-family controls.
- Saved custom envelope slots now store and read back a compact 16-character
  ASCII slot name.
- Protocol v9 saves per-slot performance settings with each custom envelope:
  baseline PD, detune, oscillator 1 and 2 wave families, ring, noise, MIDI
  input channel, Turing CV range, and Turing MIDI output/channel.
- The matching Import Lab decodes separate CZ Line 1 and Line 2 waveform/window
  words and sends them as oscillator 1 and oscillator 2 wave-family settings.
- Existing protocol v6-style settings remain accepted and copy oscillator 1
  wave family to oscillator 2.
- Hardware controls are unchanged in this first pass. The separate oscillator
  wave-family selection is Web UI only.

## Build

From the repository root:

```sh
cmake -S experimental-firmware/full-dual-oscillators -B experimental-firmware/full-dual-oscillators/build -DPICO_NO_PICOTOOL=1
cmake --build experimental-firmware/full-dual-oscillators/build -j2
```

The CMake build creates:

```text
experimental-firmware/full-dual-oscillators/build/C1ZZL3_FULL_DUAL_OSCILLATORS.uf2
```

The named test UF2 is:

```text
experimental-firmware/full-dual-oscillators/C1ZZL3_FULL_DUAL_OSCILLATORS_PROTOCOL_V8.uf2
```

The current sound-preset first pass is:

```text
experimental-firmware/full-dual-oscillators/C1ZZL3_FULL_DUAL_OSCILLATORS_PROTOCOL_V9.uf2
```

## Matching Web UI

Use:

```text
experiments/full-dual-oscillators/
```

## Hardware-Control Note

If a later experiment removes the Turing machine mode, switch up could become
oscillator 1 baseline PD/wave controls, switch middle could become oscillator
2 baseline PD/wave controls, and switch down could remain detune/ring/noise.

This first pass does not make that tradeoff.

## Protocol v9 Test Focus

Protocol v9 is the first pass at treating saved custom slots as sound presets:
the card slot should now read back envelope shape, slot name, and performance
settings together. Protocol v8 remains the rollback if the performance setting
handoff needs more work.
