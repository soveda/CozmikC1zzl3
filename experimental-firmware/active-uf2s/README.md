# Active C1ZZL3 UF2 Builds

This folder collects the UF2s that should remain visible for users.

## Recommended Choices

| File | Firmware role | Matching web app |
| --- | --- | --- |
| `C1ZZL3_WORKSHOP_RELEASE_84_V1.3.uf2` | Public Workshop Computer release 84 / version 1.3, kept as an always-available option. | Workshop release web editor |
| `C1ZZL3_PRODUCTION_1.4.uf2` | `C1ZZL3 Core`: current stable production firmware. | `web-midi/editor/` and `experiments/cz-import/` |
| `C1ZZL3_STABLE_FULL_DUAL_OSCILLATORS_PROTOCOL_V9.uf2` | `C1ZZL3 Rad`: latest stable beta / advanced-with-Turing version with sound presets, envelope-only save, Amp1/Amp2, PD1/PD2, Pitch1/Pitch2, CZ hold/end markers, slot names, saved performance settings, and separate oscillator wave families. | `experiments/full-dual-oscillators/` |
| `C1ZZL3_GNARLY_DUAL_OSCILLATOR_UI_PROTOCOL_V10.uf2` | `C1ZZL3 Gnarly`: first hardware-UI experiment with switch-up for oscillator 1, switch-middle for oscillator 2, and switch-down hold for oscillator 2 base interval/ring/noise plus save. | `experiments/full-dual-oscillators/` |

## Notes

- Production is mirrored here for convenience only. The canonical production
  UF2 remains `uf2/C1ZZL3.uf2`.
- Workshop release 84 / version 1.3 is preserved as a public release fallback
  and should remain available even as local production and experiments move on.
- Protocol v9 full dual oscillator firmware is now the latest stable beta
  option with separate `Save Envelope Only` and `Save Sound Preset` actions.
- Protocol v10 Gnarly is experimental. It keeps the Rad-compatible Web MIDI
  path for now, but changes the physical switch/knob UI and removes Turing from
  the panel.
- Older beta and rollback UF2s have been archived under
  `experimental-firmware/archive/superseded-20260721/active-uf2s/`.
