# C1ZZL3 Dual Oscillator Lanes Firmware Experiment

This folder is reserved for firmware work after the stable dual-pitch build.

The stable dual-pitch firmware reference remains in:

```text
experimental-firmware/dual-pitch-envelopes
```

Do not move or overwrite that reference while developing this experiment.

## Intended Sequence

1. Add protocol support for two PD envelope lanes.
2. Keep the current dual-pitch behaviour as the baseline.
3. Confirm old protocol v3 dual-pitch payloads still work or fail clearly.
4. Only after two-lane PD passes, consider fuller oscillator separation.

## Later Full Two-Lane Behaviour

Potential future signal path:

```text
oscillator 1 pitch envelope -> oscillator 1 phase distortion envelope -> oscillator 1 signal
oscillator 2 pitch envelope -> oscillator 2 phase distortion envelope -> oscillator 2 signal
combined/ring/noise stage -> shared amplitude envelope -> output
```

This is intentionally not implemented yet. The first experiment should be
smaller: two-lane PD envelopes only.

## Hardware Checks Before Promotion

- Boot stability.
- Web MIDI connection.
- Legacy dual-pitch payload compatibility.
- Two-lane PD send/readback.
- CZ Import Lab DCW1/DCW2 mapping.
- Rapid retrigger click test.
- High-PD/high-note harshness check.
- MIDI traffic torture test.
