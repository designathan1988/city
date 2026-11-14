## Core test of zones, lots and growth through the store (no rendering needed): a road out of the generated town is zoned and
## grows; painting another zone keeps what grew, taking the zone off removes it; industry has a low and a high density; a
## building says how run-down it is; the generated town opens in seconds.
## Run: godot --headless --path project res://tests/core/growth_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const PROFILE := 0
const OUT_M := 260.0
const PX := 0.25
const ZONE_RADIUS := 200.0
const ZONE_RES := 1
const ZONE_COM := 2
const ZONE_IND_LOW := 9
const ZONE_IND_HIGH := 10
const GROW_TICKS := 600
## The most a generated town may take to open, seconds (doc/specification.md: a new town opens in 8 s).
const OPEN_MOST_S := 8.0
const USEC := 1000000.0


func _ready() -> void:
	if not boot():
		finish()
		return
	var t0 := Time.get_ticks_usec()
	new_map(TOWN_MAP, SEED, true)
	var opened := (Time.get_ticks_usec() - t0) / USEC
	print("town opened in %.2f s, %d buildings" % [opened, CityStore.snapshot.buildings])
	check(opened < OPEN_MOST_S, "the town opens in %.2f s (under %.0f)" % [opened, OPEN_MOST_S])
	check(CityStore.snapshot.buildings > 0, "the town is filled")
	check(int(CityStore.constants.zone_codes) > ZONE_IND_HIGH, "industry has a low and a high density")
	var b: Dictionary = CityStore.query(&"city_bounds")
	var a := Vector3(b.max.x - 1.0, 0, (b.min.z + b.max.z) * 0.5)
	var e := a + Vector3(OUT_M, 0, 0)
	var mid := (a + e) * 0.5
	check(CityStore.dispatch(&"build_road", [a, e, false, Vector3.ZERO, PROFILE, PX]).ok, "a road out of the town")
	var z: Dictionary = CityStore.dispatch(&"paint_zone", [mid, ZONE_RADIUS, ZONE_RES])
	check(z.ok and z.zoned_sides > 0, "its sides zoned: %s" % z)
	CityStore.set_sim_switch("growth", true)
	var before: int = CityStore.snapshot.buildings
	ticks(GROW_TICKS)
	var grown: int = CityStore.snapshot.grown
	print("grown %d, buildings %d -> %d, cells left %d" % [grown, before, CityStore.snapshot.buildings, CityStore.snapshot.growth_cells])
	check(grown > 0, "buildings grew on the zoned road (%d)" % grown)
	check((CityStore.query(&"broken_records") as Array).is_empty(), "no record broken after growth: %s" % [CityStore.query(&"broken_records")])
	var stood: int = CityStore.snapshot.buildings
	var re: Dictionary = CityStore.dispatch(&"paint_zone", [mid, ZONE_RADIUS, ZONE_COM])
	check(re.ok and int(re.buildings_removed) == 0 and CityStore.snapshot.buildings == stood, "another zone keeps what grew: %s" % re)
	var low: Dictionary = CityStore.dispatch(&"paint_zone", [mid, ZONE_RADIUS, ZONE_IND_LOW])
	check(low.ok and int(low.buildings_removed) == 0, "low-density industry is a zone: %s" % low)
	var info: Dictionary = CityStore.query(&"inspect", [mid + Vector3(0, 0, 20.0)])
	if info.get("kind", "") != "building":
		info = CityStore.query(&"inspect", [mid - Vector3(0, 0, 20.0)])
	check(info.get("kind", "") == "building" and int(info.get("wear_percent", -1)) == 0, "a building just grown is as new: %s" % info)
	var off: Dictionary = CityStore.dispatch(&"paint_zone", [mid, ZONE_RADIUS, 0])
	check(off.ok and int(off.buildings_removed) > 0, "the zone taken off takes what grew on it: %s" % off)
	finish()
