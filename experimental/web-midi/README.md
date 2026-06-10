# C1ZZL3 Experimental Web MIDI Work

This folder is intentionally separate from the working card firmware.

The production firmware remains in `C1ZZL3.cpp`. Do not copy files from this
folder into a release build unless you are intentionally testing the
experimental Web MIDI/SysEx path.

## Contents

- `editor/`: static browser editor for envelope presets.
- `editor/app.js`: Web MIDI note auditioning and experimental C1ZZL3 SysEx
  frame generation.
- `firmware/`: isolated experimental firmware copy with a TinyUSB USB MIDI
  device SysEx receiver. The production `../../C1ZZL3.cpp` is not used or
  modified by this build.
- `protocol.md`: current experimental SysEx frame format and LED feedback
  expectations.

## Running The Editor Locally

```sh
python3 -m http.server 5173 --directory experimental/web-midi/editor
```

Then open:

```text
http://localhost:5173
```

After pressing `MIDI`, choose the C1ZZL3 device in the `MIDI output` selector
before pressing `Send`.

## Current Status

The editor can:

- edit and add envelope presets
- preview amplitude and phase-distortion envelopes
- set ring mod, noise, MIDI input channel, and Turing MIDI output channel
- audition envelopes in the browser
- use Web MIDI note input for browser pitch auditioning
- copy firmware-ready C++ for `envelopeProgram()`
- copy or send an experimental C1ZZL3 SysEx preview frame

The experimental firmware can:

- enumerate as a USB MIDI device
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
- If Chrome freezes or becomes unstable, reload the page, press `MIDI` once,
  select the C1ZZL3 MIDI output explicitly, and send one frame at a time.
- The editor no longer reconnects automatically from inside MIDI state-change
  events; it only refreshes the visible port list.

## Building The Experimental Firmware

```sh
cmake -S experimental/web-midi/firmware -B experimental/web-midi/firmware/build
cmake --build experimental/web-midi/firmware/build
```

If your Pico SDK tries to fetch `picotool` during configure and network access is
unavailable, use:

```sh
cmake -S experimental/web-midi/firmware -B experimental/web-midi/firmware/build-no-picotool -DPICO_NO_PICOTOOL=1
cmake --build experimental/web-midi/firmware/build-no-picotool
```

Use the UF2 from a normal `experimental/web-midi/firmware/build/` only for
testing the Web MIDI/SysEx path. The target is named
`C1ZZL3_web_midi_experimental`, so the test UF2 should be
`C1ZZL3_web_midi_experimental.uf2`. The `build-no-picotool` fallback is useful
for compile verification and produces ELF/BIN/HEX artifacts, but not UF2 output.
