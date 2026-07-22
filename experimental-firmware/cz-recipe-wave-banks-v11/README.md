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

## First-Pass Behaviour

This is a first recipe-bank test build, not a production replacement.

- Bank 1: original simple 8-wave family behaviour.
- Bank 2: warmer/rounder compound pairings with double-sine support.
- Bank 3: brighter, more exaggerated resonant/windowed pairings.
- Bank 4: more odd/import-faithful CZ-style pairings intended to make line
  translations easier to compare by ear.

Switch down + Main now selects the active recipe bank. Switch middle + Y selects
oscillator 1 recipe slot inside that bank. Switch up + Y selects oscillator 2
recipe slot inside that bank. Switch up + Main still controls oscillator 2
base interval/spread, so interval/spread remains available.

The bank selector uses widened hardware-friendly zones so all four banks should
be reachable even if the physical Main knob does not quite hit the mathematical
end of its ADC range.

On the switch-down page, LEDs 1 and 2 show the selected bank:

- Bank 1: LEDs 1 and 2 off.
- Bank 2: LED 1 on.
- Bank 3: LED 2 on.
- Bank 4: LEDs 1 and 2 on.

The current Envelope Lab and Import Lab are unchanged. Web MIDI still sends the
existing oscillator wave-family controls to older cards. For v11 recipe-bank
firmware, the Full Dual Osc Lab can read, send, save, and restore the selected
recipe bank alongside the oscillator wave families.

Keep v10 Gnarly available as the fallback while this experiment is tested.

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

The current labelled test UF2 is:

```text
experimental-firmware/cz-recipe-wave-banks-v11/C1ZZL3_CZ_RECIPE_WAVE_BANKS_PROTOCOL_V11.uf2
```

It is also mirrored for active testing at:

```text
experimental-firmware/active-uf2s/C1ZZL3_EXPERIMENT_CZ_RECIPE_WAVE_BANKS_PROTOCOL_V11.uf2
```

## License Notes

This firmware folder includes `computercard.h`, the ComputerCard hardware helper
by Chris Johnson. It is MIT licensed; keep its MIT notice present when copying
this folder into release packages.
