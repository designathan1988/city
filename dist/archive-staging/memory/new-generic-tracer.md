---
name: new-generic-tracer
description: "owner 2026-11-11 ordered a NEW standalone tracer (tools/tracer); old tracer stays untouched, lab not to be run; record log/tracer.md"
metadata:
  node_type: memory
  type: project
  originSessionId: bceccebc-2abc-4510-8d0b-e2ad9391ef75
  modified: 2026-11-12T06:58:53.671Z
---

On 2026-11-11 the owner ordered work on the old feature tracer (`tools/analysis/featuretrace`) and on the lab project
to STOP, and a new tool to be started in a new folder, tied to no other project, generic, for GDScript, C++ and
TypeScript/JavaScript. It is `tools/tracer` (commits f9c9aee8 and 1c438d08, 2026-11-12); its record is
`log/tracer.md`; run it with `python tools/tracer/trace.py <start> --root <folder>`.

**Why:** days on the old core (points-to analysis, contexts) without the whole lab converging; the owner's view is
that tracing is reading code and matching what the language says, not that machinery. See [[feature-tracer-generic]].

**How to apply:**
- Nothing of the old tracer was removed ("NÃO VAI TIRAR NADA AINDA"): do not delete or import it until the owner
  says. Uncommitted old-tracer work sits in `git stash` ("old tracer: scaler and typed positions"): do not drop it.
- Do not run or analyse the lab with either tool unless the owner asks; the new one was measured on this
  repository's own code only.
- What worked there (my reading, not the owner's words): when the tool misbehaves on real code, read the links it
  produced (counts by kind, the definitions most linked, where one value came from) to find the rule of the tool
  that is wrong, write the test that fails, then fix that rule; never tune a number.
- `tests/test_generic.py` there is the gate against engessamento: any name of a program, library or platform in the
  tool's code fails it.
