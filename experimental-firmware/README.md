# C1ZZL3 Experimental Firmware

Use this folder for firmware builds that sit outside the production UF2.

## Which UF2 Should I Use?

The easiest place to choose a build is:

```text
experimental-firmware/active-uf2s/
```

Current visible choices:

| UF2 | Use when |
| --- | --- |
| `C1ZZL3_WORKSHOP_RELEASE_84_V1.3.uf2` | You want the Workshop Computer release 84 / version 1.3 exactly as published. |
| `C1ZZL3_PRODUCTION_1.4.uf2` | You want `C1ZZL3 Core`, the current stable production build, mirrored here for convenience. |
| `C1ZZL3_STABLE_FULL_DUAL_OSCILLATORS_PROTOCOL_V9.uf2` | You want `C1ZZL3 Rad`, the latest stable beta / advanced-with-Turing version with sound presets, envelope-only save, separate Amp/PD/Pitch lanes, and separate oscillator wave families. |

Production remains authoritative at:

```text
uf2/C1ZZL3.uf2
```

The active UF2 copies are convenience copies so testing choices are visible in
one place.

## Visible Firmware Folders

- `full-dual-oscillators/`: latest stable beta, protocol v9 full-dual oscillator build with
  separate envelope-only and sound-preset save paths, on top of the passing
  protocol v8 slot-name test and protocol v7 full-dual-oscillator baseline.
- `workshop-release-84-v1.3/`: preserved public Workshop Computer release 84
  / version 1.3.

## Archived Firmware Experiments

Completed or superseded experiments are kept at:

```text
experimental-firmware/archive/completed-20260719/
experimental-firmware/archive/superseded-20260721/
```

These are retained for rollback/reference, but are not the first choice for
current testing.
