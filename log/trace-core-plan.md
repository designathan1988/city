# The Feature Tracer's Core Redesigned Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** One generic analysis over one core IR traces any feature of a GDScript, C++ or JavaScript/TypeScript program, its platforms modeled from their official declarations, so that no program needs a rule of its own.

**Architecture:** The readers, the index and the scopes stay (their syntax coverage is complete and gated). A new package `featuretrace/heap` lowers each unit's facts (refs and expression trees) into inclusion constraints over abstract objects — allocation sites (literals, arrays, `new`, functions and closures, classes and their prototypes, modules, the language's primitive kinds, objects of declared types) with fields per object — and solves them with difference propagation and finite contexts (argument tuples for functions, the receiver's site for methods, heap cloning one level). Calls, reads and writes become links of the same `Edge` form `walk` and `report` read. The platforms are what their declarations say: TypeScript's `lib.*.d.ts` and the packages' `.d.ts` read by the same JS reader as programs, each declared signature a generic summary (type-parameter places, callbacks, results), Godot's `extension_api.json` read the same way; what no declaration states is a generic summary derived from the declarations themselves (the placeholder-library principle of Ali and Lhoták's Averroes: what a declared function receives can reach the places and results its declared types admit, and the callbacks it receives are called with the values their declared parameters admit), never a list of functions.

**Tech Stack:** Python 3.12, tree-sitter (TSX, C++, GDScript), the tracer under `tools/analysis/featuretrace`.

**Spec:** `log/trace-rules.md`, "Why the tracer never converges: the method and the algorithm".

## Global Constraints

- Rules name no project and no library: the languages' specifications, the platforms' declarations, the analysis literature (Andersen; Doop; Agesen CPA; object sensitivity).
- **The tracer is generic, never "engessado" (owner, binding):** no rule, table, list, name set or branch for a program, library, framework, API member or code pattern, in code, plans or records. The languages' own data (node kinds, keywords, operators, the intrinsics a literal is made with) lives outside `heap`, cited to its specification; `heap` holds no table of names and compares no value with a name (`tests/test_generic.py` fails otherwise); the old core's tables are `DEBTS` there and only shrink.
- An opaque value only where no declaration exists, and every use of one counted in the report.
- Termination by design: finite sites, finite contexts; no deadline, threshold or cut decides a result.
- Files under 300 lines with a header; new files in `log/mapa.md`; English in the repository; repo files edited with the Edit tool or as bytes (never `write_text`).
- The current core stays the baseline until the new one holds its correct links on the city, the lab app and the lab with three.js; then it is retired.
- No copy of the roadcraft lab: it is input only.

## Review Focus

- A value no allocation site makes (a parameter of an entry point the engine calls): its declared type's object, or the engine's when undeclared, counted.
- A property read on an object whose class the program does not declare (a host object): the declared member when a declaration names it.
- A dynamic key (`o[k]`) read and written with keys the analysis cannot tell: every property of the objects, both ways.
- A generic declared method with a callback (`map`, `then`, `forEach`, `reduce`): the callback called with the receiver's places, its results in the method's result.
- Recursion through closures and factories (`spin(wrap(f))`, three's `Fn`): finitely many contexts, the same links each run.

---

### Task 1: Abstract objects, inclusion solver and core lowering for JavaScript (context-insensitive)

**Files:**
- Create: `heap/__init__.py`, `heap/objects.py` (site kinds, interning), `heap/solver.py` (points-to sets of variables and fields, difference propagation, cycle merging by reuse of `cycles`), `heap/lower.py` (refs and trees of a unit to constraints), `heap/edges.py` (links in `resolve.Edge` form; `Result` with `ix`, `edges`, `out`, `inn`, `external`, `unreached`), `heap/analysis.py` (`analyse(index) -> Result`)
- Test: `tests/test_heap_core.py`, fixture `tests/fixture/heap/core.ts`

**Interfaces:**
- Produces: `heap.analysis.analyse(index, progress=None) -> Result`; `Result.points(gid: int, tree: tuple) -> set[str]` (the sites a tree can hold in a unit, as readable site names: `new:<class key>@<line>`, `lit@<file>:<offset>`, `arr@<file>:<offset>`, `fn:<gid>`, `class:<key>`, `prim:<kind>`).

- [ ] Step 1: failing tests: a class's method called through a local and through a field links to it (`call`); an object literal's property read gives the value written (`points`); `new Box()` in two places are two sites; a function passed and called links; a write `o.f = v` and a read `o.f` through an alias meet (identity); an array literal's element read through an alias holds what `a[0] = v` wrote.
- [ ] Step 2: run them: `heap` does not exist.
- [ ] Step 3: implement the objects, the solver and the lowering of names, locals, parameters, returns, member and index reads and writes (string keys named fields, any other key the elements field and every field), object and array literals, `new` of a class (constructor called, `this` the new object), method calls (lookup through the class chain), calls of function values, `this`.
- [ ] Step 4: the new tests and the whole suite pass; gates; commit.

### Task 2: Prototypes, classes as objects, dynamic properties

**Files:**
- Create: `heap/fields.py` (read through the prototype chain, accessors run, an unknown key reaching every field)
- Modify: `heap/lower.py`, `heap/objects.py`
- Test: `tests/test_heap_core.py`, fixture `tests/fixture/heap/protos.js`

- [ ] Step 1: failing tests: `F.prototype.m = function` read on `new F()`; `class B extends A` reads A's methods; `static` members on the class object; a getter runs on read, a setter on write; `o[k] = v` with `k` unknown read back by `o.p`; `Object.create(p)`'s object reads p's members; `obj.constructor` is its class.
- [ ] Step 2 to 4: run, implement, pass, commit.

### Task 3: Closures, contexts and heap cloning

**Files:**
- Create: `heap/contexts.py` (argument tuples of functions keyed by the arguments' sites, the receiver's site for methods, closures carrying their maker's context, objects made in a context cloned one level)
- Modify: `heap/solver.py`, `heap/lower.py`
- Test: `tests/test_heap_contexts.py`, fixture `tests/fixture/heap/contexts.js`

- [ ] Step 1: failing tests: two memos made by one `memo(compute)` each call their own `compute`; a factory `Fn(f)` making `new Node(f)` gives each made function its own node's `f`; `spin(wrap(f), n)` ends with finitely many contexts; a method called on two receivers' sites keeps each receiver's fields apart.
- [ ] Step 2 to 4: run, implement, pass, commit.

### Task 4: The platform from declarations (TypeScript `lib.*.d.ts`, packages' `.d.ts`)

**Files:**
- Create: `heap/declared.py` (a declared signature as a summary: arguments into the receiver's type-parameter places, the result from them or a declared-type object per call site, callbacks called with the declared places), `heap/platform.py` (the declaration files a program has: TypeScript's `lib` set from `tsconfig` `target`/`lib`, `@types/*` and packages' `types`), `languages.py` (the languages' own data the analysis reads, each entry cited to its specification: the intrinsic each literal is created with, ECMA-262 ArrayCreate, OrdinaryObjectCreate, OrdinaryFunctionCreate; listed in `tests/test_generic.py` `LANGUAGE`)
- Modify: `program.py` (the declaration files read as roots, displayed as such), `heap/lower.py`
- Test: `tests/test_heap_platform.py`, fixture `tests/fixture/heap/platform.ts` with a small `lib` copy written for the test (no library text copied: declarations of the same shapes)

- [ ] Step 1: failing tests: `a.push(x)` then `a[0]` holds x through an alias; `xs.map(f)` calls f with xs's elements and holds f's results; `m.set(k, v); m.get(k)` holds v; `p.then(f)` calls f with what p resolves to; `for (const x of s)` over a Set holds its elements; a host object's declared method's result is its declared type's object.
- [ ] Step 2 to 4: run, implement, pass, commit.

### Task 5: What no declaration states, summarized generically from the declarations

**Files:**
- Create: `heap/summaries.py` (for every declared function, whatever its name: the values its call passes reach the fields of its object arguments and of its receiver, and its result, wherever their declared types admit them, each value's own fields one level down; every function-typed value it receives, directly or in a field of an argument, is called with the values its declared parameters admit; an object it receives with a string may have the member that string names read or called (reflection by name, whatever the platform); its result is also the declared result type's object per call site). No function is named anywhere: a summary is computed from the signature alone.
- Test: `tests/test_heap_summaries.py`, fixture `tests/fixture/heap/summaries.ts` with a declaration file written for the test (shapes only, its own names)

- [ ] Step 1: failing tests, each on a declared function of the fixture's own declaration file: a descriptor-shaped argument's `value` reaches the target argument's fields; a callback in an argument's field is called; a source argument's fields reach the target's; an `any`-typed result holds what the call passed; a function the declarations type precisely (Task 4) keeps its precise summary.
- [ ] Step 2 to 4: run, implement, pass, commit.

### Task 6: Modules, JSX and the platform's callbacks on the new core

**Files:**
- Modify: `heap/lower.py` (imports and exports through `index.js`, `import()`, JSX through the factory `tsconfig`'s `jsx`/`jsxImportSource` names and that runtime's declarations), `heap/edges.py`
- Test: `tests/test_heap_core.py` with the existing `jsvalues` and `project` fixtures' JS expectations ported

No event, message or listener is known by its method's name: a listener handed to the platform is called by it through Task 5's generic summary (the old core's `listen`/`fire` by name are debts).

- [ ] Step 1: failing tests: the current `tests/test_js*.py` expectations of calls and JSX hold on `heap.analysis.analyse`; a listener handed to a declared function is reached as called by the platform.
- [ ] Step 2 to 4: run, implement, pass, commit.

### Task 7: Acceptance for JavaScript and TypeScript

- [ ] Step 1: a tool `heap/accept.py` (`python -m featuretrace.heap.accept <roots>`) prints the links the current core finds and the new one does not, and the opposite, by kind.
- [ ] Step 2: the lab app and the lab with three.js: every link only the current core finds read on its line (a link the new core must have is a missing rule of the IR or the platform, fixed at its task); time and memory recorded.
- [ ] Step 3: `trace_feature.py` uses the new core for JavaScript and TypeScript; records; commit and push.

### Task 8: GDScript and Godot's `extension_api.json`

**Files:**
- Create: `heap/godot.py` (engine classes, their bases, methods with argument and result types, signals, properties, singletons from the JSON of the Godot version the project uses)
- Modify: `heap/lower.py` (GDScript's trees: `self`, the `signal` keyword's declarations and their `connect`/`emit` as the language defines them, scene connections read from the scene files; a call by a name held in a string reached through Task 5's reflection summary, never a list of the engine's method names)
- Test: `tests/test_heap_gd.py` with the existing GDScript fixtures' expectations

- [ ] Steps: failing tests, implement, acceptance on the city's GDScript against the current core, commit.

### Task 9: C++

**Files:**
- Modify: `heap/lower.py` (static types decide where the language decides: non-virtual calls, overloads; points-to for virtual calls, function pointers, lambdas and `std::function`; constructors and destructors as the current core reads them)
- Test: `tests/test_heap_cpp.py` with the existing C++ fixtures' expectations

- [ ] Steps: failing tests, implement, acceptance on the city's C++ against the current core, commit.

### Task 10: The new core is the tracer

- [ ] Step 1: `program.load` returns the new core's result for every language; the old core's modules retired; tests ported.
- [ ] Step 2: measured on the city, the lab app, the lab with three.js; records (`log/trace-rules.md`, `log/feature-trace.md`, `log/pendencias.md`, `log/mapa.md`, the ledger copy); commit and push.
