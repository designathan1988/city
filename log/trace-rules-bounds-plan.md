# Declared Types Bound Their Places (S2) Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** A place declared with a type (TypeScript annotation, or JSDoc in a JavaScript file) holds only the values the type admits, so a value no declared type allows stops at the first typed place it would enter.

**Architecture:** A bound is read from the type's syntax once, as admit tokens (`js_bounds`), and kept on the unit (per parameter and annotated local, per field or variable, per function result). The typer routes what flows into a bounded place through a filter subscription (`bounds.admits`), which resolves class names where the type is written; a filter is no identity, so the cycle sweep no longer merges typed fields with what flows past them. JavaScript files get their bounds from JSDoc (`@param`, `@type`, `@returns`), each type parsed with TypeScript's own grammar (`jsdoc`).

**Tech Stack:** Python 3.12, tree-sitter-typescript 0.23.2, the tracer under `tools/analysis/featuretrace`.

**Spec:** `log/trace-rules.md`, section "Declared types bound their places (S2)" (written in Task 1, first step), from the measurement that found the hub (`log/trace-rules.md`, "What the lab with three.js showed after J2'", G1'', J2'').

## Global Constraints

- Every rule names no project: languages' rules only (TypeScript handbook, Type Compatibility and JSDoc Reference; ECMAScript).
- Files under 300 lines, a header comment saying their responsibility; new files in `log/mapa.md`; English in the repository.
- No copy of the roadcraft lab's code; the lab is the tracer's input only.
- `any`, `unknown`, a type parameter, a type the tracer cannot read (mapped, conditional, lookup, `typeof`, `infer`) admit every value; a value the tracer cannot tell (the engine's) is admitted by every bound.
- C++ and GDScript places are not bounded by this plan (their declared type texts already are their values).
- Records appended, never pruned; files written as bytes keeping their line endings.

## Review Focus

- A parameter typed with an interface receiving an object literal of the program: kept (an object type admits any object).
- A class-typed parameter receiving an object of a subclass: kept (its chain), of an unrelated class: dropped.
- An optional parameter (`x?: T`, JSDoc `[x]`) receiving `undefined`: kept.
- `Record<string, X>` receiving a plain object literal: kept.
- A host type (`Float64Array`, `HTMLElement`) receiving an object of a class of the program whose bases the tracer read: dropped; the engine's value: kept.

---

### Task 1: Bounds from TypeScript annotations (S2)

**Files:**
- Create: `tools/analysis/featuretrace/js_bounds.py` (admit tokens from a type node), `tools/analysis/featuretrace/bounds.py` (admission and the filter subscription)
- Modify: `model.py` (Unit.bounds: name -> tokens; Unit.bound: tokens of a field, variable or result), `js_read.py` (parameters, parameter properties), `js_class.py` (fields), `js_body.py`/`js_decl.py` (annotated locals and variables), `places.py` (filtered places), `rules.py` (handlers)
- Test: `tools/analysis/featuretrace/tests/test_bounds.py`, fixture `tests/fixture/jsvalues/bounded.ts`

**Interfaces:**
- Produces: `js_bounds.of(r, annotation) -> tuple[tuple, ...]` (tokens: `("any",)`, `("number",)`, `("string",)`, `("text", s)`, `("boolean",)`, `("bigint",)`, `("symbol",)`, `("null",)`, `("undefined",)`, `("object",)`, `("function",)`, `("array",)`, `("container", head)`, `("named", text)`, `("shape",)`); `bounds.admits(t, tokens, unit, text, ctx) -> bool`; `bounds.filtered(t, node, tokens, unit) -> raw node` (what flows in goes to the raw node; the place holds what the tokens admit).

- [ ] Step 1: write the S2 section of `log/trace-rules.md` (rule, sources, what each token admits).
- [ ] Step 2: failing tests: a `number` parameter receiving a string and a Box keeps neither; a `Box` parameter keeps a Box and its subclass, drops a Tile; an interface parameter keeps an object literal; an optional parameter keeps `undefined`; a `Float64Array` field drops an object of the program; a `Record<string, Box>` parameter keeps an object literal; a field `x: number` written a string holds no string; a declared result `: number` returning a Box holds no Box.
- [ ] Step 3: run them: they fail on the values the filter will drop.
- [ ] Step 4: implement `js_bounds.of`, `bounds.admits`, `bounds.filtered`; record bounds in the readers; route parameters, annotated locals, fields and results through the filter.
- [ ] Step 5: the whole suite passes; gates on the changed files; commit.

### Task 2: JSDoc bounds in JavaScript files (S2')

**Files:**
- Create: `tools/analysis/featuretrace/jsdoc.py` (the JSDoc comment of a declaration; `@param`, `@type`, `@returns` types parsed with TypeScript's grammar into tokens)
- Modify: `js_read.py` (functions and methods: parameters and result), `js_class.py` (fields), `js_body.py` (`this.x = ...` with `@type`, annotated variables)
- Test: `tests/test_bounds.py`, fixture `tests/fixture/jsvalues/jsdoc.js`

- [ ] Step 1: failing tests: `@param {number} x` drops a string argument; `@param {number} [x]` keeps `undefined`; `/** @type {number} */ this.x = x;` in a constructor bounds the field; `@returns {Vector}` bounds the result; Closure forms `{?number}`, `{!Foo}`, `{number=}`, `{*}`, `{Array.<number>}`, `{function(number): string}`.
- [ ] Step 2: run them: they fail.
- [ ] Step 3: implement `jsdoc.py` and the readers' calls; the comment is the one written right before the declaration (its statement, its method).
- [ ] Step 4: the whole suite passes; gates; commit.

### Task 3: Measure and record

- [ ] Step 1: the city (same sources): links unchanged (C++ and GDScript are not bounded).
- [ ] Step 2: the lab app, the lab with three.js under a monitor (900 s): every link lost read on its line; the fixpoint reached or the next cause found by probes.
- [ ] Step 3: `log/trace-rules.md` (what was measured), `log/feature-trace.md`, `log/pendencias.md`, `log/mapa.md`, the ledger copy; gates; commit and push.
