## Captures of what the weapons leave that the demolition captures do not show, in phases picked by --phase=: `poles` (a line of the
## player's poles by a road past the town, a bomb among them: the poles felled, the torn wires swinging and lying), `trees` (a bomb in
## the woods past the town: the trees felled), `writhe` (a walker shot in both legs: down for good, writhing), `gore` (a walker shot
## three times in the trunk: opened, the guts out). Each is photographed before and at instants of the effects' own clock; the
## frames are named by the instant. Exit code: the failures (a check that did not hold).
## Run as a hidden task of tools/tasks (cli.py serve): godot --path project --resolution 1600x900 res://tests/visual/capture_weapons.tscn
##   -- --no-flow --phase=poles,trees,writhe,gore --capture-dir=<dir>
extends Node3D

const TOWN_MAP := 0
const SEED := 7
const WARM_TICKS := 600
const SETTLE_FRAMES := 4
const OUT_M := 300.0 # the road of poles this far past the town's edge: bare land
const ROAD_HALF_M := 120.0 # half the road's length
const POLE_SIDE_M := 6.0 # the pole run's points this far off the road's axis: on its footway
const POLE_FORCE := 12
const WOODS_OUT_M := 500.0 # the first stretch past the town is forest (capture_blast.gd EARTH_OUT_M)
const TREE_FORCE := 25
const VIEW_YAW := 25.0
const VIEW_PITCH := 35.0
const POLE_DIST := 45.0
const WOODS_DIST := 110.0
const WALKER_DIST := 7.0
const WALKER_PITCH := 30.0
const BODY_DIST := 3.5 # the body on the ground, photographed this close
const BODY_PITCH := 50.0
const BODY_YAW := 85.0 # from the side of the walk: across the body as it lies
const LEG_M := 0.4
const CHEST_M := 1.2
const LYING_M := 0.25 # a body on the ground: its trunk this high over the walker's feet
const TARGETS := 80
const LEG_SHOTS := 2 # two rounds in a leg take it off: down for good (blast.json shot.sever_hits_hp)
const TRUNK_SHOTS := 3 # three rounds in the trunk open it (blast.json shot.open_hits)
const BLAST_MOMENTS := [0.4, 1.2, 2.5, 6.0]
const BODY_MOMENTS := [1.0, 3.0, 6.0, 9.0]

var out_dir := "user://captures"
var phases := ["poles"]
var rig: CityCamera
var ui: UiDriver
var failures := 0


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
		elif arg.begins_with("--phase="):
			phases = arg.trim_prefix("--phase=").split(",")
	DirAccess.make_dir_recursive_absolute(out_dir)
	var world := CityWorld.new()
	add_child(world)
	Settings.apply(world, get_viewport()) # the game's own quality preset (MSAA, shadows), as game/play.gd
	rig = CityCamera.new()
	add_child(rig)
	var errors := CityStore.boot(world.terrain, world.materials())
	if not errors.is_empty():
		push_error("boot: %s" % errors)
		get_tree().quit(1)
		return
	CityStore.new_map(TOWN_MAP, SEED, true, true)
	CityStore.step_ticks(WARM_TICKS)
	ui = UiDriver.new(rig, get_tree(), out_dir)
	if "poles" in phases:
		await _poles()
	if "trees" in phases:
		await _trees()
	if "writhe" in phases:
		await _shoot("writhe", LEG_M, LEG_SHOTS)
	if "gore" in phases:
		await _shoot("gore", CHEST_M, TRUNK_SHOTS)
	print("FAILURES: ", failures)
	get_tree().quit(CaptureWait.exit_code(failures))


## A road past the town with a line of poles along it, and a bomb among them.
func _poles() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var x: float = b.max.x + OUT_M
	var z: float = (b.min.z + b.max.z) * 0.5
	var road: Dictionary = CityStore.dispatch(&"build_road", [ui.ground(x, z - ROAD_HALF_M), ui.ground(x, z + ROAD_HALF_M), false, Vector3.ZERO, 0, 0.25])
	_check(road.get("ok", false), "poles: the road %s" % road)
	var run: Dictionary = CityStore.dispatch(&"build_pole_run", [ui.ground(x + POLE_SIDE_M, z - ROAD_HALF_M), ui.ground(x + POLE_SIDE_M, z + ROAD_HALF_M), 0.0, 1, 0])
	_check(run.get("ok", false), "poles: the run %s" % run)
	await ui.frames(SETTLE_FRAMES)
	var at := ui.ground(x + POLE_SIDE_M, z)
	await _blow("poles", at, POLE_FORCE, POLE_DIST)
	rig.jump(at, VIEW_YAW + 90.0, VIEW_PITCH - 15.0, POLE_DIST * 0.6)
	await ui.drawn()
	await ui.shot("poles_after_side")


## A bomb in the woods past the town.
func _trees() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var at := ui.ground(b.max.x + WOODS_OUT_M, (b.min.z + b.max.z) * 0.5)
	await _blow("trees", at, TREE_FORCE, WOODS_DIST)
	rig.jump(at, VIEW_YAW + 120.0, VIEW_PITCH, WOODS_DIST * 0.6)
	await ui.drawn()
	await ui.shot("trees_after_close")


## One bomb at `at`, photographed before and at the instants of the effects' own clock.
func _blow(label: String, at: Vector3, force: int, dist: float) -> void:
	rig.jump(at, VIEW_YAW, VIEW_PITCH, dist)
	await ui.drawn()
	await ui.shot("%s_0_before" % label)
	var since: float = CityStore.query(&"effects_since_blast")
	var r: Dictionary = CityStore.dispatch(&"strike", [at, force])
	_check(r.get("ok", false), "%s: the blow" % label)
	while true: # the clock restarts when the effects play this blow (a frame later)
		var now: float = CityStore.query(&"effects_since_blast")
		if now >= 0.0 and (since < 0.0 or now < since):
			break
		await ui.frames(1)
	for t in BLAST_MOMENTS:
		while CityStore.query(&"effects_since_blast") < t:
			await ui.frames(1)
		await ui.shot("%s_%04dms" % [label, roundi(CityStore.query(&"effects_since_blast") * 1000.0)], 0)


## A walker shot `shots` times at `aim_m` over its feet, then photographed as its body lies.
func _shoot(label: String, aim_m: float, shots: int) -> void:
	CityStore.dispatch(&"set_speed", [0]) # nobody moves while the camera comes and aims
	var walkers: Array = CityStore.query(&"shot_targets", [TARGETS]).walkers.filter(func(w): return not w.fallen)
	if walkers.is_empty():
		_check(false, "%s: a walker" % label)
		return
	var w: Dictionary = walkers[0]
	rig.jump(w.position, VIEW_YAW, WALKER_PITCH, WALKER_DIST)
	await ui.drawn()
	await ui.shot("%s_before" % label)
	for k in shots:
		var now := _walker(w.id) # a round may have knocked it down: the next is aimed at where it lies
		if now.is_empty():
			break
		var origin := rig.camera.global_position
		var aim: Vector3 = now.position + Vector3(0, (LYING_M if now.fallen else aim_m) * now.scale, 0)
		var r: Dictionary = CityStore.dispatch(&"shoot", [origin, (aim - origin).normalized()])
		_check(r.get("hit", 0) == 1, "%s: round %d struck %s" % [label, k, r])
	CityStore.dispatch(&"set_speed", [CityStore.code(&"SimSpeed", &"NORMAL")])
	rig.jump(w.position, BODY_YAW, BODY_PITCH, BODY_DIST) # close over where the body falls
	var elapsed := 0.0
	for t in BODY_MOMENTS:
		await get_tree().create_timer(maxf(t - elapsed, 0.05)).timeout
		elapsed = t
		await ui.drawn()
		await ui.shot("%s_t%02d" % [label, int(t)])


## The walker `id` as the core sees it now (the first TARGETS walkers, the ones the target was drawn from), or nothing.
func _walker(id: int) -> Dictionary:
	for w in CityStore.query(&"shot_targets", [TARGETS]).walkers:
		if w.id == id:
			return w
	return {}


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	if not ok:
		failures += 1
