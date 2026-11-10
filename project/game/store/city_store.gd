## CityStore: the only owner of the C++ core (CityCore) on the script side.
## State flows one way: commands go in through dispatch(); the UI reads `snapshot` (refreshed from the core)
## and reacts to `changed`. Nothing else keeps copies of game numbers (gate ESTADO_NA_UI / owners.json).
## The snapshot is read from the core when it is read after something may have moved it (a command, the clock's
## SNAPSHOT_INTERVAL_S, refresh): a read right after a command sees it, and many commands in a frame cost one reading.
## `changed` follows at the end of the frame, once however many marks came in it.
extends Node

signal changed(snapshot: Dictionary)
signal command_failed(error_key: String)

const SNAPSHOT_INTERVAL_S := 0.25
## Reads of the core that change only when a revision of the snapshot moves (the network with edit_revision, the transport
## plan with transit_revision): `cached` asks the core once per revision, so a view drawing every frame never asks for nothing.
const CACHED_BY := {&"junctions": "edit_revision", &"transit_plan": "transit_revision", &"road_profiles": "profiles_revision"}

signal saved(slot: String, result: Dictionary)
signal loaded(slot: String, result: Dictionary)

var snapshot: Dictionary:
	get:
		if _stale:
			_stale = false
			_snapshot = _core.snapshot()
		return _snapshot
var constants: Dictionary = {}
var saves: CitySaves
var _core: Node3D
var _routes: Dictionary = {} # a StoreApi name -> the Callable on the core, or on the area of it, that runs it
var _since_refresh := 0.0
var _cache := {} # CACHED_BY query -> [the revision it was read at, its answer]
var _cost_frame := -1 # the frame frame_render_cost last read
var _cost := {} # its reading
var _snapshot: Dictionary = {} # the core's snapshot as last read
var _stale := false # something may have moved it since: the next read asks the core
var _change_due := false # `changed` is on its way, at the end of this frame


func _ready() -> void:
	_core = ClassDB.instantiate(&"CityCore")
	_core.name = "CityCore"
	add_child(_core)
	constants = _core.constants()
	saves = CitySaves.new(self, _core)
	for table: Dictionary in [StoreApi.COMMANDS, StoreApi.QUERIES]:
		for name: StringName in table:
			_routes[name] = _route(table[name])


## "<area>/<method>" runs on that area of the core (CityCore.area); a bare method, on the core itself.
func _route(spec: String) -> Callable:
	var cut := spec.find("/")
	if cut < 0:
		return Callable(_core, spec)
	var area: Object = _core.area(spec.left(cut))
	assert(area != null, "área desconhecida: %s" % spec)
	return Callable(area, spec.substr(cut + 1))


## Loads the data tables into the core and prepares rendering. Returns the list of problems (empty = ok). `catalog`: the
## tables to load instead of DataTables' (a test hands a damaged one to see it refused).
func boot(terrain: Node, materials: Dictionary, catalog: Dictionary = {}) -> PackedStringArray:
	var errors: PackedStringArray = _core.load_catalog(DataTables.catalog() if catalog.is_empty() else catalog)
	if not errors.is_empty():
		return errors
	_core.terrain_path = _core.get_path_to(terrain)
	var problems: PackedStringArray = _core.setup_render(DataTables.render(), materials)
	DataTables.release_boot_tables()
	return problems


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
	for p in cached(&"road_profiles"):
		if p.id == id:
			return p.index
	return 0


func dispatch(command: StringName, args: Array = []) -> Variant:
	assert(StoreApi.COMMANDS.has(command), "comando desconhecido: %s" % command)
	var result: Variant = _routes[command].callv(args)
	if not command.begins_with("preview"):
		_mark()
	if result is Dictionary and result.has("error") and not result.error.is_empty() and not command.begins_with("preview"):
		command_failed.emit(result.error)
	return result


func query(name: StringName, args: Array = []) -> Variant:
	assert(StoreApi.QUERIES.has(name), "consulta desconhecida: %s" % name)
	return _routes[name].callv(args)


## The core's render cost of this frame (query render_cost), read once a frame however many ask (the lab's meter, the Effects
## panel's CPU tab). Shared by every reader: read it, never change it.
func frame_render_cost() -> Dictionary:
	var frame := Engine.get_process_frames()
	if frame != _cost_frame:
		_cost_frame = frame
		_cost = query(&"render_cost")
	return _cost


## A CACHED_BY query's answer, asked of the core again only when its revision in the snapshot moved (every command
## marks the snapshot, so an edit is seen at once). Shared by every reader: read it, never change it.
func cached(name: StringName) -> Variant:
	var rev: int = snapshot.get(CACHED_BY[name], -1)
	if rev < 0:
		return query(name)
	var hit: Array = _cache.get(name, [])
	if hit.is_empty() or hit[0] != rev:
		hit = [rev, query(name)]
		_cache[name] = hit
	return hit[1]


## The Simulation panel: shares of the city's own density of vehicles and people, and the demand.
func set_levels(traffic: float, people: float, demand: float) -> void:
	_core.set_levels(traffic, people, demand)
	_mark()


## The Effects panel: a simulation system's tick on or off, to measure it ("traffic", "pedestrians", "growth").
func set_sim_switch(name: String, on: bool) -> void:
	_core.set_sim_switch(name, on)


## Tests only: advance the simulation deterministically. Refused (false) while the clock runs: open the map paused.
func step_ticks(ticks: int) -> bool:
	var stepped: bool = _core.step_ticks(ticks)
	_mark()
	return stepped


## The snapshot read from the core now (after a load, or by a caller that wants this instant's numbers); `changed` at the frame's end.
func refresh() -> void:
	_stale = false
	_snapshot = _core.snapshot()
	_change_coming()


## The core may have moved: the snapshot is read again when next read, and `changed` follows at the frame's end.
func _mark() -> void:
	_stale = true
	_change_coming()


func _change_coming() -> void:
	if not _change_due:
		_change_due = true
		_emit_changed.call_deferred()


func _emit_changed() -> void:
	_change_due = false
	changed.emit(snapshot)


func _process(delta: float) -> void:
	_since_refresh += delta
	if _since_refresh >= SNAPSHOT_INTERVAL_S:
		_since_refresh = 0.0
		_mark()
	saves.tick(delta)
