## A bad data table is refused by the path to the bad value, one case for each refusal of the strict reader
## (native/src/render/data_reader.h) in each table, and the game stays up behind the refusal: what runs each frame and what the
## script may still ask of the core find no map and answer so.
extends TestCase

const FRAMES := 30
const ROAD_M := 100.0
const PX := 0.25
const ROADCRAFT := ["styles", "roadcraft"]
## [table, path to the value, what is written there, a piece of the expected error]. The catalogue's run first, while the core
## has never held one, and the game is checked standing behind them; the render's after, against a good catalogue (ERASED is
## the missing-key case).
const RENDER_CASES := [
	["buildings", ROADCRAFT + ["architectures", 0, "opening"], [0.4, 0.4, 0.6, 0.6], "opening: expected 5 items, found 4"],
	["models", ROADCRAFT + ["part_sizes", 0, 0], "sofa", "unknown part 'sofa'"],
	["interiors", ROADCRAFT + ["sizes", 0, 0], "hammock", "unknown piece 'hammock'"],
	["transit", ["station", 0, "part"], "kiosk", "unknown part 'kiosk'"],
	["effects", ["bursts", 0, "count"], "many", "count: expected float, found String"],
]
const CATALOG_CASES := [
	["traffic", ["classes", 0], [2.4, 2.9, 6.0, 1.2, 2.0, 1.0, 4.4], "classes[0]: expected 8 items, found 7"],
	["road_profiles", ["profiles", 0, "strips", 0, "kind"], "moat", "unknown strip kind 'moat'"],
	["zones", ["zones", 0, "color"], "#88888", "color: expected '#rrggbb', found '#88888'"],
	["growth", ["plan", "centres"], "two", "centres: expected float, found String"],
	["blast", ["pressure_curves", 0, "c"], [7.2, -2.1, -0.3], "c: expected 7 items, found 3"],
	["nature", ["paint", "kinds", 0, "class"], "lava", "unknown class 'lava'"],
	["street_furniture", ["footprints", "signal_post"], [0.24, 0.24, 0.24], "signal_post: expected 2 items, found 3"],
	["maps", ["maps", 0, "tiles"], 20.5, "tiles: expected a whole number, found 20.5"],
	["scenic", ["cities", 0, "districts", 0, "local"], "goatPath", "unknown profile 'goatPath'"],
]
const ERASED := ["tuning", ["structure"], "wall_thick_m", "missing 'wall_thick_m'"]


func _ready() -> void:
	world = CityWorld.new()
	add_child(world)
	var catalog: Dictionary = DataTables.catalog().duplicate(true)
	var render: Dictionary = DataTables.render().duplicate(true)
	for case in CATALOG_CASES:
		_refused(case, _changed(catalog, case), render)
	var erased: Dictionary = catalog.duplicate(true)
	_at(erased, [ERASED[0]] + ERASED[1]).erase(ERASED[2])
	_refused(ERASED, erased, render)
	await _standing()
	for case in RENDER_CASES:
		_refused(case, catalog, _changed(render, case))
	finish()


## What runs each frame and what the script may still ask of a core that never took a catalogue.
func _standing() -> void:
	for i in FRAMES:
		await get_tree().process_frame
	CityStore.refresh()
	check(not CityStore.ready_to_read() or CityStore.snapshot.get("segments", 0) == 0, "the snapshot holds no map")
	var report: Dictionary = new_map(0, 1, false)
	check(not report.ok, "no map without a catalog")
	check(CityStore.query(&"road_profiles").is_empty(), "no roads to offer")
	check(CityStore.dispatch(&"build_road", [Vector3.ZERO, Vector3.RIGHT * ROAD_M, false, Vector3.ZERO, 0, PX]).is_empty(), "no road is laid")
	check(not CityStore.query(&"raycast", [Vector3.UP * ROAD_M, Vector3.DOWN]).hit, "a ray meets nothing")
	var scene := {"nodes": [[0, 0], [ROAD_M, 0]], "segs": [[0, 1, "avenue", 0]], "ground": {}}
	check(CityStore.query(&"grade_probe", [scene]).is_empty(), "the grade probe has no road to lay")
	for i in FRAMES:
		await get_tree().process_frame


## The boot with these tables answers with the case's error, the table's file named.
func _refused(case: Array, catalog: Dictionary, render: Dictionary) -> void:
	var errors: PackedStringArray = CityStore.boot(world.terrain, world.materials(), catalog, render)
	var all := "\n".join(errors)
	check("%s.json" % case[0] in all and case[3] in all, "%s %s refused by name: %s" % [case[0], case[1], errors])


## A copy of `tables` with the case's value written at its path.
func _changed(tables: Dictionary, case: Array) -> Dictionary:
	var out: Dictionary = tables.duplicate(true)
	var path: Array = [case[0]] + case[1]
	_at(out, path.slice(0, path.size() - 1))[path[-1]] = case[2]
	return out


## The container at `path` (dictionary keys and list indices).
func _at(root: Variant, path: Array) -> Variant:
	var node: Variant = root
	for step in path:
		node = node[step]
	return node
