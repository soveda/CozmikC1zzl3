# C1ZZL3 CZ Recipe Wave Banks V11 Experiment

This folder is the starting point for the v11/Gnarly recipe-bank experiment.
It is forked from the passing v10 Gnarly hardware-UI firmware so v10 can remain
the current fallback while recipe-bank work happens separately.

The current Envelope Lab and Import Lab are not changed by this folder.

## Experiment Goal

Explore a more CZ-faithful wave selection model without adding hardware
controls:

- Switch down + Main: choose the active recipe bank.
- Switch middle + Y: choose oscillator 1 recipe within that bank.
- Switch up + Y: choose oscillator 2 recipe within that bank.
- Switch down + X/Y: keep ring and noise/grit.

The intended recipe-bank model is:

- Simple CZ-style single families.
- Compound CZ-style pairings.
- Resonant/windowed pairings.
- Import-faithful pairings for decoded CZ patches.

## Current Status

This is currently a clean v11 development fork, not a tested v11 release.
The source starts from v10 Gnarly behaviour and the CMake target has been
renamed so future builds are clearly labelled.

Do not replace Rad, Gnarly v10, Core, or the Workshop release with this
experiment until the recipe-bank behaviour has been implemented and tested.

## Build

From the repository root:

```sh
cmake -S experimental-firmware/cz-recipe-wave-banks-v11 -B experimental-firmware/cz-recipe-wave-banks-v11/build -DPICO_NO_PICOTOOL=1
cmake --build experimental-firmware/cz-recipe-wave-banks-v11/build -j2
```

The CMake build creates:

```text
experimental-firmware/cz-recipe-wave-banks-v11/build/C1ZZL3_CZ_RECIPE_WAVE_BANKS_V11.uf2
```

