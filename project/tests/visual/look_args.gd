## A look plan's arguments for a command (look.gd's "dispatch" and "until", and its pointer's points): {"at": [x, z]} the point of the
## ground there (what the player's pointer would hit), {"pair": [x, z], "off": [dx, dz], "reach": m} the ground between the two walkers on
## their feet that walk nearest one another within `reach` metres of there (LookAim.nearest_pair; PAIR_REACH_M when left out), moved by
## `off` (metres; none when left out): a blow to one side of them throws both the same way, together, {"crowd": m, "off": [dx, dz]} the
## ground at the walker with the most others within m metres (LookAim.busiest), moved by `off`, {"profile": id} the index of that road
## class, {"v3": [x, y, z]} a vector, {"path": [[x, z], ...]} the ground's points, the rest as the JSON gives it. What cannot be found
## is said and counted in `failures`.
class_name LookArgs
extends RefCounted

const HEAD_M := 1.7 # a walker's head over its feet: where a round that kills is aimed
const ROUND_FROM := Vector3(0.0, 6.0, 12.0) # where the round comes from, off the head (shoot_test.gd STANDOFF)

var ui: UiDriver
var failures := 0


func _init(driver: UiDriver) -> void:
	ui = driver


func decode(raw: Array) -> Array:
	var out := []
	for a: Variant in raw:
		if a is Dictionary and a.has("at"):
			out.push_back(point(a.at))
		elif a is Dictionary and a.has("pair"):
			out.push_back(_pair(a.pair, a.get("off", [0.0, 0.0]), float(a.get("reach", LookAim.PAIR_REACH_M))))
		elif a is Dictionary and a.has("crowd"):
			out.push_back(_crowd(a.get("off", [0.0, 0.0]), float(a.crowd)))
		elif a is Dictionary and a.has("roundAt"):
			out.append_array(_round_at(a.roundAt))
		elif a is Dictionary and a.has("profile"):
			out.push_back(_profile(a.profile))
		elif a is Dictionary and a.has("v3"):
			out.push_back(Vector3(a.v3[0], a.v3[1], a.v3[2]))
		elif a is Dictionary and a.has("path"):
			var path := PackedVector3Array()
			for p: Array in a.path:
				path.push_back(point(p))
			out.push_back(path)
		else:
			out.push_back(a)
	return out


func point(xz: Array) -> Vector3:
	return ui.ground(xz[0], xz[1])


## The walkers on their feet, where each stands.
func _standing() -> Array:
	var walkers: Array = []
	for w: Dictionary in CityStore.query(&"shot_targets", [LookAim.WALKERS_READ]).walkers:
		if not w.get("fallen", false):
			walkers.push_back(w.position)
	return walkers


func _crowd(off: Array, crowd: float) -> Vector3:
	var heart := LookAim.busiest(_standing(), crowd)
	if heart == Vector3.INF:
		failures += 1
		print("FAIL no walkers for a crowd")
		return Vector3.ZERO
	print("crowd's heart at ", heart)
	return point([heart.x + float(off[0]), heart.z + float(off[1])])


## A round at the head of the walker on their feet nearest `xz`, from above and behind it (shoot_test's): the shoot command's origin
## and direction, two arguments. None found: said, counted, a round at the sky from the point.
func _round_at(xz: Array) -> Array:
	var walkers := _standing()
	var target := LookAim.nearest_of(walkers, Vector2(xz[0], xz[1]))
	if target == Vector3.INF:
		failures += 1
		print("FAIL no walker near ", xz)
		return [point(xz), Vector3.UP]
	var head := target + Vector3(0, HEAD_M, 0)
	var origin := head + ROUND_FROM
	print("round at the walker at ", target)
	return [origin, (head - origin).normalized()]


func _pair(xz: Array, off: Array, reach: float) -> Vector3:
	var middle := LookAim.nearest_pair(_standing(), Vector2(xz[0], xz[1]), reach)
	if middle.is_empty():
		failures += 1
		print("FAIL no two walkers near ", xz)
		return point(xz)
	print("walkers' pair at ", middle[0])
	return point([middle[0].x + float(off[0]), middle[0].z + float(off[1])])


func _profile(id: String) -> int:
	for p: Dictionary in CityStore.query(&"road_profiles"):
		if p.id == id:
			return p.index
	failures += 1
	print("FAIL unknown road class: ", id)
	return -1
