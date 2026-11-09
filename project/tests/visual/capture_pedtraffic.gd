## The walkers' traffic seen working (crossings/permission.ts, sim/agents/walk.ts): people crossing a zebra between junctions
## when the traffic leaves them a gap, people waiting at and crossing a lit junction on the walk, the busiest sidewalk, and
## people fleeing a shot. Each from several angles; the counts that say it worked are printed and checked.
## Run: godot --path project --resolution 1920x1080 res://tests/visual/capture_pedtraffic.tscn -- --no-flow --capture-dir=<dir>
extends Node3D

const TOWN_MAP := 0
const SEED := 7
const HOUR := 16.0
const NIGHT_HOUR := 21.0
const WARM_TICKS := 2400 # traffic and people fill the streets (20 ticks a second)
const SAMPLE := 200
const ZEBRA := 1 # add_crossing's kind: a zebra with no light
const ZEBRAS := 4
const WATCH_M := 9.0 # walkers this near a zebra's middle are at it
const WATCH_TICKS := 10
const WATCH_TRIES := 400
const SIGNAL_TICKS := 160 # a part of a signal cycle between two photos of the lit junction
const SIGNAL_SHOTS := 3
const CROWD_READ := 4096
const CROWD_M := 12.0 # the neighbours that make a sidewalk busy
const SHOT_FROM := Vector3(2, 40, 2) # straight down, beside the crowd: the round lands on the ground by them
const FLEE_M := 25.0 # walkers this near the shot before and after it
const FLEE_TICKS := [6, 20, 40, 80]
const SETTLE_FRAMES := 4
const JUMP_FRAMES := 30
const FOV := 50.0
const NEAR_M := 0.3
# Camera offsets from what is looked at: high oblique, low oblique from the other side, and along the street at eye height.
const HIGH := Vector3(-9, 16, 12)
const LOW := Vector3(10, 5, -8)
const EYE := Vector3(14, 1.8, 3)
const LOOK_UP_M := 0.9

var out_dir := "user://captures"
var world: CityWorld
var camera: Camera3D
var failures := 0


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
	DirAccess.make_dir_recursive_absolute(out_dir)
	world = CityWorld.new()
	add_child(world)
	camera = Camera3D.new()
	camera.fov = FOV
	camera.near = NEAR_M
	add_child(camera)
	var errors := CityStore.boot(world.terrain, world.materials())
	if not errors.is_empty():
		push_error("boot: %s" % errors)
		get_tree().quit(1)
		return
	Settings.apply(world, get_viewport())
	CityStore.new_map(TOWN_MAP, SEED, true, true)
	UiDriver.at_hour(world, HOUR)
	CityStore.step_ticks(WARM_TICKS)
	await _zebra()
	await _signal()
	await _sidewalk()
	await _flee()
	print("FAILURES: ", failures)
	get_tree().quit(CaptureWait.exit_code(failures))


func _check(ok: bool, what: String) -> void:
	print(("ok " if ok else "FAIL ") + what)
	failures += 0 if ok else 1


func _shot(name: String, from: Vector3, at: Vector3) -> void:
	var settle := JUMP_FRAMES if camera.global_position.distance_to(from) > CROWD_M else SETTLE_FRAMES
	camera.global_position = from
	camera.look_at(at)
	await CaptureWait.until_drawn(self, settle, name)
	var path := "%s/%s.png" % [out_dir, name]
	get_viewport().get_texture().get_image().save_png(path)
	print("capture: ", path)


func _views(name: String, at: Vector3) -> void:
	var target := at + Vector3.UP * LOOK_UP_M
	await _shot(name + "_high", at + HIGH, target)
	await _shot(name + "_low", at + LOW, target)
	await _shot(name + "_eye", at + EYE, target)


## Zebras put in the middle of busy streets; the first one somebody walks over is photographed while they are on it.
func _zebra() -> void:
	var made: Array[Vector3] = []
	for p: Vector3 in CityStore.query(&"traffic_probe", [SAMPLE]).sample:
		var seg: Dictionary = CityStore.query(&"segment_at", [p])
		if seg.get("seg", -1) < 0:
			continue
		var mid: Vector3 = seg.points[seg.points.size() / 2]
		if CityStore.dispatch(&"add_crossing", [mid, ZEBRA]).get("ok", false):
			made.append(mid)
		if made.size() == ZEBRAS:
			break
	_check(not made.is_empty(), "zebras put on busy streets: %d" % made.size())
	for i in WATCH_TRIES:
		CityStore.step_ticks(WATCH_TICKS)
		for at in made:
			var census: Dictionary = CityStore.query(&"people_census", [at, WATCH_M])
			if census.mid_crossing > 0:
				print("zebra at ", at, ": ", census.mid_crossing, " crossing, ", census.mid_waiting, " at the kerb")
				await _views("zebra", at)
				return
	_check(false, "somebody crossed a zebra")


## The lit junction with the most people waiting or crossing, at moments of its cycle apart; once at night.
func _signal() -> void:
	var probe: Dictionary = CityStore.query(&"traffic_probe", [SAMPLE])
	var at: Vector3 = probe.busy_crossing
	_check(probe.signals > 0, "lit junctions: %d" % probe.signals)
	for k in SIGNAL_SHOTS:
		var census: Dictionary = CityStore.query(&"people_census", [at, 2 * WATCH_M])
		print("lit junction ", at, ": ", census.modes)
		await _views("signal_%d" % k, at)
		CityStore.step_ticks(SIGNAL_TICKS)
	UiDriver.at_hour(world, NIGHT_HOUR)
	await _shot("signal_night", at + HIGH, at + Vector3.UP * LOOK_UP_M)
	UiDriver.at_hour(world, HOUR)


## The walker with the most others round it: the busiest stretch of sidewalk.
func _densest() -> Vector3:
	var walkers: Array = CityStore.query(&"shot_targets", [CROWD_READ]).walkers
	var best := Vector3.INF
	var most := -1
	for a: Dictionary in walkers:
		var n := 0
		for b: Dictionary in walkers:
			n += 1 if (a.position as Vector3).distance_to(b.position) < CROWD_M else 0
		if n > most:
			most = n
			best = a.position
	print("densest: ", most, " within ", CROWD_M, " m of ", best)
	return best


func _sidewalk() -> void:
	var at := _densest()
	_check(at != Vector3.INF, "people on the sidewalks")
	if at != Vector3.INF:
		await _views("sidewalk", at)


func _near(at: Vector3) -> int:
	return int(CityStore.query(&"people_census", [at, FLEE_M]).total)


## A round into the ground by the busiest crowd: who hears it looks, some crouch, and they run away from it.
func _flee() -> void:
	var at := _densest()
	if at == Vector3.INF:
		_check(false, "a crowd to shoot by")
		return
	var before := _near(at)
	await _shot("flee_0_before", at + HIGH, at)
	print("shot: ", CityStore.dispatch(&"shoot", [at + SHOT_FROM, Vector3.DOWN]))
	var done := 0
	for k in FLEE_TICKS.size():
		CityStore.step_ticks(int(FLEE_TICKS[k]) - done)
		done = int(FLEE_TICKS[k])
		await _shot("flee_%d_high" % (k + 1), at + HIGH, at)
		await _shot("flee_%d_low" % (k + 1), at + LOW, at + Vector3.UP * LOOK_UP_M)
	var after := _near(at)
	print("walkers within ", FLEE_M, " m: ", before, " before the shot, ", after, " after")
	_check(before > 0 and after < before, "they fled the shot (%d -> %d)" % [before, after])
