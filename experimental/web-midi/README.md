# C1ZZL3 Experimental Firmware

This folder holds isolated firmware experiments. The current experimental
firmware starts from the exact pre-MIDI v0.4 source from commit `941986e`
(`updated waveforms and reordered`, 9 June 2026), with a narrow Reverb+ 1.5
style USB MIDI interface added back for testing.

## Stability Warning

The only firmware currently confirmed as stable is the non-MIDI v0.4 build:

```text
../../uf2/C1ZZL3v04working.uf2
```

An exact copy is also kept here:

```text
experimental/web-midi/C1ZZL3_v04working_reference_stable.uf2
```

The MIDI/Web MIDI firmware experiments are not currently considered stable.
The current MIDI test build deliberately includes MIDI note input only; it does
not include Web MIDI envelope transfer, custom envelope flash writes, settings
SysEx, or Turing MIDI output.

Known unstable or not-yet-verified areas:

- Old MIDI/Web MIDI envelope handling could overload the firmware, especially
  with high X/Y settings. That Web MIDI envelope path is not included in the
  current Reverb+ 1.5 style MIDI test build.
- Combining maximum Y, maximum X, maximum detune, maximum ring, maximum noise,
  fast Pulse 2 envelope triggering, background Turing CV/pulses, and MIDI note
  input has broken the build in testing.
- Later guard/relief builds remained unstable and sometimes stopped recovering
  when switching to Turing mode and back.
- Save/Flash operations in MIDI/Web MIDI builds should be considered unproven.

For general use, flash the v0.4 non-MIDI UF2 above. Use the MIDI test file only
when deliberately testing unstable experiments.

Current experimental rebuild UF2:

```text
experimental/web-midi/C1ZZL3_v04_stable_experimental.uf2
```

This rebuild uses the same source as the known stable v0.4 commit, but the UF2
is not byte-identical because the experimental target name differs. For exact
hardware confirmation, prefer `C1ZZL3_v04working_reference_stable.uf2`.

Current MIDI interface test UF2:

```text
experimental/web-midi/C1ZZL3_reverb15_midi_experimental.uf2
```

This build should enumerate as `C1ZZL3 Reverb15 MIDI Experimental`. It uses
boot-time USB role selection on 2025 hardware: computer/DAW connection enters
USB MIDI device mode, and downstream controller connection enters USB MIDI host
mode.

Hardware-passed recovery point:

```text
experimental/web-midi/recovery/reverb15_midi_passed_20260612/
```

This recovery folder contains the passed UF2, matching source, build file, USB
MIDI support files, and checksum. Use it as the rollback point before adding
new MIDI/Web UI features.

Current knob-pickup test UF2:

```text
experimental/web-midi/C1ZZL3_reverb15_midi_knob_pickup_experimental.uf2
```

This build starts from the hardware-passed Reverb+ 1.5 style MIDI recovery
point and adds catchup behaviour when changing between synth, held performance
edit, and Turing switch positions. Main, X, and Y keep their previous mode
values until each physical knob is swept back through its stored value.

Failed extreme-guard test:

```text
experimental/web-midi/archive/failed-tests/extreme_guard_worse_20260612/
```

The attempted audio headroom guard made the card less stable. It has been
removed from the active firmware source and the UF2 has been archived as a
failed test.

Previous MIDI/Web MIDI test builds have been moved into `archive/` as
historical artifacts. They should not be treated as stable baselines.

## Contents

- `firmware/`: current v0.4-based experimental source with narrow MIDI note
  input added.
- `C1ZZL3_v04working_reference_stable.uf2`: exact copy of the confirmed stable
  non-MIDI v0.4 UF2.
- `C1ZZL3_v04_stable_experimental.uf2`: freshly rebuilt non-MIDI v0.4
  experimental UF2.
- `C1ZZL3_reverb15_midi_experimental.uf2`: current Reverb+ 1.5 style MIDI
  interface test UF2.
- `C1ZZL3_reverb15_midi_knob_pickup_experimental.uf2`: current test build with
  mode-change knob pickup added.
- `recovery/reverb15_midi_passed_20260612/`: hardware-passed rollback copy of
  the current MIDI interface test build.
- `archive/`: old MIDI/Web MIDI UF2s, previous backups, failed test builds,
  unused MIDI firmware files, and generated build folders.

## Running The Editor Locally

The web editor is not part of the current Reverb+ 1.5 style MIDI interface
test firmware. These notes are retained only for archived Web MIDI development.

```sh
python3 -m http.server 5173 --directory web-midi/editor
```

Then open:

```text
http://localhost:5173
```

After pressing `MIDI`, choose the C1ZZL3 device in the `MIDI output` selector
before pressing `Send`.

## Archived MIDI/Web MIDI Notes

This section describes the previous intended Web MIDI feature set. It does not
describe the current Reverb+ 1.5 style MIDI interface test firmware.

The editor can:

- edit and add envelope presets
- preview amplitude and phase-distortion envelopes
- set ring mod, noise, and MIDI input channel
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
- experimental Turing MIDI output is currently disabled while stability is
  being investigated

The editor keeps custom preset names in browser local storage. The card firmware
persists custom envelope shapes only; it does not store or display names.

## Testing Notes

- Do not treat a single passed test as full validation. Recent failures have
  appeared only when several heavy features were combined.
- After any overload, reset the card before judging the next test.
- Test one variable at a time where possible: first X/Y, then ring/noise, then
  detune, then Pulse 2 triggering, then MIDI/Web MIDI.
- A valid SysEx frame will switch the active envelope to the chosen custom slot
  for auditioning in normal conditions. In guarded extreme states, the firmware
  may accept/store the envelope without switching the active sound.
- Factory presets 0-8 are not overwritten by SysEx.
- Preset 0 / Off and fully silent amplitude envelopes are not sent or accepted
  as custom slots.
- Very short envelopes are blocked by the editor before sending. If the status
  shows `0.00s`, drag a node to the right or enter a longer stage time before
  using `Send`.
- Envelope retriggers use the original 9 June behaviour: the envelope restarts
  from zero and the oscillators sync on Pulse 2 / MIDI note-on.
- `Send` previews a custom slot in RAM. `Flash` writes the chosen custom slot to
  the card's flash sector for power-cycle testing.
- `Set` applies ring/noise/MIDI channel settings in RAM. `Save Set` writes those
  settings to the card's flash sector.
- This clean rebuild keeps the original 9 June oscillator, waveform, envelope,
  Turing clock, detune, and save-gesture behaviour wherever possible.
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
- Turing MIDI output is currently disabled in experimental firmware. CV and
  pulse outputs remain the test path for Turing mode.
- USB role is selected at boot/reset. Connect the USB-C port first, then power
  or reset the card. A computer connection starts Web MIDI/DAW device mode; a
  downstream-facing connection starts USB MIDI host mode on 2025 hardware.
- USB MIDI host mode enables TinyUSB hub support for setups such as a USB-B to
  USB-A controller cable into a USB-A to USB-C hub. If possible, also test with
  a direct USB-B to USB-C cable to remove the hub as a variable.
- Ring/noise/Y-max regression focus: with no MIDI connected, confirm high Y,
  Pluck, no detune, no ring, no noise, and X minimum still behave like the clean
  redo. Then add ringmod, noise, and both together at Y maximum.
- Turing background CV test: set a Turing pattern, switch to middle, and confirm
  CV out 1/2 plus pulse out 1/2 keep changing from the Turing clock while the
  synth audio remains playable as a normal synth. Then move Main/X/Y in middle
  and switch back to Turing; Turing mutation, length, and clock speed should not
  jump until each knob is swept through its held value. Repeat in the other
  direction for synth pitch, X, and Y.
- Efficient-clock test: in Turing mode, sweep Y slowly and confirm clock speed
  changes smoothly. Then stop moving Y and switch to middle; CV and pulse
  outputs should keep running at the stored Turing speed while the synth remains
  playable.
- High-X/Y relief test: with detune, ring, noise, MIDI, and Pulse 2 all off,
  raise X to maximum and sweep Y slowly to maximum. Confirm the synth remains
  responsive. Then add ring, noise, detune, background Turing CV, and MIDI input
  one at a time.
- Web-envelope/PD-safe test: with X and Y below maximum, Send should accept and
  audition the custom envelope. With X and Y both near maximum, Send should
  accept/load the slot but may not auto-audition. Flash should persist the slot
  after a power cycle.
- MIDI-envelope high-Y test: choose a factory envelope such as Pluck, set X high,
  sweep Y to maximum, and play repeated MIDI notes. The envelope should still
  gate the sound without pushing the oscillator into overload.
- Envelope amplitude-only test: select Pluck, set X high, and sweep Y from low
  to maximum. The card should remain responsive; the envelope should still gate
  amplitude, but it will not add extra phase-distortion movement.
- MIDI-note no-sync test: select Pluck, set X and Y to maximum, and play MIDI
  notes. The envelope should gate amplitude without the signal breaking up from
  repeated oscillator sync.
- Active Y/noise limit test: select Pluck, set X max, play MIDI notes, then
  sweep Y past 80% to maximum. The sound should stay alive even though the
  effective MIDI/envelope waveform is internally limited. Then add detune, ring,
  and noise; high noise should be limited rather than failing the card.
- Envelope light-resonance test: select Pluck, set X max, trigger from Pulse 2
  or MIDI, and sweep Y all the way to maximum. The full Y range should remain
  usable, but the upper range will use lighter waveform variants while the
  envelope/MIDI note is active.
- Detune overflow test: with all controls high, hold the switch down and sweep
  Main through the detune range. Release back to middle and confirm Main still
  controls oscillator pitch normally.
- Known failing torture case: maximum Y, maximum X, maximum detune, maximum
  ring, maximum noise, fast Pulse 2 envelope triggering, background Turing
  CV/pulses, and MIDI note input.
- Known risky Web MIDI case: X and Y at maximum while sending/applying a Web
  MIDI envelope. The current build should avoid auto-audition in that state,
  but it still needs hardware confirmation.
- If Chrome freezes or becomes unstable, reload the page, press `MIDI` once,
  select the C1ZZL3 MIDI output explicitly, and send one frame at a time.
- The editor no longer reconnects automatically from inside MIDI state-change
  events; it only refreshes the visible port list.

## Building The Main Firmware

```sh
cmake -S . -B build
cmake --build build
```
