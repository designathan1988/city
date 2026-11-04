## Public transport in the capture (--only=transporte): a bus line, a train line across a street at the edge of the town and a metro, laid
## through the store as the player's tools do, seen from where the photos of the 4.0 stand (ports/transit/evidence/look): offsets along a
## stop's street, out from its kerb and up, at the 4.0's lens, at 16:00.
class_name CaptureTransit
extends RefCounted

const TRAIN := 1
const METRO := 2
const HOUR := 16.0
const STOP_REACH_M := 18.0
const STOP_STEPS := [90.0, 150.0, 210.0] # from the busiest junction, along a street
const DIRECTIONS := [Vector3.RIGHT, Vector3.LEFT, Vector3.FORWARD, Vector3.BACK]
const SCAN_M := 120.0
const SCAN_REACH_M := 60.0
const SCAN_TRIES := 200
const TRACK_HALVES := [280.0, 220.0, 160.0, 120.0]
const STATION_SHARE := 0.68
const METRO_PAST_M := 60.0
const METRO_STATION_M := 35.0 # short of its ends, clear of the bus stops
const RUN_TICKS := 600
const WAIT_STEP_TICKS := 20
const WAIT_TICKS := 6000
const TRAIN_NEAR_M := 40.0
const CROSSING_OFF_M := 6.0
# Along, out, up and lens of each view (the 4.0's __rec of each photo).
const STOP_NEAR := [14.67, -18.49, 7.33, 40.0]
const STOP_HIGH := [43.23, -54.48, 64.74, 35.0]
const TERMINAL := [19.48, -24.55, 9.65, 37.7]
const BUS := [15.83, 16.3, 7.55, 40.0]
const TRACK_NEAR := [3.9, -7.67, 3.4, 47.9]
const CROSSING := [38.53, -39.68, 30.17, 35.2]
const ENTRANCE := [8.81, -11.11, 4.71, 43.9]
const CUT := [174.47, -95.31, 204.66, 35.0]
# The 4.0's plan left its stations inside a block, so these have no pair: from the platform's side, near and from above.
const STATION_NEAR := [42.0, 34.0, 12.0, 40.0]
const STATION_HALL := [-34.0, 60.0, 16.0, 40.0]
const STATION_HIGH := [80.0, 90.0, 110.0, 35.0]
const PLAN := [0.0, -90.0, 130.0, 40.0]
const RUN_OFF_M := 30.0
const RUN_LONG_M := 80.0


## `cap` is the capture scene (its _shot, _frames, camera and world).
static func shots(cap: Node) -> void:
	await _at_hour(cap)
	var bus := _lay_bus()
	var rail := _lay_rail()
	var metro := _lay_metro(bus)
	print("transporte: paradas ", bus.size(), " trilho ", rail.has("road"), " metro ", metro.size())
	var near: Vector3 = bus[1].at
	cap.camera.global_position = near + Vector3.UP * STOP_REACH_M
	await cap._frames(1)
	await _view(cap, "40_parada_perto", bus[1], STOP_NEAR)
	await _view(cap, "41_parada_alto", bus[1], STOP_HIGH)
	await _view(cap, "42_terminal", bus[0], TERMINAL)
	CityStore.step_ticks(RUN_TICKS)
	await _bus_shot(cap)
	print("transporte: ", CityStore.query(&"transit_run"))
	if rail.has("road"):
		await _rail_shots(cap, rail)
	await _view(cap, "48_metro_entrada", metro[0], ENTRANCE)
	CityStore.query(&"transit_xray", [true])
	await _view(cap, "49_metro_corte", metro[0], CUT)
	CityStore.query(&"transit_xray", [false])
	await _tool_shots(cap, bus)
	print("transporte: ", CityStore.query(&"transit_run"))


## The tool as the player has it: the plan over the map, a line and a run being laid, a double click ending the run, the lines in the drawer.
static func _tool_shots(cap: Node, bus: Array) -> void:
	var tools: ToolController = cap.get_tree().get_first_node_in_group(&"tools")
	if tools == null:
		tools = ToolController.new(CityCamera.new())
		cap.add_child(tools)
		await cap._frames(1)
	tools.set_tool(ToolController.Tool.TRANSIT, { "kind": "line", "mode": 0 })
	tools.click_world(bus[0].at)
	tools.click_world(bus[1].at)
	tools.hover_world(bus[2].at)
	await _view(cap, "50_ferramenta_linha", bus[1], PLAN)
	tools.active.cancel()
	tools.set_tool(ToolController.Tool.TRANSIT, { "kind": "track", "mode": METRO })
	var a: Vector3 = bus[1].at + bus[1].out * RUN_OFF_M
	tools.click_world(a)
	tools.hover_world(a + bus[1].along * RUN_LONG_M)
	await _view(cap, "51_ferramenta_trilho", bus[1], PLAN)
	tools.double_press = true
	tools.click_world(a + bus[1].along * RUN_LONG_M)
	tools.double_press = false
	print("transporte: trilhos ", CityStore.query(&"transit_plan").tracks)
	var drawers := cap.get_tree().root.find_children("*", "Control", true, false).filter(func(n: Node) -> bool: return n is HudDrawer)
	var drawer: HudDrawer = drawers[0]
	for c: Dictionary in DataTables.ui("drawers").categories:
		if c.id == "transit":
			drawer.open(c)
			drawer.select_tab(c.tabs[c.tabs.size() - 1])
	await _view(cap, "52_gaveta_linhas", bus[1], PLAN)
	tools.plan_changed.emit()
	await cap._frames(1)
	drawer.close()
	tools.set_tool(ToolController.Tool.NONE)


static func _at_hour(cap: Node) -> void:
	CityStore.set_clock_hour(HOUR)
	cap.world.set_hour(HOUR)
	await cap._frames(cap._sky_frames()) # the sky's radiance follows a new hour over several frames


static func _view(cap: Node, name: String, place: Dictionary, view: Array) -> void:
	await _look(cap, name, place.at, place.along, place.out, view)


static func _look(cap: Node, name: String, at: Vector3, along: Vector3, out: Vector3, view: Array) -> void:
	CityStore.set_clock_hour(HOUR)
	var lens: float = cap.camera.fov
	cap.camera.fov = view[3]
	await cap._shot(name, at + along * view[0] + out * view[1] + Vector3.UP * view[2], at)
	cap.camera.fov = lens


## Three stops along the first street from the busiest junction that has them; the first a terminal; a line through them.
static func _lay_bus() -> Array:
	var ground: Vector3 = CityStore.query(&"traffic_probe", [1]).busy_crossing
	for dir: Vector3 in DIRECTIONS:
		var found := 0
		for step: float in STOP_STEPS:
			found += 0 if CityStore.query(&"transit_footway", [ground + dir * step, STOP_REACH_M]).is_empty() else 1
		if found < STOP_STEPS.size():
			continue
		for step: float in STOP_STEPS:
			CityStore.dispatch(&"transit_stop", [ground + dir * step])
		break
	var plan: Dictionary = CityStore.query(&"transit_plan")
	var ids := PackedInt32Array()
	for s: Dictionary in plan.stops:
		ids.append(s.id)
	CityStore.dispatch(&"transit_terminal", [plan.stops[0].at])
	CityStore.dispatch(&"transit_line", [ids])
	return CityStore.query(&"transit_plan").stops


## Across a street at the edge of the town, as far out as a run the core lets through: two stations and a line.
static func _lay_rail() -> Dictionary:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var middle: Vector3 = (b.min + b.max) / 2
	var spots: Array = []
	var x: float = b.min.x
	while x <= b.max.x:
		var z: float = b.min.z
		while z <= b.max.z:
			var r: Dictionary = CityStore.query(&"transit_footway", [Vector3(x, 0, z), SCAN_REACH_M])
			if not r.is_empty():
				spots.append(r)
			z += SCAN_M
		x += SCAN_M
	spots.sort_custom(func(p: Dictionary, q: Dictionary) -> bool: return p.at.distance_to(middle) > q.at.distance_to(middle))
	for s: Dictionary in spots.slice(0, SCAN_TRIES):
		var road: Vector3 = s.at - s.out * CROSSING_OFF_M
		for half: float in TRACK_HALVES:
			var run := PackedVector3Array([road - s.out * half, road + s.out * half])
			if not CityStore.dispatch(&"transit_track", [run, TRAIN]).get("ok", false):
				continue
			var ids := PackedInt32Array()
			for side: float in [-1.0, 1.0]:
				CityStore.dispatch(&"transit_station", [road + s.out * side * half * STATION_SHARE, TRAIN])
			for q: Dictionary in CityStore.query(&"transit_plan").stops:
				if int(q.mode) == TRAIN:
					ids.append(q.id)
			CityStore.dispatch(&"transit_line", [ids])
			return { "road": road, "along": s.out, "out": s.along }
	return {}


## Under the bus street, past its stops: two stations with their ways down, and a line.
static func _lay_metro(bus: Array) -> Array:
	var a: Vector3 = bus[0].at
	var e: Vector3 = bus[bus.size() - 1].at
	var d := (e - a).normalized()
	var run := PackedVector3Array([a - d * METRO_PAST_M, e + d * METRO_PAST_M])
	CityStore.dispatch(&"transit_track", [run, METRO])
	CityStore.dispatch(&"transit_station", [a - d * METRO_STATION_M, METRO])
	CityStore.dispatch(&"transit_station", [e + d * METRO_STATION_M, METRO])
	var out: Array = []
	var ids := PackedInt32Array()
	for q: Dictionary in CityStore.query(&"transit_plan").stops:
		if int(q.mode) == METRO:
			out.append(q)
			ids.append(q.id)
	CityStore.dispatch(&"transit_line", [ids])
	return out


## The first bus stopped at a stop, from ahead of it on the footway's side.
static func _bus_shot(cap: Node) -> void:
	for tick in range(0, WAIT_TICKS, WAIT_STEP_TICKS):
		var run: Dictionary = CityStore.query(&"transit_run")
		if run.has("first_bus") and run.first_bus[1] > 0:
			break
		CityStore.step_ticks(WAIT_STEP_TICKS)
	await cap._frames(1)
	var now: Dictionary = CityStore.query(&"transit_run")
	if now.has("bus_at"):
		var ahead: Vector3 = now.bus_heading
		await _look(cap, "43_onibus", now.bus_at, ahead, Vector3(ahead.z, 0, -ahead.x), BUS)


static func _rail_shots(cap: Node, rail: Dictionary) -> void:
	var at: Vector3 = rail.road
	cap.camera.global_position = at + Vector3.UP * STOP_REACH_M
	await cap._frames(1)
	await _look(cap, "44_trilho_perto", at, rail.along, rail.out, TRACK_NEAR)
	for tick in range(0, WAIT_TICKS, WAIT_STEP_TICKS):
		var run: Dictionary = CityStore.query(&"transit_run")
		if run.has("train_at") and Vector2(run.train_at.x - at.x, run.train_at.z - at.z).length() < TRAIN_NEAR_M:
			break
		CityStore.step_ticks(WAIT_STEP_TICKS)
	await cap._frames(1)
	await _look(cap, "45_passagem", at, rail.along, rail.out, CROSSING)
	var b: Dictionary = CityStore.query(&"city_bounds")
	var middle: Vector3 = (b.min + b.max) / 2
	var station: Dictionary = {} # the one out of the town: the other may stand among its lots
	for q: Dictionary in CityStore.query(&"transit_plan").stops:
		if int(q.mode) == TRAIN and (station.is_empty() or q.at.distance_to(middle) > station.at.distance_to(middle)):
			station = q
	await _view(cap, "46_estacao_perto", station, STATION_NEAR)
	await _view(cap, "47_estacao_predio", station, STATION_HALL)
	await _view(cap, "47_estacao_alto", station, STATION_HIGH)
