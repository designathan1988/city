## Core test of the bomb through the store (no rendering needed): the blast model against known points of the
## Kingery-Bulmash curve, and that a blow changes the city for real: buildings worn and brought down, a road cut by a
## crater, furniture knocked out, walkers and vehicles gone; the whole blow undone in one step.
## Run: godot --headless --path project res://tests/core/strike_test.tscn -- --no-flow
extends TestCase

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
const FALL_LIMIT_S := 50.0 # real time a fall may take: the 3000 frames of the old 60 Hz loop
const SETTLE_FRAMES := 60
const HITS_TO_FALL := 12
const WARM_TICKS := 600
## Step of the scan for a point of road.
const SKY_M := 900.0
const TALL_M := 18.0
const SCAN_STEP_M := 40.0


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	ticks(WARM_TICKS)
	_force_steps()
	_model()
	await _scar()
	await _fall()
	_effects()
	_roads()
	finish()


func _model() -> void:
	var one: Dictionary = CityStore.query(&"blast_probe", [float(SMALL_FORCE), 1.0])
	var kg: float = one.kg
	var at: Dictionary = CityStore.query(&"blast_probe", [float(SMALL_FORCE), SCALED_Z * pow(kg, 1.0 / 3.0)])
	print("blast strength ", SMALL_FORCE, ": ", at)
	check(absf(at.pressure_kpa / KB_AT_Z10_KPA - 1.0) < KB_TOLERANCE, "K-B at Z=10: %.2f kPa (expected %.1f)" % [at.pressure_kpa, KB_AT_Z10_KPA])
	var far: Dictionary = CityStore.query(&"blast_probe", [float(SMALL_FORCE), 2.0 * at.light_range_m])
	check(far.pressure_kpa < at.pressure_kpa, "the overpressure falls with distance")
	var big: Dictionary = CityStore.query(&"blast_probe", [float(BIG_FORCE), 1.0])
	_curves(kg)
	check(big.collapse_range_m > one.collapse_range_m * 2.0, "more strength, more reach: %.1f m -> %.1f m" % [one.collapse_range_m, big.collapse_range_m])
	check(big.crater_radius_m > one.crater_radius_m, "the crater grows with the charge")


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
	check(ok, "pressure falls, duration does not and all is finite from Z=%s to %s" % [CURVE_Z[0], CURVE_Z[-1]])


func _effects() -> void:
	var ids: Array = CityStore.query(&"list_buildings")
	var civic: Dictionary = {}
	for id in ids:
		var info: Dictionary = CityStore.query(&"probe_building", [id])
		if info.floors > 0:
			civic = info
			break
	check(not civic.is_empty(), "there is a building to hit")
	var p0: Dictionary = CityStore.query(&"damage_probe")
	var spot: Vector3 = civic.position
	var roof := Vector3(spot.x, spot.y + 6.0, spot.z)
	var r: Dictionary = CityStore.dispatch(&"strike", [roof, SMALL_FORCE])
	var p1: Dictionary = CityStore.query(&"damage_probe")
	print("first strike: ", r, " ", p1)
	check(r.ok, "strike accepted")
	check(p1.hurt > p0.hurt or p1.buildings < p0.buildings, "the building felt the strike (damage or fall)")
	check(p1.strikes == p0.strikes + 1, "strike counted")
	var n := 0
	while n < HITS_TO_FALL and CityStore.query(&"damage_probe").buildings >= p0.buildings:
		CityStore.dispatch(&"strike", [roof, SMALL_FORCE])
		n += 1
	var p2: Dictionary = CityStore.query(&"damage_probe")
	print("after ", n + 1, " strikes: ", p2)
	check(p2.buildings < p0.buildings, "repeated strikes bring the building down (%d -> %d)" % [p0.buildings, p2.buildings])
	check(p2.ruins >= 1, "the ruin stayed")
	var ground := Vector3(spot.x, spot.y, spot.z)
	var before_big: Dictionary = CityStore.query(&"damage_probe")
	var big: Dictionary = CityStore.dispatch(&"strike", [ground, BIG_FORCE])
	var after_big: Dictionary = CityStore.query(&"damage_probe")
	print("big strike: ", big, " ", after_big)
	check(after_big.craters >= before_big.craters, "crater recorded")
	check(after_big.buildings < before_big.buildings, "the big bomb brought the buildings round it down")
	check(CityStore.dispatch(&"undo", []), "undone")
	var undone: Dictionary = CityStore.query(&"damage_probe")
	check(undone.buildings == before_big.buildings, "undo gave the buildings back (%d)" % undone.buildings)
	check(undone.ruins == before_big.ruins and undone.craters == before_big.craters, "and the ruins and craters")
	check(CityStore.dispatch(&"redo", []), "redone")
	check(CityStore.query(&"damage_probe").buildings == after_big.buildings, "redo repeats the strike")


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
	check(spot != Vector3.INF, "found a point on a road")
	var before: Dictionary = CityStore.query(&"damage_probe")
	var r: Dictionary = CityStore.dispatch(&"strike", [spot, BIG_FORCE])
	var after: Dictionary = CityStore.query(&"damage_probe")
	print("strike on the road: ", r, " ", after)
	check(after.craters == before.craters + 1, "a crater on the road")
	check(after.segments != before.segments or r.segments > 0, "the crater cut the road (%d -> %d stretches)" % [before.segments, after.segments])
	check(after.gone > before.gone, "furniture knocked down (%d)" % (after.gone - before.gone))
	check(CityStore.dispatch(&"undo", []), "undid the strike on the road")
	var undone: Dictionary = CityStore.query(&"damage_probe")
	check(undone.segments == before.segments and undone.gone == before.gone and undone.craters == before.craters, "the road, the furniture and the crater came back")


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
	check(r.ok and CityStore.query(&"damage_probe").ruins > ruins, "a building fell for the fracture")
	await _until(func() -> bool: return CityStore.query(&"effects_ruin_pieces") > before)
	var pieces: int = CityStore.query(&"effects_ruin_pieces")
	print("pieces of the fallen building: ", pieces - before)
	check(pieces - before >= PIECES_LEAST, "the building became pieces (%d)" % (pieces - before))
	await _until(func() -> bool: return CityStore.query(&"effects_ruin_flying") > 0)
	check(CityStore.query(&"effects_ruin_flying") > 0, "the pieces fly")
	await _until(func() -> bool: return CityStore.query(&"effects_ruin_flying") == 0)
	check(CityStore.query(&"effects_ruin_flying") == 0, "the pieces land")
	var seed: int = _a_ruin()
	await _until(func() -> bool: return CityStore.query(&"effects_ruin_digest", [seed]) != 0)
	var lying: int = CityStore.query(&"effects_ruin_digest", [seed])
	check(lying != 0, "the heap settled")
	check(CityStore.dispatch(&"undo", []), "undid the fall")
	await get_tree().process_frame
	await get_tree().process_frame
	check(CityStore.query(&"effects_ruin_pieces") <= before, "undo takes the pieces away")
	check(CityStore.dispatch(&"redo", []), "redid the fall")
	await _until(func() -> bool: return CityStore.query(&"effects_ruin_digest", [seed]) != 0)
	check(CityStore.query(&"effects_ruin_digest", [seed]) == lying, "redo lays the pieces as they fell (%d, %d)" % [lying, CityStore.query(&"effects_ruin_digest", [seed])])


## The seed of the ruin the last strike left.
func _a_ruin() -> int:
	return CityStore.query(&"ruin_seeds")[-1]


## A building's fracture is made by a job on its own thread, in real time whatever the frames' pace (the tests run in simulated time):
## the wait is on the state, its guard real time (StateWait).
func _until(met: Callable) -> void:
	await StateWait.until(get_tree(), met, FALL_LIMIT_S)


## The force goes by ones up to the fine limit and by the coarse step past it, inside the limits of the data.
func _force_steps() -> void:
	var fine := int(DataTables.tuning("strike_force_fine"))
	var coarse := int(DataTables.tuning("strike_force_coarse"))
	var blast: Dictionary = DataTables.catalog_table("blast")
	check(StrikeStrip.next_force(fine - 1, 1) == fine, "force: up by one below the fine limit")
	check(StrikeStrip.next_force(fine, 1) == fine + coarse, "force: up by the coarse step from the fine limit")
	check(StrikeStrip.next_force(fine + coarse, -1) == fine, "force: back down to the fine limit")
	check(StrikeStrip.next_force(fine, -1) == fine - 1, "force: down by one at the fine limit")
	check(StrikeStrip.next_force(int(blast.strength_min), -1) == int(blast.strength_min), "force: stops at the least")
	check(StrikeStrip.next_force(int(blast.strength_max), 1) == int(blast.strength_max), "force: stops at the most")


## A small bomb on a roof breaks part of the building and leaves it standing, drawn in pieces; undo gives it back whole.
func _scar() -> void:
	for f in SETTLE_FRAMES: # what the blows above broke is drawn first
		await get_tree().process_frame
	await _until(func() -> bool: return not CityStore.query(&"damage_probe").effects_busy)
	var bounds: Dictionary = CityStore.query(&"city_bounds")
	var roof := Vector3.INF
	var x: float = bounds.min.x
	while x < bounds.max.x and roof == Vector3.INF:
		var z: float = bounds.min.z
		while z < bounds.max.z and roof == Vector3.INF:
			var hit: Dictionary = CityStore.query(&"body_pick", [Vector3(x, SKY_M, z), Vector3.DOWN])
			if not hit.is_empty() and hit.point.y > float(CityStore.query(&"height_at", [x, z])) + TALL_M:
				roof = hit.point
			z += SCAN_STEP_M
		x += SCAN_STEP_M
	check(roof != Vector3.INF, "found a tall roof")
	var before: Dictionary = CityStore.query(&"damage_probe")
	check(CityStore.dispatch(&"strike", [roof, SMALL_FORCE]).ok, "a small bomb on the roof")
	var after: Dictionary = CityStore.query(&"damage_probe")
	check(after.scars > before.scars and after.buildings == before.buildings, "part of it broke off, and it stands (%d scars)" % after.scars)
	await _until(func() -> bool: return CityStore.query(&"damage_probe").broken > before.broken)
	check(CityStore.query(&"damage_probe").broken > before.broken, "it is drawn in pieces")
	check(CityStore.dispatch(&"undo", []), "undid the bomb on the roof")
	await _until(func() -> bool: return CityStore.query(&"damage_probe").broken == before.broken)
	var undone: Dictionary = CityStore.query(&"damage_probe")
	check(undone.scars == before.scars and undone.broken == before.broken, "undo gave the building back whole")
