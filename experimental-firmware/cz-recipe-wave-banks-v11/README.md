# C1ZZL3 Gnarly

C1ZZL3 Gnarly is a separate dual-oscillator branch of C1ZZL3 for the Music
Thing Modular Workshop Computer. It trades the Turing machine panel mode for a
more direct synthesiser interface with two oscillator lanes, recipe wave banks,
Web MIDI sound presets, and CZ-style import support.

This is not the same card experience as C1ZZL3 Core or C1ZZL3 Rad:

- Core is the simpler production C1ZZL3 with Turing machine mode.
- Rad is the advanced full-dual Web MIDI version that keeps Turing mode.
- Gnarly is the no-Turing dual-oscillator version with the physical panel
  focused on oscillator editing and performance tone controls.

## Current Status

This folder contains the stable protocol v11 Gnarly firmware. It has passed the
current hardware test set and now sits alongside Core and Rad rather than
replacing either of them.

## What It Does

- Runs two phase-distortion oscillator lanes.
- Uses separate Amp1/Amp2, PD1/PD2, and Pitch1/Pitch2 envelopes.
- Supports named sound presets saved on the card.
- Saves either envelope-only data or full sound presets with performance
  settings.
- Uses four recipe wave banks for simpler and more CZ-like oscillator pairings.
- Reads and writes settings through the Full Dual Oscillator Web MIDI Lab.
- Imports Casio CZ `.syx` patches through the matching Import Lab flow.
- Removes the Turing machine CV, pulse, and MIDI-output behaviour from this
  branch.

## Hardware Controls

### Switch Middle: Oscillator 1

- Main: shared pitch.
- X: oscillator 1 phase distortion.
- Y: oscillator 1 recipe slot in the selected bank.

### Switch Up: Oscillator 2

- Main: oscillator 2 base interval/spread, centred at unison.
- X: oscillator 2 phase distortion.
- Y: oscillator 2 recipe slot in the selected bank.

### Switch Down Hold: Performance And Bank

- Main: recipe bank.
- X: ring modulation.
- Y: noise/grit.

The card keeps the hold-to-save gesture from the C1ZZL3 family, but the switch
pages are now synth-performance pages rather than synth/Turing pages.

## Recipe Banks

- Bank 1: simple single-wave families.
- Bank 2: warmer compound pairings with double-sine support.
- Bank 3: brighter resonant/windowed pairings.
- Bank 4: odd/import-faithful CZ-style pairings for translated patches.

The bank selector uses widened hardware-friendly zones so all four banks should
be reachable even if the physical Main knob does not quite hit the mathematical
end of its ADC range.

## LEDs

On the switch-down page, LEDs 1 and 2 show the selected recipe bank:

- Bank 1: LEDs 1 and 2 off.
- Bank 2: LED 1 on.
- Bank 3: LED 2 on.
- Bank 4: LEDs 1 and 2 on.

LED 3 shows oscillator 2 interval/spread as a bipolar brightness from centre.
LEDs 4 and 5 show ring modulation and noise/grit. LED 6 shows the active edit
page: off, medium, or full brightness.

## MIDI CC Controller Map

Gnarly v11 uses `CC20` to `CC27` as an eight-knob performance block. `CC1` is
also kept as oscillator 1 phase distortion so a mod wheel remains useful.

- `CC1`: oscillator 1 phase distortion, mod-wheel friendly.
- `CC20`: oscillator 1 recipe slot.
- `CC21`: oscillator 2 recipe slot.
- `CC22`: ring modulation amount.
- `CC23`: recipe bank.
- `CC24`: oscillator 2 interval/spread.
- `CC25`: oscillator 2 phase distortion.
- `CC26`: noise/grit amount.
- `CC27`: oscillator 1 phase distortion, for eight-knob controllers.

The Full Dual Oscillator Lab has a hidden Developer-mode MIDI CC Test Suite for
checking these messages without a hardware controller. It sends individual CC
values, a neutral reset, and sweep tests through the selected Web MIDI output.

## Web MIDI Editor

Use the Full Dual Oscillator Lab:

```text
https://soveda.github.io/CozmikC1zzl3/experiments/full-dual-oscillators/
```

The lab detects Gnarly protocol v11 and exposes the recipe-bank setting. It also
keeps compatibility paths for Core and Rad cards, so this editor can remain the
shared advanced editor while the firmware branches diverge.

Use the matching Import Lab for CZ `.syx` translation:

```text
https://soveda.github.io/CozmikC1zzl3/experiments/full-dual-oscillators/import-lab/
```

## UF2

Current labelled stable UF2:

```text
experimental-firmware/cz-recipe-wave-banks-v11/C1ZZL3_CZ_RECIPE_WAVE_BANKS_PROTOCOL_V11.uf2
```

Active stable copy:

```text
experimental-firmware/active-uf2s/C1ZZL3_GNARLY.uf2
```

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

## Release Packaging Notes

If this branches into a separate public card release, package it with:

- `C1ZZL3.cpp`
- `C1ZZL3_LUT.cpp`
- `C1ZZL3_LUT.h`
- `CMakeLists.txt`
- `computercard.h`
- `tusb_config.h`
- `usb_descriptors.c`
- `usb_midi_host.c`
- `usb_midi_host.h`
- `usb_midi_host_app_driver.c`
- `C1ZZL3_CZ_RECIPE_WAVE_BANKS_PROTOCOL_V11.uf2`
- `README.md`
- `info.yaml`

## License Notes

This firmware folder includes `computercard.h`, the ComputerCard hardware
helper by Chris Johnson. It is MIT licensed; keep its MIT notice present when
copying this folder into release packages.
