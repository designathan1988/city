## Core test of what names a building by id once the building goes (doc/specs/003-roadcraft-features/sheets/backbone-records-true.md
## items 6, 7 and 13), through the store: the Interior view closes when its building is demolished or undone and never opens on the
## next building that takes the id; a fire goes out with its building and never passes to the next one; a lot drawn or a building
## moved over rubble is refused (`refuse.rubble`) and changes nothing; a command refused after its ops leaves the city's
## revisions, its saved state, its history and the far streets' furniture as they were. Each case also asks the core's invariants of its feature.
## Run: godot --headless --path project res://tests/core/records_buildings_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
## Clear of the city: a building drawn there knocks nothing down, so it takes the id freed last (ids are reused last freed first).
const FIELD_OUT_M := 300.0
const LOT_M := 30.0
const LOT_GAP_M := 2.0 * LOT_M # each case draws on its own lot, so no drawing removes another's building
const DRAWN_FN := 0
const DRAWN_ZONE := 1
const PLAN_RECTANGLE := 0
const ROAD_OUT_M := 200.0 # the refusal's road, away from the drawn lots
const ROAD_M := 200.0
const CROSS_M := 60.0 # the refused road crosses the first at its middle, this far to each side
const NO_MONEY := 0.0
const SCAN_STEP_M := 40.0 # the step of the scan for a street of the town
const PROFILE := 0
const PX := 0.25
const SKY_M := 900.0
const FORCE_MOST := 12
const SLOT := "records_buildings_test"

var field := Vector3.ZERO


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	var bounds: Dictionary = CityStore.query(&"city_bounds")
	field = Vector3(bounds.max.x + FIELD_OUT_M, 0, (bounds.min.z + bounds.max.z) * 0.5)
	_interior_after_demolish()
	_fire_after_demolish()
	_place_on_rubble()
	await _refusal_inert()
	finish()


## Item 6: the view's building demolished closes the view; the building drawn next takes the id and is not drawn cut open; a drawn
## building opened and then undone closes the view too (the history's own removal).
func _interior_after_demolish() -> void:
	var x := _standing(1)
	if not check(x >= 0, "interior: the town has a building with floors"):
		return
	var opened: Dictionary = CityStore.dispatch(&"set_interior", [true, x, 0, 0])
	check(opened.get("ok", false) and CityStore.query(&"interior").on and CityStore.query(&"interior").target == x, "interior: building %d opens" % x)
	var info: Dictionary = CityStore.query(&"probe_building", [x])
	var gone: Dictionary = CityStore.dispatch(&"demolish", [info.position])
	check(gone.get("ok", false) and gone.get("buildings_removed", 0) == 1 and not _alive(x), "interior: building %d demolished (%s)" % [x, gone])
	check(not CityStore.query(&"interior").on, "interior: demolishing its building closes the view")
	var drawn := _draw(0)
	check(drawn.get("ok", false) and drawn.get("placed", -1) == x, "interior: the building drawn next takes the freed id %d (%s)" % [x, drawn])
	check(not CityStore.query(&"interior").on, "interior: the building that took the id is not drawn cut open")
	_none_broken("interior")
	CityStore.dispatch(&"set_interior", [true, x, 0, 0])
	check(CityStore.query(&"interior").on, "interior: the drawn building %d opens" % x)
	check(CityStore.dispatch(&"undo"), "interior: the drawing undone")
	check(not _alive(x) and not CityStore.query(&"interior").on, "interior: undoing its building closes the view")
	_none_broken("interior")


## Item 7: a framed block struck on its roof at the least force that sets it alight burns and stands; demolished, its fire goes out,
## and the building drawn next on its freed id does not burn.
func _fire_after_demolish() -> void:
	var tall := _standing(int(DataTables.tuning("strike_frame_floors")))
	if not check(tall >= 0, "fire: the town has a framed block"):
		return
	var info: Dictionary = CityStore.query(&"probe_building", [tall])
	var spot: Vector3 = info.position
	var hit: Dictionary = CityStore.query(&"body_pick", [Vector3(spot.x, SKY_M, spot.z), Vector3.DOWN])
	var roof: Vector3 = hit.point if not hit.is_empty() else spot
	var force := int(DataTables.catalog_table("blast").strength_min)
	while int(CityStore.query(&"damage_probe").fires) == 0 and force <= FORCE_MOST and _alive(tall):
		CityStore.dispatch(&"strike", [roof, force])
		force += 1
	var burning: int = CityStore.query(&"damage_probe").fires
	if not check(burning > 0 and _alive(tall), "fire: block %d burns and stands (%d fires, force %d)" % [tall, burning, force - 1]):
		return
	var gone: Dictionary = CityStore.dispatch(&"demolish", [spot])
	check(gone.get("ok", false) and not _alive(tall), "fire: the burning block demolished (%s)" % [gone])
	var left: int = CityStore.query(&"damage_probe").fires
	check(left < burning, "fire: its fire went out with it (%d -> %d)" % [burning, left])
	var drawn := _draw(1)
	check(drawn.get("ok", false) and drawn.get("placed", -1) == tall, "fire: the building drawn next takes the freed id %d (%s)" % [tall, drawn])
	check(int(CityStore.query(&"damage_probe").fires) == left, "fire: the building that took the id does not burn")
	_none_broken("damage")


## A ruin keeps its lot until the player clears it: a lot drawn over rubble and a building moved onto it are refused with
## `refuse.rubble`, and the refusal changes nothing (the buildings, the map's revision, the history).
func _place_on_rubble() -> void:
	var drawn := _draw(2)
	var id: int = drawn.get("placed", -1)
	if not check(drawn.get("ok", false) and id >= 0, "rubble: a building drawn to bring down (%s)" % [drawn]):
		return
	var spot: Vector3 = CityStore.query(&"probe_building", [id]).position
	var hit: Dictionary = CityStore.query(&"body_pick", [Vector3(spot.x, SKY_M, spot.z), Vector3.DOWN])
	var roof: Vector3 = hit.point if not hit.is_empty() else spot
	var ruins: int = CityStore.query(&"damage_probe").ruins
	var force := int(DataTables.catalog_table("blast").strength_min)
	while _alive(id) and force <= int(DataTables.catalog_table("blast").strength_max):
		CityStore.dispatch(&"strike", [roof, force])
		force += 1
	if not check(not _alive(id) and int(CityStore.query(&"damage_probe").ruins) > ruins, "rubble: building %d fell and left a ruin" % id):
		return
	var other := _draw(3)
	check(other.get("ok", false), "rubble: a building drawn to move (%s)" % [other])
	var buildings: int = CityStore.snapshot.buildings
	var revision: int = CityStore.query(&"map_revision")
	var label: String = CityStore.snapshot.undo_label
	var over := _draw(2)
	check(not over.get("ok", true) and over.get("error", "") == "refuse.rubble", "rubble: a lot drawn over the ruin is refused (%s)" % [over])
	var moved: Dictionary = CityStore.dispatch(&"building_place", [other.get("placed", -1), spot.x, spot.z + LOT_M / 2.0, 0.0])
	check(not moved.get("ok", true) and moved.get("error", "") == "refuse.rubble", "rubble: a building moved onto the ruin is refused (%s)" % [moved])
	check(int(CityStore.snapshot.buildings) == buildings and int(CityStore.query(&"map_revision")) == revision and CityStore.snapshot.undo_label == label,
			"rubble: the refusals changed nothing (%d buildings, revision %d, last step %s)" % [CityStore.snapshot.buildings, CityStore.query(&"map_revision"), CityStore.snapshot.undo_label])
	check(int(CityStore.query(&"damage_probe").ruins) > ruins, "rubble: the ruin is still there")


## Item 13: with no money, a road that crosses road A is laid (A split at the crossing, the new pieces journalled) and only then
## refused for its cost (road_economy.cpp settle_road_edit), so the history rolls the ops back. The refusal moves nothing: the map's
## revision, the network's, the saved state, the history and the furniture of a street of the town far away are as before.
func _refusal_inert() -> void:
	var a := field + Vector3(0, 0, ROAD_OUT_M)
	var e := a + Vector3(ROAD_M, 0, 0)
	var laid: Dictionary = CityStore.dispatch(&"build_road", [a, e, false, Vector3.ZERO, PROFILE, PX])
	check(laid.get("ok", false), "refusal: road A laid (%s)" % [laid])
	var w: Dictionary = await save(SLOT)
	check(w.get("ok", false) and not CityStore.snapshot.unsaved, "refusal: the city saved (%s)" % [w])
	var far := _town_street()
	check(far >= 0, "refusal: a street of the town to watch")
	var spots: Array = CityStore.query(&"furniture_spots", [far])
	var funds: float = CityStore.snapshot.money
	CityStore.query(&"probe_set_money", [NO_MONEY])
	var revision: int = CityStore.query(&"map_revision")
	var edits: int = CityStore.snapshot.edit_revision
	var label: String = CityStore.snapshot.undo_label
	var mid := (a + e) * 0.5
	var across: Dictionary = CityStore.dispatch(&"build_road", [mid - Vector3(0, 0, CROSS_M), mid + Vector3(0, 0, CROSS_M), false, Vector3.ZERO, PROFILE, PX])
	CityStore.query(&"probe_set_money", [funds])
	check(not across.get("ok", true) and across.get("error", "") == "hint.rule.funds", "refusal: a road across A refused for its cost after it was laid (%s)" % [across])
	check(CityStore.query(&"furniture_spots", [far]) == spots, "refusal: the furniture of street %d far away is as it was" % far)
	check(int(CityStore.query(&"map_revision")) == revision, "refusal: the map's revision stays %d (%d)" % [revision, CityStore.query(&"map_revision")])
	check(int(CityStore.snapshot.edit_revision) == edits, "refusal: the network's revision stays %d (%d)" % [edits, CityStore.snapshot.edit_revision])
	check(not CityStore.snapshot.unsaved, "refusal: the city is still saved")
	check(CityStore.snapshot.undo_label == label, "refusal: no step was added to the history (%s)" % CityStore.snapshot.undo_label)
	_none_broken("edit")


## A street of the town (the first a scan of the city's box from its corner meets), or -1.
func _town_street() -> int:
	var bounds: Dictionary = CityStore.query(&"city_bounds")
	var x: float = bounds.min.x
	while x < bounds.max.x:
		var z: float = bounds.min.z
		while z < bounds.max.z:
			var seg: int = CityStore.query(&"segment_at", [Vector3(x, 0, z)]).get("seg", -1)
			if seg >= 0:
				return seg
			z += SCAN_STEP_M
		x += SCAN_STEP_M
	return -1


## A building with at least `least` floors, or -1.
func _standing(least: int) -> int:
	for id in CityStore.query(&"list_buildings"):
		if int(CityStore.query(&"probe_building", [id]).floors) >= least:
			return id
	return -1


func _alive(id: int) -> bool:
	return (CityStore.query(&"list_buildings") as Array).has(id)


## A rectangle drawn on lot `k` of the field.
func _draw(k: int) -> Dictionary:
	var corner := field + Vector3(k * LOT_GAP_M, 0, 0)
	return CityStore.dispatch(&"draw_building", [corner, corner + Vector3(LOT_M, 0, LOT_M), DRAWN_FN, DRAWN_ZONE, PLAN_RECTANGLE])


## The core's invariants of `feature` hold now (probes/broken_records: the whole city, and what the debug run collected after each
## command since the last ask).
func _none_broken(feature: String) -> void:
	var broken: Array = (CityStore.query(&"broken_records") as Array).filter(func(b: Dictionary) -> bool: return String(b.feature) == feature)
	check(broken.is_empty(), "%s: no record broken %s" % [feature, broken])
