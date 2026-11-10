---
name: trace-fix
description: Use when the user asks to solve, fix, correct or explain any problem, bug, defect, slowness or wrong behaviour of the game ("resolva", "corrija", "por que", a symptom report, a photo showing something wrong), before any code change, and when resuming such work after a compaction or in a new session.
---

# Trace the flow, record what fixing needs, fix the cause, test and look at the end

A problem is fixed by following the code that produces it, from what triggers it to what the player sees, and
changing the decision that causes it. One problem at a time: trace it, fix it whole, test it, look at it, then the next.

```
L="python .claude/skills/trace-fix/ledger.py"     # every command below is $L <command>
```

The ledger is your memory across compactions. It asks for nothing and blocks nothing: it keeps what you found.
`$L resume` first, in every session and after every compaction; continue from what it shows.

## 1. Trace the flow of this one problem

`$L open <slug> --request "<the user's words>"`, then find the flow:

- **By script:** `$L trace --query "words of the symptom"` or `--seed Class::method | path/file | file:line`. It puts on
  the flow, in the order the code runs: the script steps that lead in, what runs (from the compiler), the state it
  writes, and who reads that state afterwards.
- **By the state, for a wrong behaviour or a wrong picture:** say the rule the player expects (`$L rules` lists them: no
  body in another, nothing floats, nothing vanishes in sight, ...), name the state it limits, and
  `$L writers Class::member --rule "..."` gives every function that writes it and every one that reads it.
- **By hand, where a script misses a link:** start at the trigger (the button, the frame, the tick, the load) and follow
  each call to its definition (the LSP tool for C++: definition, references, call hierarchy; Grep for scripts, scenes,
  data). A name carried as text (`dispatch(&"x")`, a bound method, a signal's handler, a data key) is a link: find who it
  names. Add each file with `$L flow <file> --why "called by X at file:line" --after <caller's file>`.

Never put whole folders on the flow, and never read the whole code before fixing: the flow is what the calls and the
data of this problem lead to.

## 2. Read the flow and record everything the fix will need

`$L next` gives the files in order, in batches; read them with the Read tool, in parallel (reading is counted by
itself). As you read, record in one call, `$L record <file.json>` (a new file under `dist/trace/<slug>/`):

```json
{
  "reads": [
    {"path": "native/src/sim/x/mover.cpp", "from": 40, "to": 96,
     "saw": "Mover::step: called by Sim::tick (sim.cpp:88) on the simulation thread under traffic_mutex. Reads lane lists, writes Body::pose. Calls gap_ahead (same file:20), which sees only bodies on the lane list."}
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

A record is complete when someone who never read the code could make the fix from it:

- **`saw`, per function on the flow:** who calls it and from where, on which thread and under which lock, what state it
  reads and writes, what it calls, and what guards it relies on.
- **`problems`, every one seen, not only the one reported:** the exact line; what is wrong; the design decision behind
  it (ask "why" until you reach it); what the player sees; and **`fix`: what will change, where, who else must be
  checked, and the test that will prove it.**
- A claim in a comment, a document or a test ("this is checked", "this cannot happen") is read against the code that
  should keep it. A claim the code does not keep is a problem.

If the code you read or will change calls Godot, godot-cpp, Terrain3D or a library, look that call up in the official
documentation first (context7: `resolve-library-id`, then `query-docs`; or the vendor's page): what it guarantees, on
which thread, what it costs, its known bugs. Write what the documentation says in the `saw`.

## 3. Fix the cause

`$L problems` shows what you recorded; `$L card C<n>` (after `$L cause add`) or the problem's own `fix` gives the exact
lines, the callers, the readers and the tests. Then:

- Change the decision that produces the symptom, in its one owner. If the right fix is a rewrite, rewrite.
- Never: a threshold, timeout, tolerance or thread count moved until the symptom hides; a special case; an error
  swallowed; a reference re-blessed to pass; the cost moved elsewhere.
- Check every caller and reader the record names against the change.
- When a fix does not work, do not try another: read again until you know why, and record what the reading showed.
- `$L fix P<n> --how "what changed and why it cannot return"` (or `fix C<n>` for a cause and all its problems).

## 4. At the end: test, photograph, analyse

When every problem of this trace is fixed:

1. **Test.** The tests of what changed (`python tools/tasks/cli.py run fast <scene>`), then `tools/build/quick.sh`. A
   test that fails is read, not retried: back to step 2 with what it showed.
2. **Photograph the game as it is now**, whenever the change can reach the screen (render, terrain, shaders, meshes,
   light, the camera, the UI, the simulation's movers, their data): the element from near, from mid distance and from
   far, from several angles, by day and by night. No older photo decides anything: it may be broken.
3. **Analyse every photo, for real.** Small things do not show in a whole frame: crop and enlarge each region that
   holds vehicles, people, kerbs, junctions, building fronts, and look at the crop. For each photo, go through the
   rules of `$L rules` one by one and write what you see for each: bodies in one another, things floating or sunk,
   gaps and seams, grass where a footway should be, missing or stretched textures, wrong colours, flicker, hard level
   changes, shadows detached, lights with no source, things missing. "Looks fine" is not an analysis: name what you
   looked at.
4. **Every defect seen is a problem**, whoever caused it: record it (`$L problem`), trace it (step 1) and fix it. Then
   `$L close`, the project's close (CLAUDE.md), commit and push.

## Common mistakes

| Mistake | Instead |
|---|---|
| Reading whole folders "to understand first" | The flow of this one problem, in run order |
| A `saw` that names the function and nothing else | Callers, thread, lock, state read and written, guards |
| A problem recorded without `fix` | What changes, where, who to check, which test |
| Measuring or profiling to find the cause | Read the flow; a measure only confirms a fix |
| Fixing the place the symptom shows | Follow the state back to the decision that writes it wrong |
| A second attempt when a fix fails | Read again; record what was missed |
| Photos looked at whole and called fine | Crop, enlarge, rule by rule, write what each shows |
| Judging an engine call from memory | The official documentation, then the `saw` |

## Commands

| Command | For |
|---|---|
| `resume`, `status` | where the trace is |
| `open`, `list`, `switch`, `close` | one trace a problem |
| `trace`, `writers`, `flow` | finding the flow |
| `next`, `record`, `read`, `problem`, `note` | reading and recording |
| `notes <file>`, `problems`, `export` | what was recorded, in full |
| `cause add`, `cause link`, `card C<n>` | grouping problems; exact lines, callers, tests |
| `fix`, `drop` | closing a problem |
| `rules` | what a game's world must keep |

Use the skills that fit each step: `superpowers:systematic-debugging` for the cause, `superpowers:test-driven-development`
for the fix, `superpowers:verification-before-completion` before saying it is done.
