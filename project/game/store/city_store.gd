## CityStore: the only owner of the C++ core (CityCore) on the script side.
## State flows one way: commands go in through dispatch(); the UI reads `snapshot` (refreshed from the core)
## and reacts to `changed`. Nothing else keeps copies of game numbers (gate ESTADO_NA_UI / owners.json).
extends Node

signal changed(snapshot: Dictionary)
signal command_failed(error_key: String)

const SNAPSHOT_INTERVAL_S := 0.25

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
## `paused`: the clock starts stopped, for a test or a capture that steps it: no tick slips in before it says so.
func new_map(map: int, seed: int, with_city: bool, paused := false) -> Dictionary:
	var report: Dictionary = _core.new_map(map, seed, with_city, paused) # waits for a save in flight first
	saves.new_city()
	refresh()
	return report


## The core is up and holds a map (views that read it each frame wait for this).
func ready_to_read() -> bool:
	return _core != null and not snapshot.is_empty()


## The road the tool opens with (data/ui/drawers.json first_road).
func first_road() -> int:
	var id: String = DataTables.ui("drawers").first_road
	for p in query(&"road_profiles"):
		if p.id == id:
			return p.index
	return 0


func dispatch(command: StringName, args: Array = []) -> Variant:
	assert(StoreApi.COMMANDS.has(command), "comando desconhecido: %s" % command)
	var result: Variant = _core.callv(StoreApi.COMMANDS[command], args)
	if not command.begins_with("preview"):
		refresh()
	if result is Dictionary and result.has("error") and not result.error.is_empty() and not command.begins_with("preview"):
		command_failed.emit(result.error)
	return result


func query(name: StringName, args: Array = []) -> Variant:
	assert(StoreApi.QUERIES.has(name), "consulta desconhecida: %s" % name)
	return _core.callv(StoreApi.QUERIES[name], args)


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
