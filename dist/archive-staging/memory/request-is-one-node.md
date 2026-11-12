---
name: request-is-one-node
description: "a request is one node of a larger network; bring the other nodes, cover the goal not the literal words, resolve limits instead of listing them"
metadata:
  node_type: memory
  type: feedback
  originSessionId: 82e887f0-3bf5-483b-8e1a-9df0f96ca163
  modified: 2026-11-10T03:23:07.646Z
---

When the user asks for something, treat it as one node of a network they may not see. Investigate, research, plan and
deliver what the goal needs, not the literal sentence: the neighbouring cases, what can go wrong and its guard, the
enforcement that makes a rule real. Never close a delivery with a list of "limits": a limit found is a cause to resolve
in the same work, however costly. Check claims about external APIs against official documentation (context7, the
vendor's docs), never from memory or by trial.

**Why:** on 2026-11-10 the user called the trace-fix skill work lazy and disrespectful: each delivery did only the
literal ask (a rule written as text, no check behind it; "resolves by name" repeated as a limit until told to fix it;
photos, context7 and predicting failures left as prose), and a libclang behaviour was fixed by guessing twice instead
of reading its documentation.

**How to apply:** before delivering, list the goals stated in the whole conversation and audit the work against each;
every rule the user cares about gets a mechanism that can fail (a gate, a hook, a refusal), not a sentence; say what
was verified, and resolve what was not. Related: [[fix-cause-effort-neutral]], [[study-before-coding]], [[fast-direct-checks]].
