# C1ZZL3 Rollback Dual Amplitude Protocol v6

This folder preserves the tested rollback state for the dual-amplitude
experiment.

Use this when you want the known-good protocol v6 build before starting the
next oscillator-path experiment.

## Passing Behaviour

- CZ `.syx` decode passes on the checked patch set, including Wist Pad, Toad
  Bass, Robo Growl, pad, bass, and pluck checks.
- Envelope save passes.
- Saved envelope readback passes.
- Amp1/Amp2 readback passes.
- PD1/PD2 readback passes.
- Pitch1/Pitch2 readback passes.

## Matching Web App

Use:

```text
experiments/dual-amplitude-envelopes/
```

## UF2

Use:

```text
C1ZZL3_ROLLBACK_DUAL_AMPLITUDE_PROTOCOL_V6.uf2
```

The same UF2 is also copied to:

```text
experimental-firmware/active-uf2s/C1ZZL3_ROLLBACK_DUAL_AMPLITUDE_PROTOCOL_V6.uf2
```

## SHA256

```text
34ca2214a57203f95054bbfa8b2870952d5d4c22571a099f7168e68ce16bdd67  C1ZZL3_ROLLBACK_DUAL_AMPLITUDE_PROTOCOL_V6.uf2
```
