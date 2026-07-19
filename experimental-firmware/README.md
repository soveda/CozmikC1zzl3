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
| `C1ZZL3_PRODUCTION_1.4.uf2` | You want the current stable production build, mirrored here for convenience. |
| `C1ZZL3_STABLE_DUAL_PITCH_PROTOCOL_V3.uf2` | You are testing the stable dual-pitch Envelope Lab with separate oscillator pitch lanes. |
| `C1ZZL3_EXPERIMENT_DUAL_OSCILLATOR_LANES_PROTOCOL_V4.uf2` | You are testing the new two-lane PD / dual-oscillator-lanes firmware experiment. |

Production remains authoritative at:

```text
uf2/C1ZZL3.uf2
```

The active UF2 copies are convenience copies so testing choices are visible in
one place.

## Active Firmware Folders

- `dual-pitch-envelopes/`: stable dual-pitch reference, protocol v3.
- `dual-oscillator-lanes/`: current experiment, protocol v4 with optional PD2.

## Archived Firmware Experiments

Completed or superseded experiments are kept at:

```text
experimental-firmware/archive/completed-20260719/
```

These are retained for rollback/reference, but are not the first choice for
current testing.
