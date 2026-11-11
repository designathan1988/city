"""Readers keep the expression tree (IR) wherever a value flows: receivers, arguments, assignments, returns, loops,
container puts (log/trace-rules.md: FLOW, CALL). No reference or flow holds text to be parsed again."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))

from featuretrace.cpp_read import read_cpp  # noqa: E402
from featuretrace.gd_read import read_gd  # noqa: E402
from featuretrace.source import Source  # noqa: E402

GD = '''extends Node
var held = []
var core
func f(p, q: int):
	var a = g(p)
	core = ClassDB.instantiate(&"Core")
	core.run(a, 2)
	for t in held:
		t.go()
	held.append(a)
	if p is Foo:
		p.only_foo()
	return a
'''

CPP = '''struct S { int x; };
int f(S *p, std::map<int, S> m) {
	auto a = make(p);
	p->go(a);
	for (auto &[k, v] : m) v.touch();
	std::vector<S> list;
	list.push_back(*p);
	T obj(a);
	return a;
}
'''


def unit(facts, name):
    return next(u for u in facts.units if u.name == name)


def refs(u, kind, name):
    return [r for r in u.refs if r.kind == kind and r.name == name]


class GdScriptFlows(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.f = unit(read_gd(Source(Path("x.gd"), "x.gd", GD.encode()), "res://x.gd"), "f")

    def test_locals_and_parameters_hold_ir(self):
        self.assertEqual(self.f.types["a"], [("call", ("name", "g"), (("name", "p"),))])
        self.assertEqual(self.f.types["q"], [("cast", "int")])
        self.assertEqual(self.f.types["p"], [])
        self.assertEqual(self.f.types["t"], [("elements", ("name", "held"))])

    def test_call_receiver_and_arguments_are_ir(self):
        run = refs(self.f, "call", "run")[0]
        self.assertEqual(run.base, ("name", "core"))
        self.assertEqual(run.args, (("name", "a"), ("literal", "int")))
        self.assertEqual(refs(self.f, "call", "go")[0].base, ("name", "t"))

    def test_writes_carry_the_value(self):
        write = refs(self.f, "write", "core")[0]
        self.assertEqual(write.args, (("call", ("member", ("name", "ClassDB"), "instantiate", False),
                                       (("strname", "Core"),)),))
        put = refs(self.f, "write", "held")[0]
        self.assertEqual(put.args, (("put", ("name", "a")),))

    def test_is_narrows_the_receiver_inside_the_branch(self):
        self.assertEqual(refs(self.f, "call", "only_foo")[0].base, ("narrow", ("name", "p"), ("is", "Foo"), True))

    def test_returns_hold_ir(self):
        self.assertEqual(self.f.returns[0][2], ("name", "a"))


class CppFlows(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.f = unit(read_cpp(Source(Path("x.cpp"), "x.cpp", CPP.encode())), "f")

    def test_locals_and_parameters_hold_ir(self):
        self.assertEqual(self.f.types["a"], [("call", ("name", "make"), (("name", "p"),))])
        self.assertEqual(self.f.types["p"], [("cast", "S *")])
        self.assertEqual(self.f.types["v"], [("member", ("elements", ("name", "m")), "second", False)])
        self.assertEqual(self.f.types["obj"], [("cast", "T")])

    def test_arrow_receiver_is_what_the_pointer_points_at(self):
        go = refs(self.f, "call", "go")[0]
        self.assertEqual(go.base, ("deref", ("name", "p")))
        self.assertEqual(go.args, (("name", "a"),))

    def test_container_put(self):
        self.assertEqual(refs(self.f, "write", "list")[0].args, (("put", ("deref", ("name", "p"))),))

    def test_returns_hold_ir(self):
        self.assertEqual(self.f.returns[0][2], ("name", "a"))


if __name__ == "__main__":
    unittest.main()
