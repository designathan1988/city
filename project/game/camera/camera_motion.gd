## The camera's motion over time: held keys and wheel notches turned into a glide.
## W A S D / arrows pan, Q/E turn (Shift: a quarter turn), Page Up/Down tilt, + / - zoom about the centre, Shift held
## goes `key_shift` times faster, Home resets. A held key eases to its speed and back (time constant `key_ease_s`);
## each wheel notch is spent on a spring (`wheel_smooth_s`) about the ground point it was turned over.
class_name CameraMotion
extends RefCounted

## Key (physical) -> [axis, sign]. Q turns the view to the left: a turn of -1, which is +yaw here (`step`).
const KEYS := {
	KEY_W: [&"forward", 1], KEY_UP: [&"forward", 1], KEY_S: [&"forward", -1], KEY_DOWN: [&"forward", -1],
	KEY_A: [&"right", -1], KEY_LEFT: [&"right", -1], KEY_D: [&"right", 1], KEY_RIGHT: [&"right", 1],
	KEY_Q: [&"turn", -1], KEY_E: [&"turn", 1], KEY_PAGEUP: [&"tilt", 1], KEY_PAGEDOWN: [&"tilt", -1],
	KEY_EQUAL: [&"zoom", 1], KEY_KP_ADD: [&"zoom", 1], KEY_MINUS: [&"zoom", -1], KEY_KP_SUBTRACT: [&"zoom", -1],
}
const PCT := 0.01
const AXES: Array[StringName] = [&"forward", &"right", &"turn", &"tilt", &"zoom"]

var rig: CityCamera
var cfg: Dictionary
var held := {}
var velocity := {}
var shift := false
## Zoom still to spend (log factor), the pointer it is about and the ground point it holds there.
var pending_zoom := CameraDamp.new()
var zoom_point := Vector2.ZERO
var zoom_height := 0.0
var zoom_grabbed: Variant = null
## Yaw still to spend (Shift+Q/E, popover steps), radians; aiming: turning to north.
var pending_turn := CameraDamp.new()
var aiming := false
var aimed_for := 0.0


func _init(camera_rig: CityCamera) -> void:
	rig = camera_rig
	cfg = rig.cfg
	stop()


func stop() -> void:
	held.clear()
	for axis in AXES:
		velocity[axis] = 0.0
	pending_zoom.snap(0.0)
	pending_turn.snap(0.0)
	aiming = false


func moving() -> bool:
	return not held.is_empty() or pending_zoom.x != 0.0 or pending_turn.x != 0.0 or aiming \
			or AXES.any(func(a: StringName) -> bool: return velocity[a] != 0.0)


## A turn to spend smoothly, radians of yaw.
func turn(angle: float) -> void:
	pending_turn.x += angle


func aim_north() -> void:
	aiming = true
	aimed_for = 0.0


## Keys and the wheel: true when the event was the camera's.
func input(event: InputEvent) -> bool:
	if event is InputEventMouseButton and event.pressed:
		var dir := 0.0
		if event.button_index == MOUSE_BUTTON_WHEEL_UP:
			dir = 1.0
		elif event.button_index == MOUSE_BUTTON_WHEEL_DOWN:
			dir = -1.0
		if dir == 0.0:
			return false
		wheel(dir * cfg.wheel_log_per_notch * (event.factor if event.factor > 0.0 else 1.0), event.position)
		return true
	if event is InputEventKey:
		return _key(event)
	return false


func _key(k: InputEventKey) -> bool:
	var code := k.physical_keycode if k.physical_keycode != KEY_NONE else k.keycode
	if not k.pressed:
		held.erase(code)
		shift = k.shift_pressed
		return false
	if k.ctrl_pressed or k.alt_pressed or k.meta_pressed:
		return false
	if code == KEY_HOME:
		rig.reset_view()
		return true
	if not KEYS.has(code):
		return false
	if k.shift_pressed and (code == KEY_Q or code == KEY_E):
		if not k.echo:
			turn(deg_to_rad(cfg.quarter_turn_deg) * (1.0 if code == KEY_Q else -1.0))
		return true
	held[code] = KEYS[code]
	shift = k.shift_pressed
	return true


## A notch of the wheel: `log_factor` of zoom (positive: closer) about the pointer at `at`.
func wheel(log_factor: float, at: Vector2) -> void:
	if pending_zoom.x == 0.0 or at.distance_to(zoom_point) > cfg.wheel_new_point_px:
		var ground := CameraAim.pointer_ground(rig,at)
		var h := CameraAim.hold_height(rig, at, ground)
		zoom_point = at
		zoom_height = h
		zoom_grabbed = CameraAim.hit_at(rig, at, h)
	pending_zoom.x += log_factor


## One frame of glide, `dt` seconds (the exact closed form of the eased speed, so any frame length is right).
func step(dt: float) -> void:
	# A release the interface took (a focused field) must not leave the key held.
	for code in held.keys():
		if not Input.is_physical_key_pressed(code):
			held.erase(code)
	if not moving():
		return
	var goal := {}
	for axis in AXES:
		goal[axis] = 0.0
	for b in held.values():
		goal[b[0]] += b[1]
	var keep := exp(-dt / cfg.key_ease_s)
	var travel := {}
	for axis in AXES:
		var g: float = clampf(goal[axis], -1.0, 1.0) * (cfg.key_shift if shift else 1.0)
		var v0: float = velocity[axis]
		travel[axis] = g * dt + (v0 - g) * cfg.key_ease_s * (1.0 - keep)
		var v := g + (v0 - g) * keep
		velocity[axis] = 0.0 if g == 0.0 and absf(v) < cfg.key_stop_pct * PCT else v
	if travel.forward != 0.0 or travel.right != 0.0:
		var units: float = cfg.key_pan_px_s / CameraAim.px_per_m(rig)
		rig.slide(travel.right * units, travel.forward * units)
	var d_yaw: float = -travel.turn * cfg.key_turn_rad_s + _spend_turn(dt)
	var d_tilt: float = travel.tilt * cfg.key_tilt_rad_s
	if d_yaw != 0.0 or d_tilt != 0.0:
		rig.orbit(d_yaw, d_tilt)
	var centre := rig.get_viewport().get_visible_rect().size * 0.5
	if travel.zoom != 0.0:
		rig.zoom_at(centre, exp(travel.zoom * cfg.key_zoom_log_s), CameraAim.pointer_ground(rig,centre))
	_spend_zoom(dt, centre)


## What a remainder gives up this frame: it goes to nothing on a spring, so a notch or a turn starts and ends without a jerk.
func _spent(left: CameraDamp, dt: float, smooth_s: float) -> float:
	var before := left.x
	left.to(0.0, dt, smooth_s)
	if absf(left.x) < deg_to_rad(cfg.spend_snap_deg) and absf(left.v) < deg_to_rad(cfg.spend_snap_deg):
		left.snap(0.0)
	return before - left.x


func _spend_turn(dt: float) -> float:
	var spend := 0.0
	if pending_turn.x != 0.0:
		spend += _spent(pending_turn, dt, cfg.turn_smooth_s)
	if aiming:
		var left := angle_difference(rig.yaw, 0.0)
		aimed_for += dt
		if absf(left) < deg_to_rad(cfg.aim_close_deg) or aimed_for > cfg.aim_max_s:
			aiming = false
		else:
			spend += left if absf(left) < deg_to_rad(cfg.aim_snap_deg) else left * (1.0 - exp(-dt / cfg.turn_smooth_s))
	return spend


func _spend_zoom(dt: float, centre: Vector2) -> void:
	if pending_zoom.x == 0.0:
		return
	var s := _spent(pending_zoom, dt, cfg.wheel_smooth_s)
	if zoom_grabbed != null:
		rig.zoom_at(zoom_point, exp(s), zoom_height, zoom_grabbed)
	else:
		rig.zoom_at(centre, exp(s), CameraAim.pointer_ground(rig,centre))
