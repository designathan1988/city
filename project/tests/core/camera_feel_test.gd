## How smooth the camera is over the real city, frame by frame at a fixed step: it walks in a straight line at each zoom and bearing, and each
## frame's move of the eye and of the point looked at is held against the one before, as a share of the view's height: what the eye sees as a jolt.
## The worst frames say which of the rig's springs moved them. Run: godot --path project --resolution 640x360 res://tests/core/camera_feel_test.tscn -- --no-flow
extends Node3D

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

var _rig: CityCamera
var _worst: Array = []
var _failed := 0
var _frames := 0
var _inside := 0
var _deepest := 0.0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	var errors := CityStore.boot(world.terrain, world.materials())
	if not errors.is_empty():
		push_error("boot: %s" % [errors])
		get_tree().quit(1)
		return
	CityStore.new_map(MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	CityStore.step_ticks(1)
	_rig = CityCamera.new()
	add_child(_rig)
	_rig.set_process(false)
	_rig.set_process_input(false)
	_rig.set_process_unhandled_input(false)
	for half: float in HALVES:
		for tilt: float in TILTS:
			for yaw: float in BEARINGS:
				for start: Vector3 in STARTS:
					_walk(half, tilt, yaw, start)
					_turn(half, tilt, yaw, start)
				_zoom(tilt, yaw, STARTS[0])
	_worst.sort_custom(func(a: Dictionary, b: Dictionary) -> bool: return a.jolt > b.jolt)
	for w: Dictionary in _worst.slice(0, WORST):
		print("%.4f half %.0f tilt %.0f yaw %.0f at %s: %s" % [w.jolt, w.half, w.tilt, w.yaw, w.at, w.parts])
	print("rides over the limit: ", _failed, " of ", _worst.size())
	print("eye in a building: %d of %d frames, deepest %.3f of the view" % [_inside, _frames, _deepest])
	if float(_inside) / _frames > INSIDE_SHARE:
		print("FAIL o olho fica dentro de prédio")
		_failed += 1
	print("FALHAS: ", _failed)
	get_tree().quit(1 if _failed > 0 else 0)


## How much of the line from the eye to the point looked at, in metres, lies in a building at the eye's end.
func _in_a_building(look: Vector3, eye: Vector3) -> float:
	var free: PackedVector3Array = CityStore.query(&"eye_free", [look, eye, 0.0])
	return free[1].distance_to(eye)


func _parts() -> Dictionary:
	return {"ground": _rig._ground.x, "rise": _rig._rise.x, "over": _rig.ahead.over.x, "out": _rig.ahead.out.x, "pull": _rig.pull.shown(), "floor": _rig._eye_ground}


func _walk(half: float, tilt: float, yaw: float, start: Vector3) -> void:
	var metres := KEY_PX_S / SCREEN_H * 2.0 * half * STEP_S
	_ride(half, tilt, yaw, start, func() -> void: _rig.slide(0.0, metres))


func _turn(half: float, tilt: float, yaw: float, start: Vector3) -> void:
	_ride(half, tilt, yaw, start, func() -> void:
		_rig.yaw += TURN_RAD_S * STEP_S
		_rig._apply())


func _zoom(tilt: float, yaw: float, start: Vector3) -> void:
	_ride(ZOOM_FROM, tilt, yaw, start, func() -> void:
		_rig.half *= ZOOM_STEP
		_rig._apply())


func _ride(half: float, tilt: float, yaw: float, start: Vector3, step: Callable) -> void:
	_rig.jump(start, yaw, tilt, CameraProfile.view_distance(_rig.cfg, half))
	var last_eye: Vector3 = _rig.camera.global_position
	var last_look: Vector3 = _rig.focus
	var move_eye := Vector3.ZERO
	var move_look := Vector3.ZERO
	var before := _parts()
	var worst := {"jolt": 0.0}
	for f in WALK_FRAMES:
		step.call()
		_rig._settle(STEP_S)
		var eye: Vector3 = _rig.camera.global_position
		var look: Vector3 = _rig.focus
		var now_eye := eye - last_eye
		var now_look := look - last_look
		var jolt := maxf((now_eye - move_eye).length(), (now_look - move_look).length()) / (2.0 * _rig.half)
		_frames += 1
		var inside := _in_a_building(look, eye)
		_inside += 1 if inside > 0.0 else 0
		_deepest = maxf(_deepest, inside / (2.0 * _rig.half))
		var parts := _parts()
		if f > 1 and jolt > worst.jolt:
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
	if worst.jolt > LIMIT:
		print("FAIL solavanco ", worst)
		_failed += 1
	_worst.append(worst)
