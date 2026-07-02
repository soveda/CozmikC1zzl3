# Skill Deployment Ruleset

Use these rules when deploying the `web-hardware-editor-review` skill into any
other repository or Codex setup.

## Goal

Make the skill portable, easy to install, and easy to keep in sync across
projects.

## Rule 1: Keep The Skill Self-Contained

- The skill must live in a single folder.
- Required file:
  - `SKILL.md`
- Optional but recommended:
  - `references/checklist.md`
- Do not depend on repo-specific paths outside the skill folder.

## Rule 2: Keep Trigger Text Short And Clear

- `SKILL.md` should explain when to use the skill in one concise description.
- Put the long checklist in `references/checklist.md`, not in the trigger file.
- The description should mention:
  - browser-based hardware editors
  - Web MIDI, WebUSB, or SysEx-style APIs
  - browser support
  - device detection
  - mobile edge cases
  - diagnostics
  - deployment

## Rule 3: Prefer Copyable Relative Structure

When moving this skill to another repo, preserve this structure exactly:

```text
skills/
  web-hardware-editor-review/
    SKILL.md
    references/
      checklist.md
```

## Rule 4: Repo-Local Deployment

If the skill is only needed in one repo:

1. Copy the folder into:
```text
<repo>/skills/web-hardware-editor-review
```
2. Commit it with the repo.
3. Mention in the repo `AGENTS.md` that the skill exists if you want future
   agents to find it quickly.

## Rule 5: Personal Skill Deployment

If the skill should be available across many repos:

1. Copy the folder into your personal Codex skills directory:
```text
$CODEX_HOME/skills/web-hardware-editor-review
```
2. Keep the repo copy and personal copy synchronized.
3. Prefer updating from one canonical source repo instead of hand-editing
   multiple copies.

## Rule 6: Canonical Source Strategy

- Choose one repo as the source of truth.
- Make all edits there first.
- Copy or sync outward from that repo to other repos or your personal skills
  directory.
- Do not let multiple edited copies drift.

## Rule 7: When To Update The Skill

Update the skill when you learn something reusable about:

- browser support differences
- mobile enumeration quirks
- diagnostics patterns
- graph/editor interaction rules
- theme failures
- deployment issues

Only add lessons that generalize well across projects.

## Rule 8: What Not To Put In The Skill

Do not include:

- repo-specific bug history
- one-off commit notes
- temporary deployment IDs
- user-facing project documentation
- large copies of source code

The skill should contain reusable review guidance only.

## Rule 9: Minimal Validation Before Reuse

Before deploying this skill into another repo, check:

1. `SKILL.md` description still matches the intended use
2. referenced file paths are still valid
3. the checklist is still generic enough for cross-project use
4. no repo-specific wording leaked into the skill

## Rule 10: Suggested Install Methods

### Method A: Copy Into A Repo

Best when the guidance belongs with a single project.

```text
target-repo/
  skills/
    web-hardware-editor-review/
```

### Method B: Install As A Personal Skill

Best when you want it available everywhere.

```text
$CODEX_HOME/skills/web-hardware-editor-review/
```

### Method C: Keep In A Shared Utility Repo

Best when several repos need the same skill and you want one source of truth.

Recommended flow:

1. maintain the skill in a shared repo
2. copy or sync it into target repos as needed
3. update downstream copies only from the shared repo

## Rule 11: How To Tell Another Agent To Use It

In another repo or chat, refer to it by purpose, for example:

- use the `web-hardware-editor-review` skill
- review this app as a browser-based hardware editor
- check Web MIDI support, mobile detection, diagnostics, theming, and Pages deployment

## Rule 12: Recommended Companion Files

For best results in a repo, pair the skill with:

- `AGENTS.md` for repo-specific working rules
- project `README.md` for user-facing instructions

Use the skill for reusable expertise.
Use `AGENTS.md` for repo-local rules and expectations.
