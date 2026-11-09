## Visual capture of the vehicles themselves: every kind close up from three sides by day and by night (the lamps), the
## indicators of the cars turning or about to, nearest the busiest crossing, over a flash (a burst of shots: each lens lit on its own side), and the
## traffic flowing over that crossing, day and night. The city and its warm-up are capture.gd's.
extends "res://tests/visual/capture.gd"

## Per angle: name and the camera in the vehicle's frame (x to its left, y up, z ahead), looked at this high over its middle.
const ANGLES := [["frente", Vector3(3.0, 1.8, 7.0)], ["tras", Vector3(-3.0, 1.8, -7.0)], ["lado", Vector3(-6.5, 1.4, 0.5)]]
const LOOK_UP_M := 0.8
const HEAVY_SCALE := 1.8 # a bus or truck is seen from this much farther
## The indicator burst: the cars on a connector or within the distance a turn is signalled from (traffic.json turn_signal_min_m)
## nearest the crossing, a shot every few frames over more than one flash of 1.5 Hz.
const SIGNAL_CARS := 4
const SIGNAL_NEAR_M := 25.0
const SIGNAL_SHOTS := 5
const SIGNAL_FRAMES := 9
const SIGNAL_FROM := Vector3(-2.5, 2.2, -6.5)
## The crossing from above its corner, and from the street.
const FLOW_FROM := Vector3(18, 22, 26)
const FLOW_LOW := Vector3(6, 3, 14)


func _capture_all(probe: Dictionary, ground: Vector3) -> void:
	var daylight := DataTables.game("daylight")
	for when: Array in [["dia", float(daylight.day_hour)], ["noite", float(daylight.night_hour)]]:
		await CaptureSignals.at_hour(self, when[1])
		await _kinds(probe, ground, when[0])
		await _signals(probe, when[0])
		await _flow(when[0])


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
	var order: Array = range(probe.sample.size())
	order.sort_custom(func(a: int, b: int) -> bool: return (probe.sample[a] as Vector3).distance_to(cross) < (probe.sample[b] as Vector3).distance_to(cross))
	var taken := 0
	for k: int in order:
		if taken >= SIGNAL_CARS:
			break
		if int(probe.sample_type[k]) >= CityStore.code(&"VehicleType", &"MOTO") or float(probe.sample_mid_m[k]) > SIGNAL_NEAR_M:
			continue
		var car: Vector3 = probe.sample[k]
		for i in SIGNAL_SHOTS:
			await _shot("pisca_%d_%d_%s" % [taken, i, when], CaptureVehicles._eye(probe, k, SIGNAL_FROM), car + Vector3.UP * LOOK_UP_M)
			await _frames(SIGNAL_FRAMES)
		taken += 1


func _flow(when: String) -> void:
	var cross: Vector3 = CityStore.query(&"traffic_probe", [0]).get("busy_crossing", Vector3.ZERO)
	await _shot("fluxo_alto_%s" % when, cross + FLOW_FROM, cross)
	await _shot("fluxo_rua_%s" % when, cross + FLOW_LOW, cross + Vector3.UP)
