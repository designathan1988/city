---
name: port
description: Use when the owner asks to port, bring or continue a functionality of the reference game into City (to port, to do the port, to continue, the next functionality, the traffic, the roads), when a block is open or in verify, when game code under native/src or project/ is about to be edited, when a check failed, or after a compaction during such work.
when_to_use: Invoke it before the first tool call of any port work and again after every compaction. With a block not closed the hook refuses every edit and command until this skill was invoked and the block's note Read, since the session's start or its last compaction.
---

# Port one whole functionality

## Where the work stands now

!`python .claude/hooks/port_state.py`

## The rules that hold from the first call to the commit

1. **A functionality is a whole system** (the complete traffic, the complete roads): the first unchecked line of `doc/port.md`
   with every case under it. Never one row, never a button.
2. **Defects are found by reading, never by running.** Read `tools/gates/rules.json`'s limits when the block opens (they are
   printed). The hook reads each file's gates after every edit: fix what it says there and then. No build, test,
   measurement, photo or engine between `block open` and `block verify`. Before `block verify`: read your whole diff, file
   by file, against the design in the note, fix what you find, then `block review` (the gates of every changed file).
   `block verify` is refused before the review passes.
3. **A verification is small and takes seconds.** One run of a few checks, named in one command: the functionality's own
   test, its photo plan (the town, never the 24 km city), `e2e`: six tasks at most. Then one measure (`lab_test`). The whole
   suite, a second run, a run "to see": refused by the tools, and only the owner orders them. A block is verified three
   times at most. A failed check: read its failing lines and the code they name, with `superpowers:systematic-debugging`,
   until the cause is a `file:line`; `block open --cause "<check>: <file:line>: <why>"`; fix everything found in that one
   opening; review; verify. A guess, a "let me try", an added print "to see" is trial and error.
4. **A process is seen as it runs.** A run of the project's tools is one command of its own, started with
   `run_in_background: true`, never piped through grep, tail or head, never redirected, never joined to another command
   (`&&`, `;`); watched with `python tools/tasks/cli.py status`; its output read when it ends. A file is changed with Edit or
   Write and a script is a file written with Write into the scratchpad: no heredoc, no `sed -i`, no redirect into the
   project. The hook refuses each of these; a refusal is obeyed, never worked round.
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

## The steps, in order

| # | Step | Command |
|---|---|---|
| 1 | Skills and tools loaded | Skill tool; ToolSearch `select:mcp__plugin_context7_context7__resolve-library-id,mcp__plugin_context7_context7__query-docs,WebSearch,WebFetch` |
| 2 | One look, once: the functionality's state now (the hook refuses a second run with no block open; more only on the owner's order: `block order "<the owner's own words>"`) | `python tools/tasks/cli.py run <task>` |
| 3 | Both games traced | `python tools/tracer/port.py sheet <area>` (report: `dist/port/<area>.json`); closer: `python tools/tracer/trace.py <START> --depth N --state --root <files>` |
| 4 | Its code read whole, in both games, by you | Read; the reference: `C:\Users\Jonathan\Documents\Projects\lab\roadcraft\src` |
| 5 | Context7 for each API; the internet for each design decision and real value; the domain skill of each kind of file the work will write | `resolve-library-id`, `query-docs`, WebSearch, WebFetch; Skill `godot-skills:godot-shaders-vfx`, `godot-prompter:godot-ui`, `godot-skills:godot-performance`, `godot-prompter:gdextension`, `cpp-pro`, ... |
| 6 | The note written | `doc/work/<area>.md` |
| 7 | The whole functionality coded, every case | `python tools/tasks/cli.py block open <area>` |
| 8 | Read, then reviewed | the whole diff read by you; `python tools/tasks/cli.py block review` |
| 8b | Checked once, in seconds | `python tools/tasks/cli.py block verify`; `tools/build/build.sh core-windows`; one run: `python tools/tasks/cli.py run <its test>,<its photo plan>,zoom_ladder,e2e`; each photo opened whole and looked at, part by part (buildings, roads, vehicles, ground, sky), against `doc/look`: a part that lost detail is a failed check (the hook refuses the commit without `zoom_ladder` passed and before each of its photos and each reference of `doc/look` was opened with Read after the run); one measure: `python tools/tasks/cli.py run lab_test --all --jobs 1` |
| 9 | A pass | `superpowers:verification-before-completion`; the line checked in `doc/port.md`; commit and push; `python tools/tasks/cli.py block close`; the next line at once |

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
