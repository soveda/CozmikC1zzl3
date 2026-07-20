# C1ZZL3 Web Experiments

This folder contains browser tools that sit outside the main production
Envelope Lab.

## Active Web Apps

- `cz-import/`: current production C1ZZL3 Import Lab for translating Casio CZ
  `.syx` patches into the main Envelope Lab.
- `dco-pitch-handling/`: beta import test for choosing merged, DCO1-only, or
  DCO2-only pitch mapping while still using the production firmware.
- `dual-pitch-envelopes/`: stable dual-pitch web app for the protocol v3
  firmware.
- `dual-oscillator-lanes/`: protocol v4 experiment for two-lane PD envelopes
  plus the existing two pitch lanes. Use it with the matching dual-oscillator
  experimental UF2.
- `dual-amplitude-envelopes/`: protocol v6 experiment for separate Amp1/Amp2
  envelopes plus PD1/PD2, Pitch1/Pitch2, and CZ-style sustain markers. Use it
  only with the matching dual-amplitude experimental UF2.
- `full-dual-oscillators/`: protocol v7 first-pass experiment for separate
  oscillator wave-family settings on top of the tested dual-amplitude lanes.
  Use it only with the matching full-dual-oscillator experimental UF2.

## Archived Web Experiments

Completed or superseded web experiments are kept at:

```text
experiments/archive/completed-20260719/
```

They remain available for reference, but should not be the first choice for
current testing.
