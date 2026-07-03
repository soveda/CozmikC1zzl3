# Waveform Tuning

This project currently renders the eight phase-distortion target waves from the
lookup table in [C1ZZL3_LUT.cpp](/Users/adrianvos/coding/GitHub/CozmikC1zzl3/C1ZZL3_LUT.cpp).

For experimental waveform work, do not tune the giant table by hand.

## Recommended Branch

Use the existing experimental line as the starting point:

```text
origin/codex/overclock-stability-experiment
```

If you want a local branch, create one from that remote branch, for example:

```sh
git checkout -b experimental origin/codex/overclock-stability-experiment
```

## Source Of Truth

Use [generate_pd_wave_lut.py](/Users/adrianvos/coding/GitHub/CozmikC1zzl3/scripts/generate_pd_wave_lut.py)
as the editable source of truth for waveform tuning.

The script rebuilds the `pdWaveLUT` block from readable named wave functions:

- saw
- square
- narrow pulse
- double sine
- saw pulse
- resonant saw window
- resonant triangle window
- resonant trapezoid window

## Tuning Workflow

1. Edit the waveform formulas in:
   [generate_pd_wave_lut.py](/Users/adrianvos/coding/GitHub/CozmikC1zzl3/scripts/generate_pd_wave_lut.py)
2. Regenerate the lookup table:

```sh
python3 scripts/generate_pd_wave_lut.py --write
```

3. Review the diff in:
   [C1ZZL3_LUT.cpp](/Users/adrianvos/coding/GitHub/CozmikC1zzl3/C1ZZL3_LUT.cpp)
4. Build and audition the experimental firmware.

## Why This Helps

- The current firmware reads directly from `pdWaveLUT`.
- The old direct formulas are much easier to reason about than the expanded
  table.
- Regeneration keeps the runtime code fast while making experimental tuning
  practical.

## Important Note

This environment could not switch branches directly because writing `.git` is
blocked here, so if you want these changes on the experimental branch you may
need to copy or cherry-pick them from the current workspace into that branch in
your normal Git environment.
