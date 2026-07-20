# Active C1ZZL3 UF2 Builds

This folder collects the UF2s most likely to be useful during current testing.

## Recommended Choices

| File | Firmware role | Matching web app |
| --- | --- | --- |
| `C1ZZL3_WORKSHOP_RELEASE_84_V1.3.uf2` | Public Workshop Computer release 84 / version 1.3, kept as an always-available option. | Workshop release web editor |
| `C1ZZL3_PRODUCTION_1.4.uf2` | Current production firmware. | `web-midi/editor/` and `experiments/cz-import/` |
| `C1ZZL3_STABLE_DUAL_PITCH_PROTOCOL_V3.uf2` | Stable dual-pitch firmware with separate oscillator pitch envelopes. | `experiments/dual-pitch-envelopes/` |
| `C1ZZL3_EXPERIMENT_DUAL_OSCILLATOR_LANES_PROTOCOL_V4.uf2` | Current dual-oscillator-lanes test with optional second PD envelope lane. | `experiments/dual-oscillator-lanes/` |
| `C1ZZL3_ROLLBACK_DUAL_PD_PROTOCOL_V4.uf2` | Tested rollback experimental for dual-PD / dual-pitch protocol v4. | `experiments/dual-oscillator-lanes/` |
| `C1ZZL3_ROLLBACK_DUAL_AMPLITUDE_PROTOCOL_V6.uf2` | Tested rollback experimental for dual-amplitude protocol v6 after passing `.syx` decode, save, and full Amp1/Amp2/PD1/PD2/Pitch1/Pitch2 readback. | `experiments/dual-amplitude-envelopes/` |
| `C1ZZL3_EXPERIMENT_DUAL_AMPLITUDE_ENVELOPES_PROTOCOL_V6.uf2` | Sustain-aware dual-amplitude test with Amp1/Amp2, PD1/PD2, Pitch1/Pitch2, and CZ sustain markers. | `experiments/dual-amplitude-envelopes/` |
| `C1ZZL3_EXPERIMENT_FULL_DUAL_OSCILLATORS_PROTOCOL_V7.uf2` | First full-dual-oscillator test with Amp1/Amp2, PD1/PD2, Pitch1/Pitch2, and separate oscillator wave-family settings through Web MIDI. | `experiments/full-dual-oscillators/` |
| `C1ZZL3_EXPERIMENT_DUAL_AMPLITUDE_ENVELOPES_PROTOCOL_V5.uf2` | Previous dual-amplitude test without explicit sustain marker read/write. | `experiments/dual-amplitude-envelopes/` |

## Notes

- Production is mirrored here for convenience only. The canonical production
  UF2 remains `uf2/C1ZZL3.uf2`.
- Workshop release 84 / version 1.3 is preserved as a public release fallback
  and should remain available even as local production and experiments move on.
- Stable dual pitch remains available as a rollback/reference before testing
  protocol v4.
- Rollback dual PD preserves the fully tested protocol v4 alternate before the
  next dual-amplitude experiment starts.
- Rollback dual amplitude preserves the tested protocol v6 state where CZ
  decoding, saved envelope readback, Amp2, PD2, and dual pitch readback pass.
- Protocol v7 full dual oscillator firmware is a new first-pass experiment. It
  should not replace stable, production, or the tested v6 rollback unless it
  passes explicit hardware and web app tests.
