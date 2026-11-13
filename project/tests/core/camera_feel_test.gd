## How smooth the camera is over the real city, frame by frame at a fixed step: it walks in a straight line at each zoom and bearing, and each
## frame's move of the eye and of the point looked at is held against the one before, as a share of the view's height: what the eye sees as a jolt.
## A frame the machine lost (HITCH_EVERY) is held against the time it took, and a drag goes through the gestures as a mouse's events do: a few a frame, whole pixels, round a
## circle the hand can keep to. The worst frames say which of the rig's springs moved them. Run: godot --path project --resolution 640x360 res://tests/core/camera_feel_test.tscn -- --no-flow
extends TestCase

const MAP := 0
const SEED := 7
const STEP_S := 1.0 / 60.0
const WALK_FRAMES := 420
const HALVES := [3.0, 6.0, 12.0, 30.0, 80.0, 250.0]
const BEARINGS := [0.0, 35.0, 90.0, 200.0]
const TILTS := [48.0, 15.0]
const WORST := 8
const KEY_PX_S := 700.0
const SCREEN_H := 360.0
const STARTS := [Vector3(-60, 0, -60), Vector3(240, 0, 180)]
const TURN_RAD_S := 1.6
const ZOOM_FROM := 250.0
const ZOOM_STEP := 0.99
const INSIDE_SHARE := 0.001 # of the frames: a wall reached before the view could turn from it
const LIMIT := 0.01 # of the view's height from one frame's move to the next: 1% is a jolt anyone sees
const HITCH_EVERY := 47
const HITCH_FRAMES := 3.0 # a frame of 50 ms
const HAND_PX_S := 220.0
const HAND_CIRCLE_PX := 40.0
const EVENTS_A_FRAME := 3 # a mouse polled at 125 Hz or more, a frame at 60
const HAND_RAMP_S := 0.25 # no hand is at full speed the frame it starts
const GRAIN_PX := 2.0
const PRESS_MIDDLE := Vector2(0.5, 0.5)
const PRESS_HIGH := Vector2(0.5, 0.12) # near the horizon of a low view

var _rig: CityCamera
var _worst: Array = []
var _frames := 0
var _inside := 0
var _deepest := 0.0
var _snaps := 0 # frames where the walls took the eye out at once by more than a jolt: the eased eye trailed into one (CameraWalls.snapped)
var _largest_snap := 0.0 # the largest of those moves, a share of the view's height


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(MAP, SEED, true)
	ticks(1)
	_rig = CityCamera.new()
	add_child(_rig)
	_rig.set_process(false)
	_rig.set_process_input(false)
	_rig.set_process_unhandled_input(false)
	for half: float in HALVES:
		for tilt: float in TILTS:
			for yaw: float in BEARINGS:
				for start: Vector3 in STARTS:
					_walk(half, tilt, yaw, start, false)
					_walk(half, tilt, yaw, start, true)
					_turn(half, tilt, yaw, start)
					_drag(half, tilt, yaw, start, MOUSE_BUTTON_RIGHT, PRESS_MIDDLE)
					_drag(half, tilt, yaw, start, MOUSE_BUTTON_RIGHT, PRESS_HIGH)
					_drag(half, tilt, yaw, start, MOUSE_BUTTON_MIDDLE, PRESS_MIDDLE)
				_zoom(tilt, yaw, STARTS[0])
	_worst.sort_custom(func(a: Dictionary, b: Dictionary) -> bool: return a.jolt > b.jolt)
	for w: Dictionary in _worst.slice(0, WORST):
		print("%.4f half %.0f tilt %.0f yaw %.0f at %s: %s" % [w.jolt, w.half, w.tilt, w.yaw, w.at, w.parts])
	var over := _worst.filter(func(w: Dictionary) -> bool: return w.jolt > LIMIT).size()
	check(over == 0, "no ride jolts past %.3f of the view: %d of %d" % [LIMIT, over, _worst.size()])
	check(float(_inside) / _frames <= INSIDE_SHARE, "the eye stays out of the buildings: in one %d of %d frames, deepest %.3f of the view" % [_inside, _frames, _deepest])
	print("walls met at once (the eye taken out of a wall by more than a jolt): %d of %d frames, the largest %.3f of the view" % [_snaps, _frames, _largest_snap])
	check(float(_snaps) / _frames <= INSIDE_SHARE, "walls met at once in at most one frame in %d: %d of %d" % [roundi(1.0 / INSIDE_SHARE), _snaps, _frames])
	finish()


## How much of the line from the eye to the point looked at, in metres, lies in a building at the eye's end.
func _in_a_building(look: Vector3, eye: Vector3) -> float:
	var free: PackedVector3Array = CityStore.query(&"eye_free", [look, eye, 0.0, Vector3.ZERO]) # no margin: the walls themselves
	return free[1].distance_to(eye)


func _walk(half: float, tilt: float, yaw: float, start: Vector3, hitches: bool) -> void:
	var metres := KEY_PX_S / SCREEN_H * 2.0 * half * STEP_S
	_ride(half, tilt, yaw, start, func(frames: float) -> void: _rig.slide(0.0, metres * frames), hitches)


## The pointer round a circle from the middle of the view, in the whole pixels and the handful of events a frame the system gives.
func _drag(half: float, tilt: float, yaw: float, start: Vector3, button: MouseButton, share: Vector2) -> void:
	var at := get_viewport().get_visible_rect().size * share
	var press := InputEventMouseButton.new()
	press.button_index = button
	press.pressed = true
	press.position = at
	var angle := [0.0]
	var moving := [0.0]
	_rig.jump(start, yaw, tilt, CameraProfile.view_distance(_rig.cfg, half))
	_rig.gestures.press(press)
	# The hand's own grain, which a ride may not be held to: whole pixels make one frame's move differ from the last by two, a turn's swinging the eye about
	# what was pressed, a pan's running over the ground.
	var height: float = _rig.gestures.drag.height
	var held: Variant = CameraAim.hit_at(_rig, at, height)
	var next: Variant = CameraAim.hit_at(_rig, at + Vector2.UP * GRAIN_PX, height)
	var grain: float = 0.0
	if button == MOUSE_BUTTON_RIGHT:
		var about: Vector3 = held if held != null else _rig.focus # a pointer over the horizon turns the view about its centre
		grain = GRAIN_PX * _rig.gestures.rad_per_px * _rig.camera.global_position.distance_to(about) / (2.0 * half)
	elif held != null and next != null:
		grain = held.distance_to(next) / (2.0 * half)
	_ride(half, tilt, yaw, start, func(frames: float) -> void:
		moving[0] += STEP_S * frames
		for e in EVENTS_A_FRAME:
			angle[0] += HAND_PX_S / HAND_CIRCLE_PX * STEP_S * frames / EVENTS_A_FRAME * minf(moving[0] / HAND_RAMP_S, 1.0)
			var motion := InputEventMouseMotion.new()
			motion.position = (at + Vector2(sin(angle[0]), 1.0 - cos(angle[0])) * HAND_CIRCLE_PX).round()
			_rig.gestures.captured(motion)
		_rig.gestures.advance(STEP_S * frames), false, true, grain)
	_rig.gestures.cancel()


func _turn(half: float, tilt: float, yaw: float, start: Vector3) -> void:
	_ride(half, tilt, yaw, start, func(_frames: float) -> void:
		_rig.yaw += TURN_RAD_S * STEP_S
		_rig.apply())


func _zoom(tilt: float, yaw: float, start: Vector3) -> void:
	_ride(ZOOM_FROM, tilt, yaw, start, func(_frames: float) -> void:
		_rig.half *= ZOOM_STEP
		_rig.apply())


func _ride(half: float, tilt: float, yaw: float, start: Vector3, step: Callable, hitches := false, held := false, grain := 0.0) -> void:
	if not held:
		_rig.jump(start, yaw, tilt, CameraProfile.view_distance(_rig.cfg, half))
	var last_eye: Vector3 = _rig.camera.global_position
	var last_look: Vector3 = _rig.focus
	var move_eye := Vector3.ZERO
	var move_look := Vector3.ZERO
	var before := _rig.springs()
	var worst := {"jolt": 0.0}
	var ride_inside := 0 # this ride's frames with the eye in a building, and the first of them: where the rig let it in
	var first_inside := {}
	var met_last := false
	for f in WALK_FRAMES:
		var frames := HITCH_FRAMES if hitches and f % HITCH_EVERY == HITCH_EVERY - 1 else 1.0
		step.call(frames)
		_rig.settle(STEP_S * frames)
		var eye: Vector3 = _rig.camera.global_position
		var look: Vector3 = _rig.focus
		var now_eye := (eye - last_eye) / frames
		var now_look := (look - last_look) / frames
		var jolt := maxf((now_eye - move_eye).length(), (now_look - move_look).length()) / (2.0 * _rig.half)
		_frames += 1
		var inside := _in_a_building(look, eye)
		_inside += 1 if inside > 0.0 else 0
		_deepest = maxf(_deepest, inside / (2.0 * _rig.half))
		if inside > 0.0:
			ride_inside += 1
			if first_inside.is_empty():
				first_inside = {"frame": f, "depth_m": snappedf(inside, 0.01), "eye": eye.snappedf(0.1), "look": look.snappedf(0.1), "springs": _rig.springs()}
		var parts := _rig.springs()
		# a wall met at once: the eased eye trailed into a wall and the walls took it out this frame by more than a jolt's share of the
		# view; that frame and the next are the correction, counted apart (research R1). Every other frame, the eased corrections and the
		# margin's slide included, is held to the limit
		var met := f > 0 and _rig.solids.snapped > LIMIT * 2.0 * _rig.half
		if met:
			_snaps += 1
			_largest_snap = maxf(_largest_snap, _rig.solids.snapped / (2.0 * _rig.half))
		var counted := not met and not met_last
		met_last = met
		if f > 1 and counted and jolt > worst.jolt:
			var moved := {}
			for k: String in parts:
				if absf(parts[k] - before[k]) > 1e-3:
					moved[k] = snappedf(parts[k] - before[k], 0.001)
			worst = {"jolt": jolt, "half": half, "tilt": tilt, "yaw": yaw, "at": Vector2(look.x, look.z).snappedf(0.1), "parts": moved}
		before = parts
		last_eye = eye
		last_look = look
		move_eye = now_eye
		move_look = now_look
	worst.jolt = maxf(worst.jolt - grain, 0.0)
	if worst.jolt > LIMIT:
		print("  over the limit: ", worst)
	if ride_inside > 0:
		print("  eye in a building: %d frames, half %.0f tilt %.0f yaw %.0f from %s; first %s" % [ride_inside, half, tilt, yaw, start, first_inside])
	_worst.append(worst)
