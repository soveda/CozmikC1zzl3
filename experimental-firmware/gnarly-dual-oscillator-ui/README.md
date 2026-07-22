# C1ZZL3 Gnarly Dual Oscillator UI Firmware

This folder contains the first v10/Gnarly hardware-UI experiment. It starts
from the protocol v9 Rad full-dual oscillator firmware, but changes the
Workshop Computer switch/knob mapping so the panel exposes both oscillators.

Core and Rad are not changed by this experiment.

## Hardware UI

- Switch up: oscillator 2 relationship/edit page
  - Main: oscillator 2 base interval/spread, centred at unison
  - X: oscillator 2 baseline phase distortion
  - Y: oscillator 2 wave family
- Switch middle: oscillator 1 edit page
  - Main: shared pitch
  - X: oscillator 1 baseline phase distortion
  - Y: oscillator 1 wave family
- Switch down hold: performance and save page
  - Main: oscillator 2 base interval/spread, centred at unison
  - X: ring modulation
  - Y: noise/grit

LED 1 shows the active page's PD amount except on the performance page, LED 2
shows the active page's wave position except on the performance page, LED 3
shows oscillator 2 base interval/spread as a bipolar brightness from centre,
LEDs 4-5 retain ring/noise feedback, and LED 6 shows the current edit page:
off for oscillator 2, medium for oscillator 1, full for performance/save.

## Scope

- Turing mode is removed from the live Gnarly behaviour.
- Turing CV, pulse, and generated MIDI output are forced off in this build.
- Incoming Web MIDI sound-preset/settings Turing fields are ignored and
  settings readback reports Turing off for compatibility.
- CV and pulse outputs are held low during the synth voice in this experiment.
- The Web MIDI protocol remains compatible with the Rad/full-dual web UI.
- The new oscillator 2 baseline PD control is saved in this Gnarly performance
  state, but is not yet exposed as a separate Web MIDI setting.
- Oscillator 2 base interval/spread is also saved as a Gnarly hardware
  performance setting.

## Required Files

This folder is intentionally kept production-package clean. The required files
are:

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
- `C1ZZL3_GNARLY_DUAL_OSCILLATOR_UI_PROTOCOL_V10.uf2`
- `SHA256SUMS.txt`
- `README.md`

Keep v10 here as the current Gnarly fallback while v11 recipe-bank work happens
in `experimental-firmware/cz-recipe-wave-banks-v11/`.

## Build

From the repository root:

```sh
cmake -S experimental-firmware/gnarly-dual-oscillator-ui -B experimental-firmware/gnarly-dual-oscillator-ui/build -DPICO_NO_PICOTOOL=1
cmake --build experimental-firmware/gnarly-dual-oscillator-ui/build -j2
```

The CMake build creates:

```text
experimental-firmware/gnarly-dual-oscillator-ui/build/C1ZZL3_GNARLY_DUAL_OSCILLATOR_UI.uf2
```

## License Notes

This firmware folder includes `computercard.h`, the ComputerCard hardware helper
by Chris Johnson. It is MIT licensed; keep its MIT notice present when copying
this folder into release packages.
