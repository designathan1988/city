## The pair of a 4.0 photo (--only=paridade --pairs=<file>): each entry of the file says, in a vehicle's own frame, where the 4.0's
## camera stood and how it looked; the same class here, the same pose, the same field of view, at the hour of the photo.
class_name CaptureParity
extends RefCounted

const CAMERA_CLEAR_M := 0.5
const SAMPLE := 800
const MID_BLOCK_M := 25.0 # well clear of a junction's posts and of the cars queued at it
const SIGHT_CLEAR_M := 2.5 # no other vehicle this near the line from the camera to the vehicle
const WINDOW_TICKS := 2 # the near model and the poses follow the camera only as the simulation steps


## `cap` is the capture scene (its camera, world and _shot).
static func shots(cap: Node, file: String) -> void:
	var pairs: Array = JSON.parse_string(FileAccess.get_file_as_string(file))
	var fov: float = cap.camera.fov
	for pair: Dictionary in pairs:
		await CaptureSignals.at_hour(cap, pair.hour)
		var probe: Dictionary = CityStore.query(&"traffic_probe", [SAMPLE])
		var k := _pick(probe, pair)
		if k < 0:
			print("captura: nenhum veículo para ", pair.name)
			continue
		var was: Vector3 = probe.sample[k]
		cap.camera.global_position = _eye(probe, k, pair)
		await cap._frames(1)
		CityStore.step_ticks(WINDOW_TICKS)
		probe = CityStore.query(&"traffic_probe", [SAMPLE])
		k = _nearest(probe, pair, was)
		var ahead: Vector3 = probe.sample_dir[k]
		var left := Vector3(ahead.z, 0, -ahead.x)
		var eye := _eye(probe, k, pair)
		var look: Vector3 = ahead * pair.look[0] + left * pair.look[1] + Vector3.UP * pair.look[2]
		cap.camera.fov = pair.fov
		await cap._shot(pair.name, eye, eye + look * pair.reach)
	cap.camera.fov = fov


static func _eye(probe: Dictionary, k: int, pair: Dictionary) -> Vector3:
	var at: Vector3 = probe.sample[k]
	var ahead: Vector3 = probe.sample_dir[k]
	var left := Vector3(ahead.z, 0, -ahead.x)
	return at + ahead * pair.eye[0] + left * pair.eye[1] + Vector3.UP * pair.eye[2]


## The sample's vehicle of the pair's class whose camera point has room and sees it, in the middle of a block if one is.
static func _pick(probe: Dictionary, pair: Dictionary, mid := MID_BLOCK_M) -> int:
	for i in probe.sample.size():
		if int(probe.sample_type[i]) != int(pair.type) or float(probe.sample_mid_m[i]) < mid:
			continue
		var eye := _eye(probe, i, pair)
		if CityStore.query(&"camera_clear", [eye, CAMERA_CLEAR_M]) and _sees(probe, i, eye):
			return i
	return -1 if mid == 0.0 else _pick(probe, pair, 0.0)


static func _sees(probe: Dictionary, k: int, eye: Vector3) -> bool:
	var at: Vector3 = probe.sample[k]
	for j in probe.sample.size():
		if j == k:
			continue
		var other: Vector3 = probe.sample[j]
		var near := Geometry3D.get_closest_point_to_segment(other, eye, at)
		if near.distance_to(other) < SIGHT_CLEAR_M:
			return false
	return true


## The vehicle of the pair's class nearest to where it was before the steps.
static func _nearest(probe: Dictionary, pair: Dictionary, was: Vector3) -> int:
	var best := -1
	for i in probe.sample.size():
		if int(probe.sample_type[i]) != int(pair.type):
			continue
		if best < 0 or (probe.sample[i] as Vector3).distance_to(was) < (probe.sample[best] as Vector3).distance_to(was):
			best = i
	return best
