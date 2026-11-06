## The street's two gestures of the Inspector, seen working in traffic: a car stops for who is on a zebra between junctions and never reaches them, and a car
## turns back through the opening of a median. Run: godot --path project res://tests/core/street_crossing_test.tscn -- --no-flow
extends Node3D

const TOWN_MAP := 0
const SEED := 7
const WARM_TICKS := 1200
const SAMPLE := 200
const ZEBRA := 1
const NEAR_M := 14.0
const CLEAR_M := 1.5 # the middle of a car to the middle of a walker: under this they touch
const STEPS := 4000
const TICKS := 4
const BUSY_STREETS := 6
const OPENINGS := 10

var failures := 0


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	failures += 0 if ok else 1


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	_check(CityStore.boot(world.terrain, world.materials()).is_empty(), "boot")
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	CityStore.step_ticks(WARM_TICKS)
	_zebras()
	_openings()
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


func _middle_of(at: Vector3) -> Variant:
	var seg: Dictionary = CityStore.query(&"segment_at", [at])
	if seg.get("seg", -1) < 0:
		return null
	var points: Array = seg.points
	return points[points.size() / 2]


## Zebras on the streets with cars on them, the camera's window over the town.
func _zebras() -> void:
	var sample: Array = CityStore.query(&"traffic_probe", [SAMPLE]).sample
	var made := 0
	for p: Vector3 in sample:
		var at: Variant = _middle_of(p)
		if at != null and CityStore.dispatch(&"add_crossing", [at, ZEBRA]).get("ok", false):
			made += 1
		if made == BUSY_STREETS:
			break
	_check(made > 0, "faixas postas em ruas com carro: %d" % made)
	var crossers := 0
	var near := 0
	var held := 0
	var closest := INF
	for i in STEPS:
		CityStore.step_ticks(TICKS)
		var w: Dictionary = CityStore.query(&"street_watch", [NEAR_M])
		crossers = maxi(crossers, w.crossers)
		near += w.near
		held += w.held
		closest = minf(closest, w.closest_m)
		if near > 0 and held > 0 and i * TICKS > WARM_TICKS:
			break
	print("na faixa ", crossers, " carros perto ", near, " parados ", held, " o mais perto ", closest)
	_check(crossers > 0, "gente atravessa")
	_check(near > 0, "com carro chegando")
	_check(held > 0, "carro para para quem atravessa")
	_check(closest >= CLEAR_M, "nenhum carro alcança quem está na faixa (%.2f m)" % closest)


## Openings along the avenues, and somebody using one.
func _openings() -> void:
	var census: Array = CityStore.query(&"traffic_probe", [SAMPLE]).sample
	var made := 0
	for p: Vector3 in census:
		var info: Dictionary = CityStore.query(&"inspect", [p])
		if not info.get("median", false):
			continue
		var at: Variant = _middle_of(p)
		if at != null and CityStore.dispatch(&"add_uturn", [at, true]).get("ok", false):
			made += 1
		if made == OPENINGS:
			break
	_check(made > 0, "aberturas postas no canteiro: %d" % made)
	var turning := 0
	for i in STEPS:
		CityStore.step_ticks(TICKS)
		turning = maxi(turning, CityStore.query(&"street_watch", [NEAR_M]).turning_back)
		if turning > 0:
			break
	_check(turning > 0, "um carro volta pela abertura do canteiro")
