## City camera rig, a perspective rig: orbit about a ground point with a yaw, a
## player tilt and `half`, the half height of the ground in view there. The lens, the tilt and the eye height follow
## `half` (CameraProfile), the point looked at follows the terrain, the eye never goes under the ground. Every move
## that holds a ground point under the pointer goes through `keeping`. Mouse gestures: CameraGestures; keys and
## wheel: CameraMotion. Values come from data/game/camera.json.
class_name CityCamera
extends Node3D

## The right button clicked without a drag: a right click cancels the gesture in progress.
signal right_clicked

## Below this the ray runs along the plane and meets it nowhere.
const PARALLEL := 1e-6
const CM := 0.01

var cfg: Dictionary
var camera: Camera3D
## The point looked at: ground height plus the eye lift.
var focus := Vector3.ZERO
var yaw := 0.0
## The tilt in use (derived from far_tilt and half), radians; pi/2 looks straight down.
var pitch := 0.0
## The player's tilt as it would be far away.
var far_tilt := 0.0
var half := 0.0
var distance := 0.0
var gestures: CameraGestures
var motion: CameraMotion
var _ground_y := 0.0


func _ready() -> void:
	add_to_group(&"camera_rig")
	cfg = DataTables.game("camera")
	camera = Camera3D.new()
	camera.far = cfg.far
	add_child(camera)
	camera.current = true
	gestures = CameraGestures.new(self)
	motion = CameraMotion.new(self)
	yaw = deg_to_rad(cfg.start_yaw)
	far_tilt = deg_to_rad(cfg.start_pitch)
	half = cfg.start_view_half
	_apply()


func look_at_ground(p: Vector3) -> void:
	focus = p
	_apply()


## Camera popover: a step of turn (eased, the way Q/E turn) or tilt.
func turn(steps: int) -> void:
	motion.turn(-deg_to_rad(cfg.turn_step_deg) * steps)


func tilt(steps: int) -> void:
	orbit(0.0, deg_to_rad(cfg.tilt_step_deg) * steps)


## Compass: turns smoothly to face north.
func north() -> void:
	motion.aim_north()


func top_view() -> void:
	orbit(0.0, deg_to_rad(cfg.max_tilt_deg) - pitch)


## Jumps straight to a view: tests and framing actions. `dist` is the eye's distance from the point.
func jump(p: Vector3, yaw_deg: float, pitch_deg: float, dist: float) -> void:
	stop()
	focus = p
	yaw = deg_to_rad(yaw_deg)
	half = CameraProfile.half_for_distance(cfg, dist)
	_ask_tilt(deg_to_rad(pitch_deg))
	_apply()


## Home: the start bearing, tilt and zoom over the same point.
func reset_view() -> void:
	stop()
	yaw = deg_to_rad(cfg.start_yaw)
	half = cfg.start_view_half
	_ask_tilt(deg_to_rad(cfg.start_pitch))
	_apply()


## Everything moving stops at once (a jump, the window losing focus).
func stop() -> void:
	motion.stop()
	gestures.cancel()


func _process(dt: float) -> void:
	motion.step(dt)
	gestures.advance(dt)


## A drag in progress owns the pointer, over the interface too.
func _input(event: InputEvent) -> void:
	if gestures.captured(event):
		get_viewport().set_input_as_handled()


func _unhandled_input(event: InputEvent) -> void:
	if gestures.press(event) or motion.input(event):
		get_viewport().set_input_as_handled()


func _notification(what: int) -> void:
	if what == NOTIFICATION_APPLICATION_FOCUS_OUT and gestures != null:
		stop()


## World point on the terrain under a screen position (null when the ray misses).
func ground_at(screen: Vector2) -> Variant:
	var hit: Dictionary = CityStore.query(&"raycast", [camera.project_ray_origin(screen), camera.project_ray_normal(screen)])
	return hit.position if hit.get("hit", false) else null


## The height of the ground under a pointer: the plane a pan, zoom or turn holds.
func height_under(screen: Vector2) -> float:
	var p: Variant = ground_at(screen)
	return p.y if p != null else _ground_y


## The point under a pointer on the horizontal plane at `h`, or null when the ray never meets it.
func hit_at(screen: Vector2, h: float) -> Variant:
	var o := camera.project_ray_origin(screen)
	var d := camera.project_ray_normal(screen)
	if absf(d.y) < PARALLEL:
		return null
	var t := (h - o.y) / d.y
	return o + d * t if t > 0.0 else null


## The plane a zoom or turn holds: the ground's own if the ray meets it within `hold_reach` view distances, else
## the height the ray has reached there (a pointer over the horizon holds a point in the air, not one at infinity).
func hold_height(screen: Vector2, h: float) -> float:
	var o := camera.project_ray_origin(screen)
	var d := camera.project_ray_normal(screen)
	var limit: float = cfg.hold_reach * camera.global_position.distance_to(focus)
	if absf(d.y) >= PARALLEL:
		var t := (h - o.y) / d.y
		if t > 0.0 and t <= limit:
			return h
	return o.y + d.y * limit


## Whether a pan can grab the ground under the pointer (else it slides by the drag).
func holds(screen: Vector2, h: float) -> bool:
	return is_equal_approx(hold_height(screen, h), h)


## Screen pixels per metre at the point looked at.
func px_per_m() -> float:
	var h := get_viewport().get_visible_rect().size.y
	return h / maxf(PARALLEL, 2.0 * camera.global_position.distance_to(focus) * tan(deg_to_rad(camera.fov) * 0.5))


## Moves the view so `grabbed` (on the plane at `h`) sits under the pointer again.
func pan_to(grabbed: Vector3, screen: Vector2, h: float) -> void:
	_bring(screen, grabbed, h)


## Zooms by `factor` (above 1: closer) keeping the ground under the pointer, or `grabbed` when given.
func zoom_at(screen: Vector2, factor: float, h: float, grabbed: Variant = null) -> void:
	if not (factor > 0.0) or not is_finite(factor):
		return
	var wanted := half / factor
	if wanted < cfg.view_half_min and half <= cfg.view_half_min:
		return
	keeping(screen, func() -> void: half = clampf(wanted, cfg.view_half_min, cfg.view_half_max), h, grabbed)


## Swings the camera round (`d_yaw`) and over (`d_tilt`, positive towards a plan view), about the ground under
## `pivot` (screen position) at height `h` when a pivot is given, else about the point looked at.
func orbit(d_yaw: float, d_tilt: float, pivot: Variant = null, h := 0.0) -> void:
	var change := func() -> void:
		yaw = wrapf(yaw + d_yaw, 0.0, TAU)
		_ask_tilt(pitch + d_tilt)
	if pivot == null:
		change.call()
		_apply()
	else:
		keeping(pivot, change, h)


## Moves over the ground, metres: `right` across the screen, `forward` away from the camera.
func slide(right: float, forward: float) -> void:
	var fwd := Vector3(-sin(yaw), 0.0, -cos(yaw))
	var side := Vector3(cos(yaw), 0.0, -sin(yaw))
	focus += side * right + fwd * forward
	_apply()


## Applies `change` and brings the ground point that was under `screen` back under it.
func keeping(screen: Vector2, change: Callable, h: float, grabbed: Variant = null) -> void:
	var at_h := h if grabbed != null else hold_height(screen, h)
	var before: Variant = grabbed if grabbed != null else hit_at(screen, at_h)
	change.call()
	_apply()
	if before == null:
		return
	_bring(screen, before, at_h)


## Moves the point looked at until `point` (on the plane at `h`) is under `screen`: one step is exact on flat ground,
## the terrain under the moved point changes the view's height, so it is repeated until it holds.
func _bring(screen: Vector2, point: Vector3, h: float) -> void:
	for pass_i in int(cfg.keep_passes):
		var found: Variant = hit_at(screen, h)
		if found == null:
			return
		var d: Vector3 = point - found
		if absf(d.x) + absf(d.z) < cfg.keep_tolerance_cm * CM:
			return
		focus.x += d.x
		focus.z += d.z
		_apply()


## The tilt the player asks for, kept as the far tilt it means at this distance.
func _ask_tilt(next: float) -> void:
	var now := clampf(next, CameraProfile.min_tilt(cfg, half), deg_to_rad(cfg.max_tilt_deg))
	far_tilt = now - CameraProfile.profile_tilt(cfg, half) + deg_to_rad(cfg.far_tilt_deg)


func _ground_height(x: float, z: float) -> float:
	return CityStore.query(&"height_at", [x, z])


func _apply() -> void:
	var profile: float = CameraProfile.profile_tilt(cfg, half)
	pitch = clampf(profile + far_tilt - deg_to_rad(cfg.far_tilt_deg), CameraProfile.min_tilt(cfg, half),
			deg_to_rad(cfg.max_tilt_deg))
	camera.fov = CameraProfile.fov_deg(cfg, half)
	distance = CameraProfile.view_distance(cfg, half)
	_ground_y = _ground_height(focus.x, focus.z)
	focus.y = _ground_y + CameraProfile.eye_lift(cfg, half)
	var basis := Basis.from_euler(Vector3(-pitch, yaw, 0.0))
	var pos := focus + basis.z * distance
	var floor_y: float = _ground_height(pos.x, pos.z) + cfg.ground_clearance
	if pos.y < floor_y:
		pos.y = floor_y
		basis = Basis.looking_at(focus - pos, basis.y)
	camera.global_transform = Transform3D(basis, pos)
	camera.near = clampf(pos.distance_to(focus) * cfg.near_per_m, cfg.near_min, cfg.near_max)
