# C1ZZL3 Full Dual Oscillator Firmware Experiment

This folder contains the first full-dual-oscillator firmware draft.

It starts from the tested protocol v6 dual-amplitude rollback and keeps the
same oscillator lookup tables. The new experiment adds independent oscillator
wave-family controls through Web MIDI settings while preserving the existing
hardware panel and Turing machine behaviour.

## Current Scope

- Envelope protocol reports version 7.
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
experimental-firmware/full-dual-oscillators/C1ZZL3_FULL_DUAL_OSCILLATORS_PROTOCOL_V7.uf2
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

## Post-v7 Test Reminder

After the protocol v7 full-dual-oscillator tests are complete, look at saving
performance settings with envelope data and recalling envelope/sound preset
names from the card.

Initial implementation direction:

- Store envelope names on-card as compact ASCII alongside each custom envelope
  slot.
- Store performance settings with the saved envelope so a card slot can behave
  like a complete sound preset rather than only an envelope preset.
- Add a protocol response that returns slot name plus saved performance
  settings during envelope readback.
- Keep old unnamed slots readable by generating `Card Envelope N` in the
  browser.
- Treat this as the next protocol bump after v7, because it changes saved slot
  data and readback semantics.
