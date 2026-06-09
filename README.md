# C1ZZL3

Dual phase-distortion synthesiser and probabilistic Turing machine program card for the Music Thing Modular Workshop Computer.

Status: Hardware-tested v1  
Language: C++ / Pico SDK  
Framework: ComputerCard  
Creator: Adrian Vos

## Overview

C1ZZL3 is a CZ-inspired phase-distortion oscillator card with a second mirrored oscillator for detune, ring modulation, and noise modulation. The switch also opens a Turing machine mode, turning the card into a clocked CV and pulse source.

The current v1 build has passed hardware tests for the core oscillator, held-switch performance controls, Turing machine outputs, oscillator sync, restrained CZ-style grit, and manual flash persistence.

## Current Modes

### Switch Middle: PD Synth

Main controls oscillator pitch using the current smooth hardware-test range. This is intentionally playable and continuous, but it is not calibrated 1V/oct yet.

X controls phase-distortion amount. Fully counter-clockwise is closest to a clean sine. Turning X up introduces the selected CZ-style target waveform.

Y morphs across eight CZ-inspired waveform families:

1. Saw
2. Square
3. Needle pulse
4. Double sine
5. Saw/pulse blend
6. Plucked resonant harmonic 5
7. Decaying resonant harmonic 4
8. Decaying resonant harmonic 7

Audio out 1 carries oscillator 1.

Audio out 2 carries oscillator 2. In normal middle position, oscillator 2 is silent until it has been introduced with the held-switch controls.

CV out 1 and CV out 2 continue to carry the scaled Turing machine CV signals, so the sequencer can modulate other modules while the synth voice is playing.

Pulse out 1 carries the main Turing pulse. Pulse out 2 carries an alternate Turing bit pulse.

### Hold Switch Down: Performance Edit

Holding the switch down edits the latched second oscillator and modulation amounts. The normal Main, X, and Y synth controls are held at their last middle-position values while editing, and the held controls must move after the switch is pressed before soft pickup can edit a latched value.

Main controls oscillator 2 detune and level from the centre point. Around 12 o'clock, oscillator 2 is off. Turning left detunes down; turning right detunes up. The further from centre, the louder oscillator 2 becomes.

X controls ring modulation amount. The setting remains active after the switch is released.

Y controls noise modulation amount. The setting remains active after the switch is released.

Oscillator 2 mirrors oscillator 1's phase-distortion shape and selected waveform family.

Holding the switch down from middle synth mode for 8 seconds saves the current latched detune, ring, and noise settings to flash. Saved settings load automatically on startup. There is no autosave.

### Switch Up: Turing Machine

Main controls mutation/lock behaviour. The centre is most random. The extremes become more locked.

X controls sequence length, from 2 to 16 steps.

Y controls internal clock speed.

Pulse in 1 acts as an external clock. When an external clock is present, it takes priority over the internal clock.

Flicking the switch down from Turing mode taps the internal clock tempo.

CV out 1 carries the scaled stepped Turing CV.

CV out 2 carries a smoothed version of the scaled Turing CV.

Pulse out 1 outputs the main Turing pulse. Pulse out 2 outputs an alternate Turing bit pulse.

Audio out 1 and audio out 2 provide simple audio-rate monitor signals from the Turing state in this mode.

## Inputs And Outputs

### Inputs

Audio/CV in 1: pitch modulation in PD synth mode.

Audio/CV in 2: phase-distortion shape modulation in PD synth mode.

CV in 1: waveform morph modulation in PD synth mode.

CV in 2: positive voltage increases ring modulation while holding the switch down; negative voltage increases CZ-style noise modulation while holding the switch down.

Pulse in 1: external Turing clock.

Pulse in 2: oscillator sync in PD synth mode.

### Outputs

Audio out 1: oscillator 1 in PD synth mode; Turing monitor signal in Turing mode.

Audio out 2: oscillator 2 in PD synth mode; Turing monitor signal in Turing mode.

CV out 1: scaled stepped Turing CV.

CV out 2: smoothed scaled Turing CV.

Pulse out 1: Turing pulse.

Pulse out 2: alternate Turing bit pulse.

## LED Feedback

In PD synth mode:

- LED 0 shows phase-distortion amount.
- LED 1 shows waveform morph position.
- LED 2 shows oscillator 2 level.
- LED 3 shows ring modulation amount.
- LED 4 shows noise modulation amount.
- LED 5 lights while the switch is held down.

In Turing mode:

- LEDs 0, 1, and 2 show low bits of the Turing pattern.
- LED 3 follows pulse input 1.
- LED 4 shows sequence length.
- LED 5 follows the Turing pulse.

## Build

This project uses the Raspberry Pi Pico SDK and the ComputerCard header-only framework.

The current `CMakeLists.txt` includes Workshop Computer build settings:

- `PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64`
- USB stdio disabled
- `copy_to_ram` binary type
- warning flags for accidental floating-point promotion
- linker memory usage output

Offline verification build:

```sh
cmake -S . -B build-verify -DPICO_NO_PICOTOOL=1
cmake --build build-verify
```

Normal UF2 output requires `picotool` to be installed or available to the Pico SDK.

## Current Hardware Notes

Confirmed on hardware:

- Oscillator output is now audible and clean.
- Main pitch control works.
- X phase-distortion control works.

Recently tuned:

- Main has been restored to the smoother continuous pitch sweep after the semitone-ratio version proved too steppy on hardware.
- Main pitch smoothing has been increased slightly to reduce remaining knob stepping.
- X has a gentler response curve for more usable low-to-mid settings.
- Y waveform morphing now uses a direct waveform target, with a needle-pulse third position and a plucked sixth position for clearer contrast from the saw.
- Held-switch Y now adds restrained sample-held CZ-style grit by gently jittering phase distortion and oscillator phase instead of crossfading in plain audio noise.
- Turing internal clock range uses a faster curved response for a more useful minimum and middle setting.
- Turing CV outputs are scaled to three-quarter range with a slight upward bias for pitch-friendly modulation depth.
- Ring modulation now uses a stronger internal carrier while keeping some dry signal at the maximum setting to avoid full signal loss.
- Held-switch performance controls now require movement before soft pickup, and Main/X/Y no longer change pitch, PD amount, or waveform while editing detune/ring/noise.
- Manual flash persistence saves latched performance settings after an 8-second held-switch gesture; autosave is intentionally avoided.

## Future Work

- 1V/oct-calibrated pitch response.
- Final CZ-accurate waveform set.
- Envelope preset selection UI. A small triggerable envelope engine is scaffolded in code, but the active preset is currently Off so the hardware-tested v1 behavior is unchanged.
- Flash persistence for envelope preset selection or complex envelope data.
- Release packaging with `info.yaml`, `.uf2`, and final docs.

## Design Notes

C1ZZL3 is intended as a playable, characterful program card rather than an exact Casio CZ clone. The code uses fixed-point integer arithmetic and lookup tables to stay inside the Workshop Computer's 48 kHz audio interrupt budget.

The current implementation favours hardware-tested musical behaviour over completeness. More complex UI layers, such as envelope presets or editing, should be added in small testable steps so the validated v1 performance controls remain dependable.
