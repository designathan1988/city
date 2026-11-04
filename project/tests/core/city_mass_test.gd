## The mass of the generated city, measured as the 4.0's was: floors by distance from the middle of the built area. A record says the
## storeys its body has; the core is towers and the rim is low, as the same seed gives in the 4.0. Writes every record to
## user://city_mass.json for the report.
## Run: godot --path project res://tests/core/city_mass_test.tscn -- --no-flow
extends Node3D

const TOWN_MAP := 0
const SEED := 7
const BANDS: Array[float] = [0.0, 0.15, 0.3, 0.5, 0.75, 1.01]
const TOWER_FLOORS := 10
const OUT_FILE := "user://city_mass.json"
const CORE_TOWERS := 0.6 # the 4.0, seed 7: 88% and 78% in the two inner bands
const RIM_TOWERS := 0.05 # 0.2% from half the radius out
const CORE_BANDS := 2
const RIM_BAND := 3

var failures := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	_check(CityStore.boot(world.terrain, world.materials()).is_empty(), "boot")
	var made: Dictionary = CityStore.new_map(TOWN_MAP, SEED, true)
	print("mapa: %d nós, %d ruas, %d lotes, %d prédios em %.2f s" % [made.nodes, made.segments, made.lots, made.buildings, made.seconds])
	CityStore.dispatch(&"set_speed", [0])
	var rows: Array = []
	var middle := Vector2.ZERO
	var weight := 0.0
	for id: int in CityStore.query(&"list_buildings"):
		var b: Dictionary = CityStore.query(&"probe_building", [id])
		var area: float = b.w * b.d
		var top := _top(id)
		_check(b.floors == top, "prédio %d: a sonda diz %d andares e o corpo tem %d" % [id, b.floors, top])
		rows.append([b.position.x, b.position.z, top, b.zone, b.fn, b.w, b.d])
		middle += Vector2(b.position.x, b.position.z) * area
		weight += area
	middle /= weight
	var reach := 0.0
	for r: Array in rows:
		reach = maxf(reach, middle.distance_to(Vector2(r[0], r[1])))
	print("registros %d centro %s raio %.0f" % [rows.size(), middle, reach])
	for i in BANDS.size() - 1:
		var towers := _band(rows, middle, reach, BANDS[i], BANDS[i + 1])
		if i < CORE_BANDS:
			_check(towers > CORE_TOWERS, "miolo: %.2f das torres na faixa %d" % [towers, i])
		elif i >= RIM_BAND:
			_check(towers < RIM_TOWERS, "borda: %.2f de torres na faixa %d" % [towers, i])
	var f := FileAccess.open(OUT_FILE, FileAccess.WRITE)
	f.store_string(JSON.stringify(rows))
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


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
	print("%.2f-%.2f: n=%d andares medio %.1f max %d >=10: %d" % [from, to, count, float(floors) / maxi(1, count), top, towers])
	return float(towers) / maxi(1, count)


func _check(ok: bool, what: String) -> void:
	if ok:
		return
	failures += 1
	printerr("FALHOU: ", what)
