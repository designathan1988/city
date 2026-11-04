## Public transport in the capture (--only=transporte): a bus line, a train line across a street at the edge of the town and a metro, laid
## through the store as the player's tools do, seen from where the photos of the 4.0 stand (ports/transit/evidence/look): offsets along a
## stop's street, out from its kerb and up, at the 4.0's lens, at 16:00.
class_name CaptureTransit
extends RefCounted

const TRAIN := 1
const METRO := 2
const HOUR := 16.0
const STOP_REACH_M := 18.0
const RAIL_APART_M := 150.0 # out of the bus street's photos
const TRACK_HALVES := [280.0, 220.0, 160.0, 120.0]
const STATION_SHARE := 0.68
const METRO_PAST_M := 60.0
const METRO_STATION_M := 35.0 # short of its ends, clear of the bus stops
const RUN_TICKS := 600
const WAIT_STEP_TICKS := 20
const WAIT_TICKS := 6000
const TRAIN_NEAR_M := 40.0
const TRAIN_AWAY_M := 80.0
const CROSSING_OFF_M := 6.0
# Along, out, up and lens of each view (the 4.0's __rec of each photo).
const STOP_NEAR := [14.67, -18.49, 7.33, 40.0]
const STOP_HIGH := [43.0, -54.0, 65.0, 35.0]
const TERMINAL := [19.48, -24.55, 9.65, 37.7]
const BUS := [26.58, 27.37, 12.6, 36.71]
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
	var site := CaptureTransitSite.street([STOP_NEAR, STOP_HIGH], [TERMINAL, ENTRANCE])
	if site.is_empty():
		return
	await _counted(cap, "39_sem_transporte", site.place)
	var bus := _lay_bus(site)
	var rail := _lay_rail(bus)
	var metro := _lay_metro(bus, site.others[1].at)
	await _counted(cap, "39_com_o_plano", site.place)
	print("transporte: paradas ", bus.size(), " trilho ", rail.has("road"), " metro ", metro.size())
	var near: Vector3 = bus[1].at
	cap.camera.global_position = near + Vector3.UP * STOP_REACH_M
	await cap._frames(1)
	await _view(cap, "40_parada_perto", bus[1], STOP_NEAR)
	await _view(cap, "41_parada_alto", bus[1], STOP_HIGH)
	CityStore.step_ticks(RUN_TICKS)
	await _bus_shots(cap, bus[0])
	print("transporte: ", CityStore.query(&"transit_run"))
	if rail.has("road"):
		await _rail_shots(cap, rail)
	# a station's own frame follows its track, and only a bus stop's knows the street: the footway's, where the view was found clear
	await _view(cap, "48_metro_entrada", {"at": metro[0].at, "along": site.others[1].along, "out": site.others[1].out}, ENTRANCE)
	CityStore.query(&"transit_xray", [true])
	await _view(cap, "49_metro_corte", metro[0], CUT)
	CityStore.query(&"transit_xray", [false])
	await _counted(cap, "39_com_as_linhas", site.place)
	await _tool_shots(cap, bus)
	print("transporte: ", CityStore.query(&"transit_run"))


## The same frame high over the street, and what it drew: counts, which no load on the machine changes.
static func _counted(cap: Node, name: String, place: Dictionary) -> void:
	await _view(cap, name, place, CUT)
	print("transporte: quadro ", name, ": ", CaptureTransitSite.drawn(cap.get_viewport().get_viewport_rid()))


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
	await CaptureSignals.at_hour(cap, HOUR)


static func _view(cap: Node, name: String, place: Dictionary, view: Array) -> void:
	await _look(cap, name, place.at, place.along, place.out, view)


static func _look(cap: Node, name: String, at: Vector3, along: Vector3, out: Vector3, view: Array) -> void:
	cap.set_hour(HOUR)
	var lens: float = cap.camera.fov
	cap.camera.fov = view[3]
	await cap._shot(name, at + along * view[0] + out * view[1] + Vector3.UP * view[2], at)
	cap.camera.fov = lens


## Three stops on a street with open ground across it, as the 4.0's, every photo's pose clear; the first a terminal; a line through them.
static func _lay_bus(site: Dictionary) -> Array:
	for at: Vector3 in [site.others[0].at, site.place.at, site.third.at]:
		CityStore.dispatch(&"transit_stop", [at])
	print("transporte: rua em ", site.place.at)
	var plan: Dictionary = CityStore.query(&"transit_plan")
	var ids := PackedInt32Array()
	for s: Dictionary in plan.stops:
		ids.append(s.id)
	CityStore.dispatch(&"transit_terminal", [plan.stops[0].at])
	CityStore.dispatch(&"transit_line", [ids])
	return CityStore.query(&"transit_plan").stops


## Across a street at the edge of the town, away from the bus street, as far out as a run the core lets through, both photos clear and the
## side they see open: two stations and a line. A place is taken either way round: the same frame, turned.
static func _lay_rail(bus: Array) -> Dictionary:
	var seen := 0
	for s: Dictionary in CaptureTransitSite.footways():
		var road: Vector3 = s.at - s.out * CROSSING_OFF_M
		if road.distance_to(bus[1].at) < RAIL_APART_M:
			continue
		for turn: float in [1.0, -1.0]:
			var place := {"at": road, "along": s.out * turn, "out": s.along * turn}
			if not CaptureTransitSite.sees(place, TRACK_NEAR) or not CaptureTransitSite.sees(place, CROSSING):
				continue
			seen += 1
			if CaptureTransitSite.open_run(road, place.along) and _lay_run(road, s.out):
				print("transporte: trilho em ", road)
				return { "road": road, "along": place.along, "out": place.out }
	print("transporte: trilho sem lugar, ", seen, " com as duas vistas")
	return {}


## The longest run the core lets through across `road`, its stations and its line.
static func _lay_run(road: Vector3, across: Vector3) -> bool:
	for half: float in TRACK_HALVES:
		var run := PackedVector3Array([road - across * half, road + across * half])
		if not CityStore.dispatch(&"transit_track", [run, TRAIN]).get("ok", false):
			continue
		var ids := PackedInt32Array()
		for side: float in [-1.0, 1.0]:
			CityStore.dispatch(&"transit_station", [road + across * side * half * STATION_SHARE, TRAIN])
		for q: Dictionary in CityStore.query(&"transit_plan").stops:
			if int(q.mode) == TRAIN:
				ids.append(q.id)
		CityStore.dispatch(&"transit_line", [ids])
		return true
	return false


## Under the bus street: a station at the entrance the photo looks at, one past the stops at the far end, and a line.
static func _lay_metro(bus: Array, entrance: Vector3) -> Array:
	var far: Vector3 = bus[0].at if bus[0].at.distance_to(entrance) > bus[2].at.distance_to(entrance) else bus[2].at
	var d := (far - entrance).normalized()
	var run := PackedVector3Array([entrance - d * METRO_PAST_M, far + d * METRO_PAST_M])
	CityStore.dispatch(&"transit_track", [run, METRO])
	CityStore.dispatch(&"transit_station", [entrance, METRO])
	CityStore.dispatch(&"transit_station", [far + d * METRO_STATION_M, METRO])
	var out: Array = []
	var ids := PackedInt32Array()
	for q: Dictionary in CityStore.query(&"transit_plan").stops:
		if int(q.mode) == METRO:
			out.append(q)
			ids.append(q.id)
	CityStore.dispatch(&"transit_line", [ids])
	return out


## The first bus stopped at the terminal, as the 4.0's: the terminal, and the bus from ahead of it on the footway's side.
static func _bus_shots(cap: Node, terminal: Dictionary) -> void:
	for tick in range(0, WAIT_TICKS, WAIT_STEP_TICKS):
		var run: Dictionary = CityStore.query(&"transit_run")
		if run.has("first_bus") and run.first_bus[0] == 0 and run.first_bus[1] > 0:
			break
		CityStore.step_ticks(WAIT_STEP_TICKS)
	await cap._frames(1)
	await _view(cap, "42_terminal", terminal, TERMINAL)
	var now: Dictionary = CityStore.query(&"transit_run")
	if now.has("bus_at"):
		var ahead: Vector3 = now.bus_heading
		await _look(cap, "43_onibus", now.bus_at, ahead, Vector3(ahead.z, 0, -ahead.x), BUS)


## How far the nearest car of any train is from `at`, on the ground.
static func _nearest_car(run: Dictionary, at: Vector3) -> float:
	var best := INF
	for car: Vector3 in run.train_cars:
		best = minf(best, Vector2(car.x - at.x, car.z - at.z).length())
	return best


static func _rail_shots(cap: Node, rail: Dictionary) -> void:
	var at: Vector3 = rail.road
	cap.camera.global_position = at + Vector3.UP * STOP_REACH_M
	await cap._frames(1)
	for tick in range(0, WAIT_TICKS, WAIT_STEP_TICKS): # the crossing as the 4.0's photo has it, with no train on it
		var away: Dictionary = CityStore.query(&"transit_run")
		if _nearest_car(away, at) > TRAIN_AWAY_M and int(away.held_at_crossing) == 0: # and the cars it held gone on
			break
		CityStore.step_ticks(WAIT_STEP_TICKS)
	await cap._frames(1)
	await _look(cap, "44_trilho_perto", at, rail.along, rail.out, TRACK_NEAR)
	for tick in range(0, WAIT_TICKS, WAIT_STEP_TICKS):
		if _nearest_car(CityStore.query(&"transit_run"), at) < TRAIN_NEAR_M:
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
