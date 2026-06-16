# C1ZZL3 Overclock Stability Review

This is an experimental review area for looking at whether RP2040 overclocking
could safely buy enough headroom to re-test features removed from the production
C1ZZL3 build for stability.

The production firmware is not changed by this note.

## Current Production Baseline

Current promoted UF2:

```text
uf2/C1ZZL3.uf2
```

Matching retained UF2:

```text
uf2/C1ZZL3_web_midi_delete_slots_production_20260615.uf2
```

Checksum:

```text
22aaf7673e8169bb6f34d0fc2450f0ecccf82857278e0c428083162e854415dd
```

Hardware testing has confirmed this no-tap-tempo build passes normal and torture
tests, including custom envelope slot deletion.

## Current Processing Model

The ComputerCard framework runs audio at a fixed 48 kHz audio rate.

- `ComputerCard::BufferFull()` runs from the ADC/DMA interrupt path.
- `C1ZZL3::ProcessSample()` is called once per audio sample.
- USB MIDI device/host work runs on core 1 in `usbMidiWorker()`.
- This experimental branch runs the Turing clock in synth mode.
- Synth mode still does not render the Turing audio voice.
- Flash writes for custom envelopes still stop ADC/DMA via the framework flash
  safety path, so overclocking should not be expected to make flash operations
  invisible.

The main cost risk is therefore per-sample DSP and control work in
`ProcessSample()`, not the Web MIDI editor itself.

## Current Optimisation Experiments

The branch now includes lookup tables for the eight PD target waveforms and
restores synth-mode Turing clock persistence.

Previous behaviour:

- `morphWave()` selected two adjacent target waves.
- `czWave()` generated each target in the audio path.
- The more complex waves called sine harmonics and envelope/window maths for
  every oscillator sample.

Experimental behaviour:

- `pdWaveLUT[8][4096]` stores all eight 12-bit phase target waves.
- `czWave()` now reads `pdWaveLUT[wave][phase12]`.
- The table was generated from the existing integer waveform formulas and the
  existing `sineLUT`.
- Oscillator 2 is output at full level; the detune amount is still retained for
  LED/ring behaviour.
- Web MIDI performance settings can scale Turing CV output range from 1 to 8
  octaves, defaulting to 2 octaves.
- MIDI CC20 on the selected input channel controls the same Turing CV octave
  range live.
- The synth output now has a fixed first-order output filter: 40 Hz high-pass
  followed by 7 kHz low-pass, both 6 dB/octave.
- Turing MIDI output is enabled as an experimental device-mode note stream.
  Each Turing step sends a CV-derived note on the selected MIDI channel. The
  note is turned off when the Turing pulse window ends, or immediately before
  the next note if the clock is faster than that window.

Expected tradeoff:

- More flash use: about 64 KB extra table data.
- Less per-sample CPU work: especially for the resonant/windowed PD target
  waves, and because the target waveform is rendered for both oscillators.
- No intentional feature change.

### Synth-Mode Turing Clock Persistence

The branch now updates the Turing clock, CV, and pulse state in synth mode. This
means `CV Out 1`, `CV Out 2`, `Pulse Out 1`, and `Pulse Out 2` continue changing
after switching from Turing mode back to synth mode.

To keep the experiment focused:

- the Turing audio voice is not rendered in synth mode;
- no extra tap-tempo behaviour is added;
- synth-mode Turing clocking does not trigger the synth envelope.
- Turing MIDI output follows the Turing step stream in USB device mode.

## Features Removed Or Avoided For Stability

These are the known removals or deliberate omissions from the production build:

1. Synth-mode Turing clock persistence
   - Previously removed because clock-persistence builds locked up at maximum
     settings.
   - Reduced noise depth and MIDI setting clamps did not make that branch
     reliable.
   - Reintroduced on this branch for overclocked testing.

2. Turing MIDI output
   - Reintroduced on this branch as a device-mode note stream for testing.
   - It remains absent from production until hardware and DAW testing confirm
     that the extra cross-core event work is stable.

3. Audio In 2 utility
   - Currently unused in synth mode.
   - This was kept out to reduce routing complexity and preserve the tested
     control scheme.

## Overclocking Relevance

RP2040 overclocking can increase CPU cycles available between 48 kHz audio
samples. In this firmware, that could help only if the lockups are caused by
audio ISR work exceeding the available time budget.

Overclocking is less likely to solve problems caused by:

- flash erase/program stalls;
- USB host/device timing edge cases;
- memory corruption;
- unsafe shared state between core 0 audio and core 1 USB MIDI;
- excessive work inside interrupt context other than raw CPU arithmetic;
- unstable analogue or power behaviour at higher clock rates.

Because the card has separate power and USB role selection, USB stability should
be tested separately in both computer/device mode and host/controller mode after
any clock change.

## Suggested Experimental Order

Change only one variable at a time.

### Stage 0: Overclock Only

Build a firmware that changes only the system clock. Do not reintroduce any
removed features.

Initial branch setting:

```text
C1ZZL3_EXPERIMENTAL_OVERCLOCK_KHZ=192000
```

This requests a 192 MHz system clock before core 1 USB/MIDI is launched and
before the ComputerCard audio framework starts.

Clock speeds should be chosen as multiples of 48 MHz where possible. The
ComputerCard audio path is built around 48 kHz audio and an ADC clock derived
from 48 MHz, so 192 MHz keeps that clean clock relationship while adding one
more 48 MHz step beyond the tested 144 MHz build.

Test:

- normal synth use;
- max X/Y/ring/noise/detune;
- MIDI note triggering;
- Pulse 2 envelope triggering;
- Web MIDI Load, Save, Delete, and Set;
- USB MIDI device mode from DAW/browser;
- USB MIDI host mode from Keystep/SMK-25;
- Turing mode internal and external clocking.

If this fails, do not continue with feature reintroduction.

### Stage 1: Re-test Synth-Mode Turing Clock Persistence

This is now implemented on the branch. It reintroduces the background Turing
clock/CV/pulse update and, in this build, sends a matching USB MIDI note stream
in computer/device mode.

This should be the first meaningful overclock experiment because it is the most
musically useful removed behaviour and it directly targets the known stability
boundary.

Test:

- switch from Turing to synth and confirm CV/pulse outputs continue changing;
- max X/Y/ring/noise/detune while clock continues;
- fast internal Turing clock;
- external Pulse In 1 clock;
- MIDI notes and Pulse 2 envelope triggers while clock continues;
- Web MIDI ring/noise Set while clock continues.

### Stage 2: Turing MIDI Output Trial

Turing MIDI output is present in this branch as a deliberately small trial. The
audio core queues only note-on/off requests; core 1 sends the USB MIDI bytes in
computer/device mode.

## Oscillator 2 Level Note

Oscillator 2 full-level detune was considered for this experiment. It is mostly
a gain/headroom and patching decision rather than a CPU optimisation: oscillator
2 is already rendered in the synth path, and the current level multiply is
cheap. Keeping oscillator 2 level unchanged avoids adding another variable to
this hardware test.

## Recommended Implementation Shape

Do not make overclocking unconditional in production.

Preferred experimental mechanism:

```cpp
#if defined(C1ZZL3_EXPERIMENTAL_OVERCLOCK_KHZ) && C1ZZL3_EXPERIMENTAL_OVERCLOCK_KHZ
    set_sys_clock_khz(C1ZZL3_EXPERIMENTAL_OVERCLOCK_KHZ, true);
#endif
```

Place it at the start of `main()` before `multicore_launch_core1()` and
`card.Run()`, after including the relevant Pico SDK clock header if required.

Build as an explicitly named UF2, for example:

```text
C1ZZL3_overclock_192mhz_turing_clock_persistence_experimental.uf2
```

Only after the overclock-only test passes should the Turing clock persistence
test be treated as meaningful.

## Open Questions Before Coding

- Does this 192 MHz build remain stable in both USB device mode and USB host
  mode?
- Do we want an LED startup indication for overclocked experimental builds?
- Should overclocked builds refuse flash writes during active MIDI playback, or
  is the current flash safety path sufficient?

## Branch Note

This work is currently isolated on:

```text
codex/overclock-stability-experiment
```

Keep overclock code and UF2s on this branch until hardware testing passes.
