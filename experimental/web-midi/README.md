# C1ZZL3 Experimental Archive

The Web MIDI / USB MIDI work has been promoted into the main project.

The main firmware is now `../../C1ZZL3.cpp`, and the browser editor is now in
`../../web-midi/editor/`.

## Contents

- `firmware/`: previous isolated experimental firmware build.
- `last-good/`: last known-good experimental source and UF2 before promotion.
- `backups/` and `failed-tests/`: archived test builds from development.

## Running The Editor Locally

```sh
python3 -m http.server 5173 --directory web-midi/editor
```

Then open:

```text
http://localhost:5173
```

After pressing `MIDI`, choose the C1ZZL3 device in the `MIDI output` selector
before pressing `Send`.

## Promoted Status

The editor can:

- edit and add envelope presets
- preview amplitude and phase-distortion envelopes
- set ring mod, noise, MIDI input channel, and Turing MIDI output channel
- audition envelopes in the browser
- use Web MIDI note input for browser pitch auditioning
- copy firmware-ready C++ for `envelopeProgram()`
- copy or send an experimental C1ZZL3 SysEx preview frame

The main firmware can:

- choose USB MIDI device or host mode at boot on 2025 hardware
- enumerate as a USB MIDI device when connected to a computer/DAW/browser
- enumerate an attached class-compliant USB MIDI controller in host mode when
  the USB port is downstream-facing
- receive C1ZZL3 SysEx preview frames from the browser editor
- decode the transmitted amplitude and phase-distortion stages
- load the transmitted envelope into one of eight RAM-only custom preset slots
- keep factory presets 0-8 intact and only enumerate custom slots after them
- immediately trigger the received envelope for auditioning
- flash all six LEDs for accepted SysEx frames
- alternate even/odd LEDs for rejected SysEx frames
- persist a custom slot to flash when the editor sends the `Flash` command
- apply ring/noise/MIDI channel settings from the editor
- receive USB MIDI note on/off from a DAW or other USB MIDI host on the selected
  channel, using note-on to pitch the synth and retrigger the selected envelope
- receive USB MIDI note on/off from a class-compliant controller in host mode
- send Turing mode note-on/note-off messages back to the DAW when Turing MIDI
  output is enabled

The editor keeps custom preset names in browser local storage. The card firmware
persists custom envelope shapes only; it does not store or display names.

## Testing Notes

- A valid SysEx frame will switch the active envelope to the chosen custom slot
  for auditioning. This is expected.
- Factory presets 0-8 are not overwritten by SysEx.
- Preset 0 / Off and fully silent amplitude envelopes are not sent or accepted
  as custom slots.
- Very short envelopes are blocked by the editor before sending. If the status
  shows `0.00s`, drag a node to the right or enter a longer stage time before
  using `Send`.
- Envelope retriggers now start from the current envelope level instead of
  snapping back to zero, which should reduce trigger clicks.
- `Send` previews a custom slot in RAM. `Flash` writes the chosen custom slot to
  the card's flash sector for power-cycle testing.
- `Set` applies ring/noise/MIDI channel settings in RAM. `Save Set` writes those
  settings to the card's flash sector.
- This rebuild uses a fresh experimental saved-state version so bad settings
  left by earlier detune test builds are ignored on first boot.
- Current click-reduction test: envelope stages shorter than 240 samples are
  stretched to 240 samples so amplitude and PD cannot jump in a single sample.
- Audio-rate trigger test: Pulse 2 and MIDI note retriggers still fire the
  envelope, but oscillator phase sync is skipped when triggers arrive less than
  480 samples apart.
- Additional audio-rate test: Pulse 2 and MIDI note retriggers faster than 480
  samples are ignored rather than restarting the envelope at audio rate.
- The browser editor is capped at eight custom presets, matching the eight card
  custom slots, and custom presets can be removed from the preset list.
- The canvas handles can be dragged up/down to change stage level and sideways
  to change that stage's time. The editor shows at least four seconds, then
  expands to fit longer presets such as Strings. When
  amplitude and PD handles sit on top of each other, the editor picks the
  amplitude handle first; move the amplitude handle away, then grab the PD
  handle underneath if needed. Stage times can still be edited numerically.
- In Turing mode, turning X now briefly displays the selected sequence length as
  a binary value on the six LEDs.
- Turing MIDI output maps the generated CV to notes over roughly four octaves.
  Check for hanging notes during testing before relying on it in a DAW session.
- USB role is selected at boot/reset. Connect the USB-C port first, then power
  or reset the card. A computer connection starts Web MIDI/DAW device mode; a
  downstream-facing connection starts USB MIDI host mode on 2025 hardware.
- USB MIDI host mode enables TinyUSB hub support for setups such as a USB-B to
  USB-A controller cable into a USB-A to USB-C hub. If possible, also test with
  a direct USB-B to USB-C cable to remove the hub as a variable.
- If Chrome freezes or becomes unstable, reload the page, press `MIDI` once,
  select the C1ZZL3 MIDI output explicitly, and send one frame at a time.
- The editor no longer reconnects automatically from inside MIDI state-change
  events; it only refreshes the visible port list.

## Building The Main Firmware

```sh
cmake -S . -B build
cmake --build build
```
