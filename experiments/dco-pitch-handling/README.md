# C1ZZL3 DCO Pitch Handling Beta

This folder is a Pages-accessible beta test for comparing separate Casio CZ
DCO1 and DCO2 pitch-envelope handling without changing the production firmware.

## Goal

The current production importer decodes both CZ DCO pitch envelopes, then maps
them into one C1ZZL3 pitch lane. This experiment keeps both source envelopes
visible and lets the user choose how the single C1ZZL3 pitch lane is generated:

- `Merged DCO1 + DCO2 average`, the recommended default
- `DCO1 pitch only`
- `DCO2 pitch only`
- `Advanced: DCO1 / DCO2 difference emphasis`

## Current Scope

- No firmware protocol changes.
- Uses the current production UF2.
- The card still receives one pitch envelope.
- The imported draft preserves all pitch alternatives in browser handoff data
  so we can compare mappings before deciding whether separate oscillator pitch
  envelopes are worth implementing in firmware.

Hosted beta:

```text
https://soveda.github.io/CozmikC1zzl3/experiments/dco-pitch-handling/import-lab/
```

## Local Test

From the repository root:

```sh
python3 -m http.server 5175 --directory experiments/dco-pitch-handling
```

Open:

```text
http://localhost:5175/import-lab/
```

Test flow:

1. Import a Casio CZ `.syx` patch.
2. Compare the decoded `DCO1 pitch envelope` and `DCO2 pitch envelope` panels.
3. Change `Pitch source for C1ZZL3 pitch lane`.
4. Confirm `Selected C1ZZL3 pitch lane` changes.
5. Click `Open In Beta Envelope Lab`.
6. Confirm the pitch-source badge shows the selected mapping.
7. Audition or send as usual, remembering this still sends one pitch lane.

## Promotion Criteria

Keep this as a beta while listening tests decide whether merged/DCO1/DCO2
selection is enough, or whether future firmware should support separate pitch
envelopes for oscillator 1 and oscillator 2.
