## What the profile editor does to a road's elements (Roadcraft ui/roads/profileEditor.ts): pure, on the core's dictionaries, the left edge to
## the right. New elements go where the road can carry them, a lane's width goes to every lane.
class_name RoadSpecOps
extends RefCounted


## The road with its lanes going `dir`: 1 towards its end, -1 towards its start, 0 half each way, as a road of two ways has them.
static func directed(spec: Dictionary, dir: int) -> Dictionary:
	var out := spec.duplicate(true)
	var lanes: Array = out.elements.filter(func(e: Dictionary) -> bool: return e.kind == "lane")
	for i in lanes.size():
		lanes[i].dir = dir if dir != 0 else (-1 if i < lanes.size() / 2 else 1)
	return out


static func new_element(kind: String, spec: Dictionary, defaults: Dictionary, band_m: float) -> Dictionary:
	match kind:
		"laneForward", "laneBackward":
			var width: float = defaults.lane
			for e: Dictionary in spec.elements:
				if e.kind == "lane":
					width = e.width
			return {"kind": "lane", "width": width, "dir": 1 if kind == "laneForward" else -1}
		"parking", "cycle":
			return {"kind": kind, "width": band_m}
	return {"kind": kind, "width": defaults[kind]}


static func _last(e: Array, kind: String, dir: int) -> int:
	for i in range(e.size() - 1, -1, -1):
		if e[i].kind == kind and (dir == 0 or e[i].get("dir", 0) == dir):
			return i
	return -1


static func _band(e: Array, i: int) -> bool:
	return i >= 0 and i < e.size() and (e[i].kind == "parking" or e[i].kind == "cycle")


## Where the palette puts a new element.
static func insertion_index(spec: Dictionary, kind: String, selected: int) -> int:
	var e: Array = spec.elements
	var n := e.size()
	var has_left: bool = n > 0 and e[0].kind == "footway"
	var has_right: bool = n > 1 and e[n - 1].kind == "footway"
	var lo := 1 if has_left else 0
	var hi := n - 1 if has_right else n
	var last_back := _last(e, "lane", -1)
	var last_fwd := _last(e, "lane", 1)
	var median := _last(e, "median", 0)
	match kind:
		"footway":
			if not has_left:
				return 0
			return n if not has_right else clampi(selected + 1, lo, hi)
		"laneBackward":
			if last_back >= 0:
				return last_back + 1
			for i in n:
				if e[i].kind == "median" or (e[i].kind == "lane" and e[i].dir > 0):
					return i
			return clampi(lo + 1 if _band(e, lo) else lo, lo, hi)
		"laneForward":
			if last_fwd >= 0:
				return last_fwd + 1
			if median >= 0:
				return median + 1
			if last_back >= 0:
				return last_back + 1
			return clampi(hi - 1 if _band(e, hi - 1) else hi, lo, hi)
		"median":
			return last_back + 1 if last_back >= 0 else clampi(selected + 1, lo, hi)
	# By the kerb nearest the picked element; the other kerb when that one has its band already.
	var left_free := not _band(e, lo)
	var right_free := not _band(e, hi - 1)
	if selected >= 0 and selected < n / 2.0:
		return lo if left_free or not right_free else hi
	return hi if right_free or not left_free else lo


## A lane's width goes to every lane of the road.
static func with_width(spec: Dictionary, index: int, metres: float) -> bool:
	var target: Dictionary = spec.elements[index]
	var changed := false
	for e: Dictionary in spec.elements:
		if (e == target or (target.kind == "lane" and e.kind == "lane")) and e.width != metres:
			e.width = metres
			changed = true
	return changed


static func moved(spec: Dictionary, from: int, to: int) -> void:
	var e: Array = spec.elements
	var x: Dictionary = e[from]
	e.remove_at(from)
	e.insert(to, x)


static func total(spec: Dictionary) -> float:
	var sum := 0.0
	for e: Dictionary in spec.elements:
		sum += e.width
	return sum


## The strips the catalogue's picture draws, top to bottom as the catalogue writes them.
static func strips(spec: Dictionary) -> Array:
	var out := []
	for i in range(spec.elements.size() - 1, -1, -1):
		var e: Dictionary = spec.elements[i]
		var kind: String = {"footway": "sidewalk", "cycle": "bike"}.get(e.kind, e.kind)
		out.push_back({"kind": kind, "width": e.width, "dir": e.get("dir", 0), "bus": e.get("bus", false)})
	return out
