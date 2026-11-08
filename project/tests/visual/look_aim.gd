## The camera on somebody for a shot in a look plan: the walker nearest the map's middle, the clock stopped, so the same plan aims at
## a body in both games (the 4.0's plan picks the same with `sim.pedViews`); or a bus at its stop with people at its doors.
class_name LookAim
extends RefCounted

const WALKERS_READ := 4096
const STOP_STEP_M := 60.0 # the bus stops tried along a line through the busiest crossing, this far apart
const STOP_TRIES := 12
const OVER_STOPS := [20.0, 0.0, 60.0] # zoom, azimuth, elevation: the camera over the stops while the buses come, its window on them
const WAIT_STEP_TICKS := 10
const WAIT_TICKS := 12000
const DOOR_SIDE_M := 2.0 # the aim off the bus's middle to its kerb side: its doors and the footway in the middle of the frame


## `view` is the setView's zoom, azimuth and elevation.
static func stand_on_walker(rig: CityCamera, ui: UiDriver, view: Array) -> void:
	CityStore.dispatch(&"set_speed", [0])
	var best := Vector3.INF
	for w: Dictionary in CityStore.query(&"shot_targets", [WALKERS_READ]).walkers:
		if not w.fallen and Vector2(w.position.x, w.position.z).length() < Vector2(best.x, best.z).length():
			best = w.position
	print("walker at ", best)
	await PairCamera.stand(rig, ui, best, view[1], view[2], view[0])


## A bus line through stops on the streets of the busiest crossing (none laid yet: laid here), the simulation stepped until a bus at a
## stop has somebody stepping down from its middle door and somebody walking up to its front door (past the wait, any bus with
## somebody at its doors), then the camera on that bus: `view` is the zoom, the turn in degrees from its
## kerb side towards its front, and the elevation. False when no bus had anybody at its doors.
static func stand_on_bus_door(rig: CityCamera, ui: UiDriver, view: Array) -> bool:
	var ground: Vector3 = CityStore.query(&"traffic_probe", [1]).busy_crossing
	if CityStore.query(&"transit_plan").lines.is_empty():
		for i in STOP_TRIES:
			CityStore.dispatch(&"transit_stop", [ground + Vector3(STOP_STEP_M * (i - STOP_TRIES * 0.5), 0, 0)])
		var ids := PackedInt32Array()
		for s: Dictionary in CityStore.query(&"transit_plan").stops:
			ids.append(s.id)
		print("bus line through %d stops: %s" % [ids.size(), CityStore.dispatch(&"transit_line", [ids])])
		await PairCamera.stand(rig, ui, ground, OVER_STOPS[1], OVER_STOPS[2], OVER_STOPS[0])
	var run: Dictionary = CityStore.query(&"transit_run")
	var waited := 0
	while waited < WAIT_TICKS and not (int(run.stepping_off) > 0 and int(run.walking_up) > 0 and run.has("door_bus_at")):
		CityStore.step_ticks(WAIT_STEP_TICKS)
		waited += WAIT_STEP_TICKS
		await ui.frames(1)
		run = CityStore.query(&"transit_run")
	print("bus at its doors after %d ticks: %s" % [waited, run])
	if not run.has("door_bus_at"):
		print("FAIL no bus with people at its doors")
		return false
	var ahead: Vector3 = run.door_bus_heading
	var kerb := Vector3(-ahead.z, 0, ahead.x) # the side its doors open on (kerb_stops.cpp kerb_door_at)
	var turn := deg_to_rad(float(view[1]))
	var back := kerb * cos(turn) + ahead * sin(turn)
	await PairCamera.stand(rig, ui, run.door_bus_at + kerb * DOOR_SIDE_M, rad_to_deg(atan2(back.z, back.x)), view[2], view[0])
	return true
