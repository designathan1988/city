## Public transport in the capture (--only=transporte): a bus line on the busiest street and a train line beside the town, laid through
## the store as the player's tools do; the shelter, the platform and the train from close by, at 16:00.
class_name CaptureTransit
extends RefCounted

const TRAIN := 1
const STEP_M := 60.0
const STOPS := 12
const TRACK_OUT_M := 80.0
const TRACK_LONG_M := 500.0
const RUN_TICKS := 1500
const NEAR := Vector3(14, 7, 18)
const HOUR := 16.0


## `cap` is the capture scene (its _shot and world).
static func shots(cap: Node) -> void:
	await CaptureSignals.at_hour(cap, HOUR)
	var ground: Vector3 = CityStore.query(&"traffic_probe", [1]).busy_crossing
	for i in STOPS:
		CityStore.dispatch(&"transit_stop", [ground + Vector3(STEP_M * (i - STOPS / 2), 0, 0)])
	var bus := PackedInt32Array()
	for s: Dictionary in CityStore.query(&"transit_plan").stops:
		bus.append(s.id)
	CityStore.dispatch(&"transit_line", [bus])
	var b: Dictionary = CityStore.query(&"city_bounds")
	var a := Vector3(b.max.x + TRACK_OUT_M, 0, b.min.z)
	var e := a + Vector3(0, 0, TRACK_LONG_M)
	CityStore.dispatch(&"transit_track", [PackedVector3Array([a, e]), TRAIN])
	CityStore.dispatch(&"transit_station", [a.lerp(e, 0.15), TRAIN])
	CityStore.dispatch(&"transit_station", [a.lerp(e, 0.85), TRAIN])
	var rail := PackedInt32Array()
	var plan: Dictionary = CityStore.query(&"transit_plan")
	for s: Dictionary in plan.stops:
		if int(s.mode) == TRAIN:
			rail.append(s.id)
	CityStore.dispatch(&"transit_line", [rail])
	cap.camera.global_position = plan.stops[0].at + NEAR
	await cap._frames(1)
	CityStore.step_ticks(RUN_TICKS)
	print("transporte: ", CityStore.query(&"transit_run"))
	var stop: Vector3 = plan.stops[0].at
	await cap._shot("40_parada", stop + NEAR, stop)
	var station: Vector3 = plan.stops[plan.stops.size() - 2].at
	await cap._shot("41_estacao", station + NEAR, station)
	var mid := a.lerp(e, 0.5)
	await cap._shot("42_trilho", mid + NEAR * 2.0, mid)
