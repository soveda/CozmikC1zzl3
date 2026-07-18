# C1ZZL3 Web MIDI Editor

This folder contains the browser editor for the production C1ZZL3 Web MIDI
firmware.

## Hosted Editor

Use the hosted GitHub Pages editor:

```text
https://soveda.github.io/CozmikC1zzl3/web-midi/editor/
```

## Run Locally

From the repository root:

```sh
python3 -m http.server 5173 --directory web-midi/editor
```

Open:

```text
http://localhost:5173
```

If that port is busy, use another one:

```sh
python3 -m http.server 5174 --directory web-midi/editor
```

Use Chrome or another browser with Web MIDI and SysEx support.

## C1ZZL3 Import Lab

Use the hosted GitHub Pages import lab:

```text
https://soveda.github.io/CozmikC1zzl3/experiments/cz-import/
```

Run locally:

```sh
python3 -m http.server 5174 --directory experiments/cz-import
```

Open:

```text
http://localhost:5174
```

Import Lab decodes Casio CZ `.syx` patches into C1ZZL3 draft presets, shows a
summary of the translation, and opens the result in Envelope Lab for final
editing and sending.

Current Import Lab features:

- Light and dark mode toggle matching Envelope Lab.
- Guided import layout with validation, summary, warnings, and mapped draft panels.
- Drag-and-drop or file-picker import for Casio CZ `.syx` files.
- CZ frame awareness for common patch-send SysEx files.
- CZ envelope assignment: CZ DCW maps to C1ZZL3 phase distortion, CZ DCA maps
  to C1ZZL3 amplitude, and CZ DCO pitch envelopes map to the pitch lane.
- `Open In Envelope Lab` handoff in a new tab for final editing and card send.

## How To Use The Editor

- Choose a preset from the left-hand list.
- Factory presets are protected. The first edit creates a custom copy.
- Use the `Amplitude` and `Phase Distortion` buttons to choose which lane you
  are editing.
- Use the Pitch Envelope graph below the main graph to adjust pitch movement.
- Drag points on the graphs to change level and timing together.
- Only the active lane can be moved.
- If several stages land on the same point, only the highest stage number is
  shown on the graph.
- Use the stage table on the right to enter exact numeric values for the active
  lane.
- `Play` loops a browser preview of the envelope.
- `Stop` stops the browser preview only. It does not send a stop command to the
  hardware.
- `Bounce` is the reset preset because it shows the envelope shape clearly.
- Hardware and MIDI gates loop supported envelopes while held. Short triggers
  complete naturally, and Pulse In 2 gate-off or MIDI note-off exits the loop
  so the envelope can finish from its current point.

Button quick reference:

- `Load RAM`: send the selected envelope to the card until reset.
- `Load Envelope + Settings`: temporarily load the selected envelope and the
  current settings together.
- `Save Envelope`: store the selected custom envelope in card flash.
- `Delete Envelope Slot`: clear the selected custom slot from card flash.
- `Read Envelopes from Card`: pull occupied card envelope slots into the editor.
- `Read Settings from Card`: pull current performance settings into the editor.
- `Send Settings`: send the current settings to the card.
- `Reset Preset`: restore the selected preset to the `Bounce` preset shape.

Factory presets are protected. Custom envelope names are stored by the browser;
the card stores the amplitude, phase-distortion, and pitch envelope shapes.

Turing CV range defaults to 2 octaves. It can also be changed live with MIDI
CC24 on the selected MIDI input channel. Turing MIDI output on/off and output
channel are set from this editor.

CC20 controls detune, CC21 controls ring modulation, and CC22 controls noise
amount. These are live controls; save from the editor or the card if you want
them to survive reset.

CC1 controls phase distortion and CC23 controls waveform. These update the same
control values as the physical knobs, so the knobs use the normal pickup
behaviour afterward.

## Mobile Web MIDI Notes

- Mobile browser support is less consistent than desktop support.
- If `MIDI` connects but no output appears, open `Developer tools` in the editor
  and check `MIDI Ports Seen By Browser`.
- Some mobile browsers report MIDI access but expose ports in a non-standard
  way. The editor now tries several detection paths, but browser/device support
  can still vary.
- If ports still do not appear, reconnect the device, reload the page, and try
  another USB adapter, hub, or browser.

## Limits

The editor supports eight custom slots, matching the firmware. Custom envelope
names are kept in browser storage; the card stores the envelope shapes and
performance settings.

Import Lab prepares browser drafts only. Envelope Lab is the page that edits,
auditions, sends, saves, and reads back envelopes and settings from the card.
