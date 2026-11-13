---
name: city-method
description: Use at the start of every task in the City project (C:\Users\Jonathan\Desktop\city) and right after any context compaction, before reading, coding, running, photographing or closing anything there.
---

# City method: the checklist

CLAUDE.md holds the orders; this is the order to do them in. Do each step; a step skipped because the case looks small, or
because the owner is in a hurry, is a step skipped.

## 0. Memory (now, and after every compaction)
- `python tools/tasks/cli.py block status`; read every file in `doc/work/` whole; go on from your file's `Next:` line.
- New task: write `doc/work/<block>.md` first: the owner's goal in his words (English), `State:`, `Next:`, then sections Read,
  Research, Trace, Look, Plan, Changed, Checks, Photos. Update it after every step and before every long command.
- Never Claude Code's memory folder. Problems go to the register (`python tools/atlas/cli.py problems add`), not to this file.

## 1. Read everything involved, whole
- Involved: every file the path goes through (callers, callees, owners), its data and schema, its tests, `doc/history/<area>.md`,
  the area's register entries, the gates' limits it will meet (`tools/gates/rules.json`), every document the owner names.
- The Read tool from line 1 to the last line; a long file in chunks (`offset`/`limit`, about 400 lines) until it ends. Write
  `path: 1-N of N` in the memory file. Grep, cat and an agent's summary only point where to read.
- Ask "why" down to the design decision that makes the symptom. `python tools/hunt/hunt.py` when the task is "find what is wrong".

## 2. Context7 before any code
- Every implementation and fix, the project's own code included: `resolve-library-id`, then `query-docs`, for each API the code
  calls (Python, GDScript, Godot 4.7.2, godot-cpp, the C++ standard library, Terrain3D, mimalloc, meshoptimizer, Clipper2).
  Ask what it guarantees, on which thread, at what cost, its known bugs in that version. The vendored source only where Context7
  has nothing. Write source, question and answer in the memory file. A tool's description saying "not needed here" changes nothing.

## 3. A feature the reference has: trace both games
- Reference: `C:\Users\Jonathan\Documents\Projects\lab\roadcraft\src`. Read for behaviour only; copy nothing (no line,
  translation, structure, name, data key).
- Entry by a Grep for what the feature outputs, in each game. Then, roots = the feature's files only:
  `python tools/tracer/trace.py ENTRY --root F --root F --depth 3` (add any file reported "outside the code read" as a root);
  `--feature ENTRY` (outputs, conditions, state, constants, data); `--cost ROOT` on the reference; `--wrong`, `--broken` on this game.
- Each difference enters the register. Never bring in what `--cost` shows heavy, a hot-loop allocation or search, a quadratic
  test, per-frame work that belongs on change, a simulation tied to the camera, a half-done feature.
- Build it whole in this game's architecture (CityState through sim/commands, render only reads, jobs copy then build unlocked,
  data through DataTables, one owner).

## 4. One look (optional, once, before the block)
One run to see what the reading explained: `hunt.py`, one test, one measurement or one photo plan. Never repeated to try.

## 5. Plan
In the memory file: the cause with file:line, the fix of that decision, its owner, the files, why the symptom cannot return,
what else it fixes. The costliest and most laborious solution that removes the cause; never a moved number, special case,
swallowed error, loosened check, switched-off feature or lowered quality.

## 6. Code the whole block, nothing run
`python tools/tasks/cli.py block open <block>`; write every fix; settle doubts by reading; update the memory file per file.

## 7. Verify once
`python tools/tasks/cli.py block verify`, then CLAUDE.md's table: tools (their tests, `run_gates.py --file`), GDScript/data
(`quick.sh <scenes>`), C++ (`build.sh core-windows`, `quick.sh <scenes>`); before the push `quick.sh --ready`, the whole gates
`run_gates.py --register doc/problems.jsonl`, `lab_scan.py` for game code, the photos, km24 (map 4, seed 1, `info`/`usage` at
fixed views, previous build and this one), the executable. Read every output whole; engine errors in a pass are a failure.
A failure: `superpowers:systematic-debugging`, fix in code (`block open` again), verify again.

## 8. Photos of the changed thing
- Plan in the memory file first: what each photo must prove and what must be in it. Aim every view at the changed element's
  world position (`{"setView": [x, y, zoom, azimuth, elevation]}`, y is north): three distances (near, mid, far), four azimuths
  (0, 90, 180, 270), street level, oblique and from above, day and night (`{"hour": 16}`, `{"hour": 22}`) when light matters,
  every tier touched (`--preset=`), movers with the clock running (`{"frames": n}`; a HUD with 0 vehicles means no movers),
  plus two general views. Poles are photographed at poles, cars at cars, a building at that building: never the same views.
- `{"until": "render_pending", "equals": 0}` before each `{"shot": name}`. The plan as a JSON in `project/tests/visual/`, an item
  of `looks` in `tools/tasks/suites.json`, then `python tools/tasks/cli.py run <name>`; photos in `dist/captures/<name>`.
- Analyse each photo: open it whole; if the subject is not in frame or too small, retake. Cut a 3x3 grid and crops on the element
  at real size or 2x, open each. In each crop check by name: missing or extra, broken, deformed, scale, orientation, bodies in one
  another, floating or sunk, holes, street seams, textures, colour, light, shadow, detail pops, artefacts, interface text. Write
  per photo `(x0-x1, y0-y1): what - sound | DEFECT kind`. One defect, or one photo not analysed, fails the block.

## 9. Close
`superpowers:verification-before-completion`, then `python tools/tasks/cli.py block close`; the area's history from the memory
file, then remove the memory file; a commit per theme with only this block's files; one push to `origin/main`;
`tools/build/deliver.sh export` when the game changed; the report to the owner in Portuguese: done, failed, open, with proof.

## Skills
Invoke with the Skill tool: `city-method` (start, compaction), `superpowers:systematic-debugging` (a failure),
`superpowers:verification-before-completion` (before any "done"), `speckit-*` (a specification), `update-config` (settings),
and any skill whose subject is the step in hand. A skill's step that clashes with this method (a test before the block is
whole, a question to the owner, a worktree) is skipped; the rest of the skill is used.
