## The camera rig: orbit about a ground point with a yaw, a tilt and `half`, the half height of the ground in view there (CameraProfile).
## Moves that hold a ground point under the pointer go through `keeping`. CameraGestures, CameraMotion, CameraPull; data/game/camera.json.
class_name CityCamera
extends Node3D

## The right button clicked without a drag: a right click cancels the gesture in progress.
signal right_clicked

## Below this the ray runs along the plane and meets it nowhere.
const PARALLEL := 1e-6
const CM := 0.01
const EARLIEST := -100

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
var pull: CameraPull
var _ground_y := 0.0
var _ground := CameraDamp.new() # the height the view rests on
var _rise := CameraDamp.new() # how far the eye stands over its orbit, clear of the ground
var _eye_ground := -INF # the ground under the eye, as last read
var ahead: CameraAhead


func _ready() -> void:
	add_to_group(&"camera_rig")
	cfg = DataTables.game("camera")
	camera = Camera3D.new()
	camera.far = cfg.far
	add_child(camera)
	camera.current = true
	add_child(CameraShake.new(camera))
	gestures = CameraGestures.new(self)
	motion = CameraMotion.new(self)
	pull = CameraPull.new(cfg)
	ahead = CameraAhead.new(self)
	yaw = deg_to_rad(cfg.start_yaw)
	far_tilt = deg_to_rad(cfg.start_pitch)
	half = cfg.start_view_half
	_apply()
	process_priority = EARLIEST # the view of this frame, before anything reads it


func look_at_ground(p: Vector3) -> void:
	focus = p
	_settle(0.0)


## Camera popover: a step of turn (eased, the way Q/E turn) or tilt.
func turn(steps: int) -> void:
	motion.turn(-deg_to_rad(cfg.turn_step_deg) * steps)


func tilt(steps: int) -> void:
	orbit(0.0, deg_to_rad(cfg.tilt_step_deg) * steps)


func north() -> void:
	motion.aim_north()


## Jumps straight to a view: tests and framing actions. `dist` is the eye's distance from the point.
func jump(p: Vector3, yaw_deg: float, pitch_deg: float, dist: float) -> void:
	stop()
	focus = p
	yaw = deg_to_rad(yaw_deg)
	half = CameraProfile.half_for_distance(cfg, dist)
	_ask_tilt(deg_to_rad(pitch_deg))
	_apply()
	_settle(0.0)


## Home: the start bearing, tilt and zoom over the same point.
func reset_view() -> void:
	stop()
	yaw = deg_to_rad(cfg.start_yaw)
	half = cfg.start_view_half
	_ask_tilt(deg_to_rad(cfg.start_pitch))
	_apply()
	_settle(0.0)


## Everything moving stops at once (a jump, the window losing focus).
func stop() -> void:
	motion.stop()
	gestures.cancel()


func _process(dt: float) -> void:
	motion.step(dt)
	gestures.advance(dt)
	_settle(dt)


## A drag in progress owns the pointer, over the interface too.
func _input(event: InputEvent) -> void:
	if gestures.captured(event):
		get_viewport().set_input_as_handled()


func _unhandled_input(event: InputEvent) -> void:
	if gestures.touch(event) or gestures.press(event) or motion.input(event):
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


## Screen pixels per metre at the point looked at.
func px_per_m() -> float:
	var h := get_viewport().get_visible_rect().size.y
	return h / maxf(PARALLEL, 2.0 * camera.global_position.distance_to(focus) * tan(deg_to_rad(camera.fov) * 0.5))


## Moves the view so `grabbed` (on the plane at `h`) sits under the pointer again.
func pan_to(grabbed: Vector3, screen: Vector2, h: float) -> void:
	CameraAim.bring(self, screen, grabbed, h)


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
	var at_h := h if grabbed != null else CameraAim.hold_height(self, screen, h)
	var before: Variant = grabbed if grabbed != null else hit_at(screen, at_h)
	var from := camera.project_ray_origin(screen)
	var along := camera.project_ray_normal(screen)
	change.call()
	_apply()
	if before == null:
		return
	var reach := from.distance_to(before)
	var swing := (camera.project_ray_origin(screen) + camera.project_ray_normal(screen) * reach).distance_to(from + along * reach)
	CameraAim.bring(self, screen, before, at_h, CameraAim.firm_reach(self, screen, before, at_h, swing))


## The tilt the player asks for, kept as the far tilt it means at this distance.
func _ask_tilt(next: float) -> void:
	var now := clampf(next, CameraProfile.min_tilt(cfg, half), deg_to_rad(cfg.max_tilt_deg))
	far_tilt = now - CameraProfile.profile_tilt(cfg, half) + deg_to_rad(cfg.far_tilt_deg)


## What the player sees at (x, z), the plane the eye stays above and the focus rests on: a road deck where one is, the
## ground elsewhere (the pointer's rule, the 4.0's sceneHeightAt); the terrain where the ray finds nothing (off the map).
func seen_height(x: float, z: float) -> float:
	var hit: Dictionary = CityStore.query(&"raycast", [Vector3(x, cfg.ray_top, z), Vector3.DOWN])
	return hit.position.y if hit.get("hit", false) else CityStore.query(&"height_at", [x, z])


## The view from what the player asks and what the settled heights say: arithmetic alone, so a gesture may place it many times a frame.
func _apply() -> void:
	var profile: float = CameraProfile.profile_tilt(cfg, half)
	pitch = clampf(profile + far_tilt - deg_to_rad(cfg.far_tilt_deg), CameraProfile.min_tilt(cfg, half),
			deg_to_rad(cfg.max_tilt_deg))
	CameraProfile.lens(camera, cfg, half)
	distance = CameraProfile.view_distance(cfg, half)
	_ground_y = _ground.x
	focus.y = _ground.x + CameraProfile.eye_lift(cfg, half) + ahead.over.x
	var basis := Basis.from_euler(Vector3(-pitch, yaw, 0.0))
	var orbit_eye := focus.lerp(focus + basis.z * distance, 1.0 - maxf(pull.shown(), ahead.out.x))
	var pos := orbit_eye
	pos.y = maxf(pos.y + _rise.x, _eye_ground + cfg.ground_touch_m)
	if pos.y != orbit_eye.y: # raised over the ground, still looking at the same point
		basis = Basis.looking_at(focus - pos, basis.y)
	camera.global_transform = Transform3D(basis, pos)
	camera.near = clampf(pos.distance_to(focus) * cfg.near_per_m, cfg.near_min, cfg.near_max)


## Once a frame: what stands under and in front of the view, each height and share moved towards it on its own spring. Nothing the terrain,
## a road's edge, a roof or a wall asks of the view reaches the screen as a step. `dt` 0 takes it all at once (a jump).
func _settle(dt: float) -> void:
	_ground.to(seen_height(focus.x, focus.z), dt, cfg.ground_smooth_s) if dt > 0.0 else _ground.snap(seen_height(focus.x, focus.z))
	var base := Vector3(focus.x, _ground.x + CameraProfile.eye_lift(cfg, half), focus.z)
	var far_eye := base + Basis.from_euler(Vector3(-pitch, yaw, 0.0)).z * distance
	pull.ask(base, far_eye, half)
	if dt > 0.0:
		pull.settle(dt)
	else:
		pull.snap()
	var eye := far_eye.lerp(base, pull.shown())
	_eye_ground = seen_height(eye.x, eye.z)
	var rise := maxf(0.0, _eye_ground + cfg.ground_clearance - eye.y)
	ahead.settle(dt, base, eye + Vector3.UP * rise, far_eye.distance_to(base))
	if dt > 0.0:
		_rise.to(rise, dt, cfg.eye_rise_s if rise > _rise.x else cfg.eye_fall_s)
	else:
		_rise.snap(rise)
	_apply()
