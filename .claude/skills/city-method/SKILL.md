---
name: city-method
description: Use at the start of every task in the City project (C:\Users\Jonathan\Desktop\city) and right after any context compaction, before reading, coding, running, photographing or closing anything there.
---

# City method: the checklist

CLAUDE.md holds the orders; this is the order to do them in. Do each step; a step skipped because the case looks small, or
because the owner is in a hurry, is a step skipped. The goal behind every step: the cause fixed, never the effect.

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
- Ask "why" down to the design decision that makes the symptom. `python tools/hunt/hunt.py` only on the owner's order.

## 2. Context7 before any code; never your own memory
- Nothing you think you know about an API is a source. Every implementation and fix, the project's own code included:
  `resolve-library-id`, then `query-docs`, for each API the code calls (Python, GDScript, Godot 4.7.2, godot-cpp, the C++
  standard library, Terrain3D, mimalloc, meshoptimizer, Clipper2): what it guarantees, on which thread, at what cost, its known
  bugs in that version. The vendored source only where Context7 has nothing. Source, question and answer go in the memory file.
  A tool's description saying "not needed here" changes nothing.
- Where the game models something real (traffic rules, signals, kerbs, poles, light), values come from a cited norm or method.

## 3. Trace the feature in both games (before building or fixing anything the reference has)
What the reference is for: the roadcraft lab, `C:\Users\Jonathan\Documents\Projects\lab\roadcraft` (TypeScript in `src`, the
game with everything working), shows how the simulation and each element must behave: what happens, when, in which cases, with
which values, what the player sees. It is read to understand that. Nothing of it is copied: no line, no translation, no
structure, no name, no data key (the gate LAB_COPY and `python tools/gates/lab_scan.py` find them). The old 4.0 is no reference.

The tool: `python tools/tracer/trace.py` (the owner's tracer, from `C:\Projects\tracer`; its own docs are there, in
`specs/003-behaviour-from-the-code/spec.md` and `specs/004-evaluate-and-compare/spec.md`, "How to use"). It reads the code and
never runs it, in TypeScript/JavaScript, GDScript, C++ and shaders (`.gdshader`, `.gdshaderinc`, GLSL), following calls, callbacks
(bind, signals, events, jobs), calls between languages (GDScript to the C++ the core registers), state (who writes, who reads) and
messages between threads and workers. It needs Python 3.12 with `tree_sitter`, `z3-solver`, and for TypeScript `npm ci` once in
`tools/tracer/tracer/tools`. Run it from the repository root. A start is a function's label (`Class.method`, `name`), the end of
one, or `path:line`. Roots (`--root`, repeated) are the feature's files or folders, never a whole game; each game is traced with its
own roots. The analysis is kept in its index, so asking again is instant (`--fresh` reads again); `--seconds S` and `--memory MB`
cap a question, which then says what it did not answer. `--json` or `--out F` keeps an answer, `--html F` a page to look at.

1. Where the feature produces something, in both games: the place where it draws, writes the state the player sees, or answers
   the player, as a `path:line` or a function's name. Search the code for the thing itself (a text the player sees, a data key, a
   draw call, a command's name), never for an idea of it: in `roadcraft/src` and in this game (`native/src`, `project/`).
2. The functionality from those outputs: `trace.py --feature <path:line>[,<path:line>] --root <folder>`: everything that decides
   whether and how each output happens (the functions on the way, the conditions, the state read and who writes it, the constants
   and the data that enter) and what sets it off (the roots and the way from each). Its sheet is what the feature must do.
3. What runs from a start and what reaches it: `trace.py <START> --root ... --depth N` (`--back N` backwards, `--state` with the
   state each step reads, `--exact` without what was found by a name alone). A file reported "outside the code read" that belongs
   to the feature becomes another `--root`; `--gaps` says what cannot be followed and why.
4. When and how: `trace.py <START> --conditions --root ...` (each effect: when it happens and what it writes, in the start's
   inputs); `trace.py <START> --cases [NAME] [--range LOW:HIGH] --root ...` (alternatives: a case where none happens, more than one
   at once, a value out of range).
5. The simulation in concrete situations: `trace.py --evaluate "A;B*3" --given NAME=VALUE --root ...` (or `--situation F.json`,
   `--repeat N`): what the program produces, with where each value came from; `--why file:line`, `--why-not file:line`. Use it to
   understand how the reference behaves step by step: a car at a red light, a walker at a kerb, a lot on a corner.
6. Side by side, in the memory file: for each output, the reference's condition, values and cases against this game's. Every
   value or behaviour one has and the other lacks (a threshold, a size, a count, a case) enters the register and is read on both
   sides, whole. Where both take the same inputs: one map per game (JSON `{"root": "<folder>", "start": "<label>",
   "inputs": {"<input>": "<expression of the shared variables>"}, "given": {...}, "outputs": ["<what is called>", "file:line"]}`),
   then `trace.py --compare ref.json game.json [--shared NAME=VALUE] [--sweep NAME:LOW:HIGH:N]`: the bands where the outputs
   differ, with why on each side. Code not read with a known behaviour: a short model file in one of the languages, put among
   the roots.
7. What never comes in: `trace.py --cost <ROOT> --root ...` on the reference: every loop, what it goes over and whether that grows
   with the city, loops inside loops, what each turn makes and calls; the roots that run every frame (`requestAnimationFrame`,
   `setAnimationLoop`, `onBeforeRender` there; `_process` here). A loop every frame over a collection that grows with the city, a
   loop inside a loop over growing data, work redone each frame whose inputs do not change, a half-done or faked feature, and
   everything the reference's own reports name as a problem (`reports/`, `lab-reports/`, `ARQUITETURA.md`,
   `docs-optimization-brief.md`): never carried over. This game does the same behaviour within its architecture, decided once
   per chunk or per change, on its own threads, within the budgets.
8. This game's defects: `trace.py [<START>] --wrong --root ...` (behaviour that contradicts what the program itself states: a
   check most peers make and one skips, an invariant one write breaks, gaps and overlaps of alternatives) and `--broken` (state
   never read or never written, events without a listener, data keys missing or unread, uniforms never set or read, code never
   reached). After the fix, `trace.py --versions <old copy> --evaluate <START> --root .` shows what changed, by which line.
9. Its limits: behaviour no point of the program states is not found; a function handed as a bound callable to a signal is not
   shown reached (Ptools-7) and callers back from a store command are over-listed (Ptools-8): read those links by hand. A tracer
   failure enters the register and is fixed at its cause in `tools/tracer`.

Then the design: the behaviour sheet is the requirement, the code is this game's own. State in `CityState`, changed only by
`sim/commands`, advanced only by `sim/simulation`; render only reads; mesh jobs copy under a short shared lock and build
unlocked; the frame's budgets; values in `project/data` through `DataTables`; one owner per definition. The feature is built
complete, every case of the sheet, measured against the budgets; never a slice of it and never the reference's shape.

## 4. Nothing runs before the code is written
No test, measurement, photo, script or `hunt.py` before every fix of the block is written: a doubt is settled by reading.
After it, nothing runs until the block's code is whole.

## 5. Plan
In the memory file: the cause with file:line, the fix of that decision, its owner, the files, why the symptom cannot return,
what else it fixes. The fix that removes the cause, whatever its size (a rewrite when the design asks); never a moved number,
special case, swallowed error, loosened check, switched-off feature or lowered quality.

## 6. Code the whole block, nothing run
`python tools/tasks/cli.py block open <block>`; write every fix; settle doubts by reading; update the memory file per file.
No test, build, measurement, photo or process runs until every fix of the block is written.

## 7. Check once, after the whole block, fast
`python tools/tasks/cli.py block verify`, then only what the block reaches, each check taking seconds (CLAUDE.md's table):
tools (their tests, `run_gates.py --file` on each changed file), GDScript/data (the test scenes it reaches,
`python tools/tasks/cli.py run <task>`, and the gates of the changed files), C++ (both DLLs, `build.sh core-windows`, and the
scenes it reaches), what the player sees (the photos of the change, 8). A check that takes minutes is a defect of its tool,
registered. `quick.sh --ready`, the whole gates and `km24.py` run only on the owner's order. Read every output whole; engine
errors in a pass are a failure. A failure: `superpowers:systematic-debugging`, fix in code (`block open` again), check again.

## 8. Photos of the changed thing
- The subjects come from the change: the elements the feature draws or moves (the trace's outputs say which). Poles are
  photographed at poles, cars at cars, a building at that building, people where they walk. Never the same views for every change;
  the frozen views of `quick.sh` prove nothing about a change.
- Plan in the memory file first: for each photo, the claim it proves and what must be in it. For each element, its world position
  (read it, or a `query` step), then views aimed at it with `{"setView": [x, y, zoom, azimuth, elevation]}` (y is north; zoom in
  screen pixels a metre):
  - three distances: near (the element fills about a third of the frame), mid (it with its street or block), far (its area);
  - four azimuths at least (0, 90, 180, 270): defects follow the direction (detail swaps, culling, one side of a mesh);
  - three elevations: street level, oblique, from above;
  - several positions: each case of the behaviour sheet where the element stands (straight, corner, junction, slope, end);
  - day and night (`{"hour": 16}`, `{"hour": 22}`) when light, shadow or emission is involved; every tier touched (`--preset=`);
  - movers with the clock running, several shots a few frames apart (`{"frames": n}`); a HUD reading 0 vehicles means no movers;
  - two general views of the area.
- `{"until": "render_pending", "equals": 0}` before each `{"shot": name}`. The plan as a JSON in `project/tests/visual/`, an item
  of `looks` in `tools/tasks/suites.json` (name, plan, resolution `1920x1080`, timeout), then `python tools/tasks/cli.py run <name>`
  (hidden); photos in `dist/captures/<name>`.
- Analysis, every photo, nothing shallow:
  1. Open it whole with Read. Name what is in it. If the subject is out of frame or too small to judge, retake it.
  2. Cut a 3x3 grid and crops centred on each element at real size or 2x (a script writes them to the scratchpad); open each.
  3. In each crop, check each defect kind by name: missing or extra, broken, deformed, scale, orientation, bodies in one another,
     floating or sunk, holes, street seams, textures, colour, light, shadow, detail pops, artefacts, interface text.
  4. Hold the same element across its angles and distances in this run: what one view shows and another lacks is a defect.
  5. Write per photo `(x0-x1, y0-y1): what is there - sound | DEFECT kind`, saying what was checked, not only what was wrong.
  6. One defect, or one photo not analysed, fails the block.

## 9. Close
`superpowers:verification-before-completion`, then `python tools/tasks/cli.py block close`; the area's history from the memory
file, then remove the memory file; a commit per theme with only this block's files; one push to `origin/main`;
`tools/build/deliver.sh export` when the game changed; the report to the owner in Portuguese: done, failed, open, with proof.

## Skills
Invoke with the Skill tool: `city-method` (start, compaction), `superpowers:systematic-debugging` (a failure),
`superpowers:verification-before-completion` (before any "done"), `speckit-*` (a specification), `update-config` (settings),
and any skill whose subject is the step in hand. A skill's step that clashes with this method (a test before the block is
whole, a question to the owner, a worktree) is skipped; the rest of the skill is used.
