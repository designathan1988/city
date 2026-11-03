## CityStore: the only owner of the C++ core (CityCore) on the script side.
## State flows one way: commands go in through dispatch(); the UI reads `snapshot` (refreshed from the core)
## and reacts to `changed`. Nothing else keeps copies of game numbers (gate ESTADO_NA_UI / owners.json).
extends Node

signal changed(snapshot: Dictionary)
signal command_failed(error_key: String)

const SNAPSHOT_INTERVAL_S := 0.25
## Commands the UI/tools may send, with the CityCore method that executes each one.
const COMMANDS := {
	&"set_speed": "set_speed", &"build_road": "build_road", &"demolish": "demolish", &"paint_zone": "paint_zone",
	&"preview_road": "preview_road", &"undo": "undo", &"redo": "redo", &"begin_stroke": "begin_stroke",
	&"end_stroke": "end_stroke", &"split_road": "split_road", &"upgrade_road": "upgrade_road",
	&"roundabout": "roundabout", &"terrain_brush": "terrain_brush", &"draw_building": "draw_building",
	&"set_building": "set_building", &"set_overlay": "set_overlay",
}
const QUERIES := {
	&"raycast": "raycast", &"height_at": "height_at", &"inspect": "inspect", &"probe_building": "probe_building",
	&"list_buildings": "list_buildings", &"city_bounds": "city_bounds", &"render_pending": "render_pending",
	&"traffic_probe": "traffic_probe", &"snap_road": "snap_road", &"road_profiles": "road_profiles",
	&"segment_at": "segment_at", &"preview_roundabout": "preview_roundabout", &"preview_lot": "preview_lot",
	&"overlays": "overlays", &"save_status": "save_status",
}
## Save files (Roadcraft platform/saveFiles.ts): one file per city in user://saves; "jogo" is the game's own slot
## (Save / Load the last game / autosave every autosave_s of real time while there is unsaved work).
const SAVE_DIR := "user://saves"
const SAVE_EXT := "cidade"
const GAME_SLOT := "jogo"

signal saved(slot: String, result: Dictionary)
signal loaded(slot: String, result: Dictionary)

var snapshot: Dictionary = {}
var constants: Dictionary = {}
var _core: Node3D
var _since_refresh := 0.0
var _since_autosave := 0.0
var _save_pending := false
var _save_slot := GAME_SLOT


func _ready() -> void:
	_core = ClassDB.instantiate(&"CityCore")
	_core.name = "CityCore"
	add_child(_core)
	constants = _core.constants()


## Loads the data tables into the core and prepares rendering. Returns the list of problems (empty = ok).
func boot(terrain: Node, materials: Dictionary) -> PackedStringArray:
	var errors: PackedStringArray = _core.load_catalog(DataTables.catalog())
	if not errors.is_empty():
		return errors
	_core.terrain_path = _core.get_path_to(terrain)
	return _core.setup_render(DataTables.render(), materials)


## A new map (entry of data/catalog/maps.json); with_city = false makes an empty map. Returns the core's report.
func new_map(map: int, seed: int, with_city: bool) -> Dictionary:
	var report: Dictionary = _core.new_map(map, seed, with_city)
	_save_pending = false
	refresh()
	return report


## Writes the city to the slot (a file name without extension under SAVE_DIR).
func save_game(slot: String = GAME_SLOT, wait := true) -> Dictionary:
	DirAccess.make_dir_recursive_absolute(SAVE_DIR)
	_save_slot = slot
	var r: Dictionary = _core.save_city(save_path(slot))
	_since_autosave = 0.0
	if not r.get("pending", false):
		saved.emit(slot, r)
		return r
	if wait:
		while _core.save_status().get("busy", false):
			OS.delay_msec(1)
		var st: Dictionary = _core.save_status()
		var done := {"ok": st.get("ok", false), "error": st.get("error", ""), "bytes": st.get("bytes", 0)}
		saved.emit(slot, done)
		refresh()
		return done
	_save_pending = true
	refresh()
	return r


## Replaces the city with the saved one (the core stops the simulation, reads it and redraws everything).
func load_game(slot: String = GAME_SLOT) -> Dictionary:
	var r: Dictionary = _core.load_city(save_path(slot))
	_since_autosave = 0.0
	_save_pending = false
	refresh()
	loaded.emit(slot, r)
	return r


func save_path(slot: String) -> String:
	return "%s/%s.%s" % [SAVE_DIR, slot, SAVE_EXT]


## The saved cities, newest first: [{slot, modified (unix seconds), size (bytes)}].
func list_saves() -> Array:
	var out := []
	for f in DirAccess.get_files_at(SAVE_DIR):
		if f.get_extension() == SAVE_EXT:
			var path := "%s/%s" % [SAVE_DIR, f]
			out.append({"slot": f.get_basename(), "modified": FileAccess.get_modified_time(path),
				"size": FileAccess.get_size(path)})
	out.sort_custom(func(a: Dictionary, b: Dictionary) -> bool: return a.modified > b.modified)
	return out


func dispatch(command: StringName, args: Array = []) -> Variant:
	assert(COMMANDS.has(command), "comando desconhecido: %s" % command)
	var result: Variant = _core.callv(COMMANDS[command], args)
	if not command.begins_with("preview"):
		refresh()
	if result is Dictionary and result.has("error") and not result.error.is_empty() and not command.begins_with("preview"):
		command_failed.emit(result.error)
	return result


func query(name: StringName, args: Array = []) -> Variant:
	assert(QUERIES.has(name), "consulta desconhecida: %s" % name)
	return _core.callv(QUERIES[name], args)


## Tests only: advance the simulation deterministically.
func step_ticks(ticks: int) -> void:
	_core.step_ticks(ticks)
	refresh()


func refresh() -> void:
	snapshot = _core.snapshot()
	changed.emit(snapshot)


func _process(delta: float) -> void:
	_since_refresh += delta
	if _since_refresh >= SNAPSHOT_INTERVAL_S:
		_since_refresh = 0.0
		refresh()
	if _save_pending:
		var st: Dictionary = _core.save_status()
		if not st.get("busy", true):
			_save_pending = false
			saved.emit(_save_slot, {"ok": st.get("ok", false), "error": st.get("error", ""), "bytes": st.get("bytes", 0)})
			refresh()
	_since_autosave += delta
	if _since_autosave >= DataTables.game("saves").autosave_s and snapshot.get("ready", false) and snapshot.get("unsaved", false):
		save_game(GAME_SLOT, false)
