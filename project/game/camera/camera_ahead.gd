## Where the view is heading (the Deoccluder's look ahead): how it has been moving, eased, and what the buildings will ask of it there, so the eye starts
## round a wall before it reaches it. A reaction to a wall already reached has to be a jolt.
class_name CameraAhead
extends RefCounted

const PARALLEL := 1e-6

var over := CameraDamp.new() # how far the whole view stands over the roof the point looked at is in
var out := CameraDamp.new() # the share of the line of sight taken off to leave a building
var _focus_rate := Vector3.ZERO
var _yaw_rate := 0.0
var _half_rate := 0.0 # of the logarithm
var _last_focus := Vector3.ZERO
var _last_yaw := 0.0
var _last_half := 1.0
var _rig: CityCamera


func _init(rig: CityCamera) -> void:
	_rig = rig


## At any distance, never inside a building: how far the view must stand over the roof at its point, and the share of the line of sight to take off.
func clear_of_buildings(base: Vector3, eye: Vector3, line: float, at_half: float) -> Vector2:
	var free: PackedVector3Array = _rig.queries.ask(&"eye_free", [base, eye, CameraProfile.eye_lift(_rig.cfg, at_half)])
	return Vector2(free[0].y - base.y, clampf(1.0 - free[1].distance_to(free[0]) / maxf(line, PARALLEL), 0.0, 1.0))


## Once a frame: the view moved towards what the buildings ask now and where it is heading. `dt` 0 (a jump) takes it all at once.
func settle(dt: float, base: Vector3, eye: Vector3, line: float) -> void:
	_note(dt)
	var cfg := _rig.cfg
	var clear := clear_of_buildings(base, eye, line, _rig.half)
	var ahead := coming() if dt > 0.0 else Vector2.ZERO
	var high := maxf(clear.x, ahead.x)
	var off := maxf(clear.y, ahead.y)
	if dt > 0.0:
		over.ease_to(high, dt, cfg.eye_out_s if high > over.x else cfg.eye_back_s)
		out.ease_to(off, dt, cfg.eye_out_s if off > out.x else cfg.eye_back_s)
	else:
		over.snap(high)
		out.snap(off)


## One frame of the view's course: `dt` 0 starts it anew.
func _note(dt: float) -> void:
	if dt <= 0.0:
		_focus_rate = Vector3.ZERO
		_yaw_rate = 0.0
		_half_rate = 0.0
	else:
		var keep := exp(-dt / _rig.cfg.trend_s)
		_focus_rate = (_rig.focus - _last_focus) / dt * (1.0 - keep) + _focus_rate * keep
		_yaw_rate = angle_difference(_last_yaw, _rig.yaw) / dt * (1.0 - keep) + _yaw_rate * keep
		_half_rate = log(_rig.half / _last_half) / dt * (1.0 - keep) + _half_rate * keep
	_last_focus = _rig.focus
	_last_yaw = _rig.yaw
	_last_half = _rig.half


## What the buildings will ask where the view is heading: nothing to read while it stands still.
func coming() -> Vector2:
	var cfg := _rig.cfg
	var moving := Vector2(_focus_rate.x, _focus_rate.z).length() / _rig.half + absf(_yaw_rate) + absf(_half_rate)
	if moving < cfg.trend_still:
		return Vector2.ZERO
	var ahead_s: float = cfg.look_ahead_s
	var at := _rig.focus + _focus_rate * ahead_s
	var at_half := clampf(_rig.half * exp(_half_rate * ahead_s), cfg.view_half_min, cfg.view_half_max)
	var base := Vector3(at.x, _rig.seen_height(at.x, at.z) + CameraProfile.eye_lift(cfg, at_half), at.z)
	var tilt := clampf(CameraProfile.profile_tilt(cfg, at_half) + _rig.far_tilt - deg_to_rad(cfg.far_tilt_deg), CameraProfile.min_tilt(cfg, at_half), deg_to_rad(cfg.max_tilt_deg))
	var away := CameraProfile.view_distance(cfg, at_half)
	var eye := base + Basis.from_euler(Vector3(-tilt, _rig.yaw + _yaw_rate * ahead_s, 0.0)).z * away
	return clear_of_buildings(base, eye, away, at_half)
