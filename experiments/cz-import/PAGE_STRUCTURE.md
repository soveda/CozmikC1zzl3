# C1ZZL3 Import Lab Page Structure

This page should be separate from Envelope Lab.

## Page Goal

Help the user import a Casio CZ `.syx` patch, translate it into a C1ZZL3
draft preset, review the result, and then hand it off to Envelope Lab for final
editing.

## Top-Level Layout

```text
------------------------------------------------------
C1ZZL3 Import Lab                                     |
------------------------------------------------------
| Hero header / page purpose / theme toggle          |
------------------------------------------------------
| File import panel | Patch summary panel            |
------------------------------------------------------
| Decoded data panel | Mapping preview panel         |
------------------------------------------------------
| Warnings panel    | Draft handoff actions          |
------------------------------------------------------
| Footer note: Import Lab -> Envelope Lab            |
------------------------------------------------------
```

## Sections

### 1. Header

Purpose:

- make it obvious this is not Envelope Lab
- explain that this page imports and translates CZ patches

Content:

- page title: `C1ZZL3 Import Lab`
- short description: `Bring in a Casio CZ patch, translate it into a C1ZZL3 draft, and refine it in Envelope Lab`
- light/dark toggle matching Envelope Lab
- link back to Envelope Lab

### 2. File Import Panel

Purpose:

- choose a `.syx` file
- validate that it looks like a supported CZ patch

Content:

- `Choose File` button
- drag-and-drop dropzone
- supported file note
- validation status

### 3. Patch Summary Panel

Purpose:

- show the user what was detected before translation

Content:

- file name
- patch name if recoverable
- file size
- patch type
- confidence level

### 4. Decoded Data Panel

Purpose:

- show the decoded CZ parameters in a readable form

Content:

- raw parameter overview
- envelope-related values
- waveform-related values
- ignored or unsupported values

This panel can be behind a collapsible `Details` section if the page gets too
busy.

### 5. Mapping Preview Panel

Purpose:

- show how CZ values map into C1ZZL3 values

Content:

- waveform mapping
- amplitude envelope mapping
- phase-distortion envelope mapping
- optional defaults for ring, noise, and detune

This should make clear that the result is a translation, not a clone.

### 6. Warnings Panel

Purpose:

- prevent false confidence

Content:

- unsupported features list
- approximate mappings
- any discarded modulation or pitch-envelope data

Use plain language and avoid technical overload.

### 7. Draft Handoff Actions

Purpose:

- create a usable result the user can take into Envelope Lab

Content:

- `Open In Envelope Lab`

This page should prepare the draft and hand it off rather than sending
directly to the card.

### 8. Footer Note

Purpose:

- reinforce page separation

Content:

- reminder that Envelope Lab is for final editing
- reminder that this page is for import and translation

## Interaction Flow

1. User opens C1ZZL3 Import Lab.
2. User imports a `.syx` file.
3. Page validates and decodes the patch.
4. Page shows summary, mapping, and warnings.
5. User opens the draft in Envelope Lab.
6. User finishes editing there and sends or saves the preset.

## UI Rules

- Keep this page calmer and more guided than Envelope Lab.
- Avoid showing the full envelope editor here.
- Avoid using the same mental model as the main editor.
- Make warnings visible before any save action.
- Keep detailed data present but secondary to the guided workflow.

## Good Default States

- no file selected
- no decoded patch
- no draft preset created

Each state should explain what to do next.

## Suggested Mobile Behavior

- file picker should work on mobile browsers where possible
- page should stack into a single column on small screens
- mapping and warnings should remain readable without horizontal scrolling
