# C1ZZL3 Rollback Experimental: Dual PD Protocol V4

This folder preserves the tested dual-PD / dual-pitch protocol v4 firmware as
the rollback experimental baseline before starting the next dual-amplitude
experiment.

This is not production and does not replace the main base firmware.

## Matching Web UI

Use:

```text
https://soveda.github.io/CozmikC1zzl3/experiments/dual-oscillator-lanes/
```

## Rollback UF2

```text
C1ZZL3_ROLLBACK_DUAL_PD_PROTOCOL_V4.uf2
```

SHA256:

```text
2bd255663fbf7703fcb01c520517f791402f8d06a6bcd4296cf07819c1571c5c
```

## Confirmed Test Coverage

- Boot.
- Web MIDI connection.
- Settings send/read.
- RAM dual-PD load.
- Save/readback dual-PD.
- Saved playback.
- Power-cycle persistence.
- Hardware selection after power-cycle.
- Save overwrite.
- Multi-slot sanity.
- Temporary vs saved separation.
- Delete after temporary load.
- v3 migration.
- Pitch envelope readback.
- MIDI loop safety.
- Legacy send compatibility.
- Stress/torture.
- Audio regression.
- All 8 custom slots.

## Behaviour Notes

- Protocol v4 uses separate PD1, PD2, pitch1, and pitch2 lanes.
- Saved-envelope readback uses small chained responses for reliability.
- RAM-loaded envelopes remain playable immediately but are not reported as saved
  card slots unless `Save Envelope` was used.
- A separate saved-envelope copy is kept for readback, so temporarily loading a
  different RAM envelope does not hide or overwrite the envelope saved in flash.
- Older protocol v3 saved custom-envelope data is migrated at startup by
  preserving amp, PD1, pitch1, and pitch2, with PD2 inferred from PD1.

