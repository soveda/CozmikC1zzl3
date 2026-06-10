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

## Current Status

The editor can:

- edit and add envelope presets
- preview amplitude and phase-distortion envelopes
- audition envelopes in the browser
- use Web MIDI note input for browser pitch auditioning
- copy firmware-ready C++ for `envelopeProgram()`
- copy or send an experimental C1ZZL3 SysEx preview frame

The experimental firmware can:

- enumerate as a USB MIDI device
- receive C1ZZL3 SysEx preview frames from the browser editor
- decode the transmitted amplitude and phase-distortion stages
- load the transmitted envelope into a RAM-only `WebMidi` preset slot
- immediately trigger the received envelope for auditioning
- flash all six LEDs for accepted SysEx frames
- alternate even/odd LEDs for rejected SysEx frames

The experimental firmware does not yet persist custom envelope data to flash.
That is deliberate: saving only the preset number without saving the full
envelope shape would make reloads misleading.

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
