## Visual capture of the vehicles themselves: every kind close up from three sides by day and by night (the lamps), the
## indicators of the cars signalling nearest the busiest crossing, over a flash (a burst of shots: each lens lit on its own side), and the
## traffic flowing over that crossing, day and night; a car's tail smoke, the front wheels of cars turning, and the far traffic from high.
## The city and its warm-up are capture.gd's.
extends "res://tests/visual/capture.gd"

## Per angle: name and the camera in the vehicle's frame (x to its left, y up, z ahead), looked at this high over its middle.
const ANGLES := [["frente", Vector3(3.0, 1.8, 7.0)], ["tras", Vector3(-3.0, 1.8, -7.0)], ["lado", Vector3(-6.5, 1.4, 0.5)]]
const LOOK_UP_M := 0.8
const HEAVY_SCALE := 1.8 # a bus or truck is seen from this much farther
## The indicator burst: the cars signalling now (traffic_probe sample_signal) nearest the crossing, from behind and ahead on the side
## they signal, a shot every few frames over more than one flash of 1.5 Hz; without one, the traffic stepped on until one signals.
const SIGNAL_CARS := 3
const SIGNAL_SHOTS := 4
const SIGNAL_FRAMES := 9
const SIGNAL_FROM_LEFT := Vector3(2.2, 1.6, -5.5)
const SIGNAL_FROM_RIGHT := Vector3(-2.2, 1.6, -5.5)
const SIGNAL_WAIT_STEPS := 30
const SIGNAL_WAIT_TICKS := 20
## The crossing from above its corner, and from the street.
const FLOW_FROM := Vector3(18, 22, 26)
const FLOW_LOW := Vector3(6, 3, 14)
## Tail smoke: a car behind, low, after the frames its puffs take to rise (puffs live 2.6 s).
const SMOKE_FROM := Vector3(-1.2, 0.9, -4.5)
const SMOKE_FRAMES := 120
## The front wheels: a car on a connector (traffic_probe sample_mid_m 0), from ahead at the side, low at the wheel.
const STEER_FROM := Vector3(2.6, 0.7, 2.6)
const STEER_CARS := 2
## The far traffic: the city from high over its middle, looking out past the near reach.
const FAR_FROM := Vector3(0, 380, 900)
const FAR_AT := Vector3(0, 0, -900)


func _capture_all(probe: Dictionary, ground: Vector3) -> void:
	var daylight := DataTables.game("daylight")
	for when: Array in [["dia", float(daylight.day_hour)], ["noite", float(daylight.night_hour)]]:
		await CaptureSignals.at_hour(self, when[1])
		probe = CityStore.query(&"traffic_probe", [TRAFFIC_SAMPLE]) # where the cars stand now (the wait for a signal moves them)
		await _kinds(probe, ground, when[0])
		await _signals(probe, when[0])
		probe = CityStore.query(&"traffic_probe", [TRAFFIC_SAMPLE])
		await _smoke(probe, ground, when[0])
		await _steer(probe, ground, when[0])
		await _flow(when[0])
		await _far(ground, when[0])


func _kinds(probe: Dictionary, ground: Vector3, when: String) -> void:
	for kind: StringName in CaptureVehicles.VIEWS:
		var type := CityStore.code(&"VehicleType", kind)
		var k := CaptureVehicles._nearest_of(probe, type, ground)
		if k < 0:
			print("capture: no vehicle of kind ", kind, " in the sample")
			continue
		var scale := HEAVY_SCALE if kind == &"BUS" or kind == &"TRUCK" else 1.0
		var at: Vector3 = probe.sample[k]
		for a: Array in ANGLES:
			await _shot("v_%s_%s_%s" % [String(kind).to_lower(), a[0], when], CaptureVehicles._eye(probe, k, a[1] * scale), at + Vector3.UP * LOOK_UP_M * scale)


func _signals(probe: Dictionary, when: String) -> void:
	var cross: Vector3 = CityStore.query(&"traffic_probe", [0]).get("busy_crossing", Vector3.ZERO)
	var tries := 0
	while _signalling(probe, cross).is_empty() and tries < SIGNAL_WAIT_STEPS: # a bounded wait for a car to signal
		CityStore.step_ticks(SIGNAL_WAIT_TICKS)
		probe = CityStore.query(&"traffic_probe", [TRAFFIC_SAMPLE])
		tries += 1
	print("signalling (left, right): ", probe.signalling)
	var taken := 0
	for k: int in _signalling(probe, cross):
		var car: Vector3 = probe.sample[k]
		var from: Vector3 = SIGNAL_FROM_LEFT if int(probe.sample_signal[k]) == int(CityStore.constants.shader.SIGNAL_LEFT) else SIGNAL_FROM_RIGHT
		for view: Array in [["tras", from], ["frente", Vector3(from.x, from.y, -from.z)]]:
			for i in SIGNAL_SHOTS:
				await _shot("pisca_%d_%s_%d_%s" % [taken, view[0], i, when], CaptureVehicles._eye(probe, k, view[1]), car + Vector3.UP * LOOK_UP_M)
				await _frames(SIGNAL_FRAMES)
		taken += 1


## The cars of the sample signalling now, nearest the crossing first, at most SIGNAL_CARS.
func _signalling(probe: Dictionary, cross: Vector3) -> Array:
	var found: Array = []
	for k in probe.sample.size():
		if int(probe.sample_signal[k]) != 0 and int(probe.sample_type[k]) < CityStore.code(&"VehicleType", &"MOTO"):
			found.append(k)
	found.sort_custom(func(a: int, b: int) -> bool: return (probe.sample[a] as Vector3).distance_to(cross) < (probe.sample[b] as Vector3).distance_to(cross))
	return found.slice(0, SIGNAL_CARS)


func _smoke(probe: Dictionary, ground: Vector3, when: String) -> void:
	var k := CaptureVehicles._nearest_of(probe, CityStore.code(&"VehicleType", &"SEDAN"), ground)
	if k < 0:
		return
	var car: Vector3 = probe.sample[k]
	await _shot("fumaca_%s" % when, CaptureVehicles._eye(probe, k, SMOKE_FROM), car + Vector3.UP * LOOK_UP_M)
	await _frames(SMOKE_FRAMES)
	await _shot("fumaca_depois_%s" % when, CaptureVehicles._eye(probe, k, SMOKE_FROM), car + Vector3.UP * LOOK_UP_M)


func _steer(probe: Dictionary, ground: Vector3, when: String) -> void:
	var taken := 0
	for k in probe.sample.size():
		if taken >= STEER_CARS:
			break
		if float(probe.sample_mid_m[k]) != 0.0 or int(probe.sample_type[k]) >= CityStore.code(&"VehicleType", &"MOTO"):
			continue
		var car: Vector3 = probe.sample[k]
		if car.distance_to(ground) > FAR_FROM.z:
			continue
		await _shot("roda_%d_%s" % [taken, when], CaptureVehicles._eye(probe, k, STEER_FROM), car + Vector3.UP * LOOK_UP_M * 0.5)
		taken += 1


func _far(ground: Vector3, when: String) -> void:
	await _shot("longe_%s" % when, ground + FAR_FROM, ground + FAR_AT)
	print("far traffic drawn: ", CityStore.query(&"traffic_probe", [0]).get("drawn_far_vehicles", -1))


func _flow(when: String) -> void:
	var cross: Vector3 = CityStore.query(&"traffic_probe", [0]).get("busy_crossing", Vector3.ZERO)
	await _shot("fluxo_alto_%s" % when, cross + FLOW_FROM, cross)
	await _shot("fluxo_rua_%s" % when, cross + FLOW_LOW, cross + Vector3.UP)
