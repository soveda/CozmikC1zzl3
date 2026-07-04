# CZ To C1ZZL3 Mapping Table

This table is a sketch for approximate translation, not a claim of exact
compatibility.

## Direct Or Near-Direct Candidates

| CZ Area | C1ZZL3 Target | Notes |
| --- | --- | --- |
| Patch file | Draft custom preset | Browser-side only |
| DCA envelope | Amplitude envelope | Best direct candidate |
| DCW envelope | Phase-distortion envelope | Approximation |
| Basic waveform family | 8-wave family selector / CC23 region | Approximation |
| Brightness / timbre trend | PD starting level | Approximation |

## Harder Or Approximate Areas

| CZ Area | C1ZZL3 Target | Notes |
| --- | --- | --- |
| DCW behaviour | Wave morph plus PD envelope | Different synthesis model |
| Resonant waveform behaviour | Higher-wave region on C1ZZL3 | Ear-matched, not exact |
| Dual oscillator structure | Maybe detune/ring/noise hints | Very approximate |
| Pitch envelope | Optional amplitude/PD influence or ignore | Needs experiment |
| LFO / modulation | Ignore in first pass | Better left manual |

## Likely First-Pass Waveform Mapping

| CZ Character | Suggested C1ZZL3 Region |
| --- | --- |
| Saw-like | Saw |
| Square-like | Square |
| Pulse-like | Narrow pulse or Saw pulse |
| Double-sine or smoother harmonic shapes | Double sine |
| Resonant or brighter CZ shapes | Resonant saw / triangle / trapezoid windows |

## Envelope Mapping Strategy

### DCA To Amplitude

- Convert CZ amplitude stages into 8 C1ZZL3 amplitude stages.
- Preserve:
  - attack trend
  - decay trend
  - sustain feel
  - release feel

### DCW To Phase Distortion

- Convert CZ timbre evolution into 8 C1ZZL3 PD stages.
- Preserve:
  - opening or closing brightness
  - overall contour direction
  - strong peaks or drops where possible

## Recommended Defaults For Unsupported Features

Use these when a CZ feature has no clean match:

- detune: neutral
- ring: off
- noise: off
- Turing settings: unchanged

## Warnings To Show Users

The importer should tell the user when:

- waveform behaviour is only approximate
- modulation settings were ignored
- pitch envelope was ignored or simplified
- dual-oscillator structure could not be matched
- envelope timing had to be compressed into the C1ZZL3 stage model

## Suggested Debug Output

For the experimental version, show:

- raw decoded CZ parameter bytes
- interpreted envelope points
- selected waveform family
- chosen C1ZZL3 mapping values
- unsupported feature list

This should stay behind a developer or experimental toggle until the mapping is
proven useful.
