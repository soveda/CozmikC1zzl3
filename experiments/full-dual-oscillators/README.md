# C1ZZL3 Full Dual Oscillator Stable Beta

This folder contains the Web MIDI UI for the stable protocol v9
full-dual-oscillator firmware. It starts from the tested dual-amplitude v6
rollback, then adds separate oscillator wave-family settings.

This remains separate from the production Envelope Lab so users can choose the
stable full-dual workflow deliberately.

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
- Import Lab detects single-line CZ patches from the CZ line mode byte and
  mirrors the active line into both C1ZZL3 oscillator lanes; true dual-line
  patches preserve separate lanes.
- Protocol v7/v8/v9 settings readback returns a 24-byte response with separate
  oscillator wave-family controls.
- Envelope send/save keeps the protocol v6 lane payload shape, with protocol
  v9 adding a sound-preset payload that includes slot-name and per-slot
  performance setting readback.
- Card slot readback now restores the saved 16-character slot name and its
  saved settings.
- `Save Envelope Only` updates the slot envelope/name while leaving that slot's
  saved settings unchanged.
- `Save Sound Preset` stores the selected envelope/name and the current
  settings together on Rad/Gnarly. On Core/older cards it falls back to the
  collapsed Core-compatible envelope save because those slots do not store
  full-dual sound-preset settings.
- `Read Envelopes from Card` also restores saved settings for card slots.
- Compatibility send paths support `C1ZZL3 Core`, `C1ZZL3 Rad`, and future
  `C1ZZL3 Gnarly`-style firmware:
  - Core/older cards receive a collapsed Amp/PD/Pitch payload.
  - Rad/Gnarly-capable cards receive the full dual-oscillator payload.
  - Developer diagnostics show the detected settings protocol, envelope
    protocol, and current envelope send mode.

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

After connecting MIDI, the editor auto-checks capability before sound-preset
saves when needed. Use `Read Settings from Card` and `Read Envelopes from Card`
if you want to manually refresh the displayed Core/Rad/Gnarly capability state.

## Hardware-Control Note

This first pass keeps the existing hardware panel behaviour and Turing mode.
The separate oscillator wave-family selection is Web UI only.

If the card later trades away the Turing machine mode, switch up could become
oscillator 1 tone controls and switch middle could become oscillator 2 tone
controls, while switch down remains detune/ring/noise.

## Protocol v9 Behaviour

Protocol v9 saves performance settings with each custom envelope slot,
including PD, detune, ring, noise, MIDI/Turing settings, and oscillator 1/2 wave
families. The UI exposes both envelope-only and sound-preset save paths so the
two behaviours remain clear.
