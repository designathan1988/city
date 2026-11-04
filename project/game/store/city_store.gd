## CityStore: the only owner of the C++ core (CityCore) on the script side.
## State flows one way: commands go in through dispatch(); the UI reads `snapshot` (refreshed from the core)
## and reacts to `changed`. Nothing else keeps copies of game numbers (gate ESTADO_NA_UI / owners.json).
extends Node

signal changed(snapshot: Dictionary)
signal command_failed(error_key: String)

const SNAPSHOT_INTERVAL_S := 0.25
## Commands the UI/tools may send, with the CityCore method that executes each one.
const COMMANDS := {
	&"set_speed": "set_speed", &"set_day_hour": "set_day_hour", &"build_road": "build_road", &"demolish": "demolish", &"paint_zone": "paint_zone",
	&"preview_road": "preview_road", &"undo": "undo", &"redo": "redo", &"begin_stroke": "begin_stroke",
	&"end_stroke": "end_stroke", &"split_road": "split_road", &"upgrade_road": "upgrade_road",
	&"roundabout": "roundabout", &"terrain_brush": "terrain_brush", &"draw_building": "draw_building",
	&"set_building": "set_building", &"set_overlay": "set_overlay", &"body_storeys": "body_storeys",
	&"body_insert_storey": "body_insert_storey", &"body_resize": "body_resize", &"body_add_wing": "body_add_wing",
	&"body_add_setback": "body_add_setback", &"body_split": "body_split", &"body_remove_volume": "body_remove_volume",
	&"body_heights": "body_heights", &"body_set_bay": "body_set_bay", &"body_set_zone": "body_set_zone",
	&"body_set_pattern": "body_set_pattern", &"body_set_face": "body_set_face", &"body_set_relief": "body_set_relief",
	&"body_set_roof": "body_set_roof", &"body_roof_shape": "body_roof_shape", &"body_paint": "body_paint",
	&"body_add_gear": "body_add_gear", &"body_remove_gear": "body_remove_gear", &"body_turn_gear": "body_turn_gear",
	&"body_add_part": "body_add_part", &"body_add_part_at": "body_add_part_at", &"body_remove_part": "body_remove_part",
	&"body_turn_part": "body_turn_part", &"body_repeat_part": "body_repeat_part", &"place_model": "place_model",
}
const QUERIES := {
	&"raycast": "raycast", &"height_at": "height_at", &"inspect": "inspect", &"probe_building": "probe_building",
	&"list_buildings": "list_buildings", &"city_bounds": "city_bounds", &"render_pending": "render_pending",
	&"render_cost": "render_cost", &"pose_stats": "pose_stats", &"agent_cost": "agent_cost",
	&"traffic_probe": "traffic_probe", &"snap_road": "snap_road", &"road_profiles": "road_profiles",
	&"segment_at": "segment_at", &"preview_roundabout": "preview_roundabout", &"preview_lot": "preview_lot",
	&"model_list": "model_list", &"model_bytes": "model_bytes", &"doc_body": "doc_body", &"building_bytes": "building_bytes", &"body_size": "body_size",
	&"place_fit": "place_fit", &"place_check": "place_check", &"stage_show": "stage_show", &"stage_move": "stage_move", &"stage_free": "stage_free", &"stage_triangles": "stage_triangles",
	&"overlays": "overlays", &"save_status": "save_status", &"body_info": "body_info", &"body_bay": "body_bay", &"body_pick": "body_pick", &"body_outline": "body_outline",
}
## Save files: SaveSlots owns where they live (one per city). The city on screen is written to its own slot
## (`_city_slot`, given at its first save or by the slot it was opened from): Save, Ctrl+S and the autosave every
## autosave_s of real time while it has unsaved work and is the player's (it has a slot or they edited it). The core
## writes on its own thread; `saved` carries the outcome, so the frame never waits for a save.

signal saved(slot: String, result: Dictionary)
signal loaded(slot: String, result: Dictionary)

var snapshot: Dictionary = {}
var constants: Dictionary = {}
var _core: Node3D
var _since_refresh := 0.0
var _since_autosave := 0.0
var _save_pending := false
var _save_slot := ""
var _city_slot := ""
var _export_to := "" # a map copy to make once the save in flight is written
var _save_auto := false # the save in flight is the autosave


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
	var report: Dictionary = _core.new_map(map, seed, with_city) # waits for a save in flight first
	_settle_save()
	_city_slot = "" # a new city: its first save gives it a slot of its own
	refresh()
	return report


## The slot the city on screen is saved to ("" until its first save).
func city_slot() -> String:
	return _city_slot


## A save is being written (its result comes with `saved`).
func save_busy() -> bool:
	return _save_pending


## Save (Menu, Ctrl+S): the city on screen to its own slot. The result is `saved`'s; this returns only a refusal.
func save_city() -> Dictionary:
	if _city_slot.is_empty():
		_city_slot = SaveSlots.free_city_slot(int(snapshot.get("seed", 0)))
	return save_game(_city_slot)


## Save map: the city is saved, and its file is copied to mapa-<seed> once it is written.
func export_map() -> Dictionary:
	var to := SaveSlots.path(SaveSlots.export_slot(int(snapshot.get("seed", 0))))
	var r := save_city()
	if r.get("pending", false):
		_export_to = to
	return r


## Starts writing the city to the slot (a file name without extension, SaveSlots.path) on the core's save thread:
## {pending: true}, then `saved` with the outcome; or a refusal ({ok: false, error}) when one is already writing.
func save_game(slot: String) -> Dictionary:
	if _save_pending:
		return {"ok": false, "error": "save.busy"}
	DirAccess.make_dir_recursive_absolute(SaveSlots.dir())
	_save_slot = slot
	var r: Dictionary = _core.save_city(save_path(slot))
	_since_autosave = 0.0
	_save_pending = r.get("pending", false)
	if not _save_pending:
		_finish_save(slot, r)
	refresh()
	return r


## Replaces the city with the saved one (the core stops the simulation, reads it and redraws everything). The city
## keeps that slot, unless it is a map copy (then its first save gives it one of its own).
func load_game(slot: String) -> Dictionary:
	var r: Dictionary = _core.load_city(save_path(slot)) # waits for a save in flight first
	_settle_save()
	_since_autosave = 0.0
	if r.get("ok", false):
		_city_slot = slot if SaveSlots.is_city_slot(slot) else ""
		if not _city_slot.is_empty():
			SaveSlots.remember_last(_city_slot)
	refresh()
	loaded.emit(slot, r)
	return r


## Load the last game: the city the index names (SaveSlots.last).
func load_last() -> Dictionary:
	var slot := SaveSlots.last()
	if slot.is_empty():
		return {"ok": false, "error": "save.none"}
	return load_game(slot)


func save_path(slot: String) -> String:
	return SaveSlots.path(slot)


## The saved cities and map copies, newest first: [{slot, modified (unix seconds), size (bytes)}].
func list_saves() -> Array:
	return SaveSlots.list()


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
	if _save_pending and not _core.save_status().get("busy", true):
		_settle_save()
		refresh()
	_since_autosave += delta
	if _since_autosave >= DataTables.game("saves").autosave_s:
		_since_autosave = 0.0 # checked every autosave_s, saved only when due
		if _autosave_due():
			_save_auto = true
			save_city()


## Unsaved work of the player's: a city never saved nor edited is not written.
func _autosave_due() -> bool:
	var players: bool = not _city_slot.is_empty() or snapshot.get("edited", false)
	return snapshot.get("ready", false) and snapshot.get("unsaved", false) and not _save_pending and players


## The save thread finished (or the core waited for it before a new map or a load): its outcome goes out.
func _settle_save() -> void:
	if not _save_pending:
		return
	_save_pending = false
	var st: Dictionary = _core.save_status()
	_finish_save(_save_slot, {"ok": st.get("ok", false), "error": st.get("error", ""), "bytes": st.get("bytes", 0)})


## A save that ended: the city's own slot becomes the last city, a map copy asked for is made; listeners get the
## result (`auto` for the autosave).
func _finish_save(slot: String, result: Dictionary) -> void:
	if result.get("ok", false) and slot == _city_slot:
		SaveSlots.remember_last(slot)
	if result.get("ok", false) and not _export_to.is_empty():
		if DirAccess.copy_absolute(save_path(slot), _export_to) != OK:
			result = {"ok": false, "error": "save.writeFailed"}
	result["auto"] = _save_auto
	_export_to = ""
	_save_auto = false
	saved.emit(slot, result)
