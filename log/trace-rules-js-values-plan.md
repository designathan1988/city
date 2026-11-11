# JavaScript and TypeScript Values by the Languages' Rules Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** The tracer's JavaScript/TypeScript values follow the languages' own rules (erased types, the ECMAScript
built-ins, operators, `new`, script scope) and a lambda is never a member of what surrounds it, so no value is made of
an annotation's text, another language's library rule or a name another file's inner function happens to have.

**Architecture:** The JS reader turns an annotation's syntax tree into value trees (T1-T3); a new `js_library.py` holds
the ECMAScript built-ins' rules for canonical generic texts (index, iteration, element calls, `await`); `TypeNames.canon`
gives every JS text its resolved, file-free form or none; the index and the script scope leave lambdas out; JS
operators give the language's value; `new` evaluates its constructor expression.

**Tech Stack:** Python 3.12, tree-sitter TypeScript/TSX grammar already used by the reader.

**Spec:** `log/trace-rules.md`, "JavaScript and TypeScript values by the languages' own rules" (rules L1, L2, T1-T3,
O1, O2, N1) and the evidence above it ("Why the lab with three.js grows" and the probe).

## Global Constraints

- Every file under 300 lines, a header comment saying its responsibility, all repository text in English.
- New files into `log/mapa.md` in the same commit; gates pass on the changed files.
- Nothing names a project; every rule is a language's rule with its source in `log/trace-rules.md`.
- The owner's order: code and review everything first, tests only at the end (small first, then large); RED checked
  against the base commit (`1951d8b4`).
- No cheap path: each rule at its cause, no special case for a file, library or name.
- Stage only this plan's files; commit attribution `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`; push.

## Review Focus

1. A C++ or GDScript name that a lambda's label could match: lambda labels hold `<lambda:N>`; only JS inner functions
   carry plain names. Check that no C++/GDScript lookup reached a lambda through `members` before (city links).
2. A JS class whose name is also a type parameter's (`class T`, `function f<T>(x: T)`): the parameter wins inside the
   generic declaration (`test_type_parameters_are_erased`).
3. A JS value made in one file and read in another: its identity no longer carries the file; two classes of one name in
   two files stay apart (their keys differ) (`test_values_do_not_depend_on_the_file`).
4. A constructor function that returns nothing: `new F()` links F and gives no object of a class (`test_new_calls_a_function`).
5. GDScript and C++ operators keep their rules (C++ `operator+` of a class, GDScript Variant operators): only JS changes
   (O1); binary `*` reaches `operator*` in C++ (`test_binary_star_is_multiplication`).

---

### Task 1: Lambdas are locals; a script's globals are its top-level declarations (L1, L2)

**Files:** Modify `tools/analysis/featuretrace/index.py:49` (members leave kind `lambda` out),
`tools/analysis/featuretrace/js_modules.py:47-51` (globals: top-level declarations only).
Test: `tools/analysis/featuretrace/tests/test_js_values.py`, fixture `tests/fixture/jsvalues/{script.js,scope.js}`.

- [ ] **Step 1: Tests** `test_inner_function_is_not_global` (`scope.js:use` calls `shown` from `script.js`, not the inner
  `x` of `script.js:outer`) and `test_inner_function_is_not_a_member` (`k.helper()` on a `K` reaches nothing).
- [ ] **Step 2: Code** the two filters.

### Task 2: Erased types, the built-ins' containers, declared returns (T1, T2, T3)

**Files:** Create `tools/analysis/featuretrace/js_types.py` (annotation syntax tree -> value trees; the reader's
type-parameter scope) and `tools/analysis/featuretrace/js_library.py` (canonical generic texts; index, iteration,
element calls, `await` for them). Modify `js_read.py` (`type_values` through `js_types`; `Unit.declared` for a
function's return annotation; type parameters pushed for functions), `js_class.py` (class and interface type
parameters), `model.py` (`Unit.declared: list`), `types_of.py` (`canon` for JS: resolved form or none), `solver.py`
(`add` skips a value canon empties; `identity` of a JS value is its text), `typer.py` (`hold_node`/`ret_node` never add
a JS unit's `type_text`; `ret_node` follows `unit.declared`; `await` through `js_library.awaited` for JS),
`transfer.py` (`step_one`/`member_one` use `js_library` for a JS generic text instead of `library_step`).
Fixture `tests/fixture/jsvalues/types.ts`.

**Interfaces:** `js_types.values(r, annotation) -> list[tuple]` (value trees); `js_library.CONTAINERS`,
`js_library.canonical(text: str, resolve) -> str` ("" when it names nothing with a runtime value),
`js_library.step(step: str, text: str) -> list[str]`, `js_library.member(name: str, text: str, called: bool) -> list[str]`,
`js_library.awaited(text: str) -> list[str]`.

- [ ] **Step 1: Tests** (typer values on the fixture): `test_union_with_null_is_the_class` (`b: Building | null` holds
  only `#…Building`), `test_type_parameters_are_erased` (`t: T` holds nothing), `test_declared_return_gives_elements`
  (`for (const x of boxes()) x.open()` reaches `Box.open`), `test_map_get_and_entries` (`map.get(k).open()` and
  `for (const [k, v] of map) v.open()` reach `Box.open`), `test_promise_awaited` (`(await p).open()` reaches it),
  `test_no_annotation_text_is_a_value` (no value of any node in the fixture contains `|` or `std::`),
  `test_values_do_not_depend_on_the_file` (one `Box` value for a `Box` returned in two files).
- [ ] **Step 2: Code** by the interfaces and rules T1-T3.

### Task 3: Operators (O1, O2)

**Files:** Modify `transfer.py` (`operator_one`: no dereference for `*`), `typer.py` (`op`/`unary` of a JS unit give the
language's value). Fixture `tests/fixture/jsvalues/types.ts`, `tests/fixture/pending/lifetime.cpp` (a binary
`operator*`).

- [ ] **Step 1: Tests** `test_js_arithmetic_is_a_primitive` (`k * 2` holds only the engine's value) and
  `test_binary_star_is_multiplication` (C++ `(a * b).go()` reaches `V::go` through `V::operator*`).
- [ ] **Step 2: Code.**

### Task 4: `new` evaluates its constructor (N1)

**Files:** Modify `ir_js.py` (`_new` -> `("construct", <tree of the constructor>)`), `typer.py` (`construct` shape),
`transfer.py` (`constructs`: a class value is its instance, a function's returns, anything else the language's value),
`js_link.py` (`constructors`: a function value is linked as `new`). Fixture `tests/fixture/jsvalues/news.js`.

- [ ] **Step 1: Tests** `test_new_calls_a_function` (`new Maker(1)` links `Maker`, the object it returns reaches
  `Box.open`) and `test_new_through_a_variable` (`const C = Box; new C().open()` reaches `Box.open`).
- [ ] **Step 2: Code.**

### Task 5: Review, tests at the end, records

- [ ] **Step 1: Review** every changed line against the rules and the Review Focus.
- [ ] **Step 2: RED on the base** (`1951d8b4` copied by `git archive`, the new tests run there): each fails.
- [ ] **Step 3: Unit suite**: `python -m unittest discover -s tools/analysis/featuretrace/tests -t tools/analysis` -> OK.
- [ ] **Step 4: City** on the fixed snapshot, base against new: every difference explained by a rule of this plan.
- [ ] **Step 5: Lab** without three (`src`, `tsconfig.json`), then the lab with three, each under a monitor (900 s):
  the fixpoint, its time, links; the largest value nodes read once more (no annotation text, no inner function of
  another file).
- [ ] **Step 6: Records**: ledger `.superpowers/sdd/trace-rules-js-values-plan/progress.md` and its copy
  `log/trace-rules-js-values-progress.md`, `log/feature-trace.md`, `log/pendencias.md`, `log/mapa.md`; gates; commit;
  push.
