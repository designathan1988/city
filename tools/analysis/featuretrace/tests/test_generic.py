"""The feature tracer is generic (owner's binding order; log/trace-rules.md, "The feature tracer is generic"): it reads
the folder it is given (or the current one), its source names no project, and it holds no rule, table, list or special
case for a library, a framework, an API member or a code pattern. Every module-level table of names in its source is
the languages' own data (grammar node kinds, keywords, operators, constants and types the specifications define, file
formats of the languages' tools: `LANGUAGE`) or a debt of the old core (a platform's or library's member names written
by hand: `DEBTS`, each removed by `log/trace-core-plan.md`, so the list only shrinks). The new core (`featuretrace/heap`)
holds no table of names and compares no value with a name: what a platform does comes from its declarations, as data."""
from __future__ import annotations

import ast
import re
import subprocess
import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
TOOL = HERE.parents[1]
CORE = TOOL / "featuretrace"
CLI = TOOL / "trace_feature.py"
FIXTURE = HERE / "fixture"
# the layout and names of the repository the tool happens to live in
FOREIGN = re.compile(r"native/src|project/|dist/|\bcity\b|CityCore|road|Roadcraft|parents\[2\]", re.IGNORECASE)
IDENT = re.compile(r"^[A-Za-z_$][\w$.:]*$|^\[Symbol\.\w+\]$")

# The languages' own data, each with where it comes from.
LANGUAGE = {
    "bounds.py:PRIMITIVE_TOKENS": "TypeScript's primitive types (handbook, Everyday Types)",
    "cpp_ambig.py:VALUE_KINDS": "tree-sitter-cpp node kinds", "cpp_ambig.py:TYPE_KINDS": "tree-sitter-cpp node kinds",
    "cpp_expr.py:LITERALS": "tree-sitter-cpp node kinds", "cpp_expr.py:STATEMENTS": "tree-sitter-cpp node kinds",
    "cpp_implicit.py:OBJECT_DECLARATORS": "tree-sitter-cpp node kinds", "cpp_names.py:WRAPPERS": "tree-sitter-cpp kinds",
    "cpp_read.py:CLASS_NODES": "tree-sitter-cpp node kinds", "cpp_read.py:RECURSE": "tree-sitter-cpp node kinds",
    "flows.py:CARRYING": "the tracer's link kinds", "flows.py:WRITTEN_CALLS": "the tracer's link kinds",
    "gd_body.py:STATEMENTS": "tree-sitter-gdscript node kinds", "ir_cpp.py:BUILDERS": "tree-sitter-cpp node kinds",
    "ir_gd.py:TESTS": "GDScript operators", "ir_gd.py:LITERAL_TYPES": "tree-sitter-gdscript node kinds",
    "ir_gd.py:BUILDERS": "tree-sitter-gdscript node kinds", "ir_js.py:TESTS": "ECMA-262 relational operators",
    "ir_js.py:WORD_UNARY": "ECMA-262 unary operators", "ir_js.py:LITERALS": "tree-sitter-typescript node kinds",
    "ir_js.py:FUNCTIONS": "tree-sitter-typescript node kinds", "ir_js.py:BUILDERS": "tree-sitter-typescript node kinds",
    "js_body.py:__all__": "Python exports", "js_body.py:HOISTED": "tree-sitter-typescript node kinds",
    "js_body.py:SILENT": "tree-sitter-typescript node kinds", "js_bounds.py:PREDEFINED": "TypeScript predefined types",
    "js_bounds.py:CONSTANTS": "tree-sitter literal type kinds", "js_class.py:ACCESSORS": "ECMA-262 get and set",
    "js_config.py:PACKAGE_FIELDS": "package.json format (npm docs)", "js_decl.py:DECLARATIONS": "tree-sitter node kinds",
    "js_expr.py:PLAIN": "tree-sitter-typescript node kinds", "js_expr.py:FUNCTION_VALUES": "tree-sitter node kinds",
    "js_expr.py:SKIP": "tree-sitter-typescript node kinds", "js_functions.py:THIS_OF": "tree-sitter node kinds",
    "js_guards.py:CONSTANTS": "ECMA-262 constants", "js_guards.py:INNER": "tree-sitter-typescript node kinds",
    "js_read.py:PROPERTY_MARKS": "tree-sitter-typescript node kinds",
    "js_specifiers.py:CONDITIONS": "Node.js package resolution", "js_specifiers.py:MAIN_FIELDS": "Node.js resolution",
    "js_types.py:WRAPPERS": "tree-sitter-typescript node kinds", "js_types.py:EITHER": "tree-sitter node kinds",
    "js_types.py:NAMES": "tree-sitter-typescript node kinds", "js_types.py:PREDEFINED": "TypeScript predefined types",
    "js_types.py:CONSTANTS": "ECMA-262 constants", "js_types.py:CALLABLE_TYPES": "tree-sitter node kinds",
    "js_library.py:PRIMITIVES": "TypeScript predefined types", "link.py:JS_KINDS": "the tracer's reference kinds",
    "link.py:WANT": "the tracer's reference kinds", "model.py:FUNCTION_KINDS": "the tracer's unit kinds",
    "model.py:STATE_KINDS": "the tracer's unit kinds", "model.py:LOCAL_REFS": "the tracer's reference kinds",
    "narrowing.py:ASSIGNING": "tree-sitter node kinds", "narrowing.py:NAMES": "tree-sitter node kinds",
    "narrowing.py:PATTERNS": "tree-sitter node kinds", "narrowing.py:FUNCTIONS": "tree-sitter node kinds",
    "narrowing.py:LEAVING": "tree-sitter node kinds", "narrowing.py:BLOCKS": "tree-sitter node kinds",
    "native_frames.py:MAP_HEADER": "lld's map file format", "places.py:NO_VALUE": "the languages' types with no value",
    "receivers.py:RECEIVING": "the tracer's link kinds", "report.py:ARROW": "the tracer's link kinds",
    "resolve.py:SIGNAL_KINDS": "the tracer's unit kinds", "rule_table.py:RULE_IDS": "the tracer's rule ids",
    "rule_table.py:GD_RULES": "tree-sitter-gdscript node kinds", "rule_table.py:CPP_RULES": "tree-sitter-cpp node kinds",
    "rule_table.py:JS_RULES": "tree-sitter-typescript node kinds", "rules.py:GDSCRIPT_KEYWORDS": "GDScript keywords",
    "rules.py:CPP_KEYWORDS": "C++ keywords", "rules.py:CPP_PREPROCESSOR": "C++ preprocessor directives",
    "rules.py:JS_KEYWORDS": "ECMA-262 and TypeScript keywords", "rules.py:ROLES": "the tracer's roles",
    "rules.py:HANDLERS": "the tracer's handler registry", "source.py:LANG_BY_EXT": "the languages' file extensions",
    "source.py:CONFIG_FILES": "tsconfig and jsconfig file names", "source.py:_GRAMMARS": "the grammars",
    "templates.py:PASS_SHAPES": "the tracer's tree shapes", "transfer.py:OPERATORS": "C++ operators, [stmt.ranged]",
    "transfer.py:COMPARISONS": "the languages' comparison operators", "truth.py:JS_CONSTANTS": "ECMA-262 constants",
    "truth.py:TYPEOF": "ECMA-262 typeof results", "truth.py:KINDS": "ECMA-262 value kinds",
    "truth.py:LITERAL_VALUES": "ECMA-262 literal kinds", "ts_tree.py:OPERANDS": "tree-sitter-typescript node kinds",
    "ts_tree.py:CHAIN": "tree-sitter-typescript node kinds", "typer.py:STEPS": "the tracer's tree shapes",
    "typer.py:CONSTANT_SHAPES": "the tracer's tree shapes", "walk.py:RUNS": "the tracer's link kinds",
}

# A platform's, a library's or a program's member names written by hand: the old core's engessamento, each removed by
# log/trace-core-plan.md (the platforms from their declarations; what no declaration says, summarized generically from
# the declarations). The list only shrinks.
DEBTS = {
    "bounds.py:ARRAY_MEMBERS", "cpp_expr.py:THREAD_NAMES", "gd_calls.py:PUTS", "js_bounds.py:SAME_TYPE",
    "js_bounds.py:ANY_OBJECT", "js_bounds.py:LIBRARY", "js_defines.py:DEFINERS", "js_guards.py:LANGUAGE_PREDICATES",
    "js_library.py:CANONICAL", "js_library.py:UNWRAP", "js_library.py:ELEMENT_CALLS", "js_library.py:SAME_CALLS",
    "js_library.py:COUNTS", "js_library.py:OBJECT_READS", "js_primitives.py:OBJECT_PROTOTYPE",
    "js_primitives.py:STRING_METHODS", "js_primitives.py:KINDS", "js_transfer.py:OBJECT_MEMBERS",
    "js_transfer.py:FUNCTION_MEMBERS", "library.py:ITERATOR_CALLS", "library.py:HELD_CALLS", "library.py:PAIR_PARTS",
    "model.py:ENGINE_CALLBACKS", "model.py:STD_MUTATORS", "rules.py:NAME_CALLS", "rules.py:SIGNAL_OPS",
    "rules.py:BIND_CALLS", "rules.py:GDEXT_BIND", "rules.py:NAME_CONSTRUCTORS", "rules.py:CASTS", "rules.py:MAKERS",
    "rules.py:LIBRARY_OWNERS", "rules.py:PASS_THROUGH", "rules.py:EVENT_SUBSCRIBE", "rules.py:EVENT_FIRE",
    "rules.py:MESSAGE_FIRE", "rules.py:EVENT_OBJECTS", "rules.py:HIGHER_ORDER", "rules.py:CALL_FORMS",
    "rules.py:COERCION_MEMBERS", "transfer.py:CALLABLE_CALLS", "transfer.py:BIND_CALLS",
}


def strings(node) -> list[str]:
    return [n.value for n in ast.walk(node) if isinstance(n, ast.Constant) and isinstance(n.value, str)]


def tables(path: Path) -> dict[str, int]:
    """The module-level assignments whose value holds two names or more: `file:NAME` -> how many."""
    found = {}
    for st in ast.parse(path.read_text(encoding="utf-8")).body:
        if isinstance(st, ast.Assign):
            targets, value = [t.id for t in st.targets if isinstance(t, ast.Name)], st.value
        elif isinstance(st, ast.AnnAssign) and isinstance(st.target, ast.Name):
            targets, value = [st.target.id], st.value
        else:
            continue
        names = [s for s in strings(value) if IDENT.match(s)] if value is not None else []
        if len(names) >= 2:
            for target in targets:
                found[f"{path.relative_to(CORE).as_posix()}:{target}"] = len(names)
    return found


def written_names(path: Path) -> list[str]:
    """Every string a module writes that has the shape of a name, docstrings aside (`x == "push"`, `lookup(k, "m")`,
    `f"new:{k}"`): `line: name`. The new core writes none: its vocabulary is the tracer's own constants (`vocab`) and
    the languages' names their specifications define (`languages`), imported."""
    tree = ast.parse(path.read_text(encoding="utf-8"))
    docs = set()
    for node in ast.walk(tree):
        if isinstance(node, (ast.Module, ast.FunctionDef, ast.ClassDef, ast.AsyncFunctionDef)) and node.body \
                and isinstance(node.body[0], ast.Expr) and isinstance(node.body[0].value, ast.Constant):
            docs.add(id(node.body[0].value))
    return [f"{n.lineno}: {n.value}" for n in ast.walk(tree) if isinstance(n, ast.Constant)
            and isinstance(n.value, str) and id(n) not in docs and IDENT.match(n.value.strip(":"))]


def found_tables() -> dict[str, int]:
    found = {}
    for path in sorted(CORE.glob("*.py")):
        found |= tables(path)
    return found


class Generic(unittest.TestCase):
    def test_reads_the_current_folder_by_default(self):
        run = subprocess.run([sys.executable, str(CLI), "--no-cache", "RoadTool._lay"], cwd=FIXTURE,
                             capture_output=True, text=True, timeout=120)
        self.assertEqual(run.returncode, 0, run.stderr)
        self.assertIn("fx::Core::build_road", run.stdout)

    def test_source_names_no_project(self):
        sources = [CLI] + sorted(CORE.glob("*.py")) + sorted(CORE.glob("heap/*.py"))
        found = [f"{p.name}:{i}: {line.strip()}" for p in sources
                 for i, line in enumerate(p.read_text(encoding="utf-8").splitlines(), 1) if FOREIGN.search(line)]
        self.assertEqual(found, [])

    def test_every_table_of_names_is_the_languages_or_a_listed_debt(self):
        unknown = sorted(set(found_tables()) - set(LANGUAGE) - DEBTS)
        self.assertEqual([], unknown, "a new table of names: the tracer is generic (log/trace-rules.md); read the "
                                      "names from the languages' specifications or the platforms' declarations")

    def test_the_debts_only_shrink(self):
        self.assertEqual([], sorted(DEBTS - set(found_tables())), "a debt removed from the code: remove it from DEBTS")

    def test_the_new_core_writes_no_name(self):
        for path in sorted(CORE.glob("heap/*.py")):
            self.assertEqual({}, tables(path), f"{path.name}: the new core holds no table of names")
            self.assertEqual([], written_names(path), f"{path.name}: the new core writes no name (vocab, languages)")

    def test_the_gate_fails_where_it_must(self):
        planted = HERE / "fixture" / "generic" / "planted.py"
        self.assertEqual(["planted.py:CALLS"], [k.rsplit("/", 1)[-1] for k in tables_rel(planted)])
        self.assertEqual(["2: push", "2: pop", "6: forEach", "7: constructor"], written_names(planted))


def tables_rel(path: Path) -> list[str]:
    """`tables` of a file outside the core (the gate's own fixture)."""
    return [k.split(":", 1)[0].rsplit("/", 1)[-1] + ":" + k.split(":", 1)[1] for k in tables_outside(path)]


def tables_outside(path: Path) -> dict[str, int]:
    found = {}
    for st in ast.parse(path.read_text(encoding="utf-8")).body:
        if isinstance(st, ast.Assign):
            names = [s for s in strings(st.value) if IDENT.match(s)]
            if len(names) >= 2:
                found |= {f"{path.name}:{t.id}": len(names) for t in st.targets if isinstance(t, ast.Name)}
    return found


if __name__ == "__main__":
    unittest.main()
