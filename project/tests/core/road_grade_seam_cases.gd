## The tunnels and the junction plates of road_grade_test, against the 4.0's tests/world/elevation.spec.ts.
class_name RoadGradeSeamCases
extends RefCounted

var t: Node


func _init(test: Node) -> void:
	t = test


func run() -> void:
	_tunnels()
	_plates()


func _through(structure: int, length: float) -> Dictionary:
	return t._alone(length, {"kind": "hill", "radius": 320 * t.U, "height": 40 * t.U}, structure)


func _cover(r: Dictionary) -> float:
	var seg: Dictionary = r.segs[0]
	return seg.ground[seg.h.size() / 2] - seg.h[seg.h.size() / 2]


func _tunnels() -> void:
	var bored = _through(3, 900 * t.U)
	var surface = _through(0, 900 * t.U)
	t._check("tunnel: dives under the hill it crosses", _cover(bored) > t.BORE, _cover(bored))
	var ends: PackedFloat32Array = bored.segs[0].ground
	t._check("tunnel: meets the ground at both portals", absf(bored.nodes[0] - (ends[0] + t.CLEARANCE)) < 1.5 * t.U and absf(bored.nodes[1] - (ends[-1] + t.CLEARANCE)) < 1.5 * t.U, [bored.nodes, ends[0], ends[-1]])
	var mid: int = bored.segs[0].h.size() / 2
	t._check("tunnel: solved quite unlike the same road on the ground", surface.segs[0].h[mid] > ends[mid] - 8 * t.U and bored.segs[0].h[mid] < surface.segs[0].h[mid] - 20 * t.U, [surface.segs[0].h[mid], bored.segs[0].h[mid]])
	t._check("tunnel: continuous all along", t._steepest(bored.segs[0]) < 0.13 * 2.5, t._steepest(bored.segs[0]))
	var short = _cover(_through(3, 180 * t.U))
	var shorter = _cover(_through(3, 90 * t.U))
	t._check("tunnel: shallower the shorter it is", _cover(bored) > short and short > shorter and shorter < t.BORE, [_cover(bored), short, shorter])


## Every leg of a crossroads of chains, on broken ground, flat over its plate at its node's height (worstSeam).
func _plates() -> void:
	# the 4.0's matrix: every layout, on flat and broken ground, two boulevards or a boulevard and an avenue, two chains
	for ground in ["flat", "hilly"]:
		for east_west in ["boulevard", "avenue"]:
			for layout in ["chão", "toda elevada", "elevada sobre chão", "elevada pousando nas ruas"]:
				var worst = 0.0
				for chain in [[3, 150 * t.U], [2, 200 * t.U]]:
					worst = maxf(worst, _plate_seam(ground, east_west, layout, chain[0], chain[1]))
				t._check("plates (%s, %s, %s): every flat leg at its node's height" % [layout, east_west, ground], worst < 1e-3 * t.U, worst)


## The worst a leg's plate stands off its node's height: a crossroads of chains of `count` segments `length` long.
func _plate_seam(ground: String, east_west: String, layout: String, count: int, length: float) -> float:
	var nodes = [[0, 0]]
	var segs = []
	var arms = [[0, -1], [1, 0], [0, 1], [-1, 0]]
	for arm in arms.size():
		var prev = 0
		for k in range(1, count + 1):
			nodes.append([arms[arm][0] * length * k, arms[arm][1] * length * k])
			var raised: bool = layout == "toda elevada" or (layout == "elevada sobre chão" and arm % 2 == 0) 					or (layout == "elevada pousando nas ruas" and k < count)
			segs.append([prev, nodes.size() - 1, "boulevard" if arm % 2 == 0 else east_west, 1 if raised else 0])
			prev = nodes.size() - 1
	var r = t._solve({"kind": ground}, nodes, segs)
	var worst = 0.0
	for k in segs.size():
		var seg: Dictionary = r.segs[k]
		for i in seg.h.size():
			var s: float = i * seg.step
			if s <= seg.plate_a:
				worst = maxf(worst, absf(seg.h[i] - r.nodes[segs[k][0]]))
			elif s >= seg.len - seg.plate_b:
				worst = maxf(worst, absf(seg.h[i] - r.nodes[segs[k][1]]))
	return worst
