# C1ZZL3

Stable production firmware for the Cosmik C1ZZL3 Music Thing Modular Workshop
Computer card.

C1ZZL3 is a dual phase-distortion synthesiser with custom Web MIDI envelopes,
USB MIDI device/host support, optional Turing MIDI output, and a Turing machine
mode with CV and pulse outputs.

For the user-facing card guide, see:

```text
CARD_README.md
```

## Stable Build

Current stable UF2:

```text
uf2/C1ZZL3.uf2
```

Checksum:

```text
6cb55940ab502cb38926d20f3cdd76e5f93909ebb391569d82ee385bb20fbf55
```

This is the hardware-tested 192 MHz RP2040 build promoted on 2026-06-25.

Previous UF2s are archived in:

```text
uf2/archive/
```

Source snapshots and experiment notes are archived in:

```text
archive/
```

## Current Stable Feature Set

- Phase-distortion synth voice.
- Factory envelopes plus eight protected custom envelope slots.
- Web MIDI envelope editor.
- USB MIDI device mode for DAW/browser use.
- USB MIDI host mode for class-compliant controllers.
- MIDI notes with envelope triggering.
- MIDI CC control with knob pickup handoff.
- Turing machine audio, CV, pulse, and optional MIDI note output.
- Turing CV and pulse outputs continue running in synth mode.
- Settings readback from the card into the Web MIDI editor.
- Ring, noise, MIDI channel, Turing range, and Turing MIDI settings persist.

## Controls

Switch middle: synth mode.

- Main: pitch
- X: phase distortion
- Y: waveform
- `CV In 1`: phase-distortion modulation
- `CV In 2`: waveform modulation
- `Pulse In 2`: envelope trigger and oscillator sync

Switch down from middle: performance edit and save.

- Main: oscillator 2 detune
- X: ring modulation
- Y: noise/grit

Switch up: Turing mode.

- Main: mutation/lock
- X: sequence length, 2 to 16 steps
- Y: internal clock speed
- `Pulse In 1`: external clock
- `CV Out 1`: stepped Turing CV
- `CV Out 2`: smoothed Turing CV
- `Pulse Out 1/2`: Turing pulses

## MIDI

MIDI CC controls on the selected input channel:

- `CC1`: phase distortion
- `CC20`: oscillator 2 detune
- `CC21`: ring modulation
- `CC22`: noise amount
- `CC23`: waveform
- `CC24`: Turing CV octave range, from 1 to 8 octaves

The physical knobs and MIDI CC controls share the same control values. After a
CC change, the related knob must be swept through the current value before it
takes over again.

## Web MIDI Editor

Hosted editor:

```text
https://soveda.github.io/CozmikC1zzl3/web-midi/editor/
```

Local editor:

```sh
python3 -m http.server 5173 --directory web-midi/editor
```

Open:

```text
http://localhost:5173
```

Use Chrome or another browser with Web MIDI and SysEx support.

## Build

```sh
cmake -S . -B build
cmake --build build
```

The built UF2 will be:

```text
build/C1ZZL3.uf2
```

The stable production build currently reports:

```text
FLASH: 133168 B
RAM: 141500 B
```

## Stability Notes

This build is close to the practical processing limit of this RP2040 card
format, so future changes should be tested carefully at maximum settings.

The stable version includes the lookup-table oscillator optimisation, 192 MHz
clock, Turing MIDI output, settings readback, and full CC/knob pickup handoff.
Tap tempo remains removed; Y is the Turing internal clock control.

Possible future optimisation notes are kept in:

```text
FUTURE_NOTES.md
```

## Repository Layout

- `C1ZZL3.cpp`: main firmware
- `C1ZZL3_LUT.cpp` / `C1ZZL3_LUT.h`: phase-distortion lookup tables
- `FUTURE_NOTES.md`: deferred optimisation and cleanup notes
- `web-midi/editor/`: browser editor
- `uf2/C1ZZL3.uf2`: current stable firmware
- `uf2/archive/`: older UF2s and rollbacks
- `archive/`: source snapshots and experiment notes
- `CARD_README.md`: user-facing card guide
- `info.yaml`: Workshop Computer site metadata
