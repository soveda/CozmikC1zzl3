# C1ZZL3 Shared Advanced Envelope Lab

This folder is the development mirror for the shared C1ZZL3 Web MIDI UI. The
public editor path is
`web-midi/editor/`; this folder remains useful for developing and validating
advanced full-dual behaviour before syncing it into the hosted release copies.

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
  settings together on firmware that supports sound presets. On older cards it
  falls back to the collapsed compatible envelope save because those slots do not store
  full-dual sound-preset settings.
- On MIDI connection, the lab automatically checks settings and saved envelope
  slots so the detected firmware type is visible straight away.
- `Read Envelopes from Card` also restores saved settings for card slots.
- Compatibility send paths support older and newer C1ZZL3 firmware:
  - older cards receive a collapsed Amp/PD/Pitch payload.
  - dual-lane-capable cards receive the full oscillator payload.
  - Developer diagnostics show the detected settings protocol, envelope
    protocol, and current envelope send mode.
- Protocol v11 recipe-bank firmware is also supported by this lab. It adds a
  recipe-bank setting for CZ-style compound oscillator pairings.

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

After connecting MIDI, the editor automatically checks card settings and saved
envelope slots. Use `Read Settings from Card` and `Read Envelopes from Card` if
you want to manually refresh the displayed card capability state.

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

## Protocol v11 Behaviour

Protocol v11 keeps the full dual-oscillator payload shape and adds the recipe
bank to settings readback/save. Its firmware also exposes a controller-friendly
`CC20` to `CC27` performance block:

- `CC1`: oscillator 1 phase distortion, mod-wheel friendly.
- `CC20`: oscillator 1 recipe slot.
- `CC21`: oscillator 2 recipe slot.
- `CC22`: ring modulation amount.
- `CC23`: recipe bank.
- `CC24`: oscillator 2 interval/spread.
- `CC25`: oscillator 2 phase distortion.
- `CC26`: noise/grit amount.
- `CC27`: oscillator 1 phase distortion, for eight-knob controllers.

Open Developer tools in the lab to reveal the hidden MIDI CC Test Suite.
It can send individual `CC1` and `CC20`-`CC27` messages, a neutral reset, and
low/mid/high sweeps through the selected Web MIDI output.
