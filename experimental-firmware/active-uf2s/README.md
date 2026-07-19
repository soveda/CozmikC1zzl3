# Active C1ZZL3 UF2 Builds

This folder collects the UF2s most likely to be useful during current testing.

## Recommended Choices

| File | Firmware role | Matching web app |
| --- | --- | --- |
| `C1ZZL3_PRODUCTION_1.4.uf2` | Current production firmware. | `web-midi/editor/` and `experiments/cz-import/` |
| `C1ZZL3_STABLE_DUAL_PITCH_PROTOCOL_V3.uf2` | Stable dual-pitch firmware with separate oscillator pitch envelopes. | `experiments/dual-pitch-envelopes/` |
| `C1ZZL3_EXPERIMENT_DUAL_OSCILLATOR_LANES_PROTOCOL_V4.uf2` | Current dual-oscillator-lanes test with optional second PD envelope lane. | Matching web UI still needs to be implemented in `experiments/dual-oscillator-lanes/` |

## Notes

- Production is mirrored here for convenience only. The canonical production
  UF2 remains `uf2/C1ZZL3.uf2`.
- Stable dual pitch remains available as a rollback/reference before testing
  protocol v4.
- Protocol v4 firmware should not replace stable or production unless it passes
  explicit hardware and web app tests.
