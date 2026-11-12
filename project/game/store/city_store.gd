## CityStore: the only owner of the C++ core (CityCore) on the script side.
## State flows one way: commands go in through dispatch(); the UI reads `snapshot` (refreshed from the core)
## and reacts to `changed`. Nothing else keeps copies of game numbers (gate ESTADO_NA_UI / owners.json).
extends Node

signal changed(snapshot: Dictionary)
signal command_failed(error_key: String)

const SNAPSHOT_INTERVAL_S := 0.25
## Reads of the core that change only when a revision of the snapshot moves (the network with edit_revision, the transport
## plan with transit_revision): `cached` asks the core once per revision, so a view drawing every frame never asks for nothing.
const CACHED_BY := {&"junctions": "edit_revision", &"transit_plan": "transit_revision"}

signal saved(slot: String, result: Dictionary)
signal loaded(slot: String, result: Dictionary)

var snapshot: Dictionary = {}
var constants: Dictionary = {}
var saves: CitySaves
var _core: Node3D
var _routes: Dictionary = {} # a StoreApi name -> the Callable on the core, or on the area of it, that runs it
var _since_refresh := 0.0
var _watches: Dictionary = {}
var _next_watch := 0
var _publication := 0
var _cache := {} # CACHED_BY query -> [the revision it was read at, its answer]
var _inputs := InputSequence.new()
var _input_batch_depth := 0
var _input_changed := false



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


## Loads the data tables into the core and prepares rendering. Returns the list of problems (empty = ok).
func boot(terrain: Node, materials: Dictionary) -> PackedStringArray:
	var errors: PackedStringArray = _core.load_catalog(DataTables.catalog())
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
	if report.get("ok", false):
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
	var result: Variant = _routes[command].callv(args)
	var changed := not command.begins_with("preview") and not command in StoreApi.NO_SNAPSHOT
	if result is Dictionary:
		changed = changed and result.get("changed", result.get("ok", true))
	elif result is bool:
		changed = changed and result
	if changed:
		if _input_batch_depth > 0:
			_input_changed = true
		else:
			refresh()
	if result is Dictionary and result.has("error") and not result.error.is_empty() and not command.begins_with("preview"):
		command_failed.emit(result.error)
	return result


## Result-dependent production consumers await this boundary. Existing direct dispatch remains synchronous;
## brush producers are migrated only together with every operation which can cross their queued debt.
func ordered_dispatch(command: StringName, args: Array = [], guard: InputSequence.Guard = null) -> Variant:
	if GameFlow.state == GameFlow.State.QUITTING and not _inputs.in_operation():
		return {"ok": false, "cancelled": true}
	if guard != null and not guard.live:
		return {"ok": false, "cancelled": true}
	if not _inputs.pending() and not _inputs.pumping():
		return dispatch(command, args)
	var ticket := _inputs.command(command, args, null, &"", guard)
	return await ticket.completed


## Queue the whole result-dependent gesture; its existing internal helpers keep actual synchronous native returns.
func perform_input(recipient: Object, method: StringName, args: Array = []) -> InputSequence.Ticket:
	if GameFlow.state == GameFlow.State.QUITTING and not _inputs.in_operation():
		var cancelled := InputSequence.Ticket.new()
		cancelled.finish({"cancelled": true})
		return cancelled
	return _inputs.operation(recipient, method, args)


func in_input_operation() -> bool:
	return _inputs.in_operation()


## Opening/closing at an admitted gesture boundary never drains later input. Its earlier samples already precede it.
func begin_input_stroke(label: String) -> void:
	if _inputs.in_operation():
		dispatch(&"begin_stroke", [label])
	else:
		_inputs.command(&"begin_stroke", [label])


func end_input_stroke() -> void:
	if _inputs.in_operation():
		dispatch(&"end_stroke")
	else:
		_inputs.command(&"end_stroke")


func input_pending() -> bool:
	return _inputs.pending()


func queue_input(command: StringName, args: Array, recipient: Object = null, method: StringName = &"") -> InputSequence.Ticket:
	return _inputs.command(command, args, recipient, method)


func queue_input_path(command: StringName, args: Array, first: Vector3, last: Vector3, steps: int, recipient: Object = null, method: StringName = &"") -> InputSequence.Ticket:
	return _inputs.path(command, args, first, last, steps, recipient, method)


func queue_input_time(command: StringName, args: Array, clock: InputSequence.Clock, delta: float, quantum: float, recipient: Object = null, method: StringName = &"") -> InputSequence.Ticket:
	return _inputs.timed(command, args, clock, delta, quantum, recipient, method)


func queue_input_zone(first: Vector3, last: Vector3, steps: int, radius: float, zone_kind: int, recipient: Object = null, method: StringName = &"") -> InputSequence.Ticket:
	return _inputs.zone(first, last, steps, radius, zone_kind, recipient, method)


func ordered_barrier(name: StringName, args: Array = []) -> Variant:
	if not _inputs.pending() and not _inputs.pumping() and not name in [&"save_game", &"save_city", &"export_map", &"save_on_close", &"save_on_hide", &"replace_city"]:
		return _barrier_now(name, args, false).get("result")
	var ticket := _inputs.barrier(name, args)
	return await ticket.completed


## Borrowed by InputSequence only while it owns the frame's admitted input units.
func begin_input_batch() -> void:
	_input_batch_depth += 1


func end_input_batch() -> void:
	assert(_input_batch_depth > 0)
	_input_batch_depth -= 1
	if _input_batch_depth == 0 and _input_changed:
		_input_changed = false
		refresh()


func apply_input_command(command: StringName, args: Array) -> Variant:
	assert(_inputs.pumping())
	return dispatch(command, args)


## A barrier returns an explicit transport envelope; its native result is never mistaken for queue acceptance.
func input_barrier(name: StringName, args: Array, started: bool) -> Dictionary:
	assert(_inputs.pumping())
	var outcome := _barrier_now(name, args, started)
	if name == &"save_on_close" and not outcome.get("pending", false):
		outcome["stop_after"] = true
	return outcome


func _barrier_now(name: StringName, args: Array, started: bool) -> Dictionary:
	match name:
		&"save_game", &"save_city", &"export_map", &"save_on_close", &"save_on_hide":
			return saves.input_save_barrier(name, args, started)
		&"history":
			var action: StringName = args[0]
			var label: String = snapshot.get("undo_label" if action == &"undo" else "redo_label", "")
			return {"result": {"ok": dispatch(action), "label": label}}
		&"load_game":
			return {"result": saves.load_game(args[0], args[1])}
		&"load_last":
			return {"result": saves.load_last()}
		&"open_file":
			return {"result": saves.open_file(args[0])}
		&"replace_city":
			var saved: Dictionary = saves.input_save_barrier(&"save_on_close", [], started)
			if saved.get("pending", false):
				return saved
			return {"result": new_map(args[0], args[1], args[2], args[3])} # old close policy also proceeds after a reported save failure
		&"new_map":
			return {"result": new_map(args[0], args[1], args[2], args[3])}
		&"step_ticks":
			step_ticks(args[0])
			return {"result": null}
	return {"result": {"ok": false, "error": "tool.failed"}}


func query(name: StringName, args: Array = []) -> Variant:
	assert(StoreApi.QUERIES.has(name), "consulta desconhecida: %s" % name)
	return _routes[name].callv(args)


## A CACHED_BY query's answer, asked of the core again only when its revision in the snapshot moved (every command
## refreshes the snapshot, so an edit is seen at once).
func cached(name: StringName) -> Variant:
	if _input_changed:
		return query(name) # internal same-batch readers cannot reuse a snapshot from before an accepted edit
	var rev: int = snapshot.get(CACHED_BY[name], -1)
	if rev < 0:
		return query(name)
	var key := [snapshot.get("city_generation", -1), rev]
	var hit: Array = _cache.get(name, [])
	if hit.is_empty() or hit[0] != key:
		hit = [key, query(name)]
		_cache[name] = hit
	return hit[1]


## The Simulation panel: shares of the city's own density of vehicles and people, and the demand.
func set_levels(traffic: float, people: float, demand: float) -> void:
	if _core.set_levels(traffic, people, demand):
		refresh()


## The Effects panel: a simulation system's tick on or off, to measure it ("traffic", "pedestrians", "growth").
func set_sim_switch(name: String, on: bool) -> void:
	_core.set_sim_switch(name, on)


## Tests only: advance the simulation deterministically.
func step_ticks(ticks: int) -> void:
	_core.step_ticks(ticks)
	refresh()


## A UI subscription starts at the current values, like connecting changed. Existing ready/show flows initialize it.
## Its node lifetime owns the subscription; only selected-field changes invoke its callback.
func watch(subscriber: Node, fields: Array, callback: Callable) -> int:
	_next_watch += 1
	var token := _next_watch
	var selected: Array = []
	for field: Variant in fields:
		selected.push_back(snapshot.get(field))
	_watches[token] = {"owner": weakref(subscriber), "fields": fields.duplicate(), "callback": callback, "selected": selected}
	subscriber.tree_exiting.connect(func() -> void: _watches.erase(token), CONNECT_ONE_SHOT)
	return token


func _notify(publication: int) -> void:
	for token: Variant in _watches.keys():
		if publication != _publication:
			return # a callback published a newer snapshot; never follow it with older UI values
		if not _watches.has(token):
			continue
		var row: Dictionary = _watches[token]
		var target: Object = row.owner.get_ref()
		var callback: Callable = row.callback
		if not is_instance_valid(target) or not callback.is_valid():
			_watches.erase(token)
			continue
		var selected: Array = []
		for field: Variant in row.fields:
			selected.push_back(snapshot.get(field))
		if selected == row.selected:
			continue
		row.selected = selected # before calling: a nested refresh must retain its newer projection
		callback.call(snapshot)


func refresh() -> void:
	var next: Dictionary = _core.snapshot()
	if next == snapshot:
		return
	snapshot = next
	_publication += 1
	var publication := _publication
	_notify(publication)
	if publication == _publication:
		changed.emit(snapshot) # compatibility for clients observing the complete snapshot


func _process(delta: float) -> void:
	_inputs.advance(self, int(DataTables.game("editing").input_units_per_frame))
	_since_refresh += delta
	if _since_refresh >= SNAPSHOT_INTERVAL_S:
		_since_refresh = 0.0
		refresh()
	saves.tick(delta)
