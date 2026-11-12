---
name: code-first-measure-at-end
description: "Code first, investigation by reading; before AND after are both measured at the end (previous build vs this one, or the live instance's mark/diff), in km24"
metadata:
  node_type: memory
  type: feedback
  originSessionId: cebd78a7-4e51-40a7-9211-67c2663f3267
  modified: 2026-11-11T19:13:37.998Z
---

The owner wants the code written first; a cause is found by reading the code, never by measuring (2026-11-10). The comparison
CLAUDE.md asks for ("measured ... before and after", and on 2026-11-11 "garanta que nada quebrou, que manteve as otimizações")
is still made: both sides are measured once, at the end of the fix, in the 24 km city — the "before" from the previous commit's
build (or, once the live instance exists, its `mark` before the reload and `diff` after, same view, same state, noise stated).

**Why:** a "before" pass up front slowed the work and found nothing reading did not (2026-11-10); but a fix with no comparison
cannot show it kept the performance, which the owner demands. Earlier this memory said "no before measurement even though CLAUDE.md
says before and after": that contradicted CLAUDE.md and was corrected on 2026-11-11 at the owner's order to remove contradictions.

**How to apply:** read, fix, build; then measure previous build and this build (memory, CPU, GPU, frame) in km24, run the tests
the change reaches, take the photos the change asks for ([[photos-follow-the-change]]). Never measure to look for a cause.
Related: [[fix-cause-effort-neutral]], [[tree-back-to-nov7]].
