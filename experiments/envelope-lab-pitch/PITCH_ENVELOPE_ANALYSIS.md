# Pitch Envelope Analysis

Date: 2026-07-17

Input pack:

```text
/Users/adrianvos/Downloads/cz-pack-1
```

## Key Finding

The supplied CZ `.syx` files are 264-byte SysEx frames containing 128 decoded
patch bytes when the nibble-packed payload starts at frame offset `7`.

The current Import Lab prefers offset `9` for CZ-101/CZ-1000-style files. That
is too late for this pack and shifts the decoded patch by one byte. For these
files, offset `7` gives valid CZ section alignment:

- six valid envelope end-step bytes per patch
- complete 128-byte decoded patches
- clean DCO1 and DCO2 pitch-envelope blocks

## Relevant CZ Sections

For the decoded 128-byte patch data, the useful envelope-related sections are:

| Offset | Length | Meaning |
| --- | ---: | --- |
| `20` | 1 | DCA1 envelope end step |
| `21` | 16 | DCA1 amplitude envelope |
| `37` | 1 | DCW1 envelope end step |
| `38` | 16 | DCW1 wave/timbre envelope |
| `54` | 1 | DCO1 pitch envelope end step |
| `55` | 16 | DCO1 pitch envelope |
| `77` | 1 | DCA2 envelope end step |
| `78` | 16 | DCA2 amplitude envelope |
| `94` | 1 | DCW2 envelope end step |
| `95` | 16 | DCW2 wave/timbre envelope |
| `111` | 1 | DCO2 pitch envelope end step |
| `112` | 16 | DCO2 pitch envelope |

Each envelope block is eight rate/level pairs.

## What The Pack Shows

- Every supplied file has non-trivial DCO pitch-envelope data.
- DCO1 and DCO2 pitch envelopes often differ; only `Robo Growl.syx` appears to
  use matching DCO1/DCO2 pitch contours.
- Sustain markers are common in the pitch envelopes.
- End-step values vary across the pack, so any UI needs to show both sustain and
  end positions.
- Pitch movement is frequently large. Several patches appear to use pitch
  envelopes as a core part of the sound rather than as a subtle detail.

## Interpretation For C1ZZL3

The first experimental UI should not merge pitch into amplitude or PD silently.
It should expose pitch as a separate lane or separate editor mode.

Recommended first model:

- add a third lane named `Pitch`
- keep amplitude and PD behaviour unchanged
- show DCO1 and DCO2 source pitch envelopes in Import Lab diagnostics
- initially map DCO1+DCO2 to a single averaged pitch lane for C1ZZL3
- preserve source DCO1/DCO2 pitch data in the draft object for later refinement

Recommended pitch lane range:

- center line: no pitch offset
- upper graph area: positive pitch movement
- lower graph area: negative/return movement if we derive direction from rate
  flags or future verified CZ behaviour
- for a first pass, use level distance from baseline as movement amount and make
  the mapping clearly labelled experimental

## Import Lab Correction Needed

The CZ payload candidate selection should prefer the candidate with the best CZ
section validity score, not a fixed offset. For this pack that means selecting
offset `7`.

Suggested scoring:

- decoded length is exactly 128 bytes
- section end-step bytes at `20`, `37`, `54`, `77`, `94`, `111` are all `0..7`
- envelope blocks decode into plausible rate/level pairs

## Files Analysed

- `Afterxylo copy.syx`
- `Afterxylo.syx`
- `Creature Bass.syx`
- `Digital Noise Organ.syx`
- `Drum Bass.syx`
- `Drunk Trumpet.syx`
- `Fade Pad.syx`
- `Fairy Fly.syx`
- `Flick.syx`
- `Hollow Crystal.syx`
- `Intense Feeling.syx`
- `Laser Puddles.syx`
- `Laser Sus Pad.syx`
- `Noisy Punch Bass.syx`
- `Robo Growl.syx`
- `Saxobass.syx`
- `Snake Pad.syx`
- `Stiff Bass.syx`
- `Toad Bass.syx`
- `Wist Pad.syx`
- `Yay Afterhit.syx`

