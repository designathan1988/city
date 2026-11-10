"""The expression tree (IR) each construct builds: VALUE over the syntax tree, never over text."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))

from featuretrace import ir_cpp, ir_gd, rules  # noqa: E402
from featuretrace.source import Source  # noqa: E402


def first_expression(lang: str, code: str):
    """The expression of the first expression statement (or the value of the first declaration) in `code`."""
    src = Source(Path("x"), "x", code.encode())
    todo = [src.parse(lang)]
    while todo:
        node = todo.pop(0)
        if node.type == "expression_statement":
            return src, node.named_children[0]
        if node.type in ("init_declarator", "variable_statement"):
            return src, node.child_by_field_name("value")
        todo += node.named_children
    raise AssertionError("no expression")


def gd(code: str):
    src, node = first_expression("gd", "func f():\n\tvar _v = " + code + "\n")
    return ir_gd.build(src, node, lambda n: 7)


def cpp(code: str):
    src, node = first_expression("cpp", "void f() { " + code + "; }")
    return ir_cpp.build(src, node, lambda n: 7)


A, B = ("name", "a"), ("name", "b")


class GdScript(unittest.TestCase):
    def test_call_on_an_operator(self):
        self.assertEqual(gd("(a + b).c()"), ("call", ("member", ("op", "+", A, B), "c", False), ()))

    def test_call_index_member_call(self):
        self.assertEqual(gd("f(x)[i].g()"), ("call", ("member", ("index", ("call", ("name", "f"), (("name", "x"),)),
                                                                   ("name", "i")), "g", False), ()))

    def test_cast_and_test(self):
        self.assertEqual(gd("x as T"), ("cast", "T"))
        self.assertEqual(gd("x is T"), ("literal", "bool"))

    def test_callable_and_bind(self):
        self.assertEqual(gd('Callable(o, "m")'), ("member", ("name", "o"), "m", False))
        self.assertEqual(gd("f.bind(1)"), ("name", "f"))

    def test_collections_ternary_await_lambda_names(self):
        self.assertEqual(gd("[a, b]"), ("collection", (A, B)))
        self.assertEqual(gd("{1: a}"), ("collection", (A,)))
        self.assertEqual(gd("a if c else b"), ("union", A, B))
        self.assertEqual(gd("await a"), ("await", A))
        self.assertEqual(gd("func(): pass"), ("callable", 7))
        self.assertEqual(gd('&"m"'), ("strname", "m"))
        self.assertEqual(gd("self.a"), ("member", ("self",), "a", False))
        self.assertEqual(gd("$Node"), ("node",))
        self.assertEqual(gd('preload("res://x.gd")'), ("cast", "path:res://x.gd"))
        self.assertEqual(gd("1"), ("literal", "int"))

    def test_every_expression_construct_has_a_builder(self):
        wanted = {k for k, v in rules.GD_RULES.items() if "EXPR" in v} | {"lambda", "string_name", "base_call"}
        self.assertLessEqual(wanted, set(ir_gd.BUILDERS))


class Cpp(unittest.TestCase):
    def test_member_arrow_index_call(self):
        self.assertEqual(cpp("p->q"), ("member", ("name", "p"), "q", True))
        self.assertEqual(cpp("f(x)[i].g()"), ("call", ("member", ("index", ("call", ("name", "f"), (("name", "x"),)),
                                                                    ("name", "i")), "g", False), ()))

    def test_casts_makers_new(self):
        self.assertEqual(cpp("static_cast<T>(x)"), ("cast", "T"))
        self.assertEqual(cpp("std::make_unique<T>()"), ("cast", "std::unique_ptr<T>"))
        self.assertEqual(cpp("new T(1)"), ("new", "T"))
        self.assertEqual(cpp("(T)x"), ("cast", "T"))

    def test_operators(self):
        self.assertEqual(cpp("a + b"), ("op", "+", A, B))
        self.assertEqual(cpp("*a"), ("deref", A))
        self.assertEqual(cpp("&a"), ("addr", A))
        self.assertEqual(cpp("c ? a : b"), ("union", A, B))
        self.assertEqual(cpp("a = b"), A)
        self.assertEqual(cpp("ns::A::f"), ("name", "ns::A::f"))
        self.assertEqual(cpp("this"), ("self",))
        self.assertEqual(cpp("[&]() {}"), ("callable", 7))
        self.assertEqual(cpp("T v = {a, b}"), ("collection", (A, B)))

    def test_every_expression_construct_has_a_builder(self):
        wanted = {k for k, v in rules.CPP_RULES.items() if "EXPR" in v}
        self.assertLessEqual(wanted, set(ir_cpp.BUILDERS))


if __name__ == "__main__":
    unittest.main()
