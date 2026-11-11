"""The TypeScript tree as the language groups it (log/trace-rules.md, J13; tree-sitter-typescript issue #282): an
operator before a call written with type arguments applies to the whole call, checked on the tree the readers see and
on the links and values it gives (fixture `jsvalues/regroup.ts`)."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

import tree_sitter
import tree_sitter_typescript

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))
sys.path.insert(0, str(HERE))

from support import FixtureCase, Solved  # noqa: E402
from featuretrace import ts_tree  # noqa: E402
from featuretrace.solver import CLASS, ENGINE, FALSE, STRING, TRUE, UNDEFINED  # noqa: E402

PARSER = tree_sitter.Parser(tree_sitter.Language(tree_sitter_typescript.language_typescript()))


def shape(node, data: bytes) -> str:
    """A node as `type(children...)`, a leaf as its text."""
    named = [c for c in node.children if c.is_named]
    if not named:
        return data[node.start_byte:node.end_byte].decode()
    return f"{node.type}({', '.join(shape(c, data) for c in named)})"


def grouped(code: str) -> str:
    data = code.encode()
    statement = ts_tree.grouped(PARSER.parse(data).root_node).children[0]
    return shape(statement.children[0], data)


class Tree(unittest.TestCase):
    def test_unary_applies_to_the_whole_chain(self):
        self.assertEqual("unary_expression(call_expression(member_expression(call_expression(f, type_arguments(T), "
                         "arguments(a)), catch), arguments(b)))", grouped("void f<T>(a).catch(b);"))

    def test_await_update_and_nested_operators(self):
        self.assertEqual("await_expression(call_expression(f, type_arguments(T), arguments(x)))",
                         grouped("await f<T>(x);"))
        self.assertEqual("update_expression(member_expression(call_expression(f, type_arguments(T), arguments(x)), n))",
                         grouped("++f<T>(x).n;"))
        self.assertEqual("binary_expression(a, unary_expression(call_expression(f, type_arguments(T), arguments(x))))",
                         grouped("a + -f<T>(x);"))
        self.assertEqual("unary_expression(await_expression(call_expression(f, type_arguments(T), arguments(x))))",
                         grouped("!await f<T>(x);"))

    def test_spans_and_fields(self):
        data = b"x = void f<T>(a)[0];"
        right = ts_tree.grouped(PARSER.parse(data).root_node).children[0].children[0].child_by_field_name("right")
        self.assertEqual(b"void f<T>(a)[0]", data[right.start_byte:right.end_byte])
        inner = right.child_by_field_name("argument")
        self.assertEqual(b"f<T>(a)[0]", data[inner.start_byte:inner.end_byte])
        self.assertIs(inner, [c for c in right.children if c.is_named][0])     # a field is the child itself

    def test_well_formed_tree_is_unchanged(self):
        for code in ("void f(a).catch(b);", "a && f<T>(x);", "x = await new Foo<T>(y);", "a.b<T>(x).c<U>(y);"):
            data = code.encode()
            raw = PARSER.parse(data).root_node
            self.assertEqual(shape(raw, data), shape(ts_tree.grouped(raw), data), code)


class Values(Solved, FixtureCase):
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"

    def calls(self, spec: str) -> set:
        return {n.rsplit(".", 1)[-1] for n in self.names(spec)}           # `regroup.ts.wareOf`, `Ware.open`

    def test_links_through_the_regrouped_call(self):
        for spec in ("regroupVoid", "regroupMinus", "regroupTwice"):
            self.assertTrue({"wareOf", "open"} <= self.calls(spec), (spec, self.names(spec)))
        self.assertIn("open", self.calls("regroupAwait"))

    def test_values_of_the_operator(self):
        self.assertEqual({UNDEFINED}, self.returned("regroupVoid"))
        self.assertEqual({TRUE, FALSE}, self.returned("regroupNot"))
        self.assertEqual({STRING}, self.returned("regroupTypeof"))


class HeldCalls(Solved, FixtureCase):
    """CALL'': calling a property calls the value it holds (fixture `jsvalues/held.ts`)."""
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"

    def test_call_of_a_property_holding_a_function(self):
        self.assertEqual({CLASS + "held.ts#Lidded"}, self.returned("heldCall"))
        self.assertIn("Lidded.lid", self.names("heldChain"))

    def test_call_of_a_property_typed_as_a_function(self):
        self.assertEqual({ENGINE}, self.returned("heldTyped"))         # a function the program did not write


if __name__ == "__main__":
    unittest.main()
