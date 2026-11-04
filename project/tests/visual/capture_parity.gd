## The pair of a 4.0 photo (--only=paridade --pairs=<file>): each entry of the file says, in a vehicle's own frame, where the 4.0's
## camera stood and how it looked; the same class here, the same pose, the same field of view, at the hour of the photo.
class_name CaptureParity
extends RefCounted

const CAMERA_CLEAR_M := 0.5
const SAMPLE := 800
const MID_BLOCK_M := 25.0 # well clear of a junction's posts and of the cars queued at it
const STOPPED_MS := 0.5 # a moving vehicle is drawn behind where the sample has it
const SIGHT_CLEAR_M := 2.5 # no other vehicle this near the line from the camera to the vehicle
const SIGHT_STEP_M := 1.0
const SIGHT_NEAR := 0.6 # of the distance to the vehicle: nearer than this the sight line is the vehicle itself
const WINDOW_TICKS := 2 # the near model and the poses follow the camera only as the simulation steps
const TRIES := 8 # vehicles tried when the one chosen no longer has the view once the window has followed the camera
const DRAW_FRAMES := 30 # after the jump: the world first asks for the new view


## `cap` is the capture scene (its camera, world and _shot).
static func shots(cap: Node, file: String) -> void:
	var pairs: Array = JSON.parse_string(FileAccess.get_file_as_string(file))
	var fov: float = cap.camera.fov
	for pair: Dictionary in pairs:
		await CaptureSignals.at_hour(cap, pair.hour)
		await _pair(cap, pair)
	cap.camera.fov = fov


## The pair's photo from the first vehicle that still has the view once the camera stands there.
static func _pair(cap: Node, pair: Dictionary) -> void:
	var tried: Array[Vector3] = []
	for attempt in TRIES:
		var probe: Dictionary = CityStore.query(&"traffic_probe", [SAMPLE])
		var k := _pick(probe, pair, tried)
		if k < 0:
			break
		var was: Vector3 = probe.sample[k]
		tried.append(was)
		cap.camera.global_position = _eye(probe, k, pair)
		await CaptureWait.until_drawn(cap, DRAW_FRAMES, pair.name + " salto")
		CityStore.step_ticks(WINDOW_TICKS)
		probe = CityStore.query(&"traffic_probe", [SAMPLE])
		k = _nearest(probe, pair, was)
		var eye := _eye(probe, k, pair)
		if not CityStore.query(&"camera_clear", [eye, CAMERA_CLEAR_M]) or not _sees(probe, k, eye, pair):
			continue
		var ahead: Vector3 = probe.sample_dir[k]
		var left := Vector3(ahead.z, 0, -ahead.x)
		var look: Vector3 = ahead * pair.look[0] + left * pair.look[1] + Vector3.UP * pair.look[2]
		cap.camera.fov = pair.fov
		await cap._shot(pair.name, eye, eye + look * pair.reach)
		print("paridade: ", pair.name, " tentativa ", attempt + 1, " em ", eye)
		return
	print("captura: nenhum veículo para ", pair.name)


static func _eye(probe: Dictionary, k: int, pair: Dictionary) -> Vector3:
	var at: Vector3 = probe.sample[k]
	var ahead: Vector3 = probe.sample_dir[k]
	var left := Vector3(ahead.z, 0, -ahead.x)
	return at + ahead * pair.eye[0] + left * pair.eye[1] + Vector3.UP * pair.eye[2]


## The sample's vehicle of the pair's class whose camera point has room and sees it: standing, in the middle of a block, if one is.
static func _pick(probe: Dictionary, pair: Dictionary, tried: Array[Vector3], mid := MID_BLOCK_M, top := STOPPED_MS) -> int:
	for i in probe.sample.size():
		if int(probe.sample_type[i]) != int(pair.type) or float(probe.sample_mid_m[i]) < mid or float(probe.sample_speed[i]) > top:
			continue
		if tried.has(probe.sample[i]):
			continue
		var eye := _eye(probe, i, pair)
		if CityStore.query(&"camera_clear", [eye, CAMERA_CLEAR_M]) and _sees(probe, i, eye, pair):
			return i
	if mid > 0.0:
		return _pick(probe, pair, tried, 0.0, top)
	return -1 if top > STOPPED_MS else _pick(probe, pair, tried, MID_BLOCK_M, INF)


## Nothing along the whole line from the camera, metre by metre, up to where the vehicle itself begins.
static func _sees(probe: Dictionary, k: int, eye: Vector3, pair: Dictionary) -> bool:
	var at: Vector3 = probe.sample[k]
	var far := eye.distance_to(at)
	var walked := SIGHT_STEP_M
	while far - walked > float(pair.reach) * SIGHT_NEAR:
		if not CityStore.query(&"camera_clear", [eye.lerp(at, walked / far), CAMERA_CLEAR_M]):
			return false
		walked += SIGHT_STEP_M
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
