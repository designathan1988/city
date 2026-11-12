---
name: hook-hands-off
description: "Never change the trace-fix skill or its hooks again; the version that holds is commit b2d441f8's (restored in 58bbba6f); follow SKILL.md as written on disk"
metadata:
  node_type: memory
  type: feedback
  originSessionId: 00332dfd-d4c9-4cf0-ab01-b7d5a510442a
  modified: 2026-11-10T21:40:00.123Z
---

On 2026-11-10 the user allowed four cuts to the trace-fix hook ("PODE"), then said the changed hook solved nothing, was slower and caused more errors, and ordered: "NÃO MEXA MAIS NO HOOK". They restored the old files themselves. Later the same day they said I had loosened the hook in commit `9abc6af7`, that the old version is the one that holds, and ordered the eight skill files committed back to the version of commit `b2d441f8` with nothing else changed: done in commit `58bbba6f` (the working tree already equalled `b2d441f8`; I only staged and committed). Their closing words: "Nunca mais altere a skill nem os hooks."

**Why:** the cuts left the RECORD and FLOW debts refusing every edit and surfaced old debts as new refusals; the user saw more refusals, not fewer, and reads any loosening of the hook as me dodging the process (my interpretation of the second part).

**How to apply:** never edit, stage, commit or revert anything under `.claude/skills/trace-fix/` or any hook, whatever the friction; if the hook seems wrong, say so and keep working inside it. Follow `SKILL.md` as it is on disk (re-read it, do not go by memory): every file read whole and every opened file to its end (a partial Read of a data file counts as opened), one `record` per batch with saw / called_by / state, FLOW for every code file read before recording it (reference tree included; `rec2.py` cannot record a reference file in the older trace: record it alone with `ledger.py record`), docs looked up again after a compaction, `ledger.py status` to see what is owed, every photo and crop opened with Read (a `.seen` mark beside each) and named in `analysis.md` before `close`. The hook refuses every Write while something is owed, memory files included. `.claude` is in `.gitignore`: `git add` of its tracked files stages them but exits non-zero, so do not chain the commit with `&&`. Related: [[stage-skill-changes]].
