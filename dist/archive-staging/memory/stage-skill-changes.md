---
name: stage-skill-changes
description: "Never patch a skill or its hooks in place while sessions use them; stage a copy, test it whole, deploy only on the user's word"
metadata:
  node_type: memory
  type: feedback
  originSessionId: 82e887f0-3bf5-483b-8e1a-9df0f96ca163
  modified: 2026-11-10T06:52:37.729Z
---

On 2026-11-10 the trace-fix skill was changed in place, piece by piece, while another session used it: the hook broke twice
for seconds and edits passed unchecked, and each piece opened a defect only seen at the next check. The user lost patience
("NUNCA ISSO FICA PRONTO?") and stopped using the skill.

**Why:** a live hook or skill is production; partial edits are half-deployed code, and every round of "fix what the last fix
broke" costs the user's trust more than the fix is worth.

**How to apply:** change a skill or hook in a copy (scratchpad), run its whole self-test and a real cycle there, then deploy
in one swap, and only when the user wants it deployed. Look for every gap at once (what the live sessions did, end to end)
before writing, not one gap per user complaint. Related: [[fast-direct-checks]], [[request-is-one-node]].
