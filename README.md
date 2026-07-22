# C1ZZL3

Stable production firmware for the Cosmik C1ZZL3 Music Thing Modular Workshop
Computer card.

C1ZZL3 is a dual phase-distortion synthesiser with browser-editable amplitude,
phase-distortion, and pitch envelopes, USB MIDI device/host support, optional
Turing MIDI output, and a Turing machine mode with CV and pulse outputs.
Turing MIDI output defaults to off and must be enabled deliberately.

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
a9297f7d5d7fd7c2262d27ff6e8afa0ecd026ebe05aea205d9444b0985e8d7d6
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

Visible alternate UF2s are collected in:

```text
experimental-firmware/active-uf2s/
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
- Ring, noise, MIDI channel, Turing range, and Turing MIDI settings persist;
  the baseline for Turing MIDI output is off.

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
- CZ envelope assignment: choose merged, line 1, or line 2 mapping for CZ DCA
  amplitude and DCW phase-distortion envelopes; CZ DCO pitch envelopes map to
  the pitch lane.
- Draft handoff into Envelope Lab in a new tab for final editing and card send.
- Separate import page so CZ translation and envelope editing stay distinct.

Import Lab flow:

1. Open `C1ZZL3 Import Lab`.
2. Drop in or choose a Casio CZ patch file.
3. Review the validation, decoded summary, warnings, and mapped draft.
4. Use `Open In Envelope Lab` to carry the draft into the main editor.
5. In Envelope Lab, review the result, adjust if needed, and send or save it.

## Latest Stable Beta: Full Dual Oscillator Protocol v9

Hosted stable beta lab:

```text
https://soveda.github.io/CozmikC1zzl3/experiments/full-dual-oscillators/
```

Hosted stable beta Import Lab:

```text
https://soveda.github.io/CozmikC1zzl3/experiments/full-dual-oscillators/import-lab/
```

Matching stable beta UF2:

```text
experimental-firmware/active-uf2s/C1ZZL3_STABLE_FULL_DUAL_OSCILLATORS_PROTOCOL_V9.uf2
```

This version lives alongside production rather than replacing it. It is the
latest stable beta for the full-dual oscillator workflow.

Current scope:

- Requires the matching protocol v9 stable beta firmware.
- Preserves the production editor and production UF2 unchanged.
- Sends and reads Amp1/Amp2, PD1/PD2, Pitch1/Pitch2, CZ hold/end markers, slot
  names, saved performance settings, and separate oscillator wave-family
  settings.
- Provides `Save Sound Preset` for envelope/name/settings together.
- Provides `Save Envelope Only` for envelope/name without changing that slot's
  saved settings.
- Import Lab decodes separate CZ Line 1 and Line 2 amplitude, DCW, DCO pitch,
  and oscillator wave settings.
- Import Lab detects single-line CZ patches from the line mode byte and mirrors
  the active line into both C1ZZL3 oscillator lanes.

## Current Gnarly Experiment: Recipe Wave Banks Protocol v11

The active v11 experiment keeps the v10 no-Turing hardware UI and adds four
recipe banks for more CZ-like compound oscillator pairings.

Matching test UF2:

```text
experimental-firmware/active-uf2s/C1ZZL3_EXPERIMENT_CZ_RECIPE_WAVE_BANKS_PROTOCOL_V11.uf2
```

Use the Full Dual Oscillator Lab for this experiment:

```text
https://soveda.github.io/CozmikC1zzl3/experiments/full-dual-oscillators/
```

v11 MIDI CC performance block:

- `CC1`: oscillator 1 phase distortion, useful from a mod wheel.
- `CC20`: oscillator 1 recipe slot.
- `CC21`: oscillator 2 recipe slot.
- `CC22`: ring modulation amount.
- `CC23`: recipe bank.
- `CC24`: oscillator 2 interval/spread.
- `CC25`: oscillator 2 phase distortion.
- `CC26`: noise/grit amount.
- `CC27`: oscillator 1 phase distortion, for eight-knob controllers.
- The v9 lab can also talk to C1ZZL3 Core by collapsing the extra dual lanes
  into a Core-compatible Amp/PD/Pitch payload. Use `Read Settings from Card` and
  `Read Envelopes from Card` after connecting so the lab can detect the card
  version and choose the best send mode.

Older beta web apps and UF2s are archived under
`experiments/archive/superseded-20260721/` and
`experimental-firmware/archive/superseded-20260721/`.

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
- Stable beta full-dual lab: `Save Envelope Only` stores the envelope/name
  without changing that slot's saved settings, while `Save Sound Preset` stores
  envelope/name/settings together.
- `Delete Envelope Slot`: clear the selected custom slot from card flash.
- `Read Envelopes from Card`: load occupied card slots into the editor without
  overwriting changed local drafts.
- Stable beta full-dual lab: `Read Envelopes from Card` also restores the
  saved settings for each sound-preset slot.
- `Read Settings from Card`: pull the current performance settings into the editor.
- `Send Settings`: send the current performance settings to the card.
- `Reset Preset`: restore the selected preset to its factory value.

`Load RAM`, `Load Envelope + Settings`, and `Send Settings` are temporary.
Use `Save Envelope` or `Save Sound Preset` to retain slot data in flash. To
make the current performance settings the global startup baseline, move the
hardware switch from middle to down and hold it until the card confirms the
save.

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
FLASH: 146736 B
RAM: 155644 B
```

## Stability Notes

This build is close to the practical processing limit of this RP2040 card
format, so future changes should be tested carefully at maximum settings.

The stable version includes the lookup-table oscillator optimisation, 192 MHz
clock, optional Turing MIDI output, settings readback, and full CC/knob pickup
handoff. Turing MIDI output defaults off across current builds with Turing MIDI
support, and should be enabled deliberately when needed. Tap tempo remains
removed; Y is the Turing internal clock control.

Possible future optimisation notes are kept in:

```text
FUTURE_NOTES.md
```

## Repository Layout

- `C1ZZL3.cpp`: main firmware
- `C1ZZL3_LUT.cpp` / `C1ZZL3_LUT.h`: phase-distortion lookup tables
- `FUTURE_NOTES.md`: deferred optimisation and cleanup notes
- `web-midi/editor/`: browser editor
- `experiments/cz-import/`: production C1ZZL3 Import Lab
- `experiments/full-dual-oscillators/`: latest stable beta Web MIDI lab
- `uf2/C1ZZL3.uf2`: current stable firmware
- `uf2/archive/`: older UF2s and rollbacks
- `experimental-firmware/active-uf2s/`: visible production, Workshop release,
  and stable beta UF2 copies
- `archive/`: source snapshots and experiment notes
- `CARD_README.md`: user-facing card guide
- `info.yaml`: Workshop Computer site metadata

## License Notes

This project is released under the MIT License. The included `computercard.h`
hardware helper is ComputerCard by Chris Johnson and is also MIT licensed; keep
its MIT notice present when copying firmware files into releases or experiments.
