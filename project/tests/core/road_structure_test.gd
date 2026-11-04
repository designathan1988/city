## Core test of a raised road through the store: the deck's clearance, the ramps' grade, a road passing under with no junction,
## undo and the save keeping the profile.
## Run: godot --headless --path project res://tests/core/road_structure_test.tscn -- --no-flow
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const OUT_M := 460.0
const PX := 0.25
const ELEVATED := 1
const BRIDGE := 2
const SPAN_M := 320.0
const STEP_M := 4.0
const CLEARANCE_M := 5.6
const BRIDGE_CLEARANCE_M := 3.0
const SLACK_M := 0.3
const STEEPEST := 0.18 # the ramp's 16%, and what the sampling adds
const FOOT_M := 3.0
const FOOT_LIFT_M := 1.0
const UNDER_LEN_M := 60.0
const BRIDGE_OFF_M := 120.0

var failures := 0
var street := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	var errors := CityStore.boot(world.terrain, world.materials())
	_check(errors.is_empty(), "boot %s" % [errors])
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	for p in CityStore.query(&"road_profiles"):
		if p.id == "urbanStreet":
			street = p.index
	var b: Dictionary = CityStore.query(&"city_bounds")
	var e := Vector3(b.max.x + OUT_M, 0, (b.min.z + b.max.z) * 0.5)
	_elevated(e)
	_bridge(e + Vector3(BRIDGE_OFF_M, 0, 0))
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


func _lay(a: Vector3, b: Vector3, structure: int) -> Dictionary:
	return CityStore.dispatch(&"build_road_path", [PackedVector3Array([a, (a + b) / 2, b]), street, PX, structure])


func _y(p: Vector3) -> float:
	return CityStore.query(&"road_y_at", [p.x, p.z])


func _ground(p: Vector3) -> float:
	return CityStore.query(&"height_at", [p.x, p.z])


func _elevated(e: Vector3) -> void:
	var a := e
	var b := e + Vector3(0, 0, SPAN_M)
	var mid := (a + b) / 2
	var before: int = CityStore.snapshot.segments
	_check(_lay(a, b, ELEVATED).get("ok", false), "via elevada assentada")
	_check(_y(mid) - _ground(mid) >= CLEARANCE_M - SLACK_M, "tabuleiro acima da folga: %.2f" % (_y(mid) - _ground(mid)))
	_check(_y(a + Vector3(0, 0, FOOT_M)) - _ground(a) < FOOT_LIFT_M, "o pé da rampa fica no chão")
	var steepest := 0.0
	for i in int(SPAN_M / STEP_M):
		var p := a + Vector3(0, 0, i * STEP_M)
		steepest = maxf(steepest, absf(_y(p + Vector3(0, 0, STEP_M)) - _y(p)) / STEP_M)
	_check(steepest <= STEEPEST, "rampa dentro do limite: %.3f" % steepest)
	var laid: int = CityStore.snapshot.segments
	var deck := _y(mid)
	var half := Vector3(UNDER_LEN_M / 2, 0, 0)
	_check(_lay(mid - half, mid + half, 0).get("ok", false), "via do chão passa por baixo")
	_check(CityStore.snapshot.segments == laid + 1, "sem nó no cruzamento: %d" % (CityStore.snapshot.segments - laid))
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")
	_check(CityStore.snapshot.segments == before, "desfeita: %d" % CityStore.snapshot.segments)
	CityStore.dispatch(&"redo")
	_check(absf(_y(mid) - deck) < SLACK_M, "refeita na mesma altura: %.2f" % _y(mid))


func _bridge(e: Vector3) -> void:
	var a := e
	var b := e + Vector3(0, 0, SPAN_M)
	var mid := (a + b) / 2
	_check(_lay(a, b, BRIDGE).get("ok", false), "ponte assentada")
	_check(_y(mid) - _ground(mid) >= BRIDGE_CLEARANCE_M - SLACK_M, "ponte acima da folga: %.2f" % (_y(mid) - _ground(mid)))


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	if not ok:
		failures += 1
