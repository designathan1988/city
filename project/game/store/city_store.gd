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
	&"set_building": "set_building", &"set_overlay": "set_overlay", &"show_every_zone": "show_every_zone", &"reverse_road": "reverse_road", &"duplicate_road": "duplicate_road",
	&"set_marking_style": "set_marking_style", &"set_detail_scale": "set_detail_scale", &"set_vegetation": "set_vegetation",
	&"set_agent_detail": "set_agent_detail", &"set_population": "set_population", &"set_upload_every": "set_upload_every",
	&"set_lots_drawn": "set_lots_drawn",
}
const QUERIES := {
	&"raycast": "raycast", &"height_at": "height_at", &"inspect": "inspect", &"probe_building": "probe_building",
	&"list_buildings": "list_buildings", &"city_bounds": "city_bounds", &"render_pending": "render_pending",
	&"render_cost": "render_cost", &"pose_stats": "pose_stats", &"agent_cost": "agent_cost",
	&"traffic_probe": "traffic_probe", &"snap_road": "snap_road", &"road_profiles": "road_profiles",
	&"segment_at": "segment_at", &"preview_roundabout": "preview_roundabout", &"preview_lot": "preview_lot",
	&"overlays": "overlays", &"save_status": "save_status", &"traffic_metrics": "traffic_metrics", &"segment_traffic": "segment_traffic", &"segment_path": "segment_path",
	&"node_traffic": "node_traffic", &"impossible_nodes": "impossible_nodes", &"clear_reach": "clear_reach", &"recent_edits": "recent_edits",
}
signal saved(slot: String, result: Dictionary)
signal loaded(slot: String, result: Dictionary)

var snapshot: Dictionary = {}
var constants: Dictionary = {}
var saves: CitySaves
var _core: Node3D
var _since_refresh := 0.0


func _ready() -> void:
	_core = ClassDB.instantiate(&"CityCore")
	_core.name = "CityCore"
	add_child(_core)
	constants = _core.constants()
	saves = CitySaves.new(self, _core)


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
	saves.new_city()
	refresh()
	return report


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


## The Simulation panel: shares of the city's own density of vehicles and people, and the demand.
func set_levels(traffic: float, people: float, demand: float) -> void:
	_core.set_levels(traffic, people, demand)
	refresh()


## The Effects panel: a simulation system's tick on or off, to measure it ("traffic", "pedestrians", "growth").
func set_sim_switch(name: String, on: bool) -> void:
	_core.set_sim_switch(name, on)


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
	saves.tick(delta)
