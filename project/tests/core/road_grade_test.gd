## The road heights solver against the Roadcraft 4.0's tests/world/elevation.spec.ts, its scenes and its bounds (lengths
## in the 4.0's 0.4 m units, U, turned to metres): a flat crossroads, a crossroads over a hill, elevated spans and
## ramps, a bridge over a valley, tunnels through a hill, and every junction plate flat at its node's height.
extends TestCase

const U := 0.4
var clearance: float = DataTables.tuning("grade_clearance_m") # a road at grade over its ground
var elevated: float = DataTables.tuning("grade_clear_elevated_m")
## Where the ground closes over a bore: headroom, arch and what the ground keeps over them (tuning structure.bore_over_m).
var bore: float = DataTables.tuning("grade_tunnel_headroom_m") + DataTables.tuning("grade_tunnel_arch_m") + DataTables.tuning("structure_bore_over_m")
const SAG := 12 # the first units of a ramp, its sag curve, are not judged for grade (the 4.0's anatomy)


func _ready() -> void:
	if not boot():
		finish()
		return
	_flat_crossroads()
	_hill_crossroads()
	_elevated_chains()
	_elevated_alone()
	_bridge_over_valley()
	RoadGradeSeamCases.new(self).run()
	finish()


## One of the solver's cases: its name, whether it held and what was measured, as one check.
func judge(name: String, ok: bool, detail: Variant) -> void:
	check(ok, "%s: %s" % [name, detail])


func solve(ground: Dictionary, nodes: Array, segs: Array) -> Dictionary:
	return CityStore.query(&"grade_probe", [{"ground": ground, "nodes": nodes, "segs": segs}])


func steepest(seg: Dictionary, from := 0, to := -1) -> float:
	var h: PackedFloat32Array = seg.h
	var worst := 0.0
	for i in range(from, (h.size() if to < 0 else to) - 1):
		worst = maxf(worst, absf(h[i + 1] - h[i]) / seg.step)
	return worst


## Four arms of `arm` metres from a node at the origin: north-south boulevard, east-west avenue.
func _cross(arm: float) -> Array:
	return [[[0, 0], [0, -arm], [0, arm], [-arm, 0], [arm, 0]],
		[[1, 0, "boulevard", 0], [0, 2, "boulevard", 0], [3, 0, "avenue", 0], [0, 4, "avenue", 0]]]


func _flat_crossroads() -> void:
	var c := _cross(300 * U)
	var r := solve({"kind": "flat"}, c[0], c[1])
	var worst := 0.0
	for seg in r.segs:
		for v in seg.h:
			worst = maxf(worst, absf(v - clearance))
	judge("flat: a span of clearance over the ground, and no step", worst < 1e-4, worst)


func _hill_crossroads() -> void:
	var c := _cross(300 * U)
	var r := solve({"kind": "hill", "radius": 160 * U, "height": 18 * U}, c[0], c[1])
	var steep := 0.0
	var cut := 0.0
	var fill := 0.0
	var far := 0.0
	for seg in r.segs:
		steep = maxf(steep, steepest(seg))
		var h: PackedFloat32Array = seg.h
		var g: PackedFloat32Array = seg.ground
		for i in h.size():
			cut = minf(cut, h[i] - g[i])
			fill = maxf(fill, h[i] - g[i])
			far = maxf(far, absf(h[i] - g[i]))
	# the 4.0's measure: the second difference every 4 U along the x road, through the junction, against the ground's
	var road_curve := 0.0
	var ground_curve := 0.0
	var d := 4 * U
	for k in range(-290, 291, 4):
		var x := k * U
		road_curve += absf(_x_road(r, x - d) - 2.0 * _x_road(r, x) + _x_road(r, x + d))
		ground_curve += absf(_hill(x - d) - 2.0 * _hill(x) + _hill(x + d))
	judge("hill: cuts what rises and fills what falls", cut < -1 * U and fill > 0.2 * U, [cut, fill])
	judge("hill: the earthwork stays bounded", far < 10 * U, far)
	judge("hill: the road is much smoother than the ground", road_curve < ground_curve * 0.6, [road_curve, ground_curve])
	judge("hill: follows the hill rather than going over it", r.nodes[0] > 9 * U and r.nodes[4] < 3 * U, [r.nodes[0], r.nodes[4]])
	judge("hill: keeps to the ground ramp (12%, the plate and the curve add a thread)", steep < 0.14, steep)


## The x road of the crossroads (west 3 -> centre, centre -> east 4: segments 2 and 3) at x, between its stations.
func _x_road(r: Dictionary, x: float) -> float:
	var seg: Dictionary = r.segs[3] if x >= 0 else r.segs[2]
	var h: PackedFloat32Array = seg.h
	var f := clampf((x if x >= 0 else x + 300 * U) / seg.step, 0, h.size() - 1)
	var lo := int(floor(f))
	return lerpf(h[lo], h[mini(h.size() - 1, lo + 1)], f - lo)


## The hill of the crossroads scene (the 4.0's hill(0, 0, 160 U, 18 U)) on the x axis.
func _hill(x: float) -> float:
	var u := 1.0 - absf(x) / (160 * U)
	return 0.0 if u <= 0 else 18 * U * u * u * (3 - 2 * u)


## A chain a - b - c - d along x: ground, `structure`, ground; c's span `span` either side.
func _chain(structure: int, span: float, ground := {"kind": "flat"}) -> Dictionary:
	var nodes := [[-span, 0], [-span / 3.0, 0], [span / 3.0, 0], [span, 0]]
	var segs := [[0, 1, "avenue", 0], [1, 2, "avenue", structure], [2, 3, "avenue", 0]]
	return solve(ground, nodes, segs)


func _elevated_chains() -> void:
	var r := _chain(1, 900 * U)
	judge("elevated: lands exactly on the ground road it joins", absf(r.nodes[1] - clearance) < 1e-4 and absf(r.nodes[2] - clearance) < 1e-4, [r.nodes[1], r.nodes[2]])
	var long := _chain(1, 1400 * U)
	var h: PackedFloat32Array = long.segs[1].h
	judge("elevated: reaches the design clearance on a long span", h[h.size() / 2] > elevated * 0.9, h[h.size() / 2])
	var short := _chain(1, 220 * U)
	judge("elevated: lowers the deck rather than steepen the ramp on a short span", steepest(short.segs[1]) < 0.165, steepest(short.segs[1]))


## The heights every U along a segment (the 4.0 test reads onSegment at i * U; the field is linear between stations).
func _every_u(seg: Dictionary) -> PackedFloat32Array:
	var h: PackedFloat32Array = seg.h
	var out := PackedFloat32Array()
	for i in int(floor(seg.len / U + 1e-6)) + 1:
		var f := minf(h.size() - 1.0, i * U / seg.step)
		var lo := int(floor(f))
		out.append(lerpf(h[lo], h[mini(h.size() - 1, lo + 1)], f - lo))
	return out


## The two ramps and the deck between them, as the 4.0's anatomy(): ramps in samples of U (a ramp's grade measured over
## 4 U, its first SAG units not judged), the deck's share of the span, the steepest grade, the top over the foot.
func _anatomy(seg: Dictionary) -> Dictionary:
	var h := _every_u(seg)
	var grade := func(i: int) -> float: return absf(h[i + 4] - h[i]) / (4 * U)
	var foot := h[0]
	var up_start := 0
	while up_start < h.size() - 1 and h[up_start] <= foot + 0.02 * U:
		up_start += 1
	var up_end := up_start
	while up_end < h.size() - 5 and (grade.call(up_end) > 0.052 or up_end - up_start < SAG):
		up_end += 1
	var end := h.size() - 1
	var down_start := end
	while down_start > 0 and h[down_start] <= h[end] + 0.02 * U:
		down_start -= 1
	var down_end := down_start
	while down_end > 4 and (grade.call(down_end - 4) > 0.052 or down_start - down_end < SAG):
		down_end -= 1
	var steepest := 0.0
	var top := foot
	for i in h.size():
		top = maxf(top, h[i])
		if i + 4 < h.size():
			steepest = maxf(steepest, grade.call(i))
	return {"ramp_a": up_end - up_start, "ramp_b": down_start - down_end, "deck": float(down_end - up_end) / h.size(),
		"from": up_end, "to": down_end, "steepest": steepest, "top": top - foot, "h": h}


## Level at full height: the deck within the crest's rounding of its height, and most of the span near the top.
func _level(a: Dictionary) -> bool:
	var h: PackedFloat32Array = a.h
	var low := 1e9
	var high := -1e9
	var near := 0
	var top := -1e9
	for v in h:
		top = maxf(top, v)
	for i in range(a.from, a.to):
		low = minf(low, h[i])
		high = maxf(high, h[i])
	for v in h:
		near += 1 if v > top - 0.25 * U else 0
	return high - low < 0.25 * U and float(near) / h.size() >= 0.6


func alone(span: float, ground: Dictionary, structure := 1) -> Dictionary:
	return solve(ground, [[-span / 2.0, 0], [span / 2.0, 0]], [[0, 1, "avenue", structure]])


## One elevated segment of `span`: alone (both ends on the ground) or between two roads at grade 300 U long (the 4.0's
## elevated()); the span is the segment handed back.
func _elevated(span: float, ground: Dictionary, standalone: bool) -> Dictionary:
	if standalone:
		var r := alone(span, ground)
		return {"seg": r.segs[0], "nodes": r.nodes}
	var nodes := [[-span / 2.0 - 300 * U, 0], [-span / 2.0, 0], [span / 2.0, 0], [span / 2.0 + 300 * U, 0]]
	var r := solve(ground, nodes, [[0, 1, "avenue", 0], [1, 2, "avenue", 1], [2, 3, "avenue", 0]])
	return {"seg": r.segs[1], "nodes": [r.nodes[1], r.nodes[2]]}


func _elevated_alone() -> void:
	for standalone in [true, false]:
		var where := "alone" if standalone else "between ground roads"
		var r := _elevated(600 * U, {"kind": "flat"}, standalone)
		var a := _anatomy(r.seg)
		judge("ramp (%s): reaches the full height within 100 U and stays level between the ramps" % where, a.top > elevated - 0.1 * U and a.ramp_a <= 100 and a.ramp_b <= 100 and a.deck >= 0.6 and a.steepest < 0.162 and _level(a), [a.ramp_a, a.ramp_b, a.deck, a.steepest, a.top])
		judge("ramp (%s): comes down to the ground at both ends" % where, absf(r.nodes[0] - clearance) < 1e-3 and absf(r.nodes[1] - clearance) < 1e-3, r.nodes)
		var b := _anatomy(_elevated(600 * U, {"kind": "rolling"}, standalone).seg)
		var h: PackedFloat32Array = b.h
		var straight := 0.0
		var clear := 1e9
		for i in range(b.from, b.to + 1, 10): # a straight line from end to end: it does not follow the land
			var chord: float = h[b.from] + (h[b.to] - h[b.from]) * float(i - b.from) / maxf(1.0, b.to - b.from)
			straight = maxf(straight, absf(h[i] - chord))
		var rx: float = -300 * U
		for i in range(b.from, b.to + 1, 5): # and it still stands clear of the ground under it
			var x: float = rx + i * U
			clear = minf(clear, h[i] - (1.2 * sin(x / 36.0) + 0.8 * sin(x / 14.8)))
		judge("rolling ground (%s): short ramps and a straight deck, clear of the ground" % where, b.ramp_a <= 100 and b.ramp_b <= 100 and b.deck >= 0.6 and straight < 0.3 * U and clear > 8 * U, [b.ramp_a, b.ramp_b, b.deck, straight, clear])
	for depth in [0, 4, 8]:
		var sunk := alone(600 * U, {"kind": "sunk", "cx": 300 * U, "radius": 150 * U, "height": depth * U})
		var s := _anatomy(sunk.segs[0])
		judge("a landing sunk %d U: a clearance above it, a ramp up to 100 U" % depth, s.steepest < 0.162 and s.ramp_a <= 100, [s.steepest, s.ramp_a])
	var short := _anatomy(alone(120 * U, {"kind": "flat"}).segs[0])
	judge("a span too short for two ramps: lowers, never steepens", short.steepest < 0.162 and short.top < elevated, [short.steepest, short.top])


func _bridge_over_valley() -> void:
	var nodes := [[-650 * U, 0], [-350 * U, 0], [350 * U, 0], [650 * U, 0]]
	var segs := [[0, 1, "avenue", 0], [1, 2, "avenue", 2], [2, 3, "avenue", 0]]
	var r := solve({"kind": "valley", "radius": 180 * U, "height": 20 * U}, nodes, segs)
	var seg: Dictionary = r.segs[1]
	var at := func(x: float) -> float: return seg.h[int(round((x + 350 * U) / seg.step))]
	judge("bridge: crosses the valley level, edge to edge", at.call(0.0) > at.call(-170 * U) - 0.1 * U, [at.call(0.0), at.call(-170 * U)])
