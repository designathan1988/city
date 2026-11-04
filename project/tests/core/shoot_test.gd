## Core test of the pistol and of what a blow leaves in a save (no rendering needed): a round meets a walker where it is
## aimed (the head kills, the trunk costs a third of the health, the wound slows), a miss hits nothing, a car hit through
## its glass stops and rounds burst it; the wounded bleed; then a city with a crater, a ruin and furniture gone is saved,
## a new map replaces it and opening the file brings the same marks back.
## Run: godot --headless --path project res://tests/core/shoot_test.tscn -- --no-flow
extends Node3D

const TOWN_MAP := 0
const SEED := 7
const OTHER_SEED := 11
const WARM_TICKS := 900
const LIMIT := 40
const HEAD_AIM_M := 1.7
const TRUNK_AIM_M := 1.2
const STANDOFF := Vector3(0.0, 6.0, 12.0)
const SKY := Vector3(0.0, 1.0, 0.0)
const SKY_ORIGIN := Vector3(0.0, 300.0, 0.0)
const BLEED_SOON_TICKS := 60
const BLEED_TICKS := 2400
const SAME_SPOT_M := 0.5
const SHOTS_TO_BURST := 14
const GLASS_AIM_M := 1.2
const MOTO := 5
const FIRST_HEAVY := 6
const BIG_FORCE := 45
const SLOT := "shoot_test_slot"
var failures := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	var errors := CityStore.boot(world.terrain, world.materials())
	_check(errors.is_empty(), "boot %s" % [errors])
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	CityStore.step_ticks(WARM_TICKS)
	_walkers()
	_vehicles()
	await _save_marks()
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


## A round at `at` from above and behind it: the origin is the point plus STANDOFF, the direction through the point.
func _shoot(at: Vector3) -> Dictionary:
	var origin := at + STANDOFF
	return CityStore.dispatch(&"shoot", [origin, (at - origin).normalized()])


func _walker_of(id: int) -> Dictionary:
	for w in CityStore.query(&"shot_targets", [LIMIT * 4]).walkers:
		if w.id == id:
			return w
	return {}


func _walkers() -> void:
	var found: Dictionary = CityStore.query(&"shot_targets", [LIMIT])
	_check(found.walkers.size() >= 3, "há pedestres para o tiro (%d)" % found.walkers.size())
	if found.walkers.size() < 3:
		return
	var hp_full: float = found.walkers[0].hp
	_check(hp_full > 0.0, "o pedestre nasce com vida (%.0f)" % hp_full)
	var trunk: Dictionary = found.walkers[0]
	var r := _shoot(trunk.position + Vector3(0, TRUNK_AIM_M * trunk.scale, 0))
	_check(r.get("hit", 0) == 1 and not r.killed, "tiro no tronco acerta e não mata %s" % [r])
	var after := _walker_of(trunk.id)
	_check(not after.is_empty() and after.hp < hp_full, "a vida caiu (%.0f -> %.0f)" % [hp_full, after.get("hp", -1.0)])
	var head: Dictionary = found.walkers[1]
	var kill := _shoot(head.position + Vector3(0, HEAD_AIM_M * head.scale, 0))
	_check(kill.get("hit", 0) == 1 and kill.killed, "tiro na cabeça mata %s" % [kill])
	_check(_walker_of(head.id).is_empty(), "quem morreu saiu da calçada")
	var sky: Dictionary = CityStore.dispatch(&"shoot", [SKY_ORIGIN, SKY])
	_check(sky.get("hit", 1) == 0, "tiro para o céu não acerta nada")
	var hurt: Dictionary = found.walkers[2]
	for k in 2:
		var w := _walker_of(hurt.id)
		if not w.is_empty():
			_shoot(w.position + Vector3(0, TRUNK_AIM_M * w.scale, 0))
	var wounded := _walker_of(hurt.id)
	_check(wounded.is_empty() or wounded.hp < hp_full * 0.5, "dois tiros no tronco ferem de gravidade")
	if not wounded.is_empty() and wounded.hp > 0.0 and wounded.bleed > 0.0:
		CityStore.step_ticks(BLEED_SOON_TICKS)
		var soon := _walker_of(hurt.id)
		_check(not soon.is_empty() and soon.hp < wounded.hp, "o ferido sangra (%.0f -> %.0f)" % [wounded.hp, soon.get("hp", 0.0)])
		CityStore.step_ticks(BLEED_TICKS)
		var later := _walker_of(hurt.id)
		_check(later.is_empty() or later.position.distance_to(wounded.position) > SAME_SPOT_M, "e morre do sangramento (a vaga foi para outro)")


func _vehicles() -> void:
	var found: Dictionary = CityStore.query(&"shot_targets", [LIMIT])
	var car: Dictionary = {}
	for v in found.vehicles:
		if v.type != MOTO and v.type < FIRST_HEAVY:
			car = v
			break
	_check(not car.is_empty(), "há um carro para o tiro")
	if car.is_empty():
		return
	var before: int = CityStore.query(&"damage_probe").vehicles
	var burst := false
	var shots := 0
	while shots < SHOTS_TO_BURST and not burst:
		var now: Dictionary = {}
		for v in CityStore.query(&"shot_targets", [LIMIT * 4]).vehicles:
			if v.id == car.id:
				now = v
		if now.is_empty():
			break
		var r := _shoot(now.position + Vector3(0, GLASS_AIM_M, 0))
		burst = r.get("burst", false)
		shots += 1
		if shots == 1:
			_check(r.get("hit", 0) == 2, "o tiro acerta o carro %s" % [r])
	_check(burst or CityStore.query(&"damage_probe").vehicles < before, "rajadas fazem o carro explodir (%d tiros)" % shots)


func _save_marks() -> void:
	var spot := Vector3.INF
	for id in CityStore.query(&"list_buildings"):
		var info: Dictionary = CityStore.query(&"probe_building", [id])
		if info.floors >= 2 and info.zone > 0:
			spot = info.position
			break
	_check(spot != Vector3.INF, "um prédio para a bomba")
	CityStore.dispatch(&"strike", [spot, BIG_FORCE])
	var marks: Dictionary = CityStore.query(&"damage_probe")
	_check(marks.craters > 0 and marks.ruins > 0, "a bomba deixou cratera e ruína %s" % [marks])
	var w := await _save()
	_check(w.get("ok", false), "salvou %s" % w)
	CityStore.new_map(TOWN_MAP, OTHER_SEED, false)
	_check(CityStore.query(&"damage_probe").craters == 0, "mapa novo não herda as marcas")
	var r: Dictionary = CityStore.load_game(SLOT)
	_check(r.ok, "abriu %s" % r)
	var got: Dictionary = CityStore.query(&"damage_probe")
	for key in ["craters", "ruins", "gone", "hurt", "strikes"]:
		_check(got[key] == marks[key], "%s volta igual (%d)" % [key, marks[key]])
	DirAccess.remove_absolute(CityStore.save_path(SLOT))


func _save() -> Dictionary:
	var outcome := {}
	CityStore.saved.connect(func(_s: String, r: Dictionary) -> void: outcome.merge(r), CONNECT_ONE_SHOT)
	CityStore.save_game(SLOT)
	while CityStore.save_busy():
		await get_tree().process_frame
	return outcome


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	if not ok:
		failures += 1
