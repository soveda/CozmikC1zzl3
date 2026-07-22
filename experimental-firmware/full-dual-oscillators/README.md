# C1ZZL3 Full Dual Oscillator Firmware

This folder contains the latest stable beta protocol v9 full-dual-oscillator
firmware.

It starts from the tested protocol v6 dual-amplitude rollback and keeps the
same oscillator lookup tables. This stable beta adds independent oscillator
wave-family controls through Web MIDI settings while preserving the existing
hardware panel and Turing machine behaviour.

## Current Scope

- Envelope protocol reports version 9.
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
- Protocol v9 can save either envelope-only updates or full sound presets:
  - `Save Envelope Only` updates the envelope shape and slot name while keeping
    the slot's previously saved settings.
  - `Save Sound Preset` stores the envelope shape, slot name, and current
    settings together.
- Protocol v9 sound preset settings include:
  baseline PD, detune, oscillator 1 and 2 wave families, ring, noise, MIDI
  input channel, Turing CV range, and Turing MIDI output/channel.
- Turing MIDI output is optional and defaults off. Older saved global
  performance settings are migrated to the safer off baseline on first boot.
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

The current stable beta full-dual build is:

```text
experimental-firmware/full-dual-oscillators/C1ZZL3_FULL_DUAL_OSCILLATORS_PROTOCOL_V9.uf2
```

## Matching Web UI

Use:

```text
experiments/full-dual-oscillators/
```

The matching Web UI includes compatibility send paths for Core, Rad, and future
Gnarly-style firmware. Older/Core cards receive a collapsed Amp/PD/Pitch
payload, while Rad/Gnarly-capable cards receive the full dual-oscillator payload.
Use `Read Settings from Card` and `Read Envelopes from Card` after connecting
so the editor can choose the best send mode.

## Hardware-Control Note

If a later experiment removes the Turing machine mode, switch up could become
oscillator 1 baseline PD/wave controls, switch middle could become oscillator
2 baseline PD/wave controls, and switch down could remain detune/ring/noise.

This first pass does not make that tradeoff.

## Protocol v9 Behaviour

Protocol v9 treats saved custom slots as sound presets:
the card slot should now read back envelope shape, slot name, and performance
settings together. `Read Envelopes from Card` also restores the saved settings
for each slot in the editor. Older v7/v8 full-dual UF2s are archived under
`experimental-firmware/archive/superseded-20260721/`.

## License Notes

This firmware folder includes `computercard.h`, the ComputerCard hardware helper
by Chris Johnson. It is MIT licensed; keep its MIT notice present when copying
this folder into release packages.
