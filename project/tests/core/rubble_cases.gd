## The strike_test case of growth and rubble (doc/specs/003-roadcraft-features/sheets/backbone-records-true.md item 18): a house
## brought down leaves a ruin whose lot stays blocked past the growth hold of the blow, until the ruin is cleared; cleared, the lot
## grows again in the same time; the clearing undone puts the ruin back and takes away what grew on its lot outside the history.
class_name RubbleCases
extends RefCounted

const FALL_FORCE := 30
const SKY_M := 900.0
## What growth is given after the hold of a blow ends (sim.growth_per_s builds many a second): long enough to reach a free lot.
const GROW_AFTER_S := 30.0


## `t`: the strike_test (TestCase), whose map, ticks and check these are; its map open paused.
static func run(t: Node) -> void:
	var house := -1
	for id in CityStore.query(&"list_buildings"):
		var info: Dictionary = CityStore.query(&"probe_building", [id])
		if info.floors > 0 and info.fn == 0: # a building of the lots growth fills, not one of the plan's
			house = id
			break
	if not t.check(house >= 0, "rubble: there is a house to bring down"):
		return
	var spot: Vector3 = CityStore.query(&"probe_building", [house]).position
	var hit: Dictionary = CityStore.query(&"body_pick", [Vector3(spot.x, SKY_M, spot.z), Vector3.DOWN])
	CityStore.dispatch(&"strike", [hit.point if not hit.is_empty() else spot, FALL_FORCE])
	var ruins: int = CityStore.query(&"damage_probe").ruins
	t.check(not (CityStore.query(&"list_buildings") as Array).has(house) and covering(spot).is_empty(), "rubble: house %d fell and left its lot to the ruin" % house)
	CityStore.set_sim_switch("growth", true)
	var hz := float(DataTables.tuning("sim_tick_hz"))
	var hold := float(DataTables.catalog_table("blast").strike.hold_growth_s)
	var grown: int = CityStore.snapshot.grown
	t.ticks(ceili((hold + GROW_AFTER_S) * hz))
	print("rubble: grown past the hold ", int(CityStore.snapshot.grown) - grown)
	t.check(covering(spot).is_empty(), "rubble: nothing grew over the ruin past the hold (%s)" % [covering(spot)])
	t.check(_growth_broken().is_empty(), "rubble: no building stands on rubble %s" % [_growth_broken()])
	var cleared: Dictionary = CityStore.dispatch(&"demolish", [spot])
	t.check(cleared.get("ok", false) and int(CityStore.query(&"damage_probe").ruins) == ruins - 1, "rubble: the ruin cleared (%s)" % [cleared])
	t.ticks(ceili(GROW_AFTER_S * hz))
	t.check(not covering(spot).is_empty(), "rubble: the cleared lot grows again")
	# the clearing undone: the ruin comes back, and what grew on its lot outside the history goes (as a record put back evicts it)
	t.check(CityStore.dispatch(&"undo") and int(CityStore.query(&"damage_probe").ruins) == ruins, "rubble: the clearing undone, the ruin back")
	t.check(covering(spot).is_empty(), "rubble: what grew on the cleared lot went with the undo (%s)" % [covering(spot)])
	t.check(_growth_broken().is_empty(), "rubble: no building stands on rubble after the undo %s" % [_growth_broken()])


## The buildings whose lot covers the point (BuildingRecords::covers with no pad).
static func covering(p: Vector3) -> Array:
	var out := []
	for id in CityStore.query(&"list_buildings"):
		var b: Dictionary = CityStore.query(&"probe_building", [id])
		var dx: float = p.x - b.position.x
		var dz: float = p.z - b.position.z
		var c := cos(float(b.rot))
		var sn := sin(float(b.rot))
		if absf(dx * c - dz * sn) < b.w / 2.0 and absf(dx * sn + dz * c) < b.d / 2.0:
			out.append(id)
	return out


## The core's growth invariants broken now (probes/broken_records).
static func _growth_broken() -> Array:
	return (CityStore.query(&"broken_records") as Array).filter(func(b: Dictionary) -> bool: return String(b.feature) == "growth")
