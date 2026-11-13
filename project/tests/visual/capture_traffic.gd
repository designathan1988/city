## The parked side of the traffic for the visual capture (--only=traffic): a kerb bay, a lot's bays, a car off the road and a
## bicycle on the edge of a street and in a cycle lane, each from nearly straight over a place the core's park_sample chose
## with no tall building round it: a camera at an angle would look through the low roofs beside a lot's bays.
class_name CaptureTraffic
extends RefCounted

const CAMERA_UP := 24.0
const CAMERA_BACK := 2.0
const SHOTS := [["30_kerb_bay", "kerb"], ["31_lot_bay", "lot"], ["32_car_manoeuvre", "moving"], ["33_bicycle", "bike"]]
const BAND_SHOT := "35_cycle_lane"
const KERB_WAIT_S := 300
const KERB_TICKS := 10
const KERB_FROM := Vector3(5, 3.5, 7)
const NEAR_M := 30.0
const BAND_STEP_TICKS := 100 # the camera stands over the cycle lane while the traffic runs, this much at a time
const BAND_STEPS := 60
const BAND_BEHIND := 9.0 # along the lane, behind the bicycle and over the road: from straight above a tree may cover it
const BAND_UP := 4.5


## A capture of its own instead of the tour (--only=dock, parity, boarding or traffic): whether there was one. `cap`: the capture scene.
static func directed(cap: Node, probe: Dictionary, ground: Vector3) -> bool:
	if cap.only.has("dock"):
		await CaptureLorry.shots(cap)
	elif cap.only.has(CaptureParity.ONLY):
		await CaptureParity.shots(cap, cap.pairs)
	elif cap.only.has("boarding"):
		await kerb_shot(cap)
	elif cap.only.has("traffic"):
		await CaptureVehicles.car_shots(cap, probe, ground) # first: the wait for a bicycle moves the cars of the sample on
		await shots(cap)
	else:
		return false
	return true


## Somebody between a door and the footway, the first the traffic shows, a bounded wait.
static func kerb_shot(cap: Node) -> void:
	for i in KERB_WAIT_S:
		CityStore.step_ticks(KERB_TICKS)
		var at: Variant = CityStore.query(&"kerb_busy", [])
		if at == null:
			continue
		cap.camera.global_position = at + KERB_FROM
		await cap._frames(1)
		CityStore.step_ticks(1)
		var now: Variant = CityStore.query(&"kerb_busy", [])
		await cap._shot("37_boarding", (now if now != null else at) + KERB_FROM, now if now != null else at)
		return
	print("capture: nobody waiting at the kerb")


## `cap` is the capture scene (its _shot and _frames).
static func shots(cap: Node) -> void:
	var sample: Dictionary = CityStore.query(&"park_sample", [])
	print("park_sample: ", sample)
	print("docks on the map: ", CityStore.query(&"dock_count", []))
	print("cars' clearance in the lots: ", CityStore.query(&"lot_clearance", []))
	for key in ["kerb", "moving"]:
		if sample.has(key):
			print("vehicles near ", key, ": ", CityStore.query(&"vehicles_near", [sample[key], NEAR_M]))
	for s in SHOTS:
		if not sample.has(s[1]):
			print("capture: no target ", s[1], " with no tall building near")
			continue
		await _over(cap, s[0], sample[s[1]])
	if sample.has("band"):
		await _band_shot(cap, sample.band)
	else:
		print("capture: no cycle lane with no tall building near")


static func _over(cap: Node, name: String, at: Vector3) -> void:
	await cap._shot(name, at + Vector3(CAMERA_BACK, CAMERA_UP, CAMERA_BACK), at)


## Stands over the cycle lane until a bicycle rides one near the camera, a bounded wait; the empty lane if none comes.
static func _band_shot(cap: Node, band: Vector3) -> void:
	cap.camera.global_position = band + Vector3(CAMERA_BACK, CAMERA_UP, CAMERA_BACK)
	for i in BAND_STEPS:
		await cap._frames(1)
		var sample: Dictionary = CityStore.query(&"park_sample", [])
		if sample.has("bike_band"):
			print("a bicycle in the cycle lane after ", i * BAND_STEP_TICKS, " ticks")
			await CaptureSignals.at_hour(cap, cap.DAY) # the wait moved the clock on
			var at: Vector3 = sample.bike_band
			var way: Vector3 = sample.bike_band_dir
			await cap._shot(BAND_SHOT, at - way * BAND_BEHIND + Vector3.UP * BAND_UP, at)
			return
		CityStore.step_ticks(BAND_STEP_TICKS)
	print("capture: no bicycle in the cycle lane at ", BAND_STEPS * BAND_STEP_TICKS, " ticks")
	await CaptureSignals.at_hour(cap, cap.DAY)
	await _over(cap, BAND_SHOT, band)
