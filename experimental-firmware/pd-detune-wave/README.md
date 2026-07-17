# Experimental PD / Detune / Wave Firmware

This folder is a separate firmware experiment.

## Status

This is an unpromoted pitch-envelope test build based on stable experimental
release 1.1. Its inherited settings, envelope readback, save/delete
verification, hardware pickup, waveform selection, and restart behaviour were
hardware-tested with Envelope Lab and CZ Import before the pitch-envelope
experiment was added. The main production C1ZZL3 firmware remains unchanged.

Current UF2:

```text
C1ZZL3_PD_DETUNE_WAVE.uf2
```

Previous stable-experimental rollback:

```text
archive/C1ZZL3_PD_DETUNE_WAVE_1.0_20260706.uf2
```

It keeps the stable C1ZZL3 firmware untouched and adds an extended Web MIDI
performance settings protocol for testing:

- PD amount
- oscillator 2 detune
- waveform amount
- experimental pitch-envelope send, readback, save, and playback

## What changed

- The Web MIDI settings payload is extended from 8 bytes to 14 bytes.
- The performance settings response returns the same extended payload.
- The firmware stores the extra values in its saved performance state.
- Web MIDI settings use hardware pickup, so physical knobs do not immediately
  overwrite remotely received PD, detune, waveform, ring, or noise values.
- The eight waveform families use compressed transition regions.
- Envelope Lab can read the occupied custom-slot mask and retrieve saved
  amplitude, phase-distortion, and pitch envelopes one slot at a time.
- Envelope saves and deletions can be verified by browser readback.
- The pitch-envelope protocol is version 2. Legacy two-lane envelope payloads
  are accepted and treated as pitch-neutral.

## Compatibility

- Existing stable firmware is unchanged.
- Older Web MIDI clients can still use the shorter settings payload on this
  experimental firmware.
- Newer clients can send and read the extended payload.
- Older envelope clients can still send two-lane amplitude/PD envelopes; pitch
  remains centred when they do.
- Pitch-envelope editing requires the experimental Envelope Lab in
  `experiments/envelope-lab-pitch/`.

## Build

This folder has its own CMake target and can be built separately from the main
firmware tree.

The build output is:

```text
build/C1ZZL3_PD_DETUNE_WAVE.uf2
```

After testing, copy that file to `C1ZZL3_PD_DETUNE_WAVE.uf2` to promote it as
the current stable experimental build, then update `SHA256SUMS.txt`.
