# Experimental PD / Detune / Wave Firmware

This folder is a separate firmware experiment.

It keeps the stable C1ZZL3 firmware untouched and adds an extended Web MIDI
performance settings protocol for testing:

- PD amount
- oscillator 2 detune
- waveform amount

## What changed

- The Web MIDI settings payload is extended from 8 bytes to 13 bytes.
- The performance settings response returns the same extended payload.
- The firmware stores the extra values in its saved performance state.

## Compatibility

- Existing stable firmware is unchanged.
- Older Web MIDI clients can still use the shorter settings payload on this
  experimental firmware.
- Newer clients can send and read the extended payload.

## Build

This folder has its own CMake target and can be built separately from the main
firmware tree.
