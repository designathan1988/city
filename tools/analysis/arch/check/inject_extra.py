"""The remaining classes: each gets a case of its own, so no line of the relation stands on the real repository alone."""
from .inject_script import engine_call


def script(w, expect, stem, api, shared):
    """A second script with what is left of the classes that live in scripts and data, and a feature no one told the program about."""
    def case(*cids):
        word = w()
        for c in cids:
            expect.append((c, word))
        return word

    parts, save, tier, cmd, hour, size, focus, user, line, mask = (case(c) for c in ("B53", "B54", "B81", "B84", "D21", "D22", "D24", "D14", "D16", "D10"))
    feature, note, remark, copied, level, skin = case("feature"), case("D19"), case("D19"), case("B47"), case("tier_price"), case("B46")
    head = f'''# Injected second script, as `{remark}_note` tells.
class_name Extra_{stem}
extends Node

const {shared}_SIZE := 4
var {tier}_quality := 1
var {hour}_hour := 1
var {size}_size := 1
var cfg := ConfigFile.new()

func {parts}_load() -> Array:
	return ["res://{parts}_a.json", "res://{parts}_b.json"]

func {save}_store() -> void:
	cfg.set_value("section", "{save}_key", 1)

func {tier}_pick() -> void:
	if {tier}_quality:
		print(1)

func {cmd}_read() -> void:
	var words := OS.get_cmdline_user_args()

func {hour}_light() -> void:
	if {hour}_hour:
		print(2)

func {size}_scale() -> void:
	if {size}_size:
		print(3)

func focus_{focus}() -> void:
	pass

func {user}_path() -> String:
	return "user://{user}.cfg"

func {line}_say() -> void:
	print("LINE_{line}")

func _process(delta: float) -> void:
	var r = "{level}_span"

func material_{skin}(n: String) -> void:
	pass

func {skin}_clothe() -> void:
	material_{skin}("{skin}_coat")

func {mask}_sheets() -> void:
	{engine_call(api, "layer", "mask")}

func {feature}_start() -> void:
	var a = "{feature}_speed"
	{feature}_step()

func {feature}_step() -> void:
	var b = "{feature}_limit"
	var m := ShaderMaterial.new()
	m.set_shader_parameter(&"{feature}_tint", 1.0)
	{feature}_finish()

func {feature}_finish() -> void:
	var c = "{feature}_speed"
	var d := load("res://{feature}_rules.json")
	{feature}_start()
'''
    data = (f'{{\n "_about": "kept for `{note}_note` to read",\n "{feature}_speed": 1,\n "{feature}_limit": 2,\n "{copied}_glow": 1,\n "{skin}_coat": 1,\n'
            f' "{level}_levels": {{\n  "plain_{level}": {{"{level}_span": 1}},\n  "fine_{level}": {{"{level}_span": 9}}\n }}\n}}\n')
    shader = f"// Injected.\nshader_type spatial;\nuniform float {feature}_tint = 1.0;\nuniform float {copied}_glow = 1.0;\nvoid fragment() {{\n	ALBEDO = vec3({feature}_tint);\n}}\n"
    tool = f'"""Injected tool."""\n\n\ndef {line}_seek(text):\n    return "LINE_{line}" in text\n'
    return head, {f"{parts}_a.json": "{}\n", f"{parts}_b.json": "{}\n", f"{feature}_rules.json": data}, {f"{feature}_look.gdshader": shader}, {f"{line}_observe.py": tool}


def extras(expect, stem):
    """Classes the code already injected also stands for: the word is the case's own."""
    table = dict((c, x) for c, x in expect)
    for cid, source in (("B58", "B56"), ("B64", "B56"), ("B65", "B56"), ("B66", "B56"), ("B67", "B56"), ("B71", "B74"), ("B73", "B24"), ("B75", "streaming"), ("B82", "B30"),
                        ("B85", "gpu_upload"), ("B86", "B27"), ("B101", "no_reader"), ("B102", "no_effect"), ("D1", "B17"), ("D2", "B18"), ("D6", "B74"), ("D8", "gpu_upload"),
                        ("D20", "level_flip"), ("D23", "pipeline_compile"), ("B94", "implicit_role"), ("B95", "repeated_decision"), ("B96", "god"), ("B97", "repeated_decision"),
                        ("B98", "repeated_decision"), ("missing_owner", "implicit_role"), ("missing_owner", "repeated_rule"), ("missing_owner", "repeated_decision")):
        expect.append((cid, table[source]))
    for cid in ("B80", "B83", "B87"):
        expect.append((cid, stem))


def small_cpp(w, expect, stem, define):
    """What needs a header, a second file or a guard of its own. `define` is a macro only one build configuration sets."""
    def case(*cids):
        word = w()
        for c in cids:
            expect.append((c, word))
        return word

    inl, dic, tag, ver, dead, glob, gen1, gen2, debug, order = case("B15"), case("B51"), case("B55"), case("B76"), case("B100"), case("D13"), case("B90"), w(), case("B78"), case("D7")
    shared = w().upper()
    expect.append(("D18", shared))
    expect.append(("B90", gen2))
    header = f"// Injected header.\n#pragma once\ninline int {inl}_inline() {{ return 1; }}\n"
    source = f'''// Injected second source.
#include "{stem}_more.h"
#include <atomic>

std::atomic<int> {order}_flag{{ 0 }};
int {order}_look() {{ return {order}_flag.load(std::memory_order_acquire); }}
constexpr int TAG_{tag} = 7;
const int {shared}_SIZE = 4;
int {glob}_counter = 0;
int lookup_{dic}(const char *key) {{ return key != nullptr; }}
int {dic}_ask() {{ return lookup_{dic}("{dic}_key"); }}
int {tag}_write() {{ return TAG_{tag}; }}
int {tag}_read() {{ return TAG_{tag} + {inl}_inline(); }}
int {dead}_unused() {{ return 1; }}
#ifdef {define}
int {debug}_checked() {{ return 2; }}
#endif

struct Header_{ver} {{
	int version_{ver} = 1;
	int read() {{
		if (version_{ver} < 3) return 0;
		return 1;
	}}
}};
'''
    generated = {f"{gen1}_made.cpp": f"// Generated by the tool from models/{gen1}/one.txt: do not edit.\nint {gen1}_a = 1;\n",
                 f"{gen2}_made.cpp": f"// Generated by the tool from models/{gen2}/two.txt: do not edit.\nint {gen2}_b = 1;\n"}
    return header, source, generated, shared


def recorded(w, expect):
    """A function and the text of a profile saying it ran: the small repository only, the real one has its own recording."""
    word = w()
    expect.append(("B93", word))
    return word, f'extern "C" int {word}_ran() {{ return 1; }}\n', f"{word}_ran\n# Func Hash:\n1\n# Num Counters:\n1\n# Counter Values:\n5\n\n"


def evolved(w, expect):
    """Commits of a small tree of its own: a piece renamed, one moved, one split away. Real history has its own."""
    def case(cid):
        word = w()
        expect.append((cid, word))
        return word

    renamed, moved, split = case("B103"), case("B104"), case("B105")
    body = "	var total := 0\n	for i in 9:\n		total += i * 3 - 1\n	return total\n"
    other = "	var t := 1\n	while t < 5:\n		t *= 2\n	return t + 2\n"
    first = {
        "evolve/one.gd": f"# Injected.\nextends Node\n\nfunc prior_{renamed}() -> int:\n{body}",
        "evolve/two.gd": f"# Injected.\nextends Node\n\nfunc {moved}_walks() -> int:\n{other}",
        "evolve/three.gd": f"# Injected.\nextends Node\n\nfunc {split}_stays() -> int:\n	return 1\n\nfunc {split}_leaves() -> int:\n	var n := 5\n	n -= 2\n	return n * n\n",
    }
    second = {
        "evolve/one.gd": f"# Injected.\nextends Node\n\nfunc new_{renamed}() -> int:\n{body}",
        "evolve/two.gd": None,
        "evolve/place/two_moved.gd": f"# Injected.\nextends Node\n\nfunc {moved}_walks() -> int:\n{other}",
        "evolve/three.gd": f"# Injected.\nextends Node\n\nfunc {split}_stays() -> int:\n	return 1\n",
        "evolve/three_part.gd": f"# Injected.\nextends Node\n\nfunc {split}_leaves() -> int:\n	var n := 5\n	n -= 2\n	return n * n\n",
    }
    third = {"evolve/place/two_moved.gd": second["evolve/place/two_moved.gd"] + "\nfunc grown() -> int:\n	return 3\n"}
    return first, second, third
