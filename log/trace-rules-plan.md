# Feature tracer by the trace rules: implementation plan

> For agentic workers: REQUIRED SUB-SKILL: superpowers:executing-plans (native, in this session). Steps use `- [ ]`.

**Goal:** the feature tracer applies `log/trace-rules.md` as written: one handler per rule, one value rule per expression
construct, every grammar construct accounted for, no expression ever turned into text.

**Architecture:** readers turn each expression node into a small expression tree (IR) by its construct (EXPR rows);
facts keep IR, never text markers. The typer evaluates IR (VALUE) with the documented lookup orders (LOOK) and the
value flows (FLOW). The resolver and the linker keep their roles (CALL, NAME, SIGNAL, BIND, ENTRY), fed by IR receivers.

**Tech stack:** Python 3.12, py-tree-sitter 0.26.0, tree-sitter-gdscript 6.1.0, tree-sitter-cpp 0.23.4, unittest.

**Spec:** `log/trace-rules.md` (commit 9905aae2).

## Global constraints
- Every named kind of both grammars has exactly one rule id in code, equal to its row in `log/trace-rules.md`.
- No text marker (`<of>`, `<call>`, `<type>`, `<op>`, `<put>`, chains parsed from source text) survives anywhere.
- No "candidates" fallback: a receiver gets the classes FLOW gives it; none -> recorded as unreached (a fact of the code).
- Files under 300 lines, English, a header comment each; `log/mapa.md` updated in the same commit.
- Tests: `python -m unittest discover -s tools/analysis/featuretrace/tests -t tools/analysis`.

## Review focus
1. A chain through an operator, a call and a subscript (`(a + b).c()`, `f(x)[i].g()`): VALUE over the tree, test in Task 2.
2. An untyped parameter whose only callers pass different classes: FLOW unions both, Task 4.
3. An inherited engine member read in a script (`size`, `position`): LOOK ends in the engine base, not unreached, Task 4.
4. A C++ free function found only through an argument's namespace (ADL), Task 5.
5. A `#include`d header's internal-linkage helper used from two files, each its own, Task 5.

---

### Task 1: the rule table in code, enforced
**Files:** Modify `tools/analysis/featuretrace/rules.py`; Test `tools/analysis/featuretrace/tests/test_rules.py`.
**Produces:** `GD_RULES: dict[str, tuple[str, ...]]`, `CPP_RULES: dict[str, tuple[str, ...]]` (kind -> rule ids), `RULE_IDS`.
- [ ] Test `test_every_grammar_kind_has_its_row`: the named visible kinds of each grammar == keys of its table.
- [ ] Test `test_code_table_equals_the_log_table`: parsing the kind rows of `log/trace-rules.md` gives the same mapping.
- [ ] Test `test_every_rule_has_a_handler`: every rule id used maps to a handler registered by Tasks 2-5 (`HANDLERS`).
- [ ] Implement the two tables from the log; run; commit.

### Task 2: expression IR built per construct (VALUE input)
**Files:** Create `featuretrace/ir.py` (IR shapes), `featuretrace/ir_gd.py`, `featuretrace/ir_cpp.py` (builders);
delete `featuretrace/flows.py`. Test `tests/test_ir.py`.
**Produces:** IR as tuples: `("name", s)`, `("member", base, s, arrow: bool)`, `("call", callee, args)`,
`("index", base, arg)`, `("op", op, left, right)`, `("unary", op, operand)`, `("cast", type_text)`, `("new", type_text)`,
`("callable", local_lambda_id)`, `("literal", engine_type_or_"")`, `("union", a, b)`, `("elements", e)`, `("await", e)`,
`("strname", s)`, `("self",)`, `("super",)`, `("deref", e)`, `("addr", e)`, `("node",)`, `("collection", [values])`.
`build(r, node) -> tuple` in each builder, one branch per EXPR kind of its grammar (`GD_RULES`/`CPP_RULES` rows with EXPR).
- [ ] Tests: `(a + b).c()` -> `("call", ("member", ("op","+",("name","a"),("name","b")), "c", False), ())`;
      `f(x)[i].g()`; `x as T`; `Callable(o, "m")` -> `("member", ("name","o"), "m", False)`; `f.bind(1)` -> `("name","f")`;
      `static_cast<T>(x)` -> `("cast","T")`; `p->q` arrow True; `[a, b]` -> `("collection", [...])`; `c ? a : b` -> union.
- [ ] Test `test_every_expr_kind_has_a_builder`: each EXPR kind of each grammar has a branch (registry check).
- [ ] Implement; run; commit.

### Task 3: readers keep IR (DEF, FLOW sources)
**Files:** Modify `model.py` (`Ref.base: tuple|None` replaces `recv`; `Ref.args: tuple[IR]`; `Unit.types: dict[str, list[IR]]`;
`Unit.values: list[IR]`; `Unit.returns: list[(line, text, IR)]`), `gd_read.py`, `gd_body.py`, `gd_calls.py`, `cpp_read.py`,
`cpp_body.py`, `cpp_expr.py`.
- [ ] Tests (fixture): a call's `base` is the IR of its receiver; `var x = f()` stores `("call", ("name","f"), ())`;
      `for t in held` stores `("elements", ("name","held"))`; `held.append(v)` and `a[k] = v` store a put into `held`/`a`;
      `return v` stores IR; `auto [a, b] = m` stores parts of m; `T x(args)` in a block is a local.
- [ ] Implement; tests from Task 2 still pass; commit.

### Task 4: the typer evaluates IR (VALUE, LOOK, FLOW)
**Files:** Rewrite `typer.py`; modify `resolve.py` (`argument_values`, `assigned_values` read IR).
**Produces:** `Typer.evaluate(ir, unit) -> list[(type_text, unit)]`, `Typer.receiver(ir, unit) -> (keys, engine, callables)`.
- [ ] Tests: GDScript LOOK order local -> member (own, bases, engine base => engine) -> outer -> global (class_name,
      autoload, engine names); untyped parameter with two callers passing RoadTool and ToolBase -> both classes;
      inherited engine member `size` -> engine; `is` narrowing; `as` cast; await value; C++ `operator[]` with template
      substitution; library rules (`->`, `[]`, range-for pairs, `find`, `get`); deduced return from `return`s.
- [ ] Implement; commit.

### Task 5: resolution and linking on IR (CALL, NAME, SIGNAL, BIND, ENTRY)
**Files:** Modify `resolve.py`, `link.py`, `index.py` (ADL namespaces of argument classes; translation unit kept).
- [ ] Tests: ADL reaches `ns::f` from `f(ns::A{})`; internal-linkage helper per translation unit; qualified call is not
      virtual; overload by arity; Callable held in a container then `.callv()` reaches the function; signal handler
      parameters get the emit arguments; engine signal handlers get engine values.
- [ ] Implement; whole suite passes; commit.

### Task 6: the city, the log, the close
- [ ] `python tools/analysis/trace_feature.py project/game/tools/road_tool.gd:229`: the hand-traced road chain present;
      report lists unreached receivers (each a fact: no value of the code reaches it) — none caused by a missing rule.
- [ ] `run_gates.py --files-from` on changed files OK; `log/feature-trace.md` and `log/mapa.md` updated; commit, push.
