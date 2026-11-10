---
name: trace-fix
description: Use when the user asks to solve, fix, correct or explain any problem, bug, defect, slowness or wrong behaviour of the game ("resolva", "corrija", "por que", a symptom report, a photo showing something wrong), before any code change, and when resuming such work after a compaction or in a new session.
---

# Trace the flow, record it whole, fix the cause, test and look

A problem is fixed by following the code that produces it, end to end, from what triggers it to what the player sees,
writing down everything found on the way, and changing the decision that causes it. One problem at a time: trace it,
read and record its flow, fix it whole, test it, photograph it, analyse the photos; then the next.

```
L="python .claude/skills/trace-fix/ledger.py"     # every command below is $L <command>
```

## What holds the work

**The ledger is your memory.** `$L resume` first, in every session and after every compaction; continue from what it
shows. All you record is on disk at once, whole, in two files: `dist/trace/<slug>/notes.md` (the flow in the order the
code runs, what every chunk does, the documentation consulted) and `report.md` (the problems, their causes, the fix
planned and the fix made). After a compaction Read those two files (`export` gives the Read calls): the code is not
read twice. The trace you open, switch to or run a command on is yours: the ledger's commands go to it with no prefix;
`TRACE_FIX_SLUG=<slug> $L resume` once takes up a trace another session left.

**The flow is read with the Read tool, once, and everything read is recorded by you.** The functions of the flow, their
callers and the records of the area that speak of the problem (CLAUDE.md is already in your context: its rules bind
every step). What you read is kept by your notes, so nothing is read twice: after a compaction only the lines you are
about to change are read again. A document is read where it speaks of the problem, not from its first line to its last.
The edit hook refuses an Edit or Write and lists, all at once, what is missing:

- READ: the lines the edit replaces, read as the file is now since your last compaction (a file written over whole:
  the whole of it). A file opened in part stops nothing. A reference tree (`references.json`: the reference game) is
  read for what the game must do, never copied.
- RECORD: every line of the flow you read has a note **you** wrote (saw, called_by, state). A note an earlier session
  wrote does not count for your reading: what you understood is lost at your next compaction unless you write it.
- FLOW: every code file you read is on the flow of your trace (`flow`, then recorded) or taken off it with why (`off`).
- PLAN: an open problem in the file you change has its fix planned (`plan P<n> --fix`).
- DOCS: a file that calls the engine (a script, shader or scene; C++ that includes godot-cpp or Terrain3D) is changed
  only after you looked its calls up in this session: context7 (`resolve-library-id`, then `query-docs`; the hook keeps
  each lookup, and a compaction does not take it away: what it said is in the record's `docs`), or the vendored source
  in `native/godot-cpp` and `project/addons/terrain_3d`. A call not yet looked up is looked up before it is judged.

You are held to the trace you work on now alone: one you left asks nothing more of you.

`status` shows the same list (OWED). The trace folder (records, analysis) is always writable. `cat`, `sed`, `grep` and an agent's reading are not a reading. Files are changed with Edit and Write, never by a shell
command.

**Speed comes from order, never from skipping:** Reads go in parallel, in one message (the batches of `$L next`); one
`record` per batch, before the next batch; while working, only the debug DLL and the tests of the area; no measurement
or profile to look for a cause; the reading is yours, not an agent's.

## 1. What the project already knows

`$L open <slug> --request "<the user's words>"`. Then search `log/` for the symptom's words and the area's names (Grep)
and put every record that speaks of them first on the flow, with the list of what is pending:
`$L flow log/<record>.md log/pendencias.md --why "the record of this area"`. A defect the records already name is a
problem of this trace: record it with where the record says it, do not rediscover it.

## 2. Trace the flow of this one problem, end to end

The flow is whole when it goes from the trigger (the button, the frame, the tick, the load) through the command or the
step that changes the state, the state itself and everyone who reads it, to what draws it or shows it to the player.

- **By script:** `$L trace --query "words of the symptom"` or `--seed Class::method | path/file | file:line`. It puts on
  the flow, in the order the code runs: the script steps that lead in, what runs (from the compiler), the state it
  writes, and who reads that state afterwards. A symptom has two ends, what the player does and what the player sees:
  run one `trace` for each (`"store a note"` and `"note count"`), then join them by hand where they do not meet.
- **By the state, for a wrong behaviour or a wrong picture:** say the rule the player expects (`$L rules` lists them: no
  body in another, nothing floats, nothing vanishes in sight, ...), name the state it limits, and
  `$L writers Class::member --rule "..."` gives every function that writes it and every one that reads it.
- **By hand, where a script misses a link:** follow each call to its definition (the LSP tool for C++: definition,
  references, call hierarchy; Grep for scripts, scenes, data). A name carried as text (`dispatch(&"x")`, a bound
  method, a signal's handler, a data key) is a link: find who it names. Add each file in its place:
  `$L flow <file> --why "called by X at file:line" --after <caller's file>`.

The flow holds what the calls and the data of this problem lead to: never whole folders, never the whole code. A file
a script brought by a name alone, that no call and no data of this problem reaches, leaves with what shows it:
`$L off <file> --why "..."`.

## 3. Read the flow and record everything the fix will need

`$L next` gives the files in order, in batches of Read calls: send them in parallel. After each batch, before the
next, record what it showed in one call, `$L record <file.json>` (a new file under `dist/trace/<slug>/`):

```json
{
  "reads": [
    {"path": "native/src/sim/x/mover.cpp", "from": 40, "to": 96,
     "saw": "Mover::step moves each body along its lane by the gap gap_ahead (same file:20) gives; gap_ahead sees only bodies on the lane list. Sends poses with RenderingServer.multimesh_set_buffer (see docs).",
     "called_by": "Sim::tick (sim.cpp:88), on the simulation thread, once a tick",
     "state": "reads the lane lists; writes Body::pose; under traffic_mutex"}
  ],
  "docs": [
    {"api": "RenderingServer.multimesh_set_buffer", "source": "context7 /godotengine/godot-docs",
     "says": "replaces the whole buffer; the size must match instance_count times the stride; main thread or the server's own queue"}
  ],
  "problems": [
    {"where": "native/src/sim/x/mover.cpp:61",
     "what": "a body off the lanes is on no lane list, so gap_ahead never returns it and the mover drives through it",
     "cause": "who is in the way is asked of the lane lists, and one kind of mover is kept out of them",
     "symptom": "two bodies in one another",
     "fix": "gap_ahead asks one owner of every body's place (lanes and off-lane); callers to check: Mover::step, Junction::cross; test: a body parked off the lane is stopped for"}
  ],
  "note": "next: read Junction::cross, the second caller of gap_ahead"
}
```

A record is complete when someone who never read the code could make the fix from it alone:

- **Every read has three fields, all required** (a batch without one is refused): `saw`, what each function there does,
  what it calls and what guards it relies on; `called_by`, who calls it or what triggers it, with file:line, and on
  which thread; `state`, what it reads and writes, and under which lock. For a document or data: who reads it, what it
  sets. These are what a fix asks of a file later: written now, the file is not opened again to find them.
- **`docs`, one entry for every call into Godot, godot-cpp, Terrain3D or a library that the chunk makes or the fix
  will make:** look it up before judging it, always, never from memory: context7 (`resolve-library-id` once a
  library, then `query-docs` with the class, the method and the question); when context7 lacks it, the vendor's page;
  the vendored source in the tree is the last word. Write what it guarantees, on which thread, what it costs, its
  known issues. `"docs": []` only when the batch calls nothing from outside.
- **`problems`, every one seen, not only the one reported:** the exact line; what is wrong; the design decision behind
  it (ask "why" until you reach it); what the player sees; and `fix`, required: what will change, where, who else must
  be checked, and the test that will prove it (`$L plan P<n> --fix "..."` rewrites it after more reading).
- A claim in a comment, a document or a test ("this is checked", "this cannot happen") is read against the code that
  should keep it. A claim the code does not keep is a problem.
- `note`, required in a batch with reads or problems: the next action, as you would tell it to someone
  taking over now (it is what a compaction leaves of where you are).

## 4. Fix the cause

`$L problems` shows what you recorded. Problems of one decision go under one cause,
`$L cause add --what "the decision" --owner "its one owner" --plan "the correction" --problems P1 P2`, and `$L card C1`
gives the exact lines, the callers, the readers and the tests that reach them; a caller the card names that is not on
the flow is read before the change.

1. Name the check that fails now because of this cause: a test of the area, a rule of the audit
   (`python tools/tasks/cli.py run audit`), a probe of the core. If none fails, write it first and see it fail.
2. Change the decision that produces the symptom, in its one owner. If the right fix is a rewrite, rewrite. Never: a
   threshold, timeout, tolerance or thread count moved until the symptom hides; a special case; an error swallowed; a
   test's expected value rewritten to pass (a photo is no such value: step 5); the cost moved elsewhere.
3. Go through every caller and reader the record names, against the change.
4. Build the debug DLL (`tools/build/build.sh dll-debug`) and run that check: `python tools/tasks/cli.py run fast <scene>`.
   When it does not pass, do not try another change: read again until you know why, record what the reading showed.
5. `$L fix P<n> --how "what changed and why it cannot return"` (or `fix C<n>` for a cause and all its problems).

## 5. At the end: test, photograph, analyse

When every problem of this trace is fixed:

1. **Test.** The tests of what changed, then `tools/build/quick.sh`. A test that fails is read, not retried: back to
   step 3 with what it showed.
2. **Photograph the game as it is now**, whenever the change can reach the screen (render, terrain, shaders, meshes,
   light, the camera, the UI, the simulation's movers, their data). Write a plan, a JSON of `look.gd`'s steps (Read
   `project/tests/visual/look.gd` for every step): for each place the change reaches, a view from near, from mid
   distance and from far, from two angles, by day and by night, a shot each:

   ```json
   {"city": {"map": 0, "seed": 1, "generate": true},
    "steps": [{"dispatch": "set_speed", "args": [4]}, {"frames": 900},
              {"hour": 16}, {"setView": [0, 0, 8, 30, 35]}, {"shot": "junction_near_day"},
              {"setView": [0, 0, 4, 120, 30]}, {"shot": "junction_mid_day"},
              {"hour": 22}, {"setView": [0, 0, 8, 30, 35]}, {"shot": "junction_near_night"}]}
   ```

   `$L photos <plan.json>` runs it hidden, under a deadline, and prints every photo and its six enlarged crops
   (`$L crops <folder>` for photos taken another way). What you judge must be in the picture: movers need the clock
   running before the shot, and the HUD's own counters say whether there is any.

   **A photo is judged on its own, never against another photo.** Do not compare it pixel for pixel with an earlier
   run, an older commit or a kept reference (a golden), do not list the pixels that moved, and do not photograph an
   older commit as a "before": the game was broken there, and wind, noise and the machine move pixels between two runs
   of the same code. Equal bytes prove nothing about what is drawn, and a difference between two photos is not a
   finding. The only finding is what is wrong in this run's photo: something broken, out of shape, in another body,
   floating, missing or behaving wrongly. A check that asks for equal photos and fails is answered the same way: open
   this run's photos, judge them as in step 3, and when they are sound write its reference again and go on.
3. **Analyse every photo, for real.** Read every photo, in parallel, and the crops of the places the change reaches: a
   vehicle, a person, a kerb is a few pixels of the whole frame and is judged in its crop. Write `dist/trace/<slug>/photos/analysis.md`: for each
   photo, the rules of `$L rules` one by one, and for each rule what you looked at and what is there (bodies in one
   another, things floating or sunk, gaps and seams, grass where a footway should be, missing or stretched textures,
   wrong colours, hard level changes, shadows detached, lights with no source, things missing), with the crop where a
   defect shows. "Looks fine" is not an analysis.
4. **Every defect seen is a problem**, whoever caused it: `$L problem` it, trace it (step 2) and fix it. Then
   `$L close`: it closes only with photos taken, every photo opened with the Read tool, and every photo
   named in `analysis.md` (a change that cannot reach
   the screen, a tool or a document: `close --no-photos "why"`). Then the project's close (CLAUDE.md: both DLLs, `quick.sh --ready`, the gates, the executable, the record of
   the line), commit and push.

## Common mistakes

| Mistake | Instead |
|---|---|
| Reading whole folders "to understand first" | The flow of this one problem, in run order |
| A fix made from the lines a search gave | The function whole and who calls it, recorded: a hit is not the cause |
| A whole file or record read again to change a line of it | The lines to change: the notes hold the rest |
| The measure, the close and the executable after every fix | Once, at the end of the batch of fixes |
| Reading several batches, recording later | One `record` per batch: a compaction keeps only what is on disk |
| A `saw` that names the function and nothing else | Callers, thread, lock, state read and written, guards |
| A problem recorded without `fix` | What changes, where, who to check, which test |
| Judging an engine call from memory | context7 first, a `docs` entry, then the judgement |
| Measuring or profiling to find the cause | Read the flow; a measure only confirms a fix |
| Fixing the place the symptom shows | Follow the state back to the decision that writes it wrong |
| A second attempt when a fix fails | Read again; record what was missed |
| Reading the code again after a compaction | `resume`, then `notes.md` and `report.md` |
| Re-reading files another session noted, writing nothing | Your own note: the hook counts only yours |
| Photos looked at whole and called fine | The crops of what the change reaches, rule by rule, written in `analysis.md` |
| Hunting the pixels that differ from an older photo or a golden | This run's photo on its own: is anything broken, out of shape, overlapping or missing? Only that is a finding |

## Commands

| Command | For |
|---|---|
| `resume`, `status` | where the trace is |
| `open`, `list`, `switch`, `close` | one trace a problem |
| `trace`, `writers`, `flow`, `off` | finding the flow |
| `next`, `record`, `read`, `problem`, `plan`, `note` | reading and recording |
| `notes <file>`, `problems`, `export` | what was recorded, in full |
| `cause add`, `cause link`, `card C<n>` | grouping problems; exact lines, callers, tests |
| `fix`, `drop` | closing a problem |
| `rules`, `photos`, `crops` | what a game's world must keep; looking at it |

Use the skills that fit each step: `superpowers:systematic-debugging` for the cause, `superpowers:test-driven-development`
for the fix, `superpowers:verification-before-completion` before saying it is done.
