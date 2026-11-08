## Core test of the pistol and of what a blow leaves in a save (no rendering needed): a round meets a walker where it is
## aimed (the head kills, the trunk costs a third of the health, the wound slows), a miss hits nothing, a car hit through
## its glass stops and rounds burst it; the wounded bleed; then a city with a crater, a ruin and furniture gone is saved,
## a new map replaces it and opening the file brings the same marks back.
## Run: godot --headless --path project res://tests/core/shoot_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 7
const OTHER_SEED := 11
const WARM_TICKS := 900
const LIMIT := 40
const HEAD_AIM_M := 1.7
const ALONE_M := 6.0
const LEVEL_STANDOFF_M := 4.0 # a level round, from the side: it crosses the walker at the height it was aimed at
const LIE_AIM_M := 0.15 # a walker knocked down by the first round lies on the ground: the next is aimed at the body, not at where the chest was
const TRUNK_AIM_M := 1.2
const STANDOFF := Vector3(0.0, 6.0, 12.0)
const SKY := Vector3(0.0, 1.0, 0.0)
const SKY_ORIGIN := Vector3(0.0, 300.0, 0.0)
const BLEED_SOON_TICKS := 60
const BLEED_TICKS := 2400
const SAME_SPOT_M := 0.5
const GLASS_AIM_M := 1.2
const BIG_FORCE := 45
const BOX_HALF_M := 120.0
const ROOF_M := 6.0
const BODY_FRAMES := 1200
const BODY_SETTLE_FRAMES := 30
const RISE_FORCE := 3
const RISE_KPA := 130.0
const RISE_TRIES := 8
const RISE_LOOK_FRAMES := 20
const PAUSED_FRAMES := 60 # frames drawn with the clock stopped: a body knocked down keeps every second it has left
const RISE_SPARE_TICKS := 2 # past the longest lie: the publication that holds the walker standing again
const SLOT := "shoot_test_slot"


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	ticks(WARM_TICKS)
	var fright := ShootFrightCases.new(self, _shoot, _walker_of)
	fright.reactions()
	fright.stampede()
	_walkers()
	await _fallen()
	_vehicles()
	_demolish()
	await _save_marks()
	finish()


## A round at `at` from above and behind it: the origin is the point plus STANDOFF, the direction through the point.
func _shoot(at: Vector3) -> Dictionary:
	var origin := at + STANDOFF
	return CityStore.dispatch(&"shoot", [origin, (at - origin).normalized()])


## A round at `at` along the ground, from LEVEL_STANDOFF_M to the east: the part struck is the one at that height.
func _shoot_level(at: Vector3) -> Dictionary:
	var origin := at + Vector3(LEVEL_STANDOFF_M, 0, 0)
	return CityStore.dispatch(&"shoot", [origin, Vector3.LEFT])


func _walker_of(id: int) -> Dictionary:
	for w in CityStore.query(&"shot_targets", [LIMIT * 4]).walkers:
		if w.id == id:
			return w
	return {}


## The walkers with nobody else within ALONE_M: a round that passes close to another walker may hit that one instead.
func _alone(all: Array) -> Array:
	return all.filter(func(w: Dictionary) -> bool: return all.all(func(o: Dictionary) -> bool: return o.id == w.id or o.position.distance_to(w.position) > ALONE_M))


func _walkers() -> void:
	var found: Dictionary = CityStore.query(&"shot_targets", [LIMIT])
	var walkers := _alone(found.walkers)
	check(walkers.size() >= 3, "there are lone pedestrians for the shot (%d of %d)" % [walkers.size(), found.walkers.size()])
	if walkers.size() < 3:
		return
	var hp_full: float = walkers[0].hp
	check(hp_full > 0.0, "the pedestrian starts alive (%.0f)" % hp_full)
	var trunk: Dictionary = walkers[0]
	var r := _shoot(trunk.position + Vector3(0, TRUNK_AIM_M * trunk.scale, 0))
	check(r.get("hit", 0) == 1 and not r.killed, "a shot in the trunk hits and does not kill %s" % [r])
	var after := _walker_of(trunk.id)
	check(not after.is_empty() and after.hp < hp_full, "the life fell (%.0f -> %.0f)" % [hp_full, after.get("hp", -1.0)])
	var head: Dictionary = walkers[1]
	var kill := _shoot(head.position + Vector3(0, HEAD_AIM_M * head.scale, 0))
	check(kill.get("hit", 0) == 1 and kill.killed, "a shot in the head kills %s" % [kill])
	check(_walker_of(head.id).is_empty(), "who died left the pavement")
	var sky: Dictionary = CityStore.dispatch(&"shoot", [SKY_ORIGIN, SKY])
	check(sky.get("hit", 1) == 0, "a shot at the sky hits nothing")
	var hurt: Dictionary = walkers[2]
	var rounds := []
	for k in 2:
		var w := _walker_of(hurt.id)
		if not w.is_empty():
			rounds.append([w.hp, w.fallen, _shoot_level(w.position + Vector3(0, (LIE_AIM_M if w.fallen else TRUNK_AIM_M * w.scale), 0))])
	var wounded := _walker_of(hurt.id)
	check(wounded.is_empty() or wounded.hp < hp_full * 0.5, "two shots in the trunk wound gravely %s -> %s" % [rounds, wounded])
	if not wounded.is_empty() and wounded.hp > 0.0 and wounded.bleed > 0.0:
		ticks(BLEED_SOON_TICKS)
		var soon := _walker_of(hurt.id)
		check(not soon.is_empty() and soon.hp < wounded.hp, "the wounded bleeds (%.0f -> %.0f)" % [wounded.hp, soon.get("hp", 0.0)])
		ticks(BLEED_TICKS)
		var later := _walker_of(hurt.id)
		check(later.is_empty() or later.position.distance_to(wounded.position) > SAME_SPOT_M, "and dies of the bleeding (the slot went to another)")


## Somebody shot lies down as a body of their own, which comes to rest.
func _fallen() -> void:
	for f in BODY_SETTLE_FRAMES: # the blows above come and go first: their undo starts the effects over
		await get_tree().process_frame
	var walkers := _alone(CityStore.query(&"shot_targets", [LIMIT]).walkers)
	check(not walkers.is_empty(), "someone for the body shot")
	var kill := _shoot(walkers[0].position + Vector3(0, HEAD_AIM_M * walkers[0].scale, 0))
	print("body shot: ", kill)
	for f in BODY_FRAMES:
		if CityStore.query(&"effects_bodies") > 0:
			break
		await get_tree().process_frame
	check(CityStore.query(&"effects_bodies") > 0, "who fell is a body")
	for f in BODY_FRAMES:
		if CityStore.query(&"effects_bodies_moving") == 0:
			break
		await get_tree().process_frame
	check(CityStore.query(&"effects_bodies_moving") == 0, "the body lands")
	await _rises()


## Somebody knocked over by a small blast gets up when the simulation stands them again: with the clock stopped the body lies on,
## losing none of its seconds; the ticks of the longest lie bring the walker back and the body goes. Far enough off that nobody
## dies, and a drum goes one time in two: several tries.
func _rises() -> void:
	var off := 1.0
	while CityStore.query(&"blast_probe", [float(RISE_FORCE), off]).pressure_kpa > RISE_KPA:
		off += 0.5
	var people: Dictionary = DataTables.catalog_table("blast").people
	var lie_ticks := ceili((float(people.knock_min_s) + float(people.knock_span_s)) * float(DataTables.tuning("sim_tick_hz"))) + RISE_SPARE_TICKS
	var risen := 0
	var found: Array = _alone(CityStore.query(&"shot_targets", [LIMIT]).walkers)
	for who: Dictionary in found.slice(0, RISE_TRIES):
		var before: int = CityStore.query(&"effects_bodies")
		CityStore.dispatch(&"strike", [who.position + Vector3(off, 0, 0), RISE_FORCE])
		var most := before
		for f in RISE_LOOK_FRAMES:
			await get_tree().process_frame
			most = maxi(most, CityStore.query(&"effects_bodies"))
		var left: float = CityStore.query(&"damage_probe").bodies_rising_left
		if most > before and left > 0.0: # somebody only knocked down (a body that gets up): the others of a try stay down
			for f in PAUSED_FRAMES:
				await get_tree().process_frame
			var still: Dictionary = CityStore.query(&"damage_probe")
			check(still.bodies_rising_left == left and still.bodies == most, "paused, who fell lies on (%.2f s left, then %.2f)" % [left, still.bodies_rising_left])
			ticks(lie_ticks)
			for f in RISE_LOOK_FRAMES:
				await get_tree().process_frame
			risen += most - CityStore.query(&"effects_bodies")
		CityStore.dispatch(&"undo")
		if risen > 0:
			break
	print("on their feet again: ", risen)
	check(risen > 0, "who fell gets up")


func _vehicles() -> void:
	var found: Dictionary = CityStore.query(&"shot_targets", [LIMIT])
	var car: Dictionary = {}
	for v in found.vehicles:
		if v.type != CityStore.code(&"VehicleType", &"MOTO") and v.type < CityStore.code(&"VehicleType", &"BUS"):
			car = v
			break
	check(not car.is_empty(), "there is a car for the shot")
	if car.is_empty():
		return
	var before: int = CityStore.query(&"damage_probe").vehicles
	var burst := false
	var shots := 0
	while shots < int(DataTables.catalog_table("blast").shot.vehicle_shots_to_burst) and not burst:
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
			check(r.get("hit", 0) == 2, "the shot hits the car %s" % [r])
	check(burst or CityStore.query(&"damage_probe").vehicles < before, "bursts make the car explode (%d shots)" % shots)


## A blow leaves rubble; a drag over it clears the rubble, the buildings and the roads in the box in one step, and Undo
## brings every one of them back.
func _demolish() -> void:
	var spot := _house()
	check(spot != Vector3.INF, "a building to demolish")
	CityStore.dispatch(&"strike", [spot + Vector3(0, ROOF_M, 0), BIG_FORCE])
	var before: Dictionary = CityStore.query(&"damage_probe")
	check(before.ruins > 0, "the strike left rubble (%d)" % before.ruins)
	var corner := Vector3(BOX_HALF_M, 0, BOX_HALF_M)
	var r: Dictionary = CityStore.dispatch(&"demolish_box", [spot - corner, spot + corner])
	var after: Dictionary = CityStore.query(&"damage_probe")
	check(r.get("ok", false) and after.buildings < before.buildings, "the box demolished buildings (%d -> %d)" % [before.buildings, after.buildings])
	check(after.ruins == 0 and after.segments < before.segments, "and the rubble and the roads inside it (%d ruins, %d stretches)" % [after.ruins, after.segments])
	check(CityStore.dispatch(&"undo", []), "undone")
	var undone: Dictionary = CityStore.query(&"damage_probe")
	check(undone.buildings == before.buildings and undone.ruins == before.ruins and undone.segments == before.segments, "Undo gave back all of the box")


func _house() -> Vector3:
	for id in CityStore.query(&"list_buildings"):
		var info: Dictionary = CityStore.query(&"probe_building", [id])
		if info.floors >= 2 and info.zone > 0:
			return info.position
	return Vector3.INF


func _save_marks() -> void:
	var spot := _house()
	check(spot != Vector3.INF, "a building for the bomb")
	CityStore.dispatch(&"strike", [spot, BIG_FORCE])
	var marks: Dictionary = CityStore.query(&"damage_probe")
	check(marks.craters > 0 and marks.ruins > 0, "the bomb left a crater and a ruin %s" % [marks])
	var w: Dictionary = await save(SLOT)
	check(w.get("ok", false), "saved %s" % w)
	new_map(TOWN_MAP, OTHER_SEED, false)
	check(CityStore.query(&"damage_probe").craters == 0, "a new map does not inherit the marks")
	var r: Dictionary = await CityStore.saves.load_game(SLOT, true)
	check(r.ok, "opened %s" % r)
	var got: Dictionary = CityStore.query(&"damage_probe")
	for key in ["craters", "ruins", "gone", "hurt", "strikes"]:
		check(got[key] == marks[key], "%s comes back the same (%d)" % [key, marks[key]])
	DirAccess.remove_absolute(CityStore.saves.save_path(SLOT))
