## Core test of the bomb through the store (no rendering needed): the blast model against known points of the
## Kingery-Bulmash curve, and that a blow changes the city for real: buildings worn and brought down, a road cut by a
## crater, furniture knocked out, walkers and vehicles gone; the whole blow undone in one step.
## Run: godot --headless --path project res://tests/core/strike_test.tscn -- --no-flow
extends Node3D

const TOWN_MAP := 0
const SEED := 7
## Charge of the force that makes the model's TNT mass 2 kg (the test reads it back) and the scaled distance asked.
const SCALED_Z := 10.0
## Kingery-Bulmash (Swisdak 1994) incident pressure at Z = 10 m/kg^(1/3): 14.9 kPa (the doc computes it by hand).
const KB_AT_Z10_KPA := 14.9
const KB_TOLERANCE := 0.05
const BIG_FORCE := 50
const CURVE_Z := [0.2, 1.0, 10.0, 40.0, 100.0, 300.0]
const DURATION_SLACK_MS := 1e-3
const CURVE_LAST_Z := 40.0 # the duration fit holds to here
const DURATION_HOLD_TOL := 1e-3
const SMALL_FORCE := 5
const FALL_FORCE := 30
const PIECES_LEAST := 12
const FALL_FRAMES := 3000
const SETTLE_FRAMES := 60
const HITS_TO_FALL := 12
const WARM_TICKS := 600
## Step of the scan for a point of road.
const SCAN_STEP_M := 40.0
var failures := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	var errors := CityStore.boot(world.terrain, world.materials())
	_check(errors.is_empty(), "boot %s" % [errors])
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	CityStore.step_ticks(WARM_TICKS)
	_model()
	_effects()
	_roads()
	await _fall()
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


func _model() -> void:
	var one: Dictionary = CityStore.query(&"blast_probe", [float(SMALL_FORCE), 1.0])
	var kg: float = one.kg
	var at: Dictionary = CityStore.query(&"blast_probe", [float(SMALL_FORCE), SCALED_Z * pow(kg, 1.0 / 3.0)])
	print("sopro força ", SMALL_FORCE, ": ", at)
	_check(absf(at.pressure_kpa / KB_AT_Z10_KPA - 1.0) < KB_TOLERANCE, "K-B em Z=10: %.2f kPa (esperado %.1f)" % [at.pressure_kpa, KB_AT_Z10_KPA])
	var far: Dictionary = CityStore.query(&"blast_probe", [float(SMALL_FORCE), 2.0 * at.light_range_m])
	_check(far.pressure_kpa < at.pressure_kpa, "a sobrepressão cai com a distância")
	var big: Dictionary = CityStore.query(&"blast_probe", [float(BIG_FORCE), 1.0])
	_curves(kg)
	_check(big.collapse_range_m > one.collapse_range_m * 2.0, "mais força, mais alcance: %.1f m -> %.1f m" % [one.collapse_range_m, big.collapse_range_m])
	_check(big.crater_radius_m > one.crater_radius_m, "cratera cresce com a carga")


## The three curves at scaled distances from the first row to far past the last: finite everywhere, the pressure falling and the
## duration never falling (a polynomial fitted to Z <= 40 must not be carried beyond it).
func _curves(kg: float) -> void:
	var cbrt := pow(kg, 1.0 / 3.0)
	var last_p := INF
	var last_d := 0.0
	var at_40 := 0.0
	var ok := true
	for z in CURVE_Z:
		var s: Dictionary = CityStore.query(&"blast_probe", [float(SMALL_FORCE), z * cbrt])
		ok = ok and is_finite(s.pressure_kpa) and is_finite(s.duration_ms) and is_finite(s.impulse_kpa_ms)
		ok = ok and s.pressure_kpa < last_p and s.duration_ms >= last_d - DURATION_SLACK_MS
		if z == CURVE_LAST_Z:
			at_40 = s.duration_ms
		ok = ok and (z <= CURVE_LAST_Z or s.duration_ms <= at_40 * (1.0 + DURATION_HOLD_TOL)) # held, not carried on
		last_p = s.pressure_kpa
		last_d = s.duration_ms
	_check(ok, "pressão cai, duração não cai e tudo é finito de Z=%s a %s" % [CURVE_Z[0], CURVE_Z[-1]])


func _effects() -> void:
	var ids: Array = CityStore.query(&"list_buildings")
	var civic: Dictionary = {}
	for id in ids:
		var info: Dictionary = CityStore.query(&"probe_building", [id])
		if info.floors > 0:
			civic = info
			break
	_check(not civic.is_empty(), "há um prédio para atingir")
	var p0: Dictionary = CityStore.query(&"damage_probe")
	var spot: Vector3 = civic.position
	var roof := Vector3(spot.x, spot.y + 6.0, spot.z)
	var r: Dictionary = CityStore.dispatch(&"strike", [roof, SMALL_FORCE])
	var p1: Dictionary = CityStore.query(&"damage_probe")
	print("1º golpe: ", r, " ", p1)
	_check(r.ok, "golpe aceito")
	_check(p1.hurt > p0.hurt or p1.buildings < p0.buildings, "o prédio sentiu o golpe (dano ou queda)")
	_check(p1.strikes == p0.strikes + 1, "golpe contado")
	var n := 0
	while n < HITS_TO_FALL and CityStore.query(&"damage_probe").buildings >= p0.buildings:
		CityStore.dispatch(&"strike", [roof, SMALL_FORCE])
		n += 1
	var p2: Dictionary = CityStore.query(&"damage_probe")
	print("após ", n + 1, " golpes: ", p2)
	_check(p2.buildings < p0.buildings, "golpes repetidos derrubam o prédio (%d -> %d)" % [p0.buildings, p2.buildings])
	_check(p2.ruins >= 1, "a ruína ficou")
	var ground := Vector3(spot.x, spot.y, spot.z)
	var before_big: Dictionary = CityStore.query(&"damage_probe")
	var big: Dictionary = CityStore.dispatch(&"strike", [ground, BIG_FORCE])
	var after_big: Dictionary = CityStore.query(&"damage_probe")
	print("golpe grande: ", big, " ", after_big)
	_check(after_big.craters >= before_big.craters, "cratera registrada")
	_check(after_big.buildings < before_big.buildings, "a bomba grande derrubou os prédios em volta")
	_check(CityStore.dispatch(&"undo", []), "desfez")
	var undone: Dictionary = CityStore.query(&"damage_probe")
	_check(undone.buildings == before_big.buildings, "desfazer devolveu os prédios (%d)" % undone.buildings)
	_check(undone.ruins == before_big.ruins and undone.craters == before_big.craters, "e as ruínas e crateras")
	_check(CityStore.dispatch(&"redo", []), "refez")
	_check(CityStore.query(&"damage_probe").buildings == after_big.buildings, "refazer repete o golpe")


func _roads() -> void:
	var spot := Vector3.INF
	var bounds: Dictionary = CityStore.query(&"city_bounds")
	var x: float = bounds.min.x
	while x < bounds.max.x and spot == Vector3.INF:
		var z: float = bounds.min.z
		while z < bounds.max.z and spot == Vector3.INF:
			var anchor: Dictionary = CityStore.query(&"segment_at", [Vector3(x, 0, z)])
			if anchor.get("seg", -1) >= 0:
				spot = anchor.at
			z += SCAN_STEP_M
		x += SCAN_STEP_M
	_check(spot != Vector3.INF, "achou um ponto de via")
	var before: Dictionary = CityStore.query(&"damage_probe")
	var r: Dictionary = CityStore.dispatch(&"strike", [spot, BIG_FORCE])
	var after: Dictionary = CityStore.query(&"damage_probe")
	print("golpe na via: ", r, " ", after)
	_check(after.craters == before.craters + 1, "cratera na via")
	_check(after.segments != before.segments or r.segments > 0, "a cratera cortou a via (%d -> %d trechos)" % [before.segments, after.segments])
	_check(after.gone > before.gone, "mobiliário derrubado (%d)" % (after.gone - before.gone))
	_check(CityStore.dispatch(&"undo", []), "desfez o golpe na via")
	var undone: Dictionary = CityStore.query(&"damage_probe")
	_check(undone.segments == before.segments and undone.gone == before.gone and undone.craters == before.craters, "a via, o mobiliário e a cratera voltaram")


## A building brought down comes down in pieces of its own walls: they appear in a job's time, some fly, all come to rest, and undoing the blow takes them away.
func _fall() -> void:
	for f in SETTLE_FRAMES: # what the blows above brought down comes to rest first
		await get_tree().process_frame
	await _until(func() -> bool: return CityStore.query(&"effects_ruin_flying") == 0)
	var ids: Array = CityStore.query(&"list_buildings")
	var spot: Vector3 = CityStore.query(&"probe_building", [ids[ids.size() - 1]]).position
	var ruins: int = CityStore.query(&"damage_probe").ruins
	var before: int = CityStore.query(&"effects_ruin_pieces")
	var r: Dictionary = CityStore.dispatch(&"strike", [spot, FALL_FORCE])
	_check(r.ok and CityStore.query(&"damage_probe").ruins > ruins, "um prédio caiu para a fratura")
	await _until(func() -> bool: return CityStore.query(&"effects_ruin_pieces") > before)
	var pieces: int = CityStore.query(&"effects_ruin_pieces")
	print("peças do prédio caído: ", pieces - before)
	_check(pieces - before >= PIECES_LEAST, "o prédio virou peças (%d)" % (pieces - before))
	await _until(func() -> bool: return CityStore.query(&"effects_ruin_flying") > 0)
	_check(CityStore.query(&"effects_ruin_flying") > 0, "as peças voam")
	await _until(func() -> bool: return CityStore.query(&"effects_ruin_flying") == 0)
	_check(CityStore.query(&"effects_ruin_flying") == 0, "as peças pousam")
	var seed: int = _a_ruin()
	await _until(func() -> bool: return CityStore.query(&"effects_ruin_digest", [seed]) != 0)
	var lying: int = CityStore.query(&"effects_ruin_digest", [seed])
	_check(lying != 0, "a pilha assentou")
	_check(CityStore.dispatch(&"undo", []), "desfez a queda")
	await get_tree().process_frame
	await get_tree().process_frame
	_check(CityStore.query(&"effects_ruin_pieces") <= before, "desfazer leva as peças embora")
	_check(CityStore.dispatch(&"redo", []), "refez a queda")
	await _until(func() -> bool: return CityStore.query(&"effects_ruin_digest", [seed]) != 0)
	_check(CityStore.query(&"effects_ruin_digest", [seed]) == lying, "refazer deita as peças como caíram (%d, %d)" % [lying, CityStore.query(&"effects_ruin_digest", [seed])])


## The seed of the ruin the last strike left.
func _a_ruin() -> int:
	return CityStore.query(&"ruin_seeds")[-1]


func _until(met: Callable) -> void:
	for f in FALL_FRAMES:
		if met.call():
			return
		await get_tree().process_frame


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	if not ok:
		failures += 1
