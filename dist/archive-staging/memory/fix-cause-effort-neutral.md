---
name: fix-cause-effort-neutral
description: "Owner 2026-11-11: cheap fixes that do not solve the cause are forbidden; never copy roadcraft code; always the most expensive, laborious solution at the cause"
metadata:
  node_type: memory
  type: feedback
  originSessionId: ae604b5b-1c16-42cb-a751-32e586ea2973
  modified: 2026-11-11T04:13:03.965Z
---

The owner's order, stated expressly on 2026-11-11 and binding in every prompt, in CLAUDE.md, in the trace-fix skill and in every other order:

- **Cheap solutions that do not solve the cause are not allowed.** Always solve the cause, always choosing the most expensive and laborious solution there is. Never offer cheap and complete options to choose from: understand, research, plan and build the complete one.
- **Never copy code from the roadcraft lab** (`C:/Users/Jonathan/Documents/Projects/lab/roadcraft`). It serves only as a reference for how to reach the goals of the features (what the game must do, the rules, the cases, the values, what the player sees). No line, no line-by-line translation, no structure, no names, no functions of it. The lab ports in the tag `antes-da-volta-2026-11-10` came from it: they never come back.
- **What the owner did authorise (2026-11-10), not to be confused with the above:** bringing back from the tag the fixes made earlier to this game's own code (performance and optimisation of code that already existed here), because they were nothing of the roadcraft, verified so. That authorisation never covered any port.

**Why:** Claude habitually picked the cheapest option and copied the reference's code and names; that is what broke the game. On 2026-11-11 Claude also mixed the two things up, writing that the authorised copies of its own fixes had been forbidden; the owner corrected it: do not confuse the authorised return of own fixes with the forbidden copying of roadcraft code.

**How to apply:** for every problem, keep asking "why" to the design decision that produces the symptom and change that, rewriting whenever the right fix is a rewrite; read the reference for behaviour only and design new code for this game's architecture (CLAUDE.md); before closing anything, check that no name, structure or line came from the reference; of the tag's commits brought back, check that none carried port code. Verification stays proportional (investigation by reading code). See [[no-reference-game]], [[tree-back-to-nov7]].
