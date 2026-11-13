## A catalog with a key missing is refused by name, and the game stays up behind the refusal: what runs each frame and what the
## script may still ask of the core find no map and answer so.
extends TestCase

const MISSING_GROUP := "structure"
const MISSING_KEY := "wall_thick_m"
const FRAMES := 30
const ROAD_M := 100.0
const PX := 0.25


func _ready() -> void:
	world = CityWorld.new()
	add_child(world)
	var tables: Dictionary = DataTables.catalog().duplicate(true)
	tables.tuning[MISSING_GROUP].erase(MISSING_KEY)
	var errors: PackedStringArray = CityStore.boot(world.terrain, world.materials(), tables)
	check(not errors.is_empty() and MISSING_KEY in "\n".join(errors), "the refusal names the key: %s" % [errors])
	for i in FRAMES:
		await get_tree().process_frame
	CityStore.refresh()
	check(not CityStore.ready_to_read() or CityStore.snapshot.get("segments", 0) == 0, "the snapshot holds no map")
	var report: Dictionary = new_map(0, 1, false)
	check(not report.ok, "no map without a catalog")
	check(CityStore.query(&"road_profiles").is_empty(), "no roads to offer")
	var laid: Dictionary = CityStore.dispatch(&"build_road", [Vector3.ZERO, Vector3.RIGHT * ROAD_M, false, Vector3.ZERO, 0, PX])
	check(not laid.get("ok", true) and laid.get("error", "") == "hint.noCity", "no road is laid, and the refusal says why: %s" % [laid])
	check(not CityStore.query(&"raycast", [Vector3.UP * ROAD_M, Vector3.DOWN]).hit, "a ray meets nothing")
	var scene := {"nodes": [[0, 0], [ROAD_M, 0]], "segs": [[0, 1, "avenue", 0]], "ground": {}}
	check(CityStore.query(&"grade_probe", [scene]).is_empty(), "the grade probe has no road to lay")
	for i in FRAMES:
		await get_tree().process_frame
	finish()
