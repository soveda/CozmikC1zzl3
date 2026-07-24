# Active C1ZZL3 UF2 Builds

This folder collects the UF2s that should remain visible for users. It is kept
deliberately small: one current build each for Core, Rad, and Gnarly.

## Recommended Choices

| File | Firmware role | Matching web app |
| --- | --- | --- |
| `C1ZZL3_CORE.uf2` | `C1ZZL3 Core`: current stable production firmware with the simpler interface and Turing mode. | `web-midi/editor/` and `experiments/cz-import/` |
| `C1ZZL3_RAD.uf2` | `C1ZZL3 Rad`: stable advanced-with-Turing version with sound presets, envelope-only save, Amp1/Amp2, PD1/PD2, Pitch1/Pitch2, CZ hold/end markers, slot names, saved performance settings, separate oscillator wave families, and Turing MIDI defaulting off. | `web-midi/editor/` and `experiments/cz-import/` |
| `C1ZZL3_GNARLY.uf2` | `C1ZZL3 Gnarly`: stable no-Turing dual-oscillator version with recipe wave banks, two oscillator recipe slots, ring/noise performance controls, full sound presets, and the `CC20`-`CC27` controller block. | `web-midi/editor/` and `experiments/cz-import/` |

## Notes

- Production is mirrored here for convenience only. The canonical Core
  production UF2 remains `uf2/C1ZZL3.uf2`.
- Core, Rad, and Gnarly have all passed the current hardware sustain behaviour:
  held MIDI notes and held Pulse2 gates sustain stock envelopes until note-off
  or gate-off, then complete naturally.
- Older active copies, including Workshop release 84 / version 1.3 and the
  Gnarly v10 fallback, have been archived under
  `experimental-firmware/archive/active-uf2s-20260724/`.
- Older beta and rollback UF2s are also archived under
  `experimental-firmware/archive/superseded-20260721/active-uf2s/`.
