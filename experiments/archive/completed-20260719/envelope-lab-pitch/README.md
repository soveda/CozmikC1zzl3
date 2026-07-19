# C1ZZL3 Pitch Envelope Lab

Experimental copy of the production Envelope Lab for pitch-envelope research.

Production files in `web-midi/editor/` should not be edited for this work until
the pitch-envelope model has been tested and promoted deliberately.

## Current Purpose

- Inspect Casio CZ DCO pitch envelopes from `.syx` files.
- Work out a useful C1ZZL3 pitch-envelope UI without disrupting the stable
  amplitude/phase-distortion editor.
- Prototype import and handoff behaviour before any firmware protocol changes.

## Current Editor Behaviour

- The main graph still shows amplitude and phase distortion.
- A separate pitch-envelope graph now sits directly below the amplitude/PD
  graph so all envelope lanes are visible together.
- The pitch lane has its own editable stage table.
- Browser audition uses the pitch lane as a temporary pitch sweep preview.
- Current card SysEx remains amplitude/PD-only; pitch data is not sent to
  hardware yet.

## Local Files

- `index.html`: experimental lab entry point.
- `app.js`: copied editor application code for isolated experiments.
- `styles.css`: copied editor styling for isolated experiments.
- `import-lab/`: experimental CZ Import Lab with pitch-envelope decoding.
- `analyze-cz-pitch.mjs`: repeatable pitch-envelope inspection script.
- `PITCH_ENVELOPE_ANALYSIS.md`: current findings from the supplied CZ pack.

## Experimental Import Lab

The Import Lab in `import-lab/` is separate from the current Import Lab at
`experiments/cz-import/`.

It currently:

- scores possible CZ payload offsets using known envelope-section markers
- selects offset `7` for the supplied 264-byte CZ pack files
- decodes DCA, DCW, DCO1 pitch, and DCO2 pitch envelope sections
- displays DCO1 and DCO2 pitch envelopes separately
- preserves CZ source envelopes in the handoff draft for later editor work
