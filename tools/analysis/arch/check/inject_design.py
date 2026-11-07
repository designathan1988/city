"""Injected structure: a feature nobody told the program about, a god, a repeated rule, a decision taken in many places, a role without a contract."""


def files(w, expect, places, join):
    def case(cid):
        word = w()
        expect.append((cid, word))
        return word

    god, rule, decide, role, scattered = case("god"), case("repeated_rule"), case("repeated_decision"), case("implicit_role"), case("scattered")
    out = {}
    pick = lambda: places["any_script"]() or "project"  # noqa: E731
    parts = []
    for i in range(12):
        name = w()
        parts.append(name)
        out[join(pick(), f"{name}.gd")] = f"""# Injected part {i}.
class_name Part_{name}
extends Node

var {name}_state := 0
var mode_{decide} := Shared_{decide}.flag_{decide}

func {name}_one() -> void:
	{name}_state = 1
	{name}_two()
	All_{god}.new().{god}_{name}()

func {name}_two() -> void:
	{name}_state = 2
	if Shared_{decide}.flag_{decide}:
		{name}_one()

func {role}_open() -> void:
	{name}_state = 3

func {role}_shut() -> void:
	{name}_state = 4

func {role}_fill() -> void:
	{name}_state = 5

func {role}_drain() -> void:
	{name}_state = 6

func {role}_lift() -> void:
	{name}_state = 7

func {role}_drop() -> void:
	{name}_state = 8

func {role}_mark() -> void:
	{name}_state = 9

func {role}_clean() -> void:
	{name}_state = 10

func {rule}_{name}(a: int, b: int) -> int:
	var t := a * b + 7
	for i in a:
		t += i * b - 3
		if t > b:
			t -= a
	return t + b

func {scattered}_{name}() -> void:
	Shared_{decide}.{scattered}_first = 1
	Shared_{decide}.{scattered}_second = 2
"""
    out[join(pick(), f"shared_{decide}.gd")] = f"""# Injected shared state.
class_name Shared_{decide}
extends Node

static var flag_{decide} := false
static var {scattered}_first := 0
static var {scattered}_second := 0
"""
    calls = "\n".join(f"func {god}_{n}() -> void:\n	var p := Part_{n}.new()\n	p.{n}_one()\n	p.{n}_state = 9\n" for n in parts)
    out[join(pick(), f"{god}_all.gd")] = f"# Injected class that does everything.\nclass_name All_{god}\nextends Node\n\n{calls}\n"
    return out
