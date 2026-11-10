## A mesh job copies what it reads of the map under a short lock and builds with the lock let go (native/src/sim/state/city_cut.h):
## no producer builds while its thread holds the map, so a command, a grown building or a save never waits for a mesh. Read on the
## core's count of the vertices and instances builders took under the map (probe map_hold, counted by the tests' DLL), with the
## town streamed in from near, from the street and from far.
extends Node

const TOWN_MAP := 0
const SEED := 1
## The views, metres from the town's middle at a pitch in degrees: the near levels, the street's props, and the far levels.
const VIEWS := [[30.0, 25.0, 476.0], [120.0, 35.0, 300.0], [210.0, 15.0, 1400.0]]
const SETTLE_FRAMES := 60
const SETTLE_LIMIT := 3000 # frames a view may take to be built
## The producers that build off the lock: each lane (core/job_pool.h job_lane_name) must have built nothing under the map. The
## others are named as they are converted (log/performance.md C9); what they still build under it is printed.
const OFF_LOCK := ["building", "building_super"]


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	var rig := CityCamera.new()
	add_child(rig)
	if not CityStore.boot(world.terrain, world.materials()).is_empty():
		get_tree().quit(1)
		return
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.query(&"map_hold", [true])
	for v: Array in VIEWS:
		rig.jump(rig.focus, v[0], v[1], v[2])
		await _settled()
	var failures := 0
	var roads := int(CityStore.query(&"road_mesh_counts").roads.chunks)
	failures += _check(roads > 0 and int(CityStore.query(&"render_pending")) == 0, "the town is streamed in (%d road chunks, nothing pending)" % roads)
	var seen: Dictionary = CityStore.query(&"map_hold", [false])
	print("built under the map: ", seen.built)
	failures += _check(seen.counted, "this DLL counts what is built under the map (the tests' DLL)")
	for lane: String in OFF_LOCK:
		var built := int(seen.built.get(lane, 0))
		failures += _check(built == 0, "%s built nothing while it held the map (%d vertices and instances)" % [lane, built])
	print("FAILURES: ", failures)
	get_tree().quit(0 if failures == 0 else 1)


## Until roads stand and the stream has nothing left to draw: a first frame has not asked for anything yet.
func _settled() -> void:
	for i in SETTLE_FRAMES:
		await get_tree().process_frame
	var left := SETTLE_LIMIT
	while left > 0 and (int(CityStore.query(&"road_mesh_counts").roads.chunks) == 0 or int(CityStore.query(&"render_pending")) > 0):
		left -= 1
		await get_tree().process_frame


func _check(good: bool, what: String) -> int:
	print(("ok   " if good else "FAIL ") + what)
	return 0 if good else 1
