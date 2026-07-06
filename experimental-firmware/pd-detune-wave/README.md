# Experimental PD / Detune / Wave Firmware

This folder is a separate firmware experiment.

## Status

This is the current stable experimental firmware. It has been hardware-tested
with Envelope Lab and CZ Import while the main stable C1ZZL3 firmware remains
unchanged.

Current UF2:

```text
C1ZZL3_PD_DETUNE_WAVE.uf2
```

It keeps the stable C1ZZL3 firmware untouched and adds an extended Web MIDI
performance settings protocol for testing:

- PD amount
- oscillator 2 detune
- waveform amount

## What changed

- The Web MIDI settings payload is extended from 8 bytes to 14 bytes.
- The performance settings response returns the same extended payload.
- The firmware stores the extra values in its saved performance state.
- Web MIDI settings use hardware pickup, so physical knobs do not immediately
  overwrite remotely received PD, detune, waveform, ring, or noise values.
- The eight waveform families use compressed transition regions.

## Compatibility

- Existing stable firmware is unchanged.
- Older Web MIDI clients can still use the shorter settings payload on this
  experimental firmware.
- Newer clients can send and read the extended payload.

## Build

This folder has its own CMake target and can be built separately from the main
firmware tree.

The build output is:

```text
build/C1ZZL3_PD_DETUNE_WAVE.uf2
```

After testing, copy that file to `C1ZZL3_PD_DETUNE_WAVE.uf2` to promote it as
the current stable experimental build, then update `SHA256SUMS.txt`.
