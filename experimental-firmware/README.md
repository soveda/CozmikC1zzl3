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
| `C1ZZL3_CORE.uf2` | You want `C1ZZL3 Core`, the current stable production build with the simpler interface and Turing mode. |
| `C1ZZL3_RAD.uf2` | You want `C1ZZL3 Rad`, the stable advanced-with-Turing version with sound presets, envelope-only save, separate Amp/PD/Pitch lanes, separate oscillator wave families, and Turing MIDI defaulting off. |
| `C1ZZL3_GNARLY.uf2` | You want `C1ZZL3 Gnarly`, the stable no-Turing dual-oscillator version with recipe banks and expanded MIDI CC performance control. |

Production remains authoritative at:

```text
uf2/C1ZZL3.uf2
```

The active UF2 copies are convenience copies so testing choices are visible in
one place.

## Visible Firmware Folders

- `full-dual-oscillators/`: Rad stable protocol v9 full-dual oscillator build with
  separate envelope-only and sound-preset save paths, on top of the passing
  protocol v8 slot-name test and protocol v7 full-dual-oscillator baseline.
- `gnarly-dual-oscillator-ui/`: archived v10/Gnarly hardware-UI experiment. It
  keeps the Rad dual-oscillator engine/protocol but remaps the hardware switch
  to oscillator 1, oscillator 2, and performance pages, with generated Turing
  CV, pulse, and MIDI output removed from the live behaviour.
- `cz-recipe-wave-banks-v11/`: stable Gnarly v11 firmware with CZ-style
  compound recipe banks. Its stable UF2 is mirrored into `active-uf2s/` as
  `C1ZZL3_GNARLY.uf2`.
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

## License Notes

Firmware folders include `computercard.h`, the ComputerCard hardware helper by
Chris Johnson. It is MIT licensed; keep its MIT notice present when copying
firmware folders into releases, active UF2 packages, or new experiments.
