# C1ZZL3 Import Lab Experiment

This folder contains the user-facing C1ZZL3 Import Lab work for decoding Casio
CZ `.syx` patches into approximate C1ZZL3 draft presets.

User-facing page:

- Hosted: `https://soveda.github.io/CozmikC1zzl3/experiments/cz-import/`
- Local: `http://localhost:5174` when served from `experiments/cz-import`

This is not a direct SysEx passthrough feature. It is a browser translation
layer with a handoff into Envelope Lab.

## Goal

Allow a user to:

1. choose a Casio CZ patch file
2. decode the patch in the browser
3. map the parts that make sense onto C1ZZL3 controls
4. hand the draft into Envelope Lab
5. review and send it from there

This stays on a separate page from Envelope Lab so users do not confuse:

- envelope editing
- CZ patch import
- translated patch review

## Why Translation Is Needed

Casio CZ patches target a different synth architecture.

C1ZZL3 has:

- one waveform morph control
- one phase-distortion control
- custom amplitude envelope
- custom phase-distortion envelope
- detune, ring, and noise controls

Casio CZ patches include things like:

- CZ waveform selection and combinations
- DCW behaviour that does not map 1:1 to C1ZZL3
- DCA and DCW envelopes
- pitch envelope
- modulation settings
- tone structure choices that may not match C1ZZL3 directly

So the import result must be presented as:

- a best-effort approximation
- reviewable in Envelope Lab before sending
- not a guaranteed faithful clone

It is intentionally presented as a dedicated import workflow, not mixed into
the main envelope editor page.

## Current User-Facing Features

- `C1ZZL3 Import Lab` title and guided hero section.
- Light and dark mode toggle matching Envelope Lab.
- Drag-and-drop or file-picker import.
- Validation, summary, decoded readout, warnings, and draft mapping panels.
- CZ frame awareness for common `F0 44 00 00 7n 20 ... F7` patch-send
  SysEx files, including command, location, channel, and selected data offset.
- `Open In Envelope Lab` handoff for final editing and card send.

## Decoder Accuracy Notes

The importer now checks for the common Casio CZ family header and patch-send
command before creating a draft. It also reports the data offset it selected,
because CZ files can include command/location bytes before the nibble-packed
patch data.

This is still a draft translator rather than a verified CZ librarian. The next
accuracy step is to compare decoded named parameters against known-good CZ
software such as Patch Base, CZSYSEXY, Arturia CZ V, or other reference editors.

## Editor Handoff

The import page opens the main Envelope Lab editor in a new tab after draft
handoff.

- Local editor route: `../../web-midi/editor/index.html`
- Hosted editor route: `https://soveda.github.io/CozmikC1zzl3/web-midi/editor/index.html`

The import page chooses the hosted link when it is served from GitHub Pages.

## Files

- [IMPORT_SPEC.md](/Users/adrianvos/coding/GitHub/CozmikC1zzl3/experiments/cz-import/IMPORT_SPEC.md):
  import workflow and user-facing design
- [MAPPING_TABLE.md](/Users/adrianvos/coding/GitHub/CozmikC1zzl3/experiments/cz-import/MAPPING_TABLE.md):
  parameter mapping ideas and unsupported areas
