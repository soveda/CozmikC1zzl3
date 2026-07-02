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

Use Chrome or another browser with Web MIDI and SysEx support. Press `MIDI`,
select the C1ZZL3 MIDI output, then use:

- `Load RAM` to send the selected envelope to a custom RAM slot until reset
- `Save Slot` to write the selected custom slot to card flash
- `Delete Slot` to clear the selected custom slot from card flash
- `Read` to load the current card settings into the editor
- `Set` to save ring, noise, MIDI input channel, Turing CV range, and Turing
  MIDI output settings to the card
- `Reset Preset` to restore the selected envelope to the `Bounce` preset shape

## How To Use The Editor

- Choose a preset from the left-hand list.
- Factory presets are protected. The first edit creates a custom copy.
- Use the `Amplitude` and `Phase Distortion` buttons to choose which lane you
  are editing.
- Drag points on the graph to change level and timing together.
- Only the active lane can be moved.
- If several stages land on the same point, only the highest stage number is
  shown on the graph.
- Use the stage table on the right to enter exact numeric values for the active
  lane.
- `Play` loops a browser preview of the envelope.
- `Stop` stops the browser preview only. It does not send a stop command to the
  hardware.
- `Bounce` is the reset preset because it shows the envelope shape clearly.

Factory presets are protected. Custom envelope names are stored by the browser;
the card stores the envelope shapes.

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
