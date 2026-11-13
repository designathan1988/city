---
name: port
description: Use when the owner asks to port, bring or continue a functionality of the reference game into City (to port, to do the port, to continue, the next functionality, the traffic, the roads), when a block is open or in verify, when game code under native/src or project/ is about to be edited, when a check failed, or after a compaction during such work.
when_to_use: Invoke it before the first tool call of any port work and again after every compaction. The hook refuses an edit of the game's code until this skill was invoked in the session.
---

# Port one whole functionality

## Where the work stands now

!`python .claude/hooks/port_state.py`

## The rules that hold from the first call to the commit

1. **A functionality is a whole system** (the complete traffic, the complete roads): the first unchecked line of `doc/port.md`
   with every case under it. Never one row, never a button.
2. **Nothing runs while code is written.** No build, test, measurement, photo, gate or engine between `block open` and
   `block verify`. The tools refuse it; do not look for a way round them.
3. **A failed check is never run again as it is.** Read its failing lines and the code they name, with
   `superpowers:systematic-debugging`, until the cause is a `file:line` you can state. Then
   `block open --cause "<check>: <file:line>: <why>"`, fix it in the code, `block verify`, run that check once.
   A guess, a "let me try", an added print "to see" is trial and error: it is what the owner forbids.
4. **Two verifications may fail. A third stops the work**: tell the owner what failed and why each fix did not hold.
5. **Your own knowledge is not a source.** Every API the code calls is asked of Context7 first; every value of the real
   world and every design decision is searched on the internet first, and the source's own sentence is what the data's
   `_doc` cites. A figure written from memory is a defect.
6. **You do the reading.** No agent reads, traces or codes the port.
7. **The note `doc/work/<area>.md` is written before the first line of code** and as each case is coded: the owner's orders
   in their words, facts with `file:line`, each case's design, what is done, what is left. After a compaction the work goes
   on from the note, never from memory.

## The steps, in order

| # | Step | Command |
|---|---|---|
| 1 | Skills and tools loaded | Skill tool; ToolSearch `select:mcp__plugin_context7_context7__resolve-library-id,mcp__plugin_context7_context7__query-docs,WebSearch,WebFetch` |
| 2 | One look, once: the functionality's state now | `python tools/tasks/cli.py run <task>` |
| 3 | Both games traced | `python tools/tracer/port.py sheet <area>` (report: `dist/port/<area>.json`); closer: `python tools/tracer/trace.py <START> --depth N --state --root <files>` |
| 4 | Its code read whole, in both games, by you | Read; the reference: `C:\Users\Jonathan\Documents\Projects\lab\roadcraft\src` |
| 5 | Context7 for each API; the internet for each design decision and real value | `resolve-library-id`, `query-docs`, WebSearch, WebFetch |
| 6 | The note written | `doc/work/<area>.md` |
| 7 | The whole functionality coded, every case | `python tools/tasks/cli.py block open <area>` |
| 8 | Checked once, at the end | `python tools/tasks/cli.py block verify`; `tools/build/build.sh core-windows`; `python tools/tasks/cli.py run <its tests>`; photos (day and night), each opened and analysed; `python tools/tasks/cli.py run lab_test --jobs 1` alone for CPU, GPU and memory; `python tools/gates/run_gates.py --file X`; `python tools/gates/lab_scan.py` |
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
