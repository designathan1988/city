## The car traffic's cases photographed where the city has them (sim/traffic): zebras put on streets with cars, shot while a car stands
## for somebody at one, and the busiest junction with no signal. Run hidden: godot --path project --resolution 1600x900
## res://tests/visual/capture_traffic_cases.tscn -- --no-flow --capture-dir=<dir>
extends Node3D

const MAP := 0
const SEED := 7
const WARM_TICKS := 1200
const SAMPLE := 200
const ZEBRA := 1 # add_crossing's kind (street_crossing_test)
const ZEBRAS := 4
const NEAR_M := 14.0
const STEP_TICKS := 4
const MOST_STEPS := 3000
## PairCamera views: azimuth, tilt and zoom (the 4.0's pixels a metre) of the zebra and of the junction, and a second angle.
const ZEBRA_VIEW := [35.0, 30.0, 36.0]
const ZEBRA_SIDE := [215.0, 22.0, 30.0]
const JUNCTION_VIEW := [60.0, 35.0, 24.0]
const JUNCTION_SIDE := [240.0, 25.0, 30.0]
const JUNCTION_SHOTS := 3
const JUNCTION_GAP_TICKS := 80
const PRIORITY := 1 # node_traffic control: Control::PRIORITY

var rig: CityCamera
var ui: UiDriver
var world: CityWorld
var shots := 0


func _ready() -> void:
	var out_dir := "user://captures"
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
	DirAccess.make_dir_recursive_absolute(out_dir)
	world = CityWorld.new()
	add_child(world)
	PairCamera.quality(world, get_viewport())
	rig = CityCamera.new()
	add_child(rig)
	if not CityStore.boot(world.terrain, world.materials()).is_empty():
		get_tree().quit(1)
		return
	CityStore.new_map(MAP, SEED, true, true)
	ui = UiDriver.new(rig, get_tree(), out_dir)
	add_child(RoadcraftHud.new())
	UiDriver.at_hour(world, 16.0)
	CityStore.step_ticks(WARM_TICKS)
	await _zebras()
	await _junction()
	print("FAILURES: ", 0 if shots > 0 else 1)
	get_tree().quit(0 if shots > 0 else 1)


func _stand(at: Vector3, view: Array) -> void:
	await PairCamera.stand(rig, ui, at, view[0], view[1], view[2])


func _shot(name: String) -> void:
	await CaptureWait.until_drawn(self, 4, name)
	await ui.shot("%02d-%s" % [shots, name])
	shots += 1


## Zebras mid-street where cars pass; the town runs until a car stands for somebody, then each zebra from two sides.
func _zebras() -> void:
	var made: Array[Vector3] = []
	for p: Vector3 in CityStore.query(&"traffic_probe", [SAMPLE]).sample:
		var seg: Dictionary = CityStore.query(&"segment_at", [p])
		if seg.get("seg", -1) < 0:
			continue
		var at: Vector3 = seg.points[seg.points.size() / 2]
		if CityStore.dispatch(&"add_crossing", [at, ZEBRA]).get("ok", false):
			made.append(at)
		if made.size() == ZEBRAS:
			break
	print("zebras at ", made)
	for i in MOST_STEPS:
		CityStore.step_ticks(STEP_TICKS)
		var w: Dictionary = CityStore.query(&"street_watch", [NEAR_M])
		if w.held > 0 and w.near > 0:
			print("a car stands for somebody after %d ticks: %s" % [i * STEP_TICKS, w])
			break
	for k in made.size():
		await _stand(made[k], ZEBRA_VIEW)
		await _shot("zebra_%d" % k)
		await _stand(made[k], ZEBRA_SIDE)
		await _shot("zebra_%d_side" % k)


## The busiest junction with no signal near the vehicles sampled (a node at an end of their segments, on priority), over a few moments.
func _junction() -> void:
	var best := -1
	var best_at := Vector3.ZERO
	var most := -1
	for p: Vector3 in CityStore.query(&"traffic_probe", [SAMPLE]).sample:
		var seg: Dictionary = CityStore.query(&"segment_at", [p])
		if seg.get("seg", -1) < 0:
			continue
		for end: Vector3 in [seg.points[0], seg.points[seg.points.size() - 1]]:
			var node: Dictionary = CityStore.query(&"inspect", [end])
			if node.get("kind", "") != "node":
				continue
			var traffic: Dictionary = CityStore.query(&"node_traffic", [node.id])
			if traffic.get("control", -1) == PRIORITY and int(traffic.get("volume", 0)) > most:
				most = int(traffic.volume)
				best = node.id
				best_at = end
	print("busiest junction on priority: ", best, " volume ", most, " at ", best_at)
	if best < 0:
		return
	for k in JUNCTION_SHOTS:
		await _stand(best_at, JUNCTION_VIEW)
		await _shot("junction_%d" % k)
		CityStore.step_ticks(JUNCTION_GAP_TICKS)
	await _stand(best_at, JUNCTION_SIDE)
	await _shot("junction_side")
