# How the game is developed: the binding way, in one place

Written 2026-11-11 at the owner's order ("verify in your memory, or anywhere else, information contrary to doing it right, correct
it, make explicit the right way of doing everything"). CLAUDE.md holds the rules; this page holds them in the order they are
applied, and the contradictions that were found and how each was settled. Where anything (a memory, a record, a skill, an old plan)
says otherwise, this page and CLAUDE.md win; a new contradiction found is settled here and in CLAUDE.md in the same change.

## The way, step by step

1. **Read before anything.** The code involved, whole, personally (the `trace-fix` skill: flow traced end to end, every file read
   whole, recorded on disk). Ask "why" until the design decision that produces the symptom. No measurement, profile or heavy test to
   look for a cause: investigating is reading.
2. **Research every outside API, every time.** Every implementation or fix that calls Godot, godot-cpp, Terrain3D or a library is
   preceded by a context7 lookup (or the vendored source, or the engine's source): what it guarantees, on which thread, its cost,
   its known bugs. Never from memory.
3. **The roadcraft lab is read for behaviour only.** What the player sees, the rules, the cases, the values: written down as
   behaviour. Its code is never copied, translated (by hand or by the converter), renamed or reproduced in structure, name or
   function. What came from it and still stands is redesigned whole for this game: optimised, measured, tested, photographed; a
   rename is still a copy.
4. **Design for this architecture.** `CityState` is the one source of truth; one writer (the player's commands, the simulation's
   clock and systems, a load, a generation); one owner per definition; every stateful thing has one state controller (one member,
   one setter, one transition table: `GameFlow.TRANSITIONS` is the model); the C++ layers core -> sim -> render (read only) -> api;
   values in `project/data` read by `DataTables`; the job pool and the map's locks as CLAUDE.md describes.
5. **The cause, the costliest way, no patch.** The decision that produces the symptom changes, rewritten when that is what it
   takes. Never: a threshold, timeout, tolerance or thread count moved; a special case; an error swallowed; a check loosened; a
   reference re-blessed to pass; the cost moved elsewhere. Never offer cheap options.
6. **Test first.** The check that fails because of the cause is written (or named) before the code, then the code is written once.
   A failure sends you back to reading, never to another attempt.
7. **Verify what the change reaches.** Its tests (one Godot at a time while working); its photos planned by what it touches — poles
   from near, far and several angles, people with the clock running, vehicles at the junctions touched — plus a few general views;
   every photo and every crop opened and judged piece by piece, on its own, never against older photos; both builds (previous and
   this) measured at the end in km24 (memory, CPU, GPU, frame) with the noise stated.
8. **Every contract has a mechanism.** A rule that matters is a gate, a test or a run-time check that fails on its own, not a
   sentence; a new rule enters CLAUDE.md with its guard.
9. **Close the theme whole.** Both DLLs, `quick.sh --ready`, every gate, the executable (`tools/build/deliver.sh export`), the
   line's record with time and commit, commit and push to `origin/main`; tell the owner in Portuguese.
10. **Work alone and watched.** No agents, subagents or new sessions unless the owner orders them in so many words; no process
    in the background unwatched; never close the game the owner has open; one Godot at a time while working.
11. **Use the skills that fit.** `trace-fix` for every reported problem; `superpowers:brainstorming` before a new feature or tool,
    then `superpowers:writing-plans`; `superpowers:systematic-debugging`, `superpowers:test-driven-development` and
    `superpowers:verification-before-completion` in their steps; context7 always (step 2).

## Contradictions found on 2026-11-11 and how they were settled

| Where | What it said | Settled |
|---|---|---|
| CLAUDE.md, tools/converter | "Translated code (e.g. flow_control) is regenerated, never fixed by hand" | Never regenerated: translated code is a debt redesigned at its cause; the converter produces no game code |
| CLAUDE.md, core/ | "the runtime of the translated code" as a part of core | A debt to redo, listed in `log/pendencias.md` |
| CLAUDE.md, Execução | "every port ... measured before and after" | There are no ports: every feature and fix is measured, both builds at the end |
| CLAUDE.md, Luz e relógio | "at night the sun becomes the moon without shadow" | The moon casts all night (P347, 2026-11-11) |
| Memory `code-first-measure-at-end` | "no before measurement even though CLAUDE.md says before and after" | Both measured, at the end: previous build and this one |
| Memory `delegate-port-with-trace` | agents start from the converter's machine translation | Superseded: no translation of the roadcraft, no agents |
| Memory `coordinate-parallel-sessions` | subagents in worktrees are fine | No agents unless ordered |
| Memory `never-wake-stopped-agents` | an order for agents means a handful | No agents (2026-11-11); an explicit order is the only exception |
| Memory `judge-photos-by-quality` | if photos are sound, bless the goldens and go on | Frozen photos are compared with nothing; `--bless` only for the digest of a change meant to move it, its cause written |
| Memory `diagnosis-2026-11-08` | ".claude has no hooks" | History of that date: hooks exist now |
| Skills `roadcraft-port`, `roadcraft-reading` (account skills, not in the repository) | their names speak of porting | Not used: porting the roadcraft is forbidden; reading it for behaviour is step 3; the owner may remove or rewrite them in the account |
