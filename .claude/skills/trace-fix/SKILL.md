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

## The rules of the game's world

A game shows a world, and a world has rules every player takes for granted: a car is never inside another, nothing floats,
nothing vanishes while it is looked at, a shadow touches what casts it. `$L rules` lists them (rules.json): each with what
breaks it in a photo and the state it limits in the code. You know these rules; the skill makes you apply every one, both ways:

- **In the code.** Every rule limits some state, and every function that writes that state must keep the rule.
  `$L writers <Class::member> --rule "..."` gives, from the compiler, every function that writes it and every function that
  reads it, and puts them on the flow. A writer that moves the state without the guard the rule needs is the cause: read each
  one asking "what stops this from breaking the rule here?". Any defect of behaviour or of the picture is found this way, in
  the code, before the game is ever run.
- **In a photo.** Every photo is judged against every rule, one by one, never by a glance at the whole frame. What a rule
  forbids and the photo shows is a problem, whoever caused it.

A rule broken is a problem with a cause in the code. When its cause is fixed, the rule becomes a check of the core, run in a
test of seconds, that fails while any writer breaks it.

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
What a C or C++ seed runs comes from the architecture analyser, which reads the compiler's own tree of the code on disk:
calls through base classes, templates, lambdas and jobs are on the route, in program order (the first trace after a C++
change waits for its scan, a few minutes). Scripts, scenes and data come from the tracer by name. The tests are left out
(`--include project/tests` brings them in). What the reading still shows beyond the route goes on with `flow`.

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

## 3. Read the route, record what is wrong

Work one problem at a time: its route, its causes, their fixes. Do not put the whole code on the flow to read it all
before fixing anything; a request for "every problem" is a list of symptoms, each traced, fixed and closed in turn.

1. `$L next` gives a batch: up to 25 chunks (at most 400 lines each), each with its file, `offset`, `limit` and the
   functions of it that are on the route. Read exactly those with the Read tool, **all in one message, in parallel**.
2. **Reading is recorded by reading.** A hook logs every Read call and the ledger counts those lines as read: there is
   no command to run for it. Only `cat`, `sed`, `grep`, a subagent, or a Read of more than 400 lines do not count.
3. Record what the chunks show, in one call, `$L record <file.json>` (written with the Write tool as a new file under
   `dist/trace/<slug>/`), whole or not at all:

```json
{
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

   Every problem of those chunks, **not only the one the user reported**; what the chunks call or feed that is not on the
   flow yet; the next action. A note on a chunk (`"reads": [{"path", "from", "to", "saw"}]`) is optional: write one only
   where it will save a later reading.

Rules of the reading:
- **Read it yourself.** No subagent reads for you, no sampling, no "the rest is similar", no grep in place of reading.
- **No fixing before the cause's own flow is read.** `$L fix` refuses a cause while a file of its problems, of their callers
  or of the readers of what they write is unread (the fix card puts those on the flow).
- **Outside API in the chunk: the official documentation, always.** When the chunk calls Godot, godot-cpp, Terrain3D or any
  library, look the call up with context7 (`resolve-library-id`, then `query-docs` for Godot 4.7) or the vendor's page
  before judging it: what it guarantees, on which thread it may run, what it costs, its known limits and known bugs. A hook
  logs every lookup, and `$L fix` refuses a cause that names an API no lookup names. Never judge such a call from memory.
- **A claim is checked, never taken.** A document, a header comment, a test's own description or a log that says something is
  checked, guarded or done is read against the code that should do it. A claim the code does not keep is a problem
  (the document contradicts the code), recorded where the claim is.
- **A check proves only what it can fail on.** Before taking a test, a probe, an audit or a gate as proof that a defect is
  absent, read it: does it cover the entities the defect involves (not a sample, not one kind), is its threshold able to
  catch it (a distance between centres does not catch two bodies crossing), does it run on the path the defect takes? A
  check that cannot fail for this defect proves nothing; the missing check is a problem of its own.
- **Every chunk is read against the rules of the world** (`$L rules`): for the state the chunk writes, which rules limit it
  and what in this code keeps each. A write with no guard for a rule that applies is a problem, recorded at that line.
  For a symptom the player reports, name the rule it breaks and the state, and `$L writers <state> --rule "..."` puts every
  writer and reader on the flow.
- Ask "why" of each problem until you reach the design decision; that goes in `"cause"`.
- A problem that further reading proves not to be one: `$L drop <id> --why "..."`.

## 4. Causes: the decision, its owner, the correction, what it touches, what it can break

Many symptoms come from one decision. As soon as the route of a problem is read, put its problems under a cause:

`$L cause add --what "the design decision" --owner "the one file or module that owns the correction" --plan "the correction that changes the decision, and why the symptoms cannot return another way" --api "Class.method, ... or none" --risks "what the fix can break, and the test or photo that guards each" --problems P3 P7`

`--api` names every outside API the fix touches (each needs a documentation lookup, or `none`). `--risks` is the forecast:
think of the request as one node of a net. Who else calls this, who reads what it writes, which other flows pass here, which
saves, tests, tiers and platforms see it; for each, what guards it. `$L cause link`, `$L cause set` correct a cause.

## 5. Fix, cause by cause

1. **`$L card C<n>`** computes what the fix needs, so nothing is found again: the function holding each problem with its
   exact lines, who calls it, the state it writes and who reads it, every other place that writes its name (a call through
   a base class or by name shows there), the test scenes that reach it, and whether it can reach the screen. It puts the
   callers' and readers' files on the flow: read what is unread of them (`$L next`).
2. Read the line ranges the card names, write the test that fails, make the change, check every caller, reader and mention
   the card lists. Follow CLAUDE.md (its sections on fixing the cause and on the architecture rules): no threshold, timeout,
   tolerance or thread-count change, no special case, no swallowed error, no `--bless` to pass, no moved cost. If the right
   fix is a rewrite, rewrite. If a fix does not work: no second attempt by trial; read again until you understand.
3. Run the tests the card lists (`python tools/tasks/cli.py run fast <scene>`).
4. **If the change can reach the screen** (render, terrain, shaders, meshes, light, the camera, the UI, or their data and
   assets, whatever the purpose of the fix): photograph the game as it is now, the element from near, from mid distance
   and from far, from several angles, by day and by night, into `dist/review-c<n>/`. No comparison with an older picture
   decides anything: the older one may be broken. Look at every photo yourself, with attention, as someone who plays this
   game: what is in it, element by element (land, roads, kerbs and footways, junctions, buildings, lots, trees, water,
   vehicles, people, light, shadows, sky), whether each is right for this game, and anything wrong, whoever caused it
   (floating or sunk things, gaps and seams, grass where a footway should be, overlaps, missing or stretched textures,
   wrong colours, flicker, hard level changes, shadows detached or missing, lights with no source, empty places). A problem
   the fix brought goes back to the reading; any other is recorded with `$L problem`. Then answer every rule of `$L rules`
   from the photos, in a JSON sheet (`dist/review-c<n>/rules.json`): `"B1": "ok"`, `"not in view"`, or the id of the
   problem recorded for it (`"P12"`). Zoom into the photo for the small things (vehicles, people, kerbs): crop and enlarge
   the region and look at the crop, not at the whole frame.
5. `$L fix C<n> --how "what changed, why it cannot return" --commit <sha> [--tests <task> ...] [--photos dist/review-c<n> --seen "what each photo shows and every problem seen" --checked dist/review-c<n>/rules.json]`,
   then commit and push by theme. The ledger checks on disk, and refuses otherwise: the cause's own flow read in whole, a
   documentation lookup for each API it names, its risks written, a test PASS recorded by the orchestrator after the last
   edit of the changed files, and, when a changed file can reach the screen, at least three photos taken after that edit
   with their analysis and every rule of the world answered.

## 6. Close

When every cause of the request is fixed: `$L gate close`, then the project's close (`quick.sh --ready`, the full gates,
the new executable, the line's record in `log/` from `report.md` of `$L export`, commit and push). Deliver the report to
the user: causes, their problems, what changed, the proof of each. Never say read, tested or fixed about what the ledger
and the output do not show.

## Forbidden: stalling

| What you are tempted to do | What you do instead |
|---|---|
| Measure, profile or sample to find where the problem is | Read the flow. A measurement only checks a fix its cause already explains |
| Run the suite or the game "to see what happens" | Read the flow. Run only the test that proves a fix |
| Take captures to investigate | The rule, the state it limits and its writers (`writers`); a check of the core in a test of seconds |
| Write an analyser, a script or a tool to look for the cause | Read the flow |
| Read the first 100 lines and the functions that look relevant | `$L next` until `gate read` passes |
| Send agents to read and summarise | Read it yourself; agents cost credits and their summary is not a reading |
| Read the whole code before fixing anything | One problem's route at a time: trace, read, cause, card, fix |
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
