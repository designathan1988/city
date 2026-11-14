---
name: port
description: Use when the owner asks to port, bring or continue a functionality of the reference game into City (to port, to do the port, to continue, the next functionality, the traffic, the roads), when a block is open or in verify, when game code under native/src or project/ is about to be edited, when a check failed, or after a compaction during such work.
when_to_use: Invoke it before the first tool call of any port work and again after every compaction. With a block not closed the hook refuses every edit and command until this skill was invoked and the block's note Read, since the session's start or its last compaction.
---

# Port one whole functionality

## Where the work stands now

!`python "${CLAUDE_PROJECT_DIR}/.claude/hooks/port_state.py"`

## The rules that hold from the first call to the commit

1. **A functionality is a whole system** (the complete traffic, the complete roads): the first unchecked line of `doc/port.md`
   with every case under it. Never one row, never a button.
2. **Defects are found by reading, never by running.** Read `tools/gates/rules.json`'s limits when the block opens (they are
   printed). The hook reads each file's gates after every edit: fix what it says there and then. No build, test,
   measurement, photo or engine between `block open` and `block verify`. The code is read once, as it is written (the
   gates after each edit, the diff against the note's design); then `block review` (the gates of every changed file, a
   command of seconds) and `block verify`, refused before the review passes. No second round of reading, no review of
   the review: what is left is found by the verification's one run.
3. **A verification is small and takes seconds.** One run of a few checks, named in one command: the functionality's own
   test, its own photo plan (rule 10), `e2e`: six tasks at most. Then one measure (`lab_test`). The whole suite, a second
   run, a run "to see": refused by the tools, and only the owner orders them. A block is verified three times at most.
   A failed check: read its failing lines and the code they name, with `superpowers:systematic-debugging`, until the
   cause is a `file:line`; `block open --cause "<check>: <file:line>: <why>"`; fix everything found in that one opening;
   review; verify. A guess, a "let me try", an added print "to see" is trial and error.
4. **Every process ends by itself, in seconds, is seen as it runs, and none is left running.** Claude Code ends nothing
   here: a background command of this session has no time limit, and the call's `timeout` only moves a foreground one
   to the background. So the deadline is the process's own: a run carries `--seconds <n>` (`python tools/tasks/cli.py run
   <tasks> --seconds 120`, `tools/build/build.sh core-windows --seconds 300`: it ends itself there, its unfinished tasks
   TIMEOUT); the tracer has its own (`port.py`; `trace.py --seconds <n>`); anything else sent to the background starts
   as `timeout <n> <command>` (Bash); 300 s at most, longer only on the owner's order. A run of the project's tools is
   one command of its own, started with `run_in_background: true`, never piped through grep, tail or head, never
   redirected, never joined to another command (`&&`, `;`); watched with `python tools/tasks/cli.py status`; its output
   read when it ends. One that did not end is stopped by its ID (TaskStop) and its cause read in the code: the hook
   refuses the stop of a turn while a command runs with nothing to end it. A file is changed with Edit or Write and a
   script is a file written with Write into the scratchpad: no heredoc, no `sed -i`, no redirect into the project. The
   hook refuses each of these; a refusal is obeyed, never worked round.
5. **Two verifications may fail. A third stops the work**: tell the owner what failed and why each fix did not hold.
6. **Your own knowledge is not a source.** Every API the code calls is asked of Context7 first; every value of the real
   world and every design decision is searched on the internet first, and the source's own sentence is what the data's
   `_doc` cites. A figure written from memory is a defect. A kind of file is written with its domain skill, used before
   its first edit and again after a compaction (Skill tool, or its SKILL.md Read whole; the hook names it and its path
   when it refuses the edit): a shader, the interface, a test of the game, the renderer, the extension's boundary, every
   C++ file (CLAUDE.md lists each). Its rules and pitfalls are held to; a step of it that runs, plans or asks does not apply.
7. **You do the reading.** No agent reads, traces or codes the port.
8. **The note `doc/work/<area>.md` is written before the first line of code** and as each case is coded: the owner's orders
   in their words, facts with `file:line`, each case's design, what is done, what is left. Its first section, `## Now`, is
   the state at this moment (the owner's standing orders, where the block stands, what is in hand, the next steps),
   rewritten each time, never appended to: it is what the hook gives back whole. The hook refuses the game's code, a run
   and a commit while the note is fifteen minutes behind.
9. **After a compaction, and at a session's start with a block not closed: this skill, then the note Read whole, before
   anything else.** The hook refuses every edit and command until both are done. The summary of a compaction is no source:
   a fact is in the note, or it is read again in the code.

10. **Each functionality has its own photos, of what it is.** Before the block's verification, list what the player sees
    of the functionality (each case of its line in `doc/port.md`: the tool in hand, the state before, the action, the
    result, each variant, how it meets what is around it) and write a photo plan for it
    (`project/tests/visual/<area>_plan.json`, a task of `tools/tasks/suites.json`, in `doc/map.md`): a small scene built
    for the cases, driven through each one, photographed from near, each shot named by what it proves. After the run
    each photo is opened whole with Read and judged against its case (a photo that shows the case wrong, or does not
    show it, is a failed check: its cause is read in the code); then the photos are sent whole to the owner
    (SendUserFile), each named by what it proves. The far views of the generated town (`zoom_ladder`) are the same for
    every functionality and prove nothing of one: the tools refuse a block's commit and its close on them alone.

## The steps, in order

| # | Step | Command |
|---|---|---|
| 1 | Skills and tools loaded | Skill tool; ToolSearch `select:mcp__plugin_context7_context7__resolve-library-id,mcp__plugin_context7_context7__query-docs,WebSearch,WebFetch` |
| 2 | One look, once: the functionality's state now (the hook refuses a second run with no block open; more only on the owner's order: `block order "<the owner's own words>"`) | `python tools/tasks/cli.py run <task> --seconds 120` |
| 3 | Both games traced, in seconds | `python tools/tracer/port.py sheet <area>` (report: `dist/port/<area>.json`); closer: `python tools/tracer/trace.py <START> --depth N --state --root <files> --seconds 60` |
| 4 | Its code read whole, in both games, by you | Read; the reference: `C:\Users\Jonathan\Documents\Projects\lab\roadcraft\src` |
| 5 | Context7 for each API; the internet for each design decision and real value; the domain skill of each kind of file the work will write | `resolve-library-id`, `query-docs`, WebSearch, WebFetch; Skill `godot-skills:godot-shaders-vfx`, `godot-prompter:godot-ui`, `godot-skills:godot-performance`, `godot-prompter:gdextension`, `cpp-pro`, ... |
| 6 | The note written, with the list of what will be photographed (rule 10) | `doc/work/<area>.md` |
| 7 | The whole functionality coded, every case, with its test and its own photo plan | `python tools/tasks/cli.py block open <area>` |
| 8 | Reviewed, once | `timeout 120 python tools/tasks/cli.py block review` |
| 8b | Checked once, in seconds | `python tools/tasks/cli.py block verify`; `tools/build/build.sh core-windows --seconds 300`; one run: `python tools/tasks/cli.py run <its test>,<its own photo plan>,e2e --seconds 180`; each photo of the plan opened whole with Read and judged against its case, then sent whole to the owner (SendUserFile); one measure: `python tools/tasks/cli.py run lab_test --all --jobs 1 --seconds 120` |
| 9 | A pass | `superpowers:verification-before-completion`; the line checked in `doc/port.md`; commit and push (the hook refuses the commit, and the tool the close, before the block's own plan passed, each of its photos was opened after the run and one was sent to the owner); `python tools/tasks/cli.py block close`; the next line at once |

## What the design must hold (CLAUDE.md, Architecture and Never)

- The reference's behaviour, never its code: no line, structure, name or data key of it.
- None of its costs: no whole-map work on an edit or a frame, no per-frame work on unchanged inputs, no allocation or
  search in a hot loop, no quadratic proximity.
- State in `CityState`, changed only by `sim/commands`, advanced only by `sim/simulation`; the render only reads; every
  value in `project/data/**` with its schema; no number in the code.
- A cause is fixed where the design decision is wrong, whatever its size: never a tolerance moved, a special case, a check
  loosened.

## Red flags: stop and go back to the rule

| The thought | What it is |
|---|---|
| "Let me run it to see what happens" | Trial and error. Read the code. |
| "I think the cause is X, let me fix and rerun" | A guess. State the `file:line` and why, or keep reading. |
| "I know this API / this norm's value" | Memory. Ask Context7, search the internet, cite the sentence. |
| "Just a quick build to check it compiles" | A run while coding. The editor's diagnostics show errors without a build. |
| "This case is too big, I'll leave it" | The functionality is whole. Say so to the owner before the block, not after. |
| "I'll write the note later" | A compaction loses everything not in it. Write it now. |
| "An agent can read this for me" | The owner forbade it. Read it. |
| "A quick heredoc to patch this file" | It eats backslashes and broke the hook. Edit, or a script written with Write. |
| "I'll pipe the run through grep to keep it short" | A muted run. Start it alone, in the background; read its output at the end. |
| "Verify and run in one command" | Joined commands: one waits on the other and one result hides the other. One each. |
| "The hook refused; another tool does the same" | A way round. The refusal is the owner's order: do what it says. |
| "This is a good place to stop and report" | The block is not closed. Go on with the note's next step. |
| "The build and the tests will tell me what is wrong" | The reading tells you. The gates after each edit, your diff read whole, `block review`. |
| "Run the whole suite to be sure" | Minutes, and the owner forbade it. Its own test, its photo, `e2e`: seconds. |
| "A small fix, verify again" | Three verifications a block. Find everything in one reading, fix it in one opening. |
| "It is running, I will wait for it" | For how long? A process with no deadline of its own waited seven hours. `--seconds`, or `timeout <n>`. |
| "The call has a `timeout`, that is its deadline" | Claude Code does not apply it in the background. The deadline is the process's own. |
| "Let me read the diff once more to be sure" | A round of re-review. One reading as it is written, one `block review`, the one run. |
| "The usual photos of the town will do" | They show nothing of what was built. The functionality's own plan, each case from near. |
| "I noted the owner's correction in memory" | A memory is read, not obeyed. The rule goes into the hook, the tool and this skill, with its test. |
