# Narrowing at branch conditions — implementation plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** the feature tracer reads values through branch conditions as the languages do: a name read inside a guarded
region holds only the values the guard can let through (TypeScript's control-flow narrowing, GDScript `is`), with the
JS language constants and JS properties written outside their class that the guards need.

**Architecture:** the readers keep a stack of facts (`narrowing.py`) opened by conditions (`js_guards.py` for JS/TS,
`gd_body` for GDScript `is`); every tree built while facts hold has its narrowed names wrapped in a new IR shape
`("narrow", name_tree, guard, positive)`; the typer wires that shape as a filter of the name's node
(`narrow_values.py`). JS literals `true/false/null/undefined` become values of their own (`truth.py`, solver
constants), and a property written where no class declares it becomes the class's (`#prop` nodes, `transfer` and
`js_link`).

**Tech Stack:** Python 3.12, tree-sitter (typescript/tsx, gdscript), unittest.

**Spec:** `log/trace-rules.md`, "Narrowing at branch conditions" (V1, V2, G1, G2, R1, R2, N1, N2) and "Why the lab with
three.js does not reach its fixpoint after P3".

## Global Constraints

- Every new or changed text in English; every file starts with a header comment saying its responsibility.
- Files under 300 lines (`tools/gates/rules.json` `max_lines` `.py`: 300); split by responsibility, never to fit.
- No project name in the tracer: only the languages' rules.
- New files go into `log/mapa.md` in the same commit (gate `MAPA_DESATUALIZADO`); every new function a rule depends on
  goes into `rules.HANDLERS` (`tests/test_rules.py` imports each one).
- Nothing of the roadcraft lab is copied; the lab is only run as an input of the tracer.
- Tests: `python -m unittest discover -s tools/analysis/featuretrace/tests -t tools/analysis` (all pass at the end of
  every task); gates on the changed files: `python tools/gates/run_gates.py --files-from <list>`.
- Commit at the end of each task on `main`, only this work's files, ending with
  `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`; push at the end of the theme.

## Review Focus

- A name narrowed in a region and assigned by a closure written later in the same function: read whole (R2) —
  test `test_closure_write_keeps_the_name_whole` (Task 3).
- Nested regions (an `if` inside an `if`, a ternary inside a branch): both facts apply in turn — test
  `test_nested_regions_compose` (Task 3).
- Negations and groupings (`!(a && b)`, `!(x instanceof C)` followed by a return, `x?.p`): the right polarity —
  tests `test_negated_conjunction_proves_nothing_when_true` and `test_optional_chain_member_guard` (Task 4).
- A narrowed parameter that keys a function's templates: the template sees the narrowed value (`templates.reads`
  walks the shape; `reaches` passes it) — test `test_narrowed_key_parameter` (Task 3).
- `typeof x === t` with `t` not a string literal, `x === y` with `y` a name: no fact, the name read whole — test
  `test_unknown_comparison_proves_nothing` (Task 4).

---

### Task 1: The language's constants (V1)

**Files:**
- Create: `tools/analysis/featuretrace/truth.py`
- Modify: `tools/analysis/featuretrace/solver.py` (constants, `identity`), `types_of.py` (`canon`, `type_class`),
  `transfer.py` (`member_one`, `step_one`), `typer.py` (`wire`), `ir_js.py` (`LITERALS`, `_identifier`), `ir.py`
  (shape list), `rules.py`
- Test: `tools/analysis/featuretrace/tests/test_narrowing.py`, fixture `tests/fixture/jsvalues/guards.js`

**Interfaces:**
- Produces: in `solver.py` `LANG = "<lang>"`, `TRUE, FALSE, NULL, UNDEFINED = LANG + "true", ...`, and
  `is_language(text: str) -> bool` (ENGINE or a `LANG` constant); in `truth.py`
  `can_be_truthy(text: str) -> bool`, `can_be_falsy(text: str) -> bool`, `typeof_results(text: str) -> frozenset[str]`,
  `can_equal(text: str, literal: str) -> bool`, `can_differ(text: str, literal: str) -> bool`,
  `is_nullish(text: str) -> bool | None` (None: cannot tell).
- `ir_js` gives `("literal", "true" | "false" | "null" | "undefined")` for those literals and for the global name
  `undefined`; the typer turns exactly these four into the constants (JS only; every other literal stays ENGINE).

- [ ] **Step 1:** fixture `guards.js` with `function yes() { return true; }`, `function none() { return null; }`,
  `function gone() { return undefined; }`; test `test_literal_constants`: the returns of `yes`, `none`, `gone` are
  `{TRUE}`, `{NULL}`, `{UNDEFINED}`; test `test_constants_are_the_languages_values`: a member read on `yes()` gives
  ENGINE (no link, no error).
- [ ] **Step 2:** run, see both fail (the returns are ENGINE).
- [ ] **Step 3:** implement: constants and `is_language` in `solver.py` (`identity` keeps a constant as itself);
  `canon` and `type_class` let them through untouched; every `text == ENGINE` test in `transfer.member_one`/`step_one`
  that means "a value of the language" becomes `is_language(text)`; `truth.py` by MDN's tables (a class value: truthy,
  `{"object", "function"}`; FN: truthy, `{"function"}`; TEXT: truthy unless empty, `{"string"}`; ELEM or a JS container
  text: truthy, `{"object"}`; TRUE/FALSE: `{"boolean"}`; NULL: falsy, `{"object"}`; UNDEFINED: falsy, `{"undefined"}`;
  ENGINE: both, every result).
- [ ] **Step 4:** run the suite: all pass (update an older test's expectation only where it asserted ENGINE for one
  of these four literals, with a ledger ruling).
- [ ] **Step 5:** commit.

### Task 2: Properties written where no class declares them (V2)

**Files:**
- Modify: `tools/analysis/featuretrace/transfer.py` (`member_one`), `js_link.py` (new `expando_writes`), `link.py`
  (`link_ref`, JS writes), `rules.py`
- Test: `tests/test_narrowing.py`, fixture `guards.js`

**Interfaces:**
- Produces: `transfer.prop_node(key: str, name: str) -> tuple` = `("#prop", key, name)`;
  `js_link.expando_writes(rs, unit, ref) -> bool` (subscribes the written value to the `#prop` node of every class value
  of the receiver that declares no such member; returns whether the receiver holds a value the program does not define,
  so `link_ref` hands callables only then); `transfer.chain_read(ix, key) -> bool` (every base of `key`, transitively, is
  a class the tracer read).

- [ ] **Step 1:** fixture: `function Flagged() {}`, `Flagged.prototype.flag = true;`, `function flagOf() { return new
  Flagged().flag; }`; `const bag = {}; bag.extra = new Box(); function useBag() { bag.extra.open(); }` (Box from
  `types.ts`); `class Plain { a = 1; }`, `function missing() { return new Plain().b; }`. Tests:
  `test_prototype_property` (`flagOf` returns `{TRUE}`), `test_property_written_outside_its_class` (`useBag` links
  `Box.open`), `test_property_found_nowhere_is_undefined` (`missing` returns `{UNDEFINED}`).
- [ ] **Step 2:** run, see the three fail.
- [ ] **Step 3:** implement in `member_one` (JS, a class of the program, no member found): `prototype` gives the class's
  value itself; every class of the chain contributes its `#prop` node (`follow`); `UNDEFINED` when `chain_read`, else
  ENGINE (the inner-class rule stays first); in `link_ref`, a JS write calls `expando_writes` and `handed` only when it
  returns true.
- [ ] **Step 4:** suite green.
- [ ] **Step 5:** commit.

### Task 3: Regions, facts and the narrow shape for `if` (G1 core, R1 if/else, R2, N1)

**Files:**
- Create: `tools/analysis/featuretrace/narrowing.py` (reader side), `js_guards.py` (JS conditions),
  `narrow_values.py` (typer side)
- Modify: `scopes.py` (`scoped_tree` applies the facts), `c_family.py` (`if_statement` takes the language's guard
  reader), `js_body.py` (passes `js_guards.facts`), `js_read.py` (function start: closure-written names), `typer.py`
  (`wire` for `narrow`), `templates.py` (`PASS_SHAPES` gains `narrow`), `ir.py`, `rules.py`
- Test: `tests/test_narrowing.py`, fixture `guards.js`

**Interfaces:**
- Produces: a fact is `(name_tree, guard, positive: bool)`; guards are tuples: `("truthy",)`, `("member", p)`,
  `("typeof", t)`, `("instance", class_tree)`, `("array",)`, `("has", p)`, `("equals", literal_text)`, `("nullish",)`,
  `("member-equals", p, literal_text)`, `("predicate", callee_tree, index)`, `("is", type_text)`.
  `narrowing.open(r, facts: list, region_nodes: list) -> None` (pushes a frame without the facts whose name the
  region nodes assign or a closure of the function assigns), `narrowing.close(r) -> None`,
  `narrowing.apply(r, tree) -> tuple` (each `("name", key)` with facts wrapped, innermost frame last),
  `narrowing.wrap(tree, facts) -> tuple`, `narrowing.leaves(node) -> bool` (per language: the statement always ends by
  `return`/`throw`/`break`/`continue`), `narrowing.assigned(r, nodes) -> set[str]`;
  `js_guards.facts(r, test_node, unit) -> tuple[list, list]` (facts when true, facts when false);
  `narrow_values.narrows(t, guard, positive, unit) -> transfer` and `narrow_values.wire(t, node, tree, unit)`.
- Shape: `("narrow", name_tree, guard, positive)`; `templates.reaches` passes it like `addr`.

- [ ] **Step 1:** fixture: `class V3 { x = 0; copy(v) { this.x = v.x; } set(x) { this.x = x; } }`,
  `V3.prototype.isV3 = true;`, `function look(x) { const t = new V3(); if (x.isV3) { t.copy(x); } else { t.set(x); } }`,
  `look(new V3()); look(5);`; `function pick(c) { if (c instanceof Box) c.open(); else c.lay(); }` with `pick(new
  Box()); pick(new Tile());` (Tile from `helpers.ts`); `function kept(c) { if (c instanceof Box) { c = new Tile();
  c.lay(); } }`; `function later(c) { if (c instanceof Box) c.open(); const swap = () => { c = new Tile(); }; swap(); }`;
  `function nested(c) { if (c instanceof Box) { if (c.open) c.open(); } }`; `function keyed(x) { if (x instanceof Box)
  return x; return null; }` with `keyed(new Box()).open(); keyed(new Tile());`. Tests: `test_member_truthiness_overload`
  (V3.set's `x` holds no `#…V3`; V3.copy's `v` holds it), `test_instanceof_branches` (`pick` links `Box.open` and
  `Tile.lay`, and the `c` read in the `else` holds no Box), `test_assignment_in_region_keeps_the_name_whole` (`kept`'s
  `c` in the branch holds Tile), `test_closure_write_keeps_the_name_whole`, `test_nested_regions_compose`,
  `test_narrowed_key_parameter` (`keyed`'s result holds Box and no Tile).
- [ ] **Step 2:** run, see them fail.
- [ ] **Step 3:** implement: reader side as the interfaces say (facts on names only; `assigned` scans assignment
  targets, update operands, destructuring and loop bindings in the region nodes; the closure-written names of the
  function are scanned once when the function starts); `c_family.if_statement` opens the true facts over the
  consequence and the false facts over the alternative; typer side: each value goes through the guard (V1/V2 values via
  `truth.py`; a class by its chain; `member` and `member-equals` by a subscription to that value's member node, deciding
  as its values come; `instance` and `predicate` by a second subscription to the class's or callee's node, the held
  values tested again as it grows; `is` by the class the type names in the value's scope).
- [ ] **Step 4:** suite green.
- [ ] **Step 5:** commit.

### Task 4: The other regions and guards (R1 complete, G1 complete)

**Files:**
- Modify: `js_guards.py`, `js_body.py` (`block`: the facts after an `if` whose other branch leaves; `while`/`for`
  bodies; `switch` cases), `js_expr.py` (`?:`, `&&`, `||` operands while their references are made), `ir_js.py`
  (`_ternary`, `_binary` wrap their operands' trees), `js_read.py` (a function's return annotation `x is T`:
  `Unit.predicate`), `model.py` (`Unit.predicate: tuple = ()`), `rules.py`
- Test: `tests/test_narrowing.py`, fixtures `guards.js`, `tests/fixture/jsvalues/predicates.ts`

**Interfaces:**
- Consumes: Task 3's `narrowing.open/close/wrap/leaves`, `js_guards.facts`.
- Produces: `Unit.predicate = (param_index, type_trees)` read from a `type_predicate_annotation`.

- [ ] **Step 1:** fixture cases and tests: `test_early_return` (`if (!(c instanceof Box)) return; c.open();` with Box and
  Can: the `c` after the `if` holds no Can), `test_ternary_and_logical` (`c instanceof Box ? c.open() : c.lay()`,
  `c instanceof Tile && c.lay()`, `c instanceof Box || c.lay()`), `test_typeof_and_equality` (`typeof c === "string"`,
  `c === null`, `c == null`, `c !== undefined`), `test_in_and_isarray` (`"open" in c`, `Array.isArray(c)`),
  `test_discriminant` (`s.kind === "box"` over two object literals with `kind` and a `go()` each: only the box's `go`
  linked from that branch; the same by `switch (s.kind) { case "box": ... }`), `test_type_predicate` (`function isBox(x:
  Box | Can): x is Box`, `if (isBox(v)) v.open()`), `test_while_body`, `test_negated_conjunction_proves_nothing_when_true`,
  `test_optional_chain_member_guard` (`x?.isV3`), `test_unknown_comparison_proves_nothing`.
- [ ] **Step 2:** run, see them fail.
- [ ] **Step 3:** implement as the interfaces say: after an `if`, the false facts hold for the rest of the block when the
  consequence leaves and there is no alternative or the alternative does not leave (the true facts when only the
  alternative leaves); a `switch` case gets `member-equals`/`equals` facts when its test is a literal and the case before
  it leaves (or it is the first); loops' bodies get their condition's true facts.
- [ ] **Step 4:** suite green.
- [ ] **Step 5:** commit.

### Task 5: GDScript `is` by the same owner (G2)

**Files:**
- Modify: `gd_body.py` (`if`/`elif`/`else` open facts through `narrowing`; `narrow` replaced), `gd_read.py` (`ir`
  without its own `narrowed`; the field and function removed), `rules.py`
- Test: `tests/test_narrowing.py` (GD cases), fixture `tests/fixture/pending/guards.gd`

**Interfaces:**
- Consumes: `narrowing.open/close/apply/leaves`; guard `("is", type_text)`.

- [ ] **Step 1:** fixture `guards.gd` (`class_name` script with `func use(x): if x is Box: x.open() else: x.lay()` and an
  early `if not (x is Box): return`), tests `test_gd_is_branches` and `test_gd_early_return`; the existing GD `is` tests
  stay green.
- [ ] **Step 2:** run, see the new ones fail.
- [ ] **Step 3:** implement: GDScript facts from `is`, `not`, `and`, `or`; `elif` gets the false facts of the conditions
  before it and its own true facts; the old replacement by the tested type is removed (one owner).
- [ ] **Step 4:** suite green.
- [ ] **Step 5:** commit.

### Task 6: Measure and record

- [ ] **Step 1:** city (same sources, the previous commit's tracer against this one): every link that disappears is
  read against its source line; none of a value that can be there.
- [ ] **Step 2:** lab app; lab with three.js under a monitor (900 s): fixpoint reached, time and links recorded; if
  not, probes (stack with locals, writes into a polluted field) and back to the cause.
- [ ] **Step 3:** `log/trace-rules.md` (what was measured), `log/feature-trace.md`, `log/pendencias.md`, `log/mapa.md`,
  the ledger copy; gates on the changed files; commit and push.
