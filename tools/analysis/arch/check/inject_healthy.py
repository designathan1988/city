"""Healthy code that looks like a defect, one piece for each detector: the negative control. Any finding here means a detector answers to the look of a thing
and not to the thing. Words are fresh like those of the cases, so the small repository cannot be told from code that is simply fine."""
from .inject_script import engine_call


def files(w, expect, api, places, join, stem):
    def calm(*kinds):
        word = w()
        for kind in kinds:
            expect.append(("not:" + kind, word))
        return word

    a, b, c, d, f, g = calm("grows_with_state"), calm("nested_loops"), calm("allocation"), calm("lock_contention", "lock_held_work"), calm("copy"), calm("linear_search")
    h, i, j, k, m, n = calm("redone_work"), calm("memory_growth"), calm("streaming"), calm("loading"), calm("long_save"), calm("thread_churn")
    o, p, q, r, s = calm("false_sharing"), calm("costly_option"), calm("boundary_conversion"), calm("test_code_shipped"), calm("serial_decode")
    bl = calm("boundary_in_loop")
    fine = w()
    cpp = f"""// Healthy code that looks like trouble.
#include <algorithm>
#include <atomic>
#include <cmath>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

void {r}_shared() {{}}
void {r}_bind(const char *name, void (*fn)()) {{ (void)name; (void)fn; }}
void {r}_register() {{ {r}_bind("{r}_shared", &{r}_shared); }}

struct Quiet_{fine} {{
	virtual ~Quiet_{fine}() {{}}
	virtual void _process(double delta) {{ (void)delta; }}
	virtual void _ready() {{}}
}};

struct Record_{i} {{
	std::vector<int> taken;
	void take(int v) {{ taken.push_back(v); }}
}};

struct Calm_{fine} : Quiet_{fine} {{
	std::vector<int> items, {h}_rows, {h}_table, {i}_bag;
	std::mutex {d}_mutex, {m}_mutex;
	std::atomic<int> {o}_a{{ 0 }}, {o}_b{{ 0 }};
	int {d}_value = 0, {j}_rev = 0, {j}_seen = 0, {p}_mode = 0, {p}_n = 0, {m}_flag = 0;
	std::vector<std::function<void()>> pending;

	int {a}_first() {{ for (int v : items) {{ if (v) return v; }} return 0; }}
	int {b}_once() {{ int t = 0; for (int x : items) for (int y : items) t += x * y; return t; }}
	int {b}_fixed() {{ int t = 0; for (int x : items) for (int k = 0; k < 4; k++) t += x * k; return t; }}
	void {a}_trim() {{ while (items.size() > 9) items.pop_back(); }}
	double {bl}_math() {{ double t = 0; for (int v : items) t += std::fabs(double(v)); return t; }}
	int {c}_outside() {{ std::vector<int> scratch; for (int i = 0; i < 3; i++) scratch.push_back(i); return int(scratch.size()); }}
	void {d}_quick() {{ std::lock_guard<std::mutex> hold({d}_mutex); {d}_value = 1; }}
	int {f}_view() {{ const std::vector<int> &mine = items; return int(mine.size()); }}
	int {g}_once() {{ int t = 0; for (int i = 0; i < 3; i++) t += int(std::count(items.begin(), items.end(), i)); return t; }}
	int {h}_times(int by) {{ int t = 0; for (int v : {h}_table) t += v * by; return t; }}
	int {h}_walk() {{ {h}_rows.push_back(1); int t = 0; for (int v : {h}_rows) t += v; {h}_rows.clear(); return t; }}
	void {i}_keep() {{ {i}_bag.push_back(1); if ({i}_bag.size() > 9) {i}_bag.clear(); Record_{i} here; here.take(1); }}
	void submit(std::function<void()> job) {{ pending.push_back(job); }}
	void {j}_touch() {{ {j}_rev = 1; }}
	void {j}_sync() {{ if ({j}_rev != {j}_seen) {{ {j}_seen = 1; submit([this]() {{ {d}_value = 2; }}); }} }}
	int {k}_heavy() {{ int t = 0; for (int x : items) for (int y : items) t += x + y; return t; }}
	int {s}_read(const std::string &text) {{ return int(text.size()); }}
	void {m}_save() {{ std::lock_guard<std::mutex> hold({m}_mutex); {m}_flag = 1; }}
	void {n}_start() {{ std::thread t([]() {{}}); t.detach(); }}
	void {o}_write() {{ {o}_a = 1; {o}_b = 1; }}
	void {p}_step() {{ if ({p}_mode) {{ {p}_n = 1; }} }}
	void {q}_name() {{ std::string text = "x"; (void)text; }}
	void _ready() override {{
		{h}_table.push_back(1); {b}_once(); {g}_once(); {j}_touch(); {k}_heavy(); {s}_read("a"); {s}_read("b"); {m}_save(); {n}_start(); {r}_shared();
		{p}_mode = 1;
	}}
	void _process(double delta) override {{
		(void)delta;
		{a}_first(); {h}_times(int(delta)); {b}_fixed(); {a}_trim(); {bl}_math(); {c}_outside(); {d}_quick(); {f}_view(); {h}_walk(); {i}_keep(); {j}_sync(); {o}_write(); {p}_step(); {q}_name();
	}}
}};
"""
    t, u, v, x, y, z = calm("query_per_frame", "nested_loops"), calm("gpu_upload"), calm("shadow"), calm("material"), calm("tiny_triangles"), calm("wide_bounds")
    aa, ab, ac, ad, ae = calm("level_flip"), calm("pipeline_compile"), calm("measure_feedback"), calm("rebuilt_interface"), calm("engine_copies")
    af, ag = calm("scattered"), calm("repeated_rule")
    ah, role, klass = calm("repeated_decision"), calm("implicit_role"), calm("god")
    gd = f"""# Healthy script that looks like trouble.
class_name Plain_{klass}
extends Node

var {af}_one := 0
var {af}_two := 0
var {ah}_flag := false
var {a}_pool := []
const {a}_AXES := [1, 2, 3]

func _ready() -> void:
	{u}_send()
	{v}_cast()
	{x}_change()
	{y}_draw()
	{z}_box()
	{aa}_range()
	{ab}_make()
	{ae}_one()

func _process(delta: float) -> void:
	self.call(&"{t}_cheap")
	var now := Time.get_ticks_usec()
	if str(now) == "{t}_costly":
		pass
	{ac}_cheap()
	{bl}_values(get_meta("n"))
	{a}_fill()

func _input(event: InputEvent) -> void:
	{ad}_clear()

func {bl}_values(sizes) -> int:
	var total := 0
	for i in sizes.size():
		total += int(absf(i))
	for c in get_children():
		total += 1
	return total

func {a}_fill() -> int:
	while {a}_pool.size() < 3:
		{a}_pool.append(1)
	var total := 0
	for x in {a}_AXES:
		total += x
	return total

func {t}_cheap() -> int:
	return 1

func {t}_costly() -> int:
	var n := 0
	for a in get_children():
		for b in get_children():
			n += 1
	return n

func {ac}_cheap() -> int:
	return 2

func {ad}_clear() -> void:
	for c in get_children():
		c.queue_free()

func {u}_send() -> void:
	{engine_call(api, "multimesh", "set", "buffer")}

func {v}_cast() -> void:
	{engine_call(api, "cast", "shadows")}

func {x}_change() -> void:
	{engine_call(api, "material", "set", "param")}

func {y}_draw() -> void:
	{engine_call(api, "multimesh", "allocate")}

func {z}_box() -> void:
	var measured = get_meta("box")
	RenderingServer.instance_set_custom_aabb(RID(), measured)

func {aa}_range() -> void:
	RenderingServer.instance_geometry_set_visibility_range(RID(), 0.0, 9.0, 1.0, 1.0, 0)

func {ab}_make() -> void:
	{engine_call(api, "shader", "create")}

func {ae}_one() -> void:
	{engine_call(api, "instance", "create")}

func {af}_first() -> void:
	{af}_one = 1
	{af}_two = 2

func {af}_second() -> void:
	{af}_one = 3
	{af}_two = 4

func {ag}_a(x: int) -> int:
	var t := x * 2
	t += {ag}_b(x)
	return t + 1

func {ag}_b(x: int) -> int:
	var t := x * 2
	t += {ag}_a(x)
	return t + 1

func {ah}_left() -> void:
	if {ah}_flag:
		{ah}_right()

func {ah}_right() -> void:
	if {ah}_flag:
		{ah}_left()
"""
    base = f"# A contract of the project.\nclass_name Kin_{role}\nextends Node\n\nfunc {role}_open() -> void:\n	pass\n\nfunc {role}_shut() -> void:\n	pass\n\nfunc {role}_fill() -> void:\n	pass\n"
    kin = lambda name: f"# Answers the contract.\nclass_name {name}_{role}\nextends Kin_{role}\n\nfunc {role}_open() -> void:\n	print(1)\n\nfunc {role}_shut() -> void:\n	print(2)\n\nfunc {role}_fill() -> void:\n	print(3)\n"  # noqa: E731
    plain = w()
    script = f"# A plain script with functions to move around.\nextends Node\n\nvar {plain}_n := 0\n\nfunc {plain}_a() -> int:\n	return {plain}_free(2) + {plain}_n\n\nfunc {plain}_b() -> int:\n	return {plain}_free(3) + 1\n\nfunc {plain}_free(x: int) -> int:\n	var t := x * 2\n	return t + 1\n"
    home = places["gd"] or "project"
    out = {join(places["cpp"] or "native", f"{stem}_calm.cpp"): cpp, join(home, f"{stem}_calm.gd"): gd, join(home, f"{role}_base.gd"): base, join(home, f"{role}_one.gd"): kin("One"), join(home, f"{role}_two.gd"): kin("Two"), join(home, f"{plain}_plain.gd"): script}
    out[f"project/tests/{r}_also_test.gd"] = f'# Injected test.\nextends Node\n\nfunc _ready() -> void:\n	self.call(&"{r}_shared")\n'
    return out
