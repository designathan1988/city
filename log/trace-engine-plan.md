# The Feature Tracer's Native Engine Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.
> Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** The new core's analysis (`tools/analysis/featuretrace/heap`) runs in a native C++ engine fast enough to solve
the whole lab and the city inside a short budget, its results identical to the Python semantics on every heap test.

**Architecture:** Python keeps reading (tree-sitter readers, the index, module and name resolution) and writes one
input file per program: the units, references, expression trees and declared types, each tree hash-consed to an
integer, with every name the analysis would resolve already resolved (definitions, bindings, exports, class keys,
aliases, members by owner, global types). The engine (`tools/analysis/engine/`, C++20, CMake, llvm-mingw) reads it,
runs the same analysis as `heap/` over integer nodes and objects (worklist solver, difference propagation, edges as
integer sets), and writes the links and the points-to sets a caller asks for. `heap.analysis.analyse(ix)` calls it and
returns the same `Result`; the Python modules stay as the specification each engine module is checked against.

**Tech Stack:** C++20 (llvm-mingw clang, the project's toolchain), CMake 4.4, Python 3.12, the tracer's tests.

**Spec:** `log/trace-research.md` (F1-F15, the Decision), `log/trace-core-plan.md` (Tasks 1-6 and their rulings in
`log/trace-core-progress.md`).

## Global Constraints

- The engine is generic like the tracer: it holds no name of a program, library, platform or API; every name it needs
  comes from the input file (the vocabulary and the languages' data the Python side writes into it).
- Termination and finiteness as in Python: static sites, bounded contexts, the worklist discipline; no deadline or
  threshold decides a result. Every run prints its progress and ends inside the budget it is given.
- Results identical to the Python semantics on every test of `tests/test_heap_*.py` (the oracle); a difference is a
  bug of one side, fixed at its cause.
- Files under 300 lines with a header; new files in `log/mapa.md`; English; repo files written as bytes or with the
  Edit tool; the C++ follows `CLAUDE.md`'s rules for `native/` where they apply (no magic numbers, one owner).
- No process in the background unwatched; builds and runs in the foreground with a deadline and live output.

## Tasks

### Task E1: The input file — facts, trees and resolutions, written by Python
- Create `tools/analysis/featuretrace/heap/export.py`: hash-cons every expression tree and declared type of the index
  into integer ids; write units, references, classes, and the resolution tables the heap modules query (names by unit,
  bindings, exports, class_of and alias_of of every type text, members by owner, globals, tsconfig JSX options,
  intrinsic types) as one binary file keyed by the index's content.
- Test: `tests/test_engine_export.py` — the file round-trips (read back in Python, every tree and table equal).
- Ruling (2026-11-11, log/trace-research.md F17-F18): built as `featuretrace/engine_input.py`, one pickle (protocol 5)
  of the units, the classes and the tables, the vocabulary from each constant's owner; the engine reads the pickle
  opcodes `tests/test_engine_input.py` lists (Lib/pickletools.py) and hash-conses the trees itself. Measured on the
  lab: read 2.4 s warm, tables and file 7.2 s, 179 MB. The building phases run without the cyclic collector.

### Task E2: The engine's core — objects, nodes, worklist solver
- Create `tools/analysis/engine/` (CMakeLists, `input.*`, `objects.*`, `solver.*`, `main.cpp`): read the file, intern
  objects and nodes as integers, the worklist solver with identity edges and subscriptions as typed records.
- Test: a solver unit test (C++) and `tests/test_engine_core.py` on `heap/core.ts` against the Python result; the
  pickle reader's dump of every fixture's input equal to Python's value (`tests/test_engine_input.py`'s files).

### Task E3: Trees, names, references, members, contexts (heap Tasks 1-3)
### Task E4: Declared types, values, summaries, protocols, JSX (heap Tasks 4-6)
### Task E5: `heap.analysis.analyse` through the engine; every heap test against it
### Task E6: Measured on the lab and the city, previous and this one; records; the CLI switched (core plan Task 10)

Each of E3-E6 gets its own failing tests first (the heap tests run against the engine), then the code, then the
measurements, as `superpowers:executing-plans` requires.

### E6 status (2026-11-12 04:00)
- Done: the engine is the Python semantics (every heap test on both cores, links identical on nine fixture sets); the
  language's merges fixed in both cores (F20, F23); the engine's tables and memos (F22-F24). Every folder of the lab but
  `src/world` converges in 1-7 s; `src/world`'s `doc.ts` + `rescale.ts` in 0.86 s with contexts.
- Open (log/trace-research.md F24, in order): number keys write the elements only (ECMA-262 ToPropertyKey), both cores,
  with a canonical-numeric name read reading the elements; contexts selected by flows from parameters (Zipper); the
  engine's constant factors. Then the lab and the city measured, previous and this one, and the CLI switched.

### Task E7: Contexts where they change the result (log/trace-research.md F21-F25; Zipper, OOPSLA 2018)
- Criterion, per function, from its own trees only (no threshold, no name of a program or library): a run needs its own
  call context when objects that differ between callers can reach its outputs — its returns and yields, or a store into
  an object reached from an input. Inputs are its parameters and `this`; a parameter whose declared type admits only the
  language's primitive values (number, string, boolean, bigint, symbol, undefined, null) carries no object a context
  could tell apart. A tree carries what its value can hold: a name its parameter or its local's assignments; a member or
  an index its base's; a call its receiver's and arguments' (the callee may hand them back); `new`, a literal, an array,
  a closure an allocation (the run's own objects); an operator, a literal, a typeof, a text none (fresh primitives).
  A run whose outputs carry neither an input nor an allocation of its own runs once per environment (`of_call(env,
  ROOT)`); every other keeps the call site (and the receiver for a method).
- Tests first (both cores): a function whose result is a number computed from its parameters runs once; an identity
  (`<T>(x: T) => x`), a factory (`() => ({})`), a closure maker and a store into a parameter's object keep their contexts
  (test_heap_contexts must stay green as it is).
- Measured on the lab: runs, nodes, items, time to converge, links against the full-context run on the parts that
  converge (`src/sim`, `src/world/doc.ts` + `rescale.ts`).

### E6/E7 status (2026-11-12 07:00)
- Since 04:00: number keys (F25), contexts where they change the result (E7, F26), calls of what no call runs skipped
  (F27), fields initialized per object and literals where evaluated (F28). The lab with contexts reaches ~370 k links
  in 150 s and still grows; what is left is the graph's size (F29). Next, by the owner's choice of design: the solver's
  representation (a) or demand-driven queries (b).
