# Call Results per Argument Values (Templates) Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** A call of a function whose result depends on its arguments gives the result of its own argument values (Agesen's CPA on the functions Zipper's flow patterns select), so a helper called everywhere no longer hands every caller what any caller passed.

**Architecture:** `templates.py` selects the key parameters of a function from its trees (C1) and routes a call's argument values to the template of each tuple, following the template's result into the call (C2). A template is a `Frame` (the unit read under a key) whose nodes are the unit's trees keyed by `(gid, key)`; `Unit.key` is its gid. Calls (`transfer.calls`, called members) hand their argument nodes to the router; links stay the unit's (C3).

**Tech Stack:** Python 3.12, the tracer's solver.

**Spec:** `log/trace-rules.md`, "Call results per argument values: templates" (C1-C3), and the probes above it.

## Global Constraints

- Files under 300 lines with a header; English; new files in `log/mapa.md`; gates on changed files.
- No language named in the rule: C1-C3 hold for GDScript, C++ and JS alike.
- Code and review first, tests at the end (RED on `abd5f047`), then city (differences explained), the lab app, the lab with three.js.
- Stage only this work's files; commit with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`; push.

## Review Focus

1. Recursion through a selected function: templates per value tuple, finite because values are.
2. A selected function also called by the engine (handed): its unit keeps the engine's values; the template only refines program calls.
3. A call whose callee can be both a selected function and another: each callee answered by its own rule.
4. Product of two key parameters at a site with many values each: the templates are the tuples that occur.
5. A template's locals never mix with the unit's (`(tree, (gid, key))` nodes).

---

### Task 1: Frames and the selection (C1)

**Files:** Create `tools/analysis/featuretrace/templates.py` (`Frame`; `key_params(t, unit) -> tuple[int, ...]`).
Modify `model.py` (`Unit.key` property), `typer.py` (`node_of`, `wire_local`, `ret_node` key by `unit.key`).

- [ ] Tests in `tests/test_templates.py` on `tests/fixture/jsvalues/helpers.ts`: `key_params` of an identity helper is (0,),
  of a getter through its parameter (0,), of a function with an inner function (), of a function of a constant ().

### Task 2: The router (C2)

**Files:** `templates.py` (`route(t, call_node, gid, arg_nodes)`), `transfer.py` (`calls`, `members` take the call's
argument nodes; a selected function's result through `route`), `typer.py` (`wire_plain_call` passes them).

- [ ] Tests: `label(k)` called with `"a"` at one site and `"b"` at another gives each site only its own string;
  `pick(box)` / `pick(tile)` through an identity helper: each site's `.open()` / `.lay()` reaches only its own class;
  a recursive helper terminates with the union of its values.

### Task 3: Review, tests at the end, records

- [ ] Review; RED on `abd5f047`; suite; city (each difference a refined result); lab app; lab with three.js under a
  monitor; ledger `log/trace-rules-templates-progress.md`; `log/feature-trace.md`; `log/pendencias.md`; gates; commit; push.
