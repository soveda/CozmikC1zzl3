# CZ To C1ZZL3 Import Spec

## Summary

The importer should accept a Casio CZ single-patch SysEx file and translate it
into a new editable C1ZZL3 custom preset inside the web editor.

The import should happen in the browser, not in firmware.

## User Story

1. User opens a dedicated CZ import page.
2. User chooses `Import CZ Patch`.
3. User selects a `.syx` file.
4. The browser validates that the file looks like a supported Casio CZ patch.
5. The browser decodes the patch into readable parameters.
6. The browser maps those parameters into a draft C1ZZL3 preset.
7. The browser shows:
   - imported patch name if available
   - mapped waveform region
   - mapped amplitude envelope
   - mapped phase-distortion envelope
   - warnings for unsupported or approximate values
8. User edits the result if needed.
9. User saves the translated preset to a custom browser slot or sends it to the
   card.

## Scope For First Experiment

### In Scope

- single-patch Casio CZ SysEx files
- browser-side file import
- patch validation
- translation into a custom editor preset
- warning banner for approximate mapping

### Out Of Scope

- direct card-side CZ patch loading
- firmware support for native CZ SysEx
- exact emulation of CZ architecture
- bank import for the first pass
- automatic audio matching

## Proposed UI

Do not place this feature directly inside the Envelope Lab page.

Use a separate page, for example:

- `CZ Import Lab`

Reasons:

- avoids mixing two different mental models
- keeps envelope editing simple
- makes it easier to explain translation warnings
- avoids users assuming CZ patches load natively into the card

The Envelope Lab page can link to the import page with a simple experimental
link or button, but the import workflow should remain separate.

## Proposed UI For The Import Page

Add a clear top-level action:

- `Import CZ Patch`

Recommended flow after file selection:

- modal or side panel summary
- imported patch info
- mapping confidence
- warning list
- `Create Draft Preset`
- `Open In Envelope Lab`

## Validation Rules

The importer should reject files that do not look like supported CZ single
patches.

Validation checks:

- file contains at least one SysEx frame
- frame starts with Casio manufacturer ID `F0 44`
- payload shape matches expected single-patch format
- nibble-packed payload length matches expected patch size

If validation fails, the user should get a plain-language message such as:

- `This file does not look like a supported Casio CZ single-patch SysEx file.`

## Import Output

The importer should generate:

- preset name:
  - file name fallback if no patch name is recoverable
- amplitude envelope:
  - derived from CZ loudness envelope if available
- phase-distortion envelope:
  - derived from CZ timbre/DCW envelope if available
- waveform starting point:
  - chosen from a best-fit mapping into the C1ZZL3 wave morph range
- optional performance defaults:
  - ring off
  - noise off
  - detune neutral

After creating the draft preset, the user can be sent into Envelope Lab to make
final edits and save to the card.

## Confidence Model

The importer should label mappings by confidence:

- `High`: straightforward amplitude envelope transfer
- `Medium`: waveform family approximation
- `Low`: CZ-specific features that do not map cleanly

This matters because users should understand the difference between:

- imported exactly enough to trust
- imported as a starting point for editing

## Suggested Implementation Order

1. parse SysEx container
2. detect supported Casio CZ single-patch frame
3. unpack nibbles into parameter bytes
4. expose decoded fields in a debug object
5. map waveform family
6. map amplitude envelope
7. map DCW envelope into C1ZZL3 PD envelope
8. create draft preset in the editor
9. add warnings for unsupported features

## Engineering Recommendation

Keep the importer entirely in the web app.

Reasons:

- easier to iterate
- easier to explain warnings
- no firmware risk
- users can review the translated preset before writing to the card

## Success Criteria

The first experiment is successful if:

- a user can import a CZ patch file
- the editor creates a usable draft preset
- the result sounds recognisably related
- the user can refine and save it without touching firmware code
