# C1ZZL3 Dual Amplitude Envelopes Firmware Experiment

This folder contains the first firmware draft for separate oscillator
amplitude envelopes. It builds from the tested dual-PD protocol v4 rollback,
then adds a sixth envelope lane and bumps the Web MIDI envelope protocol to
version 5.

Do not use this as the public production build. The tested rollback remains in:

```text
experimental-firmware/rollback-dual-pd-protocol-v4
```

## First Pass Scope

- Envelope SysEx protocol is bumped to version 5.
- The custom envelope program now has six lanes:
  - oscillator 1 amplitude
  - oscillator 1 phase distortion
  - oscillator 1 pitch
  - oscillator 2 pitch
  - oscillator 2 phase distortion
  - oscillator 2 amplitude
- Oscillator 1 uses Amp1, PD1, and Pitch1.
- Oscillator 2 uses Amp2, PD2, and Pitch2.
- Factory envelopes and older payloads fall back to shared amplitude behaviour.
- Protocol v4 saved envelopes migrate forward with Amp2 copied from Amp1.
- Protocol v3 saved envelopes migrate forward with Amp2 copied from Amp1 and
  PD2 copied from PD1.
- Saved envelope readback uses small chained responses: the main response
  returns Amp1 and PD1; then separate Amp2, PD2, and Pitch responses complete
  the slot.

## Build

From the repository root:

```sh
cmake -S experimental-firmware/dual-amplitude-envelopes -B experimental-firmware/dual-amplitude-envelopes/build -DPICO_NO_PICOTOOL=1
cmake --build experimental-firmware/dual-amplitude-envelopes/build -j2
./experimental-firmware/dual-amplitude-envelopes/build/_deps/picotool/picotool uf2 convert experimental-firmware/dual-amplitude-envelopes/build/C1ZZL3_DUAL_AMPLITUDE_ENVELOPES.elf -t elf experimental-firmware/dual-amplitude-envelopes/C1ZZL3_DUAL_AMPLITUDE_ENVELOPES_PROTOCOL_V5.uf2 -t uf2
```

## Matching Web UI

Use:

```text
experiments/dual-amplitude-envelopes/
```

This is a separate experimental web UI and import lab. It should not replace
the main Envelope Lab unless this experiment passes hardware testing.

## Hardware Checks Before Promotion

- Boot stability.
- Web MIDI connection.
- Protocol v4 saved-envelope migration.
- Protocol v5 Load RAM with distinct Amp1/Amp2 shapes.
- Protocol v5 Save Envelope and Read Envelopes from Card.
- CZ Import Lab DCA1/DCA2 mapping into Amp1/Amp2.
- Rapid retrigger click test.
- Audio balance with intentionally different oscillator amplitude envelopes.
