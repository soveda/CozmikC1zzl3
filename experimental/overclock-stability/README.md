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
- Current synth mode does not run the Turing clock in the background.
- Current synth mode only holds the last Turing CV and pulse values.
- Flash writes for custom envelopes still stop ADC/DMA via the framework flash
  safety path, so overclocking should not be expected to make flash operations
  invisible.

The main cost risk is therefore per-sample DSP and control work in
`ProcessSample()`, not the Web MIDI editor itself.

## Features Removed Or Avoided For Stability

These are the known removals or deliberate omissions from the production build:

1. Synth-mode Turing clock persistence
   - Removed because clock-persistence builds locked up at maximum settings.
   - Reduced noise depth and MIDI setting clamps did not make that branch
     reliable.
   - Current behaviour: CV and pulse outputs hold the last Turing values after
     leaving Turing mode.

2. Turing MIDI output
   - Deliberately not included.
   - It would add cross-core MIDI/event work and more state to maintain for a
     feature that is not essential to the card.

3. Tap tempo
   - Removed from the current production build.
   - The retained rollback UF2 is archived at:
     `uf2/archive/previous-versions-20260615/C1ZZL3_with_tap_tempo_rollback_20260615.uf2`
   - Current behaviour: Y is the only internal Turing clock-speed control.

4. Audio In 2 utility
   - Currently unused in synth mode.
   - This was kept out to reduce routing complexity and preserve the tested
     control scheme.

5. Background Turing CV updates in synth mode
   - Not present in production.
   - This is closely related to synth-mode Turing clock persistence.

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
C1ZZL3_EXPERIMENTAL_OVERCLOCK_KHZ=150000
```

This requests a conservative 150 MHz system clock before core 1 USB/MIDI is
launched and before the ComputerCard audio framework starts.

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

If Stage 0 passes, reintroduce only the background Turing clock/CV update while
leaving Turing MIDI output and tap tempo absent.

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

### Stage 2: Re-test Tap Tempo Only

If Stage 1 passes, test tap tempo separately. Keep Turing MIDI output absent.

Tap tempo is lower value than clock persistence but may be acceptable if it does
not add meaningful load.

### Stage 3: Avoid Turing MIDI Output Unless Strongly Needed

Turing MIDI output should remain out unless there is a clear musical need. It
adds more cross-core/event work and another class of timing failure for less
benefit than CV/pulse persistence.

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
C1ZZL3_overclock_150mhz_no_feature_changes_experimental.uf2
```

Only after that passes should feature reintroduction begin.

## Open Questions Before Coding

- What exact overclock target should be tested first?
- Does the Workshop Computer card format have a known safe system clock used by
  other cards?
- Should the first test use conservative 150 MHz rather than a larger jump?
- Do we want an LED startup indication for overclocked experimental builds?
- Should overclocked builds refuse flash writes during active MIDI playback, or
  is the current flash safety path sufficient?

## Branch Note

The intended branch name for this work is:

```text
codex/overclock-stability-experiment
```

This environment currently cannot create Git branches because `.git` is
read-only to the sandbox. Create the branch locally with:

```sh
git switch -c codex/overclock-stability-experiment
```

Then keep any overclock code and UF2s on that branch until hardware testing
passes.
