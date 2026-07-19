# C1ZZL3 Dual Oscillator Lanes Experiment

This folder is reserved for the next web UI experiment after the stable
dual-pitch Envelope Lab.

The stable dual-pitch reference remains in:

```text
experiments/dual-pitch-envelopes
```

Do not move or overwrite that reference while developing this experiment.

## Intended Sequence

1. Add two-lane PD envelope handling.
2. Keep amplitude shared until the two-lane PD behaviour is understood.
3. Test import choices for DCW1/DCW2:
   - merged
   - line 1 only
   - line 2 only
   - dual lane
4. If two-lane PD is useful, expand toward full two-lane oscillator behaviour.

## Full Two-Lane Behaviour To Consider Later

- oscillator 1 pitch envelope
- oscillator 2 pitch envelope
- oscillator 1 PD envelope
- oscillator 2 PD envelope
- possible oscillator-specific amplitude trim
- possible oscillator-specific wave family
- shared final amplitude envelope
- ring/noise position in the signal path

## Compatibility Goal

This experiment should be designed so the current stable dual-pitch web app and
firmware remain available as a rollback and comparison target.
