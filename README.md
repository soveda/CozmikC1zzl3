# C1ZZL3

Stable production firmware for the Cosmik C1ZZL3 Music Thing Modular Workshop
Computer card.

C1ZZL3 is a dual phase-distortion synthesiser with browser-editable amplitude,
phase-distortion, and pitch envelopes, USB MIDI device/host support, optional
Turing MIDI output, and a Turing machine mode with CV and pulse outputs.

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
41c11af1b546cd3e1b459b166da8d6ecc9f972290e896e0cc088ceb4910d640a
```

This is hardware-tested production release 1.4, promoted on 2026-07-18.

The previous production release 1.3 is archived as a fallback at:

```text
uf2/archive/production-1.3-pre-pitch-envelope-20260718/C1ZZL3_1.3_pre_pitch_envelope.uf2
```

Release 1.4 works with Envelope Lab and C1ZZL3 Import Lab and includes Web
MIDI PD, detune, eight waveform families, card-to-editor envelope readback,
browser CZ patch import handoff, pitch envelopes, gate-held envelope looping
with natural completion on gate/note release, corrected CZ DCW-to-PD,
DCA-to-amplitude, and DCO-to-pitch import mapping, high-PD audio smoothing, and
rapid-retrigger oscillator phase continuity.

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
- Web MIDI envelope editor with amplitude, phase-distortion, and pitch lanes.
- USB MIDI device mode for DAW/browser use.
- USB MIDI host mode for class-compliant controllers.
- MIDI notes with gate-held envelope sustain/release.
- MIDI CC control with knob pickup handoff.
- Turing machine audio, CV, pulse, and optional MIDI note output.
- Turing CV and pulse outputs continue running in synth mode.
- Settings readback from the card into the Web MIDI editor.
- Saved envelope readback from the card, including pitch envelope data.
- Ring, noise, MIDI channel, Turing range, and Turing MIDI settings persist.

## Controls

Switch middle: synth mode.

- Main: pitch
- X: phase distortion
- Y: waveform
- `CV In 1`: phase-distortion modulation
- `CV In 2`: waveform modulation
- `Pulse In 2`: gate-held envelope trigger/release and oscillator sync

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

## C1ZZL3 Import Lab

Hosted import lab:

```text
https://soveda.github.io/CozmikC1zzl3/experiments/cz-import/
```

Local import lab:

```sh
python3 -m http.server 5174 --directory experiments/cz-import
```

Open:

```text
http://localhost:5174
```

Use this page to decode Casio CZ `.syx` patches into C1ZZL3 drafts, then open
the result in Envelope Lab for final editing and sending.

Current Import Lab features:

- Light and dark mode toggle matching the Envelope Lab palette.
- Larger `C1ZZL3 Import Lab` header and a clearer guided import workflow.
- Drag-and-drop or file-picker import for Casio CZ `.syx` files.
- Browser-side validation, patch summary, decoded data, and draft mapping.
- CZ frame awareness for common patch-send SysEx files, including command,
  location, channel, selected data offset, and payload candidates.
- CZ envelope assignment: CZ DCW maps to C1ZZL3 phase distortion, CZ DCA maps
  to C1ZZL3 amplitude, and CZ DCO pitch envelopes map to the pitch lane.
- Draft handoff into Envelope Lab in a new tab for final editing and card send.
- Separate import page so CZ translation and envelope editing stay distinct.

Import Lab flow:

1. Open `C1ZZL3 Import Lab`.
2. Drop in or choose a Casio CZ patch file.
3. Review the validation, decoded summary, warnings, and mapped draft.
4. Use `Open In Envelope Lab` to carry the draft into the main editor.
5. In Envelope Lab, review the result, adjust if needed, and send or save it.

## Beta Test: DCO Pitch Import

Hosted beta:

```text
https://soveda.github.io/CozmikC1zzl3/experiments/dco-pitch-handling/import-lab/
```

This beta uses the current production firmware and does not require a different
UF2. It is for comparing how decoded CZ DCO1 and DCO2 pitch envelopes should be
mapped into the single C1ZZL3 pitch lane before any future firmware work on
separate oscillator pitch envelopes.

Beta pitch options:

- `Merged DCO1 + DCO2 average`: recommended default and usually the most useful.
- `DCO1 pitch only`: useful when oscillator 1 has the clearer contour.
- `DCO2 pitch only`: useful when oscillator 2 has the clearer contour.
- `Advanced: DCO1 / DCO2 difference emphasis`: can be interesting when the two
  envelopes differ, but may lose character when they are too similar.

## How To Use The Editor

1. Pick a preset on the left, or add a custom one.
2. Choose `Amplitude` or `Phase Distortion` to focus the main graph lane.
3. Use the Pitch Envelope graph below it to adjust pitch movement.
4. Drag points on the graphs to change both level and timing.
5. Watch the point numbers. When stages stack, only the highest number is shown.
6. Use the tables below the graphs for exact values when you want precise edits.
7. Use the action buttons on the right when you want to send, save, read, or reset.

Button quick reference:

- `Load RAM`: send the envelope to the card until reset.
- `Load Envelope + Settings`: temporarily load the selected envelope and send
  all current settings in one action.
- `Save Envelope`: store the selected custom envelope in flash.
- `Delete Envelope Slot`: clear the selected custom slot from card flash.
- `Read Envelopes from Card`: load occupied card slots into the editor without
  overwriting changed local drafts.
- `Read Settings from Card`: pull the current performance settings into the editor.
- `Send Settings`: send the current performance settings to the card.
- `Reset Preset`: restore the selected preset to its factory value.

`Load RAM`, `Load Envelope + Settings`, and `Send Settings` are temporary.
Use `Save Envelope` to retain an envelope in flash. To make the current
performance settings the startup baseline, move the hardware switch from
middle to down and hold it until the card confirms the save.

The card can save up to eight custom envelopes. The browser can retain additional
local drafts. Factory presets are not overwritten.
Custom presets are labelled `Local only`, `Saved - slot N`, or `Changed - slot N`.
Envelope readback confirms which custom slots are occupied and verifies saves and
deletions when supported by the card firmware.

Envelope behaviour:

- Pulse In 2 and MIDI note-on trigger the selected envelope and sync the
  oscillators when the envelope starts from inactive.
- While the gate or MIDI note is held, loop-capable envelopes cycle their middle
  stages.
- A short trigger runs the envelope through to completion.
- Pulse In 2 gate-off or MIDI note-off exits the loop and lets the envelope
  complete naturally from its current point.
- Turing-triggered envelopes continue to run through without waiting for a gate
  release.
- Rapid retriggers keep oscillator phase continuous while the envelope is active
  to reduce clicks.

## Build

```sh
cmake -S . -B build
cmake --build build
```

The built UF2 will be:

```text
build/C1ZZL3.uf2
```

The production source build currently reports:

```text
FLASH: 146704 B
RAM: 155612 B
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
