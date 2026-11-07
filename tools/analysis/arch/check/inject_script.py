"""Injected scripts, data, scenes and shaders: the classes that live outside C++. Engine calls are picked from the engine's description by the words
of a cost, so no method is written here."""


def engine_call(api, *words):
    """A call of the description's singletons whose name has these words, with null for every argument."""
    want = set(words)
    from ..core import words_of
    for name in sorted(api.methods):
        for owner, args, _, _ in api.methods[name]:
            if owner in api.singletons and want <= set(words_of(name)):
                return f"{owner}.{name}({', '.join('null' for _ in args)})"
    return "pass"


def texts(w, expect, api, data_name, shader_name, scene_name, include_name, stem):
    def case(cid):
        word = w()
        expect.append((cid, word))
        return word

    d, q, tb, en, cl, bi, ca, cv, df, hm, gs = (case(c) for c in ("B16", "B17", "B19", "B18", "B20", "B21", "B22", "B23", "B24", "B25", "B26"))
    ma, sg, co, ti, gr, nd = (case(c) for c in ("B29", "B31", "B32", "B34", "B38", "B39"))
    tr_, st, aw, fn = (case(c) for c in ("B52", "D4", "D5", "B68"))
    for cid in ("B35", "B36", "B37", "B40", "B41", "B42", "B44", "B69", "B70", "B72"):
        expect.append((cid, stem))
    cb, inp, pl, ld = w(), w(), w(), w()
    upl, shd, mat, tin, wid, pc, mf, ri, ec, lf, qf = (case(c) for c in ("gpu_upload", "shadow", "material", "tiny_triangles", "wide_bounds", "pipeline_compile",
                                                                         "measure_feedback", "rebuilt_interface", "engine_copies", "level_flip", "query_per_frame"))
    script = f"""# Injected by the selfcheck: every case below stands for one class of link or one cost.
class_name Script_{d}
extends Node

signal {sg}_altered(a: int)
enum Kind_{en} {{ ONE_{en}, TWO_{en} }}
const {tb}_TABLE := {{&"{d}_key": "area/{d}_method", &"{q}_key": "area/{q}_method"}}
var {st}_flag := true:
	set(v):
		{st}_flag = v
		{st}_after()
var {fn}_way := {{}}
var {gs}_flag := 0
var {mf}_rows: Array = []

func {st}_after() -> void:
	pass

func {d}_method() -> void:
	pass

func {q}_method() -> void:
	pass

func {cl}_handler(a: int = 0) -> void:
	pass

func {bi}_handler(a: int = 0) -> void:
	pass

func {ca}_handler() -> void:
	pass

func {cv}_handler() -> void:
	pass

func {df}_handler() -> void:
	pass

func {hm}_handler() -> void:
	pass

func {co}_pressed() -> void:
	pass

func {fn}_keep() -> void:
	{fn}_way["x"] = Callable(self, "{cb}_task")

func {d}_send() -> void:
	{d}_post(&"{d}_key")
	{d}_post(&"{q}_key")

func {d}_post(word: StringName) -> void:
	pass

func {co}_handler(a: int = 0) -> void:
	pass

func {cb}_task() -> void:
	pass

func _ready() -> void:
	add_to_group(&"{gr}_group")
	{sg}_altered.connect({co}_handler)
	var hold := Callable(self, "{cl}_handler")
	hold.call()
	var made := {bi}_handler.bind(1)
	self.call(&"{ca}_handler")
	self.callv(&"{cv}_handler", [])
	self.call_deferred(&"{df}_handler")
	if has_method(&"{hm}_handler"):
		self.set(&"{gs}_flag", 1)
	get_node("{nd}_path")
	var a := preload("res://{include_name}")
	var b := load("res://" + "{ld}_{include_name}")
	var c := load("res://{scene_name}")
	var e := load("res://{shader_name}")
	tr("{tr_}.%s" % name)
	{st}_flag = false

func {aw}_later() -> void:
	await get_tree().create_timer(1.0).timeout
	print(1)

func _input(event: InputEvent) -> void:
	{inp}_seen(event)

func {inp}_seen(event: InputEvent) -> void:
	pass

func {pl}_loaded() -> void:
	pass

func {ti}_timer() -> void:
	await get_tree().create_timer(2.0).timeout

func {ma}_decide(kind: int, word: String) -> void:
	match word:
		&"{ma}_a":
			{cb}_task()
		_:
			pass
	var tool := Kind_{en}[word]

func {mf}_heavy() -> void:
	for r in {mf}_rows:
		print(r)

func {ri}_remake() -> void:
	for c in get_children():
		c.queue_free()

func {ec}_per_item() -> void:
	for i in 3:
		{engine_call(api, "instance", "create")}

func {upl}_send() -> void:
	{engine_call(api, "multimesh", "set", "buffer")}

func {shd}_throw() -> void:
	{engine_call(api, "cast", "shadows")}

func {mat}_alter() -> void:
	{engine_call(api, "material", "set", "param")}

func {tin}_draw() -> void:
	{engine_call(api, "multimesh", "allocate")}

func {wid}_box() -> void:
	{engine_call(api, "custom", "aabb")}

func {pc}_make() -> void:
	{engine_call(api, "shader", "create")}

func {lf}_range() -> void:
	{engine_call(api, "visibility", "range")}

func {qf}_ask() -> void:
	self.call(&"{qf}_answer")

func {qf}_answer() -> void:
	for r in {mf}_rows:
		print(r)

func _process(delta: float) -> void:
	{upl}_send()
	{shd}_throw()
	{mat}_alter()
	{tin}_draw()
	{pc}_make()
	{ri}_remake()
	{qf}_ask()
	var now := Time.get_ticks_usec()
	{mf}_heavy()
"""
    return script


def data_text(w, expect):
    def case(cid):
        word = w()
        expect.append((cid, word))
        return word

    key, choice, dead, tier = case("B50"), case("B30"), case("no_reader"), case("default_tier")
    return (f'{{\n "{key}_used": 1,\n "{dead}_unread": 2,\n "{choice}_mode": "{choice}_value",\n "default_preset_{tier}": 3\n}}\n',
            f'	var {key}_a = "{key}_used"\n	var {choice}_b = "{choice}_value"\n	var {tier}_c = "default_preset_{tier}"\n')


def shader_text(w, expect):
    def case(cid):
        word = w()
        expect.append((cid, word))
        return word

    un, gl, ins, va, inc, gp, var = (case(c) for c in ("B47", "B48", "B49", "D9", "B45", "gpu_pass", "variants"))
    cost = w()
    big = "\n".join(f"	v += sin(v) * cos(v * {i}.5) + texture(tex_{cost}, vec2(v)).r;" for i in range(1, 2000))
    return f"""// Injected by the selfcheck: a shader with one case of each class.
shader_type spatial;
#include "res://{inc}_inc.gdshaderinc"
uniform float {un}_boost = 1.0;
global uniform float {gl}_dusk;
instance uniform float {ins}_keep = 1.0;
uniform sampler2D {gp}_screen : hint_screen_texture;
uniform sampler2D tex_{cost};
varying float {va}_borne;

float {cost}_many(float v) {{
{big}
	return v;
}}

void vertex() {{
	{va}_borne = {un}_boost + {gl}_dusk + {ins}_keep;
}}

void fragment() {{
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
	ALBEDO = vec3({va}_borne);
#else
	ALBEDO = vec3({cost}_many({va}_borne));
#endif
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
	ROUGHNESS = 1.0;
#endif
#if {var}_X == 1
	ROUGHNESS = 0.5;
#endif
	ALPHA = {cost}_many(0.5);
}}
""", f"// Injected include.\nfloat {inc}_helper(float v) {{ return v; }}\n", un


def scene_text(w, expect, script_name, word_of_connection):
    def case(cid):
        word = w()
        expect.append((cid, word))
        return word

    con, sc = word_of_connection, "root"
    expect.append(("B33", con))
    expect.append(("B43", script_name.split(".")[0]))
    return f"""[gd_scene load_steps=2 format=3]

[ext_resource type="Script" path="res://{script_name}" id="1"]

[node name="Root_{sc}" type="Node"]
script = ExtResource("1")

[node name="Child_{con}" type="Button" parent="."]

[connection signal="pressed" from="Child_{con}" to="." method="{con}_pressed"]
"""
