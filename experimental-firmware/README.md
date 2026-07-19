# C1ZZL3 Experimental Firmware

Use this folder for firmware builds that sit outside the production UF2.

## Which UF2 Should I Use?

The easiest place to choose a build is:

```text
experimental-firmware/active-uf2s/
```

Current active choices:

| UF2 | Use when |
| --- | --- |
| `C1ZZL3_WORKSHOP_RELEASE_84_V1.3.uf2` | You want the Workshop Computer release 84 / version 1.3 exactly as published. |
| `C1ZZL3_PRODUCTION_1.4.uf2` | You want the current stable production build, mirrored here for convenience. |
| `C1ZZL3_STABLE_DUAL_PITCH_PROTOCOL_V3.uf2` | You are testing the stable dual-pitch Envelope Lab with separate oscillator pitch lanes. |
| `C1ZZL3_EXPERIMENT_DUAL_OSCILLATOR_LANES_PROTOCOL_V4.uf2` | You are testing the new two-lane PD / dual-oscillator-lanes firmware experiment. |
| `C1ZZL3_ROLLBACK_DUAL_PD_PROTOCOL_V4.uf2` | You want the fully tested dual-PD protocol v4 rollback experimental before trying later experiments. |
| `C1ZZL3_EXPERIMENT_DUAL_AMPLITUDE_ENVELOPES_PROTOCOL_V5.uf2` | You are testing separate Amp1/Amp2 envelopes alongside PD1/PD2 and Pitch1/Pitch2. |

Production remains authoritative at:

```text
uf2/C1ZZL3.uf2
```

The active UF2 copies are convenience copies so testing choices are visible in
one place.

## Active Firmware Folders

- `dual-pitch-envelopes/`: stable dual-pitch reference, protocol v3.
- `dual-oscillator-lanes/`: current experiment, protocol v4 with optional PD2.
- `rollback-dual-pd-protocol-v4/`: preserved tested rollback experimental for
  dual-PD / dual-pitch protocol v4.
- `dual-amplitude-envelopes/`: first draft protocol v5 experiment with
  separate oscillator amplitude lanes.
- `workshop-release-84-v1.3/`: preserved public Workshop Computer release 84
  / version 1.3.

## Archived Firmware Experiments

Completed or superseded experiments are kept at:

```text
experimental-firmware/archive/completed-20260719/
```

These are retained for rollback/reference, but are not the first choice for
current testing.
