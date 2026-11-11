## A mesh job copies what it reads of the map under a short lock and builds with the lock let go (native/src/sim/state/city_cut.h):
## no producer builds while its thread holds the map, so a command, a grown building or a save never waits for a mesh. Read on the
## core's count of the vertices and instances builders took under the map (probe map_hold, counted by the tests' DLL), with the
## town streamed in from near, from the street and from far. And the map has one writer at a time, named on every exclusive hold
## (native/src/sim/state/map_hold.h WriterHold): the probe counts the holds by writer.
extends TestCase

const TOWN_MAP := 0
const SEED := 1
## The views, metres from the town's middle at a pitch in degrees: the near levels, the street's props, and the far levels.
const VIEWS := [[30.0, 25.0, 476.0], [120.0, 35.0, 300.0], [210.0, 15.0, 1400.0]]
const SETTLE_LIMIT_S := 50.0 # real time a view may take to be built (3000 frames of the old 60 Hz loop): StateWait
## The producers that build off the lock: each lane (core/job_pool.h job_lane_name) must have built nothing under the map.
const OFF_LOCK := ["building", "building_super", "roads", "roads_far", "verges", "structures", "water", "props", "trees_near", "trees_far", "cover"]
const ROUNDABOUT_M := 20.0 # the radius a roundabout's preview is asked for
const LOT_SIDE := Vector3(20.0, 0.0, 20.0) # the corner a lot's preview is dragged to, from the first


func _ready() -> void:
	var rig := CityCamera.new()
	add_child(rig)
	if not boot():
		finish()
		return
	CityStore.query(&"map_hold", [true])
	new_map(TOWN_MAP, SEED, true)
	var made: Dictionary = CityStore.query(&"map_hold", [true])
	check(made.has("writers"), "the probe counts the map's exclusive holds by writer")
	check(int(made.get("writers", {}).get("generation", 0)) >= 1, "the new map was made under a hold named for its writer (%s)" % made.get("writers", {}))
	for v: Array in VIEWS:
		rig.jump(rig.focus, v[0], v[1], v[2])
		# until roads stand and the stream has nothing left to draw: a first frame has not asked for anything yet
		await StateWait.until(get_tree(), func() -> bool: return int(CityStore.query(&"road_mesh_counts").roads.chunks) > 0 and int(CityStore.query(&"render_pending")) == 0, SETTLE_LIMIT_S)
	var roads := int(CityStore.query(&"road_mesh_counts").roads.chunks)
	check(roads > 0 and int(CityStore.query(&"render_pending")) == 0, "the town is streamed in (%d road chunks, nothing pending)" % roads)
	var seen: Dictionary = CityStore.query(&"map_hold", [false])
	print("built under the map: ", seen.built)
	check(seen.counted, "this DLL counts what is built under the map (the tests' DLL)")
	for lane: String in OFF_LOCK:
		var built := int(seen.built.get(lane, 0))
		check(built == 0, "%s built nothing while it held the map (%d vertices and instances)" % [lane, built])
	_check_writers(rig.focus)
	finish()


## A pointer's previews only read the map (shared: the tick and the mesh jobs go on), an undo writes it as a command, and no
## exclusive hold is counted for no writer.
func _check_writers(at: Vector3) -> void:
	var before := _writers()
	CityStore.query(&"segment_at", [at])
	CityStore.query(&"preview_roundabout", [at, ROUNDABOUT_M, false])
	CityStore.query(&"preview_lot", [at, at + LOT_SIDE])
	var read := _writers()
	check(int(read.get("command", 0)) == int(before.get("command", 0)), "the previews held the map for no command (%s, then %s)" % [before, read])
	CityStore.dispatch(&"undo")
	var wrote := _writers()
	check(int(wrote.get("command", 0)) == int(read.get("command", 0)) + 1, "an undo held the map once, as a command (%s)" % wrote)
	check(not wrote.is_empty() and not wrote.has("none"), "every exclusive hold of the map named its writer (%s)" % wrote)


func _writers() -> Dictionary:
	return CityStore.query(&"map_hold", [false]).get("writers", {})
