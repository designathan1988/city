## A lorry at a dock for the visual capture (--only=doca), the way a player gets one: the generated town has no industry, so a
## new street is zoned industrial, grows, and the camera waits over it for a truck to be called, each step bounded.
class_name CaptureLorry
extends RefCounted

const ZONE_RADIUS_M := 160.0
const STREET_M := 300.0
const SAMPLE := 400
const PX := 0.25 # metres a screen pixel covers: how near a click snaps to a road or a node
const PROFILE := 0
const STEP_TICKS := 200
const GROW_STEPS := 60
const WAIT_STEPS := 150
const CAMERA_UP := 30.0
const CAMERA_BACK := 6.0


## A new street out of the far end of the town, where a works has the depth its yard needs (the lots of the built streets are too
## shallow): out from the vehicle farthest from the middle. Its middle.
static func _new_street() -> Vector3:
	var far := Vector3.ZERO
	for p: Vector3 in CityStore.query(&"traffic_probe", [SAMPLE]).sample:
		if Vector2(p.x, p.z).length() > Vector2(far.x, far.z).length():
			far = p
	var out := Vector3(far.x, 0, far.z).normalized()
	var end := far + out * STREET_M
	print("new road: ", CityStore.dispatch(&"build_road", [far, end, false, Vector3.ZERO, PROFILE, PX]))
	return (far + end) * 0.5


## `cap` is the capture scene (its camera, _frames and _shot).
static func shots(cap: Node) -> void:
	var at := _new_street()
	print("industrial zone at ", at, ": ", CityStore.dispatch(&"paint_zone", [at, ZONE_RADIUS_M, ZoneTable.code("industrial")]))
	cap.camera.global_position = at + Vector3(CAMERA_BACK, CAMERA_UP, CAMERA_BACK)
	for i in GROW_STEPS:
		await cap._frames(1)
		CityStore.step_ticks(STEP_TICKS)
		if CityStore.query(&"dock_count", []) > 0:
			break
	print("docks after growing: ", CityStore.query(&"dock_count", []))
	var seen := {}
	for i in WAIT_STEPS:
		await cap._frames(1)
		CityStore.step_ticks(STEP_TICKS)
		var s: Dictionary = CityStore.query(&"park_sample", [])
		var key := "%d %d %d" % [s.lorries_sent, s.lorries_docked, s.lorries_left]
		if not seen.has(key):
			seen[key] = true
			print("lorries (called, docked, left) ", key, " passo ", i)
			if s.has("lorry"):
				await CaptureSignals.at_hour(cap, cap.DAY) # the wait moved the clock on
				await cap._shot("36_doca_%d" % seen.size(), s.lorry + Vector3(CAMERA_BACK, CAMERA_UP, CAMERA_BACK), s.lorry)
		if s.lorries_left > 0:
			return
	print("capture: no lorry made the cycle in ", WAIT_STEPS * STEP_TICKS, " ticks")
	print(CityStore.query(&"park_sample", []))
