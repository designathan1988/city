## The camera rig: orbit about a ground point with a yaw, a tilt and `half`, the half height of the ground in view there (CameraProfile).
## Moves that hold a ground point under the pointer go through `keeping` (CameraAim). What stands under the view is CameraHeights', what the
## buildings ask of the eye CameraWalls'. CameraGestures, CameraMotion, CameraPull; data/game/camera.json.
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
## What the settle asks of the core, answered once for the same arguments while the map stays (CameraQueries).
var queries := CameraQueries.new()
## What stands under the view (CameraHeights) and the eye among the buildings (CameraWalls).
var heights: CameraHeights
var solids: CameraWalls
var _step := CameraWalls.STILL # the frame's time while a settle places the view


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
	pull = CameraPull.new(cfg, queries)
	heights = CameraHeights.new(cfg, queries)
	solids = CameraWalls.new(cfg, queries)
	yaw = deg_to_rad(cfg.start_yaw)
	far_tilt = deg_to_rad(cfg.start_pitch)
	half = cfg.start_view_half
	apply()
	process_priority = EARLIEST # the view of this frame, before anything reads it


func look_at_ground(p: Vector3) -> void:
	focus = p
	settle(0.0)


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
	apply()
	settle(0.0)


## Home: the start bearing, tilt and zoom over the same point.
func reset_view() -> void:
	stop()
	yaw = deg_to_rad(cfg.start_yaw)
	half = cfg.start_view_half
	_ask_tilt(deg_to_rad(cfg.start_pitch))
	apply()
	settle(0.0)


## Everything moving stops at once (a jump, the window losing focus).
func stop() -> void:
	motion.stop()
	gestures.cancel()


## Holds the camera at `eye` looking at `at`, as a photo or a test needs a view the orbit cannot give (a person's height, a
## reference game's camera): the rig stops processing (no settle, gesture, input or shake moves the camera) until `unpin`, which
## gives the view back to the rig's own state, unchanged meanwhile. The rig is the camera's one writer: nothing else places it.
func pin(eye: Vector3, at: Vector3) -> void:
	stop()
	process_mode = Node.PROCESS_MODE_DISABLED
	camera.global_position = eye
	camera.look_at(at)


func unpin() -> void:
	process_mode = Node.PROCESS_MODE_INHERIT


func _process(dt: float) -> void:
	motion.step(dt)
	gestures.advance(dt)
	settle(dt)


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
		apply()
	else:
		keeping(pivot, change, h)


## Moves over the ground, metres: `right` across the screen, `forward` away from the camera.
func slide(right: float, forward: float) -> void:
	var fwd := Vector3(-sin(yaw), 0.0, -cos(yaw))
	var side := Vector3(cos(yaw), 0.0, -sin(yaw))
	focus += side * right + fwd * forward
	apply()


## Applies `change` and brings the ground point that was under `screen` back under it.
func keeping(screen: Vector2, change: Callable, h: float, grabbed: Variant = null) -> void:
	var at_h := h if grabbed != null else CameraAim.hold_height(self, screen, h)
	var before: Variant = grabbed if grabbed != null else CameraAim.hit_at(self, screen, at_h)
	var from := camera.project_ray_origin(screen)
	var along := camera.project_ray_normal(screen)
	change.call()
	apply()
	if before == null:
		return
	var reach := from.distance_to(before)
	CameraAim.bring(self, screen, before, at_h, CameraAim.firm_reach(self, screen, before, at_h, from + along * reach, reach))


## The tilt the player asks for, kept as the far tilt it means at this distance.
func _ask_tilt(next: float) -> void:
	var now := clampf(next, CameraProfile.min_tilt(cfg, half), deg_to_rad(cfg.max_tilt_deg))
	far_tilt = now - CameraProfile.profile_tilt(cfg, half) + deg_to_rad(cfg.far_tilt_deg)


## What each of the rig's springs holds now, metres and shares: a view that jumps names the spring that moved it.
func springs() -> Dictionary:
	return {"ground": heights.ground(), "rise": heights.rise(), "pull": pull.shown(), "floor": heights.under_eye,
			"shift": solids.shift().length(), "lift": solids.lift(), "snapped": solids.snapped}


## The view from what the player asks and what the settled heights say, then what the buildings ask of the eye (CameraWalls), the ground
## under the eye kept, and the near plane short of the nearest building. Answers are kept per arguments (CameraQueries), so a gesture may
## place it many times a frame; only a settle steps the springs.
func apply() -> void:
	_orbit_shape()
	var lift := CameraProfile.eye_lift(cfg, half)
	focus.y = heights.ground() + lift
	var basis := Basis.from_euler(Vector3(-pitch, yaw, 0.0))
	var orbit_eye := focus.lerp(focus + basis.z * distance, 1.0 - pull.shown()) + Vector3.UP * heights.rise()
	var placed := solids.place(focus, orbit_eye, lift, half, _step)
	var at := placed[0]
	var pos := placed[1]
	# never under the ground: the eased rise keeps the clearance where the eye stands (settle reads the ground there), the touch is the floor
	pos.y = maxf(pos.y, heights.seen(pos.x, pos.z) + cfg.ground_touch_m)
	if pos != orbit_eye or at != focus: # moved by the buildings or the ground, still looking at the point
		basis = Basis.looking_at(at - pos, basis.y)
	camera.global_transform = Transform3D(basis, pos)
	camera.near = solids.near(pos, at, half)


## The orbit's tilt, lens and distance for the zoom asked now: what `settle` asks the buildings about and what `apply` places are then
## one eye, not this frame's and the last's.
func _orbit_shape() -> void:
	var profile: float = CameraProfile.profile_tilt(cfg, half)
	pitch = clampf(profile + far_tilt - deg_to_rad(cfg.far_tilt_deg), CameraProfile.min_tilt(cfg, half),
			deg_to_rad(cfg.max_tilt_deg))
	CameraProfile.lens(camera, cfg, half)
	distance = CameraProfile.view_distance(cfg, half)


## Once a frame: what stands under and in front of the view, each height and share moved towards it on its own spring. Nothing the terrain,
## a road's edge, a roof or a wall asks of the view reaches the screen as a step. `dt` 0 takes it all at once (a jump).
func settle(dt: float) -> void:
	queries.next_settle()
	_orbit_shape()
	heights.rest_on(focus, dt)
	var base := Vector3(focus.x, heights.ground() + CameraProfile.eye_lift(cfg, half), focus.z)
	var far_eye := base + Basis.from_euler(Vector3(-pitch, yaw, 0.0)).z * distance
	pull.ask(base, far_eye, half)
	if dt > 0.0:
		pull.settle(dt)
	else:
		pull.snap()
	heights.clear(far_eye.lerp(base, pull.shown()) + solids.shift(), dt) # where the buildings hold the eye
	_step = dt
	apply()
	_step = CameraWalls.STILL
