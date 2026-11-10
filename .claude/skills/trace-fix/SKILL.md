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
planned and the fix made). After a compaction Read those two files: the code is not read twice.

**Everything is read whole, with the Read tool:** the records of the area in `log/`, every file of the flow, every
file you change, from the first line to the last (CLAUDE.md is already in your context: its rules bind every step).
Two hooks hold this, and their refusal lists the Read calls that are missing (send them all in one message, then the
same edit again):

- an Edit or Write of an existing file is refused until this session has read that file whole, as it is now, since
  its last compaction;
- while a file of the repository that you opened is left read in part, every edit is refused.

`cat`, `sed`, `grep` and an agent's reading are not a reading. Files are changed with Edit and Write, never by a shell
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
  writes, and who reads that state afterwards.
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
     "saw": "Mover::step: called by Sim::tick (sim.cpp:88) on the simulation thread under traffic_mutex. Reads lane lists, writes Body::pose. Calls gap_ahead (same file:20), which sees only bodies on the lane list. Sends poses with RenderingServer.multimesh_set_buffer (see docs)."}
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

- **`saw`, per function on the flow:** who calls it and from where, on which thread and under which lock, what state it
  reads and writes, what it calls, and what guards it relies on.
- **`docs`, one entry for every call into Godot, godot-cpp, Terrain3D or a library that the chunk makes or the fix
  will make:** look it up before judging it, always, never from memory: context7 (`resolve-library-id` once a
  library, then `query-docs` with the class, the method and the question); when context7 lacks it, the vendor's page;
  the vendored source in the tree is the last word. Write what it guarantees, on which thread, what it costs, its
  known issues. `"docs": []` only when the batch calls nothing from outside.
- **`problems`, every one seen, not only the one reported:** the exact line; what is wrong; the design decision behind
  it (ask "why" until you reach it); what the player sees; and `fix`: what will change, where, who else must be
  checked, and the test that will prove it.
- A claim in a comment, a document or a test ("this is checked", "this cannot happen") is read against the code that
  should keep it. A claim the code does not keep is a problem.
- `note`: the next action, as you would tell it to someone taking over now.

## 4. Fix the cause

`$L problems` shows what you recorded; `$L card C<n>` (after `$L cause add`, for problems of one decision) gives the
exact lines, the callers, the readers and the tests that reach them.

1. Name the check that fails now because of this cause: a test of the area, a rule of the audit
   (`python tools/tasks/cli.py run audit`), a probe of the core. If none fails, write it first and see it fail.
2. Change the decision that produces the symptom, in its one owner. If the right fix is a rewrite, rewrite. Never: a
   threshold, timeout, tolerance or thread count moved until the symptom hides; a special case; an error swallowed; a
   reference re-blessed to pass; the cost moved elsewhere.
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
   running before the shot, and the HUD's own counters say whether there is any. No older photo decides anything: it
   may be broken.
3. **Analyse every photo, for real.** Read every photo and every crop, in parallel: a vehicle, a person, a kerb is a
   few pixels of the whole frame and is judged in the crop. Write `dist/trace/<slug>/photos/analysis.md`: for each
   photo, the rules of `$L rules` one by one, and for each rule what you looked at and what is there (bodies in one
   another, things floating or sunk, gaps and seams, grass where a footway should be, missing or stretched textures,
   wrong colours, hard level changes, shadows detached, lights with no source, things missing), with the crop where a
   defect shows. "Looks fine" is not an analysis.
4. **Every defect seen is a problem**, whoever caused it: `$L problem` it, trace it (step 2) and fix it. Then
   `$L close`, the project's close (CLAUDE.md: both DLLs, `quick.sh --ready`, the gates, the executable, the record of
   the line), commit and push.

## Common mistakes

| Mistake | Instead |
|---|---|
| Reading whole folders "to understand first" | The flow of this one problem, in run order |
| A file opened at the lines a search gave | The file whole; the hooks refuse the edit otherwise |
| Reading several batches, recording later | One `record` per batch: a compaction keeps only what is on disk |
| A `saw` that names the function and nothing else | Callers, thread, lock, state read and written, guards |
| A problem recorded without `fix` | What changes, where, who to check, which test |
| Judging an engine call from memory | context7 first, a `docs` entry, then the judgement |
| Measuring or profiling to find the cause | Read the flow; a measure only confirms a fix |
| Fixing the place the symptom shows | Follow the state back to the decision that writes it wrong |
| A second attempt when a fix fails | Read again; record what was missed |
| Reading the code again after a compaction | `resume`, then `notes.md` and `report.md` |
| Photos looked at whole and called fine | Every crop, rule by rule, written in `analysis.md` |

## Commands

| Command | For |
|---|---|
| `resume`, `status` | where the trace is |
| `open`, `list`, `switch`, `close` | one trace a problem |
| `trace`, `writers`, `flow`, `off` | finding the flow |
| `next`, `record`, `read`, `problem`, `note` | reading and recording |
| `notes <file>`, `problems`, `export` | what was recorded, in full |
| `cause add`, `cause link`, `card C<n>` | grouping problems; exact lines, callers, tests |
| `fix`, `drop` | closing a problem |
| `rules`, `photos`, `crops` | what a game's world must keep; looking at it |

Use the skills that fit each step: `superpowers:systematic-debugging` for the cause, `superpowers:test-driven-development`
for the fix, `superpowers:verification-before-completion` before saying it is done.
