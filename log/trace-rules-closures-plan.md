# Closures Valued by the Call That Makes Them (C4) Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** A function written inside another is one value per template of its maker that binds its free variables, so the functions one factory returns keep their own captured values and their own properties.

**Architecture:** `templates.reaches` and `templates.reads` see through a closure to its free variables (C4): a maker whose returned closure reads a parameter gets templates per that parameter's values. A closure built in such a template is the value `<fn>gid~env` (`solver`: `fn_of`, `env_of`, `closure`), `env` an id the typer gives the template. Calling it runs the function in a closure frame (`templates.Frame` with `env`): its free variables read the maker's template (`places.wire_local`, `wire_name`), its parameters the unit's arguments, and its own templates nest under that frame (frames keyed by the frame they are made in). A function's own properties are keyed by function and environment (`js_transfer.fn_prop_node`).

**Tech Stack:** Python 3.12, the tracer under `tools/analysis/featuretrace`.

**Spec:** `log/trace-rules.md`, "Closures valued by the call that makes them (C4)".

## Global Constraints

- Rules name no project: ECMAScript, C++ [expr.prim.lambda.closure], GDScript lambdas; Agesen CPA; closure conversion.
- Files under 300 lines with a header; new files in `log/mapa.md`; English in the repository; no copy of the roadcraft lab.
- The call graph stays per unit: a link reaches the function; environments change values, never links.
- Records appended, never pruned; files written as bytes keeping their line endings.

## Review Focus

- A closure made in a maker no template keys (its free variables read nothing of the parameters): one value, as before.
- A closure inside a closure (`fn.setLayout = (l) => { ...; return fn; }` inside `Fn`): the inner one reads the outer's environment through the chain.
- `bind` on a closure value keeps its environment; `call`/`apply` run it in its environment.
- A closure handed to the engine and called by it: its free variables still read its maker's template.
- A GDScript or C++ lambda returned by a function that captures a parameter: the same rule.

---

### Task 1: Makers keyed by their closures' free variables; closures valued per template (C4, C4')

**Files:**
- Modify: `templates.py` (`free_names`, `reaches`/`reads` through `callable`, `Frame.env`, frame cache keyed by the frame made in), `places.py` (`wire_name` for local functions, `wire_local` free variables and arguments of a closure frame, `arg_node`), `typer.py` (`constant` of a callable in a template, the environments' registry), `solver.py` (`ENV`, `fn_of`, `env_of`, `closure`), `transfer.py` (`calls`, `result` and `function_member` run a closure value in its environment), `rules.py`
- Test: `tests/test_closures.py`, fixture `tests/fixture/jsvalues/closures.js`

**Interfaces:**
- Produces: `templates.free_names(t, unit) -> frozenset[str]`; `solver.env_of(text) -> int` (0: none); `solver.closure(gid, env) -> str`; `typer.Typer.environment(frame) -> int`, `Typer.closure_frame(unit, env) -> Frame`; `transfer.result(..., env=0)`.

- [ ] Step 1: failing tests: `memo(f)` returning `(k) => f(k)` called through two memos gives each memo its own function's result alone; a closure reading a local made from the parameter (`const box = new Box(p); return () => box`) the same; a closure inside a closure reads the outer maker's parameter; a closure whose free variables read nothing of the parameters is one value (`<fn>gid`).
- [ ] Step 2: run them: the memos' results are each other's.
- [ ] Step 3: implement as the Architecture says.
- [ ] Step 4: the whole suite passes; gates on the changed files; commit.

### Task 2: A closure's own properties and the language's calls keep its environment (C4')

**Files:**
- Modify: `js_transfer.py` (`fn_prop_node` keyed by function and environment), `js_link.py` (`expando_writes` on closure values), `transfer.py` (`bind` keeps the environment)
- Test: `tests/test_closures.py`, fixture `closures.js`

- [ ] Step 1: failing tests: `make(x)` returning `fn` with `fn.tag = x` gives each made function its own `tag`; `f.bind(o)` of a closure value runs in its environment; `f.call(o, a)` too.
- [ ] Step 2: run them: the tags are each other's.
- [ ] Step 3: implement.
- [ ] Step 4: the whole suite passes; gates; commit.

### Task 3: Measure and record

- [ ] Step 1: the city (same sources): every link that moves read on its line.
- [ ] Step 2: the lab app (the `identityMemo` merge gone: `Building.volumes` holds no `TrackStretch`), the lab with three.js (time and links against 457 s / 435,615).
- [ ] Step 3: `log/trace-rules.md`, `log/feature-trace.md`, `log/pendencias.md`, `log/mapa.md`, the ledger copy; gates; commit and push.
