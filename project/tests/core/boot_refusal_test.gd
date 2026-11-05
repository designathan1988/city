## A catalog with a key missing is refused by name, and the game stays up behind the refusal: what runs each frame and what the
## script may still ask of the core find no map and answer so.
extends Node

const MISSING_GROUP := "structure"
const MISSING_KEY := "wall_thick_m"
const FRAMES := 30
const ROAD_M := 100.0
const PX := 0.25

var failures := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	var tables: Dictionary = DataTables.catalog().duplicate(true)
	tables.tuning[MISSING_GROUP].erase(MISSING_KEY)
	var core: Node = CityStore.get_child(0)
	var errors: PackedStringArray = core.load_catalog(tables)
	_check(not errors.is_empty() and MISSING_KEY in "\n".join(errors), "the refusal names the key: %s" % [errors])
	var render: PackedStringArray = core.setup_render(DataTables.render(), world.materials())
	_check(not render.is_empty(), "no render without a catalog: %s" % [render])
	for i in FRAMES:
		await get_tree().process_frame
	CityStore.refresh()
	_check(not CityStore.ready_to_read() or CityStore.snapshot.get("segments", 0) == 0, "the snapshot holds no map")
	var report: Dictionary = CityStore.new_map(0, 1, false)
	_check(not report.ok, "no map without a catalog")
	_check(CityStore.query(&"road_profiles").is_empty(), "no roads to offer")
	_check(CityStore.dispatch(&"build_road", [Vector3.ZERO, Vector3.RIGHT * ROAD_M, false, Vector3.ZERO, 0, PX]).is_empty(), "no road is laid")
	_check(not CityStore.query(&"raycast", [Vector3.UP * ROAD_M, Vector3.DOWN]).hit, "a ray meets nothing")
	var scene := {"nodes": [[0, 0], [ROAD_M, 0]], "segs": [[0, 1, "avenue", 0]], "ground": {}}
	_check(CityStore.query(&"grade_probe", [scene]).is_empty(), "the grade probe has no road to lay")
	for i in FRAMES:
		await get_tree().process_frame
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


func _check(ok: bool, what: String) -> void:
	print("ok   " if ok else "FAIL ", what)
	if not ok:
		failures += 1
