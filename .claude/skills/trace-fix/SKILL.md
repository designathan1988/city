---
name: trace-fix
description: Use when the user asks to solve, fix, correct or explain any problem, bug, defect, slowness or wrong behaviour of the game ("resolva", "corrija", "por que", a symptom report), before any measurement, profile, test run, capture or code change, and when resuming such work after a compaction or in a new session.
---

# Trace, read in whole, record, then fix

The problem is in the code. You find it by following the flow that produces the symptom and reading every file
on that flow **in whole, chunk by chunk**, recording each chunk and each problem **on disk** as you go. Only when
the whole flow is read do you fix, by cause. Measuring, profiling, running long tests, taking captures or writing
new tools to *find* a problem is stalling, and it is forbidden here.

The ledger is your memory. Your context will be compacted; the ledger will not. What is not in the ledger was
not read and was not found.

```
L="python .claude/skills/trace-fix/ledger.py"     # every command below is $L <command>
```

## 0. First action, always: resume

Run `$L resume` before anything else, in every session and after every compaction (a session hook also prints it).
It is always a short answer: the request, the next action, what to do now, the progress by folder, the causes and
the open problems. The detail is asked for, never dumped:
`$L notes <file or folder>` (what was read there), `$L problems` (full text), `$L export` (writes `report.md` and
`notes.md` in the trace's folder: read them with the Read tool). A listing that is cut says how to continue (`--from`).
- A trace is open: continue it from the recorded point. Never read again what the ledger shows as read; `notes`
  gives what those chunks do.
- No trace open and the user asked to solve a problem: go to step 1.
- The user asks for a different problem while one is open: `$L open` a new one (the old one stays, `$L list`,
  `$L switch`).

## 1. Open

`$L open <slug> --request "<the user's words, verbatim>"`. Then say, in one list, which skills and project tools
you will use and where (table at the end). Do not ask for approval; stop only if something truly blocks.

Add `--reuse` only when the user says an earlier reading may be reused. Then a file that has not changed since
another trace read it in whole is offered by `next` as known: `$L notes <file> --prior` shows what was read and
found there, and `$L carry <file> --saw "what of it matters for this symptom"` counts it as read. Those notes were
written with another symptom in mind: when the file is at the heart of this one, read it instead. Without `--reuse`
everything is read anew.

## 2. Trace the flow

State the symptom as the player sees it. Find where that flow **starts** (the player's action, the tick, the
frame sync, the load).

**Start with the route the script finds.** `$L trace --query "words of the symptom"` (or `--seed Class::method`,
`--seed path/file`, `--seed file:line`; `--dry` only shows it) asks the repository's feature tracer and puts on the flow,
in the order the code runs, every file involved: the entries that trigger it, the seeds, what they run, the
infrastructure they use, the state they write and who reads that state later, each file with why it is there. `next`
then gives the reading batches along that route and names, per chunk, the functions of it that are on the route.
The tracer resolves by name and leaves out what only runs inside the calls (`--deep` adds it) and the tests
(`--include project/tests`): the route is where to start, in order, not the whole truth. Complete it as you read.

**Locate what the route misses with the code's own index, not by guessing.** In the C++ core use the LSP tool (clangd; `native/.clangd` names
the compilation database it indexes): go to definition, find references, incoming and outgoing calls.
That gives who really calls a function and what it really calls, through overloads, templates and virtual calls that
a text search misses. Grep and Glob locate what clangd does not index: GDScript (`CityStore.dispatch`, the `CityCore`
methods it calls), shaders, scenes and data keys. Locating is never reading.

Put files on the flow **in the order the code runs**, each with the reason it is there:

`$L flow <file|folder|glob> --why "called by X::y / writes the field Z that W reads" [--after <file on the flow>]`

`--after` puts a callee right after its caller; without it the file goes to the end. A whole folder goes on the flow
only when the request is about the whole of it; for one symptom, the flow is the files the calls and the data lead to.

Follow, as you read:
- **calls:** every function of this project called from what you read: its file goes on the flow;
- **data:** for every field read, who writes it; for every field written, who reads it; data files (`project/data/**`)
  that feed a value on the flow are part of the flow;
- **threads and locks:** which thread runs it, which mutex it holds (CLAUDE.md's section on data flow and threads).

The project's map of flows is `log/mapa.md`, `log/feature-trace.md` and CLAUDE.md: read the part of your line
first, then confirm it in the code (a record is history, not truth).

The flow ends where the player sees the result (HUD, instance buffer sent to the GPU, pixels) or where the state
is stored. It grows while you read: a newly found callee is added with `flow` at once.

## 3. Read in whole, in chunks

Loop until `$L next` says everything is read:

1. `$L next` gives a batch: up to 25 chunks (at most 400 lines each, about 2000 lines in all), each with its file,
   `offset` and `limit`. Read exactly those with the Read tool, **all in one message, in parallel**.
2. Write the batch's record as a new JSON file with the Write tool (`dist/trace/<slug>/batch-<n>.json`) and record it
   in one call, `$L record <that file>`. It is recorded whole or not at all, and answers with the next batch:

```json
{
  "reads": [
    {"path": "native/src/render/render_world.cpp", "from": 1, "to": 344,
     "saw": "what this chunk does, what it calls, which state it reads and writes, on which thread"}
  ],
  "problems": [
    {"where": "native/src/render/render_world.cpp:98", "what": "what is wrong",
     "cause": "the design decision that produces it", "symptom": "what the player sees"}
  ],
  "flow": [
    {"paths": ["native/src/render/chunk_queue.h"], "why": "called by RenderWorld::sync", "after": "native/src/render/render_world.cpp"}
  ],
  "note": "where I am and what I do next"
}
```

   - `reads`: every chunk of the batch, each with its `saw`.
   - `problems`: every problem of those chunks, **all of them, not only the one the user reported**.
   - `flow`: what the chunks call or feed that is not on the flow yet.
   - `note`: the next action, for whoever resumes. Keep it current.
3. Next batch. The single commands remain for one item: `$L read <file> <from> <to> --saw "..."`,
   `$L problem <file>:<line> --what --cause --symptom`, `$L flow`, `$L note "..."`.

Rules of the reading:
- **Read it yourself.** No subagent reads for you, no sampling, no "the rest is similar", no grep in place of reading,
  no summary from memory or from a log. A file is read when its ranges cover line 1 to the last.
- **Record before moving on.** A chunk whose `read` was not recorded was not read: after a compaction you will
  have to read it again.
- **The reading is checked.** A hook logs every Read tool call (file, lines, content hash), and `$L read` refuses a
  chunk whose lines the Read tool did not return, in calls of at most 400 lines, for the file as it is now. Output of
  `cat`, `sed`, `grep` or of a subagent proves nothing. When `read` is refused, read those lines and record again;
  never work around it.
- **No fixing while reading.** A fix made on half the flow is a patch on a symptom. Record and continue.
- **External API in the chunk: context7, always.** When the chunk calls Godot, godot-cpp, Terrain3D or any other
  library (a `RenderingServer` call, a node or resource method, a GDExtension binding, a shader built-in), look the
  call up with context7 (`resolve-library-id`, then `query-docs` for the project's version: Godot 4.7) before recording
  the chunk: what it guarantees, on which thread it may run, what it costs, its known limits. Never judge such a call from
  memory. Once per API: write what the documentation says in the chunk's `--saw`, so it is not looked up again, and
  record a `problem` where the code uses the API against it. If context7 has nothing on it, the official documentation.
- Ask "why" of each problem until you reach the design decision; that goes in `--cause`.
- A problem that further reading proves not to be one: `$L drop <id> --why "..."`.

`$L gate read` closes the reading. It fails while any line of the flow is unread, and `$L fix` refuses before it passes.

## 4. Understand and plan: problems under causes

Many symptoms come from one decision. Read the problems in full (`$L problems`, or `report.md` after `$L export`) and
put every open one under a cause:

`$L cause add --what "the design decision" --owner "the one file or module that owns the correction" --plan "the correction that changes the decision, and why the symptoms cannot return another way" --problems P3 P7 P12`

`$L cause link C2 P9 P15` adds problems to a cause; `$L cause set C2 --plan "..."` corrects its text. For the user's
symptom write, in the cause's plan, the chain *symptom ← code (file:line) ← design decision*. Order the work so that
what contaminates the checking of the others goes first, and `$L note` that order.

`$L gate plan` closes the plan. It fails while an open problem has no cause, or a cause has no owner or no plan, and
`$L fix` refuses before it passes. If a cause is not on the flow you read, the flow was wrong: add the files (the
gates open again), read them, do not guess.

## 5. Fix, cause by cause

Follow CLAUDE.md (its sections on fixing the cause and on the architecture rules): no threshold, timeout, tolerance or thread-count
change, no special case, no swallowed error, no `--bless` to pass, no moved cost. If the right fix is a rewrite, rewrite.
For each cause: the test that fails before and passes after, the change, the check of what the change reaches
(CLAUDE.md verification table, the fastest test that proves it), then

**A fix that can reach the screen is proven by photos, every time.** When the change touches render, terrain, shaders,
meshes, light, the camera or the data that feeds them (`project/data/render`, `project/data/game`), whatever its purpose
(a performance fix on the land's heights can still move the land): photos of the element before the change and after it,
from near and from several angles, by day and by night where light is involved, into `dist/review-c<n>/before` and
`/after`. Look at each picture yourself.
- **The before photo is not the reference.** It may show the very defect being fixed, or another one. It only shows what
  changed. Never make the after match the before, and never count "equal to before" as correct.
- **The after photo is judged on its own:** is what the player sees right? The reference of right is the Roadcraft lab in
  the same view and the project's own rules (CLAUDE.md, the user's memory on street edges), not the old picture.
- **Every difference is explained by its cause** (`tools/build/photodiff.py` says where to look): the ones the fix was
  meant to make, and any other. A fix meant to change nothing on the screen that changed something is explained too: it
  is either a defect the fix brought (back to the reading) or a defect the before had (say which, with the code).
- A defect seen in either picture that this cause does not own is recorded as a problem, not ignored.
No photos, no `fix`.

`$L fix C<n> --how "what changed, why it cannot return" --commit <sha>` and commit and push by theme. A fix goes by
cause: it closes every problem under that cause, so before it check each of them in the changed code. A problem the
correction did not reach belongs to another cause: `$L cause link` it there first.

If a fix does not work: no second attempt by trial. Read again until you understand, record what the reading showed.
A problem found while fixing is recorded like any other (`$L problem ... --of C<n>` when its cause is known; without
`--of` the plan gate opens again until it has one).

## 6. Close

`$L gate close` (fails with an open problem or with no cause fixed). Then the project's close: `quick.sh --ready`, the
full gates, the new executable, the line's record in `log/` from `report.md` (`$L export`), commit and push. Deliver
the report to the user (causes, their problems, what changed), plus what was left unverified. Never say read, tested
or fixed about what the ledger and the output do not show.

## Forbidden: stalling

| What you are tempted to do | What you do instead |
|---|---|
| Measure, profile or sample to find where the problem is | Read the flow. A measurement only checks a fix its cause already explains |
| Run the suite or the game "to see what happens" | Read the flow. Run only the test that proves a fix |
| Take captures to investigate | Read the code that draws it. Photos only prove a visual fix |
| Write an analyser, a script or a tool to look for the cause | Read the flow |
| Read the first 100 lines and the functions that look relevant | `$L next` until `gate read` passes |
| Send agents to read and summarise | Read it yourself; agents cost credits and their summary is not a reading |
| Fix the first problem found and go on reading later | Record it; fix after `gate read` |
| "I already read this before the compaction" | If the ledger does not show it, read it. If it shows it, do not read it again |
| Ask the user whether to continue | Continue |
| Read another session's transcript to see what it did | Its branch, diff, files and logs |

## Skills and tools to use (invoke with the Skill tool; say at the start which and where)

| Step | Use |
|---|---|
| Trace and read | this skill; `superpowers:systematic-debugging` for its root-cause discipline (its "reproduce" and "gather evidence" steps are done here by reading, not by running) |
| Who calls what, in the C++ core | the LSP tool (clangd): definition, references, incoming and outgoing calls; Grep only for GDScript, shaders, scenes and data |
| Any Godot, godot-cpp, Terrain3D or library call, while reading it and again before writing code that calls it | context7 (`resolve-library-id`, `query-docs`), mandatory; the official documentation when context7 lacks it; never memory alone |
| Plan with several causes | `superpowers:writing-plans`; `engineering:architecture` or `engineering:system-design` when the cause is a design to replace |
| Fix | `superpowers:test-driven-development` (the failing test first) |
| Before saying it is done | `superpowers:verification-before-completion`, then `/code-review` on the diff and `simplify` |
| Project tools | `tools/tasks/cli.py run fast <scene>`, `tools/build/quick.sh`, `tools/gates/run_gates.py --file` (CLAUDE.md's table of commands) |

Order of authority: the user's message, then CLAUDE.md, then this skill, then the other skills. Where another skill asks
for measurement or reproduction to investigate, this skill wins: read.
