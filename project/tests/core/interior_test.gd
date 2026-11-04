## Core test of the Interior layer through the store: a building opens at a floor no higher than its own, a building that is not
## there leaves the layer off, an opened building has lamps on its floor and a cut inside of the same size every time, the record's
## floors are the body's, and closing leaves the layer off. It prints the cost
## of the inside of each kind of building (triangles, instanced boxes, bytes) for the report.
## Run: godot --headless --path project res://tests/core/interior_test.tscn -- --no-flow
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const HOUSE_FLOORS := 2
const TOWER_FLOORS := 8
const FAR_FLOOR := 99
const NOT_A_BUILDING := 99999999
const STEPS := 8

var failures := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	_check(CityStore.boot(world.terrain, world.materials()).is_empty(), "boot")
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	_check(not CityStore.query(&"interior").on, "the layer starts off")
	var house := _find(1, HOUSE_FLOORS)
	var tower := _find(TOWER_FLOORS, 1000)
	_check(house >= 0 and tower >= 0, "the map has a house and a tower")
	_opens(house, "house")
	_opens(tower, "tower")
	_refuses()
	_costs()
	_bye()


func _find(least: int, most: int) -> int:
	for id in CityStore.query(&"list_buildings"):
		var info: Dictionary = CityStore.query(&"probe_building", [id])
		if info.floors >= least and info.floors <= most:
			return id
	return -1


func _opens(id: int, what: String) -> void:
	var r: Dictionary = CityStore.dispatch(&"set_interior", [true, id, FAR_FLOOR, 0])
	var s: Dictionary = CityStore.query(&"interior")
	_check(r.get("ok", false) and s.on and s.target == id, "%s opens" % what)
	_check(s.floors >= 1 and s.level == s.floors - 1, "%s: the floor asked for is limited to its top (%d of %d)" % [what, s.level, s.floors])
	var revision: int = CityStore.query(&"interior_revision")
	CityStore.dispatch(&"set_interior", [true, id, 0, 3])
	s = CityStore.query(&"interior")
	_check(s.level == 0 and s.dir == 3, "%s: floor and direction are kept" % what)
	_check(int(CityStore.query(&"interior_revision")) != revision and s.revision == CityStore.query(&"interior_revision"), "%s: the cheap revision follows every command" % what)
	var info: Dictionary = CityStore.query(&"probe_building", [id])
	var off_lot := Vector2(info.focus.x - info.position.x, info.focus.z - info.position.z).length()
	_check(info.extent > 0.0 and off_lot <= maxf(info.w, info.d), "%s: the body's focus (%.1f m from the lot's middle) and extent (%.1f m) are inside its lot" % [what, off_lot, info.extent])
	_check(info.floors == s.floors, "%s: the record's floors (%d) are the body's (%d): the body is the one owner of the height" % [what, info.floors, s.floors])
	print("INTERIOR %s id=%d floors=%d lamps=%d" % [what, id, s.floors, s.lamps.size()])


func _refuses() -> void:
	var r: Dictionary = CityStore.dispatch(&"set_interior", [true, NOT_A_BUILDING, 0, 0])
	_check(not r.get("ok", true) and r.error == "game.toast.insideNone", "a building that is not there is refused")
	_check(not CityStore.query(&"interior").on, "and leaves the layer off")


## The cost of the inside of the first buildings of each count of floors, and the same twice (a cut is a pure function of
## the building, its floor and its direction).
func _costs() -> void:
	var seen := {}
	var n := 0
	for id in CityStore.query(&"list_buildings"):
		var info: Dictionary = CityStore.query(&"probe_building", [id])
		var key := mini(info.floors, TOWER_FLOORS)
		if seen.has(key) or info.floors == 0:
			continue
		seen[key] = true
		var a: Dictionary = CityStore.query(&"interior_cost", [id, 0, 0])
		var b: Dictionary = CityStore.query(&"interior_cost", [id, 0, 0])
		_check(a == b, "the inside of building %d is the same every time" % id)
		_check(a.inside_triangles > 0, "building %d has a floor to see" % id)
		var top: Dictionary = CityStore.query(&"interior_cost", [id, FAR_FLOOR, 0])
		var last: Dictionary = CityStore.query(&"interior_cost", [id, info.floors - 1, 0])
		_check(top.inside_triangles > 0 and last.shell_triangles > 0, "the top floor is limited to the building's own")
		print("COST floors=%d id=%d shell_tris=%d shell_boxes=%d inside_tris=%d inside_boxes=%d inside_bytes=%d" % [
			info.floors, id, a.shell_triangles, a.shell_boxes, a.inside_triangles, a.inside_boxes, a.inside_bytes])
		n += 1
	_check(n >= 2, "buildings of several heights were costed")


func _bye() -> void:
	CityStore.dispatch(&"set_interior", [false, -1, 0, 0])
	_check(not CityStore.query(&"interior").on, "closing leaves the layer off")
	print("interior_test: ", "OK" if failures == 0 else "%d FALHAS" % failures)
	get_tree().quit(failures)


func _check(ok: bool, what: String) -> void:
	if not ok:
		failures += 1
		print("FAIL: ", what)
