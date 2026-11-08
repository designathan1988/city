## The mass of the generated city, measured as the 4.0's was: floors by distance from the middle of the built area. A record says the
## storeys its body has; the core is towers and the rim is low, as the same seed gives in the 4.0. Writes every record to
## city_mass.json in the tests' own folder (TestCase.SCRATCH) for the report.
## Run: godot --path project res://tests/core/city_mass_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const LARGE_MAP := 5
const SEED := 7
const BANDS: Array[float] = [0.0, 0.15, 0.3, 0.5, 0.75, 1.01]
const TOWER_FLOORS := 10
const OUT_FILE := "city_mass.json"
const CORE_TOWERS := 0.6 # the 4.0, seed 7: 88% and 78% in the two inner bands
const RIM_TOWERS := 0.05 # 0.2% from half the radius out
const CORE_BANDS := 2
const RIM_BAND := 3
const TWIN_REACH_M := 32.0 # the 4.0's tests/world/variety.spec.ts: a building and the nearest other one
const TWIN_ALIKE := 1.2
const TWINS_MOST := 0.05


func _ready() -> void:
	if not boot():
		finish()
		return
	var map := _arg("--map=", TOWN_MAP)
	var made: Dictionary = new_map(map, _arg("--seed=", SEED), true)
	print("map: %d nodes, %d streets, %d lots, %d buildings, %d squares in %.2f s" % [made.nodes, made.segments, made.lots, made.buildings, made.squares, made.seconds])
	var rows: Array = []
	var apart: Array = []
	var middle := Vector2.ZERO
	var weight := 0.0
	for id: int in CityStore.query(&"list_buildings"):
		var b: Dictionary = CityStore.query(&"probe_building", [id])
		var area: float = b.w * b.d
		var top := _top(id)
		if b.floors != top:
			apart.append([id, b.floors, top])
		rows.append([b.position.x, b.position.z, top, b.zone, b.fn, b.w, b.d])
		middle += Vector2(b.position.x, b.position.z) * area
		weight += area
	check(apart.is_empty(), "every building's probe and body agree on its storeys ([id, probe, body] apart: %s)" % [apart.slice(0, 8)])
	middle /= weight
	var reach := 0.0
	for r: Array in rows:
		reach = maxf(reach, middle.distance_to(Vector2(r[0], r[1])))
	print("records %d centre %s radius %.0f" % [rows.size(), middle, reach])
	for i in BANDS.size() - 1:
		var towers := _band(rows, middle, reach, BANDS[i], BANDS[i + 1])
		if map != TOWN_MAP:
			continue
		if i < CORE_BANDS:
			check(towers > CORE_TOWERS, "core: %.2f of the towers in band %d" % [towers, i])
		elif i >= RIM_BAND:
			check(towers < RIM_TOWERS, "edge: %.2f towers in band %d" % [towers, i])
	var look: Dictionary = CityStore.query(&"look_twins", [TWIN_REACH_M, TWIN_ALIKE])
	print("neighbours alike: %d of %d pairs" % [look.twins, look.pairs])
	if map == TOWN_MAP:
		check(float(look.twins) / look.pairs < TWINS_MOST, "neighbours do not repeat: %d of %d pairs alike" % [look.twins, look.pairs])
	var f := FileAccess.open(scratch(OUT_FILE), FileAccess.WRITE)
	f.store_string(JSON.stringify(rows))
	if map == TOWN_MAP:
		_large_city()
	finish()


## The large city, on its own radius: every record still says what its body has.
func _large_city() -> void:
	var made: Dictionary = new_map(LARGE_MAP, SEED, true)
	print("big city: %d nodes, %d streets, %d buildings, %d squares" % [made.nodes, made.segments, made.buildings, made.squares])
	var apart: Array = []
	for id: int in CityStore.query(&"list_buildings"):
		var floors: int = CityStore.query(&"probe_building", [id]).floors
		var top := _top(id)
		if floors != top:
			apart.append([id, floors, top])
	check(apart.is_empty(), "big city: every building's probe and body agree on its storeys ([id, probe, body] apart: %s)" % [apart.slice(0, 8)])


func _arg(prefix: String, fallback: int) -> int:
	for a in OS.get_cmdline_user_args():
		if a.begins_with(prefix):
			return int(a.trim_prefix(prefix))
	return fallback


## The storeys of the body drawn, not the record's: a tower chooses its own.
func _top(id: int) -> int:
	var top := 0
	for v: Dictionary in CityStore.query(&"body_info", [id]).volumes:
		top = maxi(top, v.base + v.storeys)
	return top


func _band(rows: Array, middle: Vector2, reach: float, from: float, to: float) -> float:
	var count := 0
	var floors := 0
	var top := 0
	var towers := 0
	for r: Array in rows:
		var k := middle.distance_to(Vector2(r[0], r[1])) / reach
		if k >= from and k < to:
			count += 1
			floors += r[2]
			top = maxi(top, r[2])
			towers += 1 if r[2] >= TOWER_FLOORS else 0
	print("%.2f-%.2f: n=%d mean storeys %.1f max %d >=10: %d" % [from, to, count, float(floors) / maxi(1, count), top, towers])
	return float(towers) / maxi(1, count)
