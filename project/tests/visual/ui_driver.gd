## Real input for the visual captures: mouse moves, clicks, holds and drags over ground points or controls, Ctrl keys,
## waiting until the render has caught up (CaptureWait, the one wait of the captures) and saving the viewport. Events go
## through Input like a player's. Each step is written to the log as it starts ("step: ..."), so a capture that crashes or
## hangs shows the step it was on.
class_name UiDriver
extends RefCounted

## The frames a shot waits by default, for a script that did not wait for the drawing first (CaptureWait.until_drawn).
const SETTLE_FRAMES := 8
const DRAWN_SETTLE := 2 # the frames the world takes to ask for what a move showed
## The frames a shot of a drawn view waits when a temporal filter (TAA or FSR2, game/settings/quality.gd) resolves the picture
## from the frames before it: its history fills with the view as it stands. Without one the drawn picture is the photo at once.
const TEMPORAL_SETTLE := 8

var rig: CityCamera
var tree: SceneTree
var out_dir: String


func _init(camera_rig: CityCamera, scene_tree: SceneTree, dir: String) -> void:
	rig = camera_rig
	tree = scene_tree
	out_dir = dir


func ground(x: float, z: float) -> Vector3:
	var hit: Dictionary = CityStore.query(&"raycast", [Vector3(x, 2000, z), Vector3.DOWN])
	return hit.position if hit.get("hit", false) else Vector3(x, 0, z)


## Where a click lands against where it aimed, in pixels, at each distance of the camera: the pixel grid and the ray to the
## ground, all a test can see of the pointer's error. The player's hand is not in this number.
func pointer_error(p: Vector3, view: Array, distances: Array[float], aims: int) -> float:
	var worst := 0.0
	var size: Vector2 = rig.get_viewport().get_visible_rect().size
	for dist in distances:
		rig.jump(p, view[0], view[1], dist)
		await frames(2)
		for i in aims:
			var aim: Variant = rig.ground_at(size * Vector2(fposmod(i * 0.381966, 1.0), fposmod(i * 0.618034, 1.0)))
			if aim == null:
				continue
			var seen := rig.camera.unproject_position(aim)
			var hit: Variant = rig.ground_at(seen.round())
			if hit != null:
				worst = maxf(worst, rig.camera.unproject_position(hit).distance_to(seen))
	return worst


func hover(p: Vector3) -> void:
	await move(rig.camera.unproject_position(ground(p.x, p.z)))


func move(at: Vector2) -> void:
	var m := InputEventMouseMotion.new()
	m.position = at
	m.global_position = at
	Input.parse_input_event(m)
	await frames(2)


func button(at: Vector2, pressed: bool) -> void:
	var ev := InputEventMouseButton.new()
	ev.button_index = MOUSE_BUTTON_LEFT
	ev.pressed = pressed
	ev.position = at
	ev.global_position = at
	Input.parse_input_event(ev)
	await frames(1)


func click(p: Vector3) -> void:
	step("click at %s" % p)
	await hover(p)
	var at := rig.camera.unproject_position(ground(p.x, p.z))
	await button(at, true)
	await button(at, false)


## A control just added (a drawer row rebuilt) has its place only once its container sorts it, and the sort is deferred to
## the end of the frame (Godot 4.7 scene/gui/container.cpp: add_child_notify -> queue_sort -> call_deferred): a frame first,
## or the click lands where the control is not yet.
func press(b: Control) -> void:
	step("button %s \"%s\"" % [b.name, b.get("text") if b.get("text") != null else ""])
	await frames(1)
	var at := b.get_global_rect().get_center()
	await move(at)
	await button(at, true)
	await button(at, false)


func hold(p: Vector3, frames: int) -> void:
	step("hold at %s" % p)
	await hover(p)
	var at := rig.camera.unproject_position(ground(p.x, p.z))
	await button(at, true)
	await frames(frames)
	await button(at, false)


## A held drag from `p0` to `p1` over the ground. `held`, when given, runs at the end with the button still down, before the
## release (a capture of the gesture's preview).
func drag(p0: Vector3, p1: Vector3, held: Callable = Callable()) -> void:
	step("drag from %s to %s" % [p0, p1])
	await hover(p0)
	await button(rig.camera.unproject_position(ground(p0.x, p0.z)), true)
	await hover(p0.lerp(p1, 0.5))
	await hover(p1)
	if held.is_valid():
		await held.call()
	await button(rig.camera.unproject_position(ground(p1.x, p1.z)), false)


func key(code: Key, ctrl: bool, shift: bool = false) -> void:
	step("key %s%s%s" % ["Ctrl+" if ctrl else "", "Shift+" if shift else "", OS.get_keycode_string(code)])
	for pressed in [true, false]:
		var k := InputEventKey.new()
		k.keycode = code
		k.ctrl_pressed = ctrl
		k.shift_pressed = shift
		k.pressed = pressed
		Input.parse_input_event(k)
		await frames(1)


## A modifier held down or let go, for what reads it while clicking.
func modifier(code: Key, pressed: bool) -> void:
	var k := InputEventKey.new()
	k.keycode = code
	k.pressed = pressed
	Input.parse_input_event(k)
	await frames(2)


## Until the world has drawn all it was asked to: CaptureWait's rule, which counts a photo that leaves with drawing pending.
func drawn() -> void:
	step("waits for the render")
	await CaptureWait.until_drawn(rig, DRAWN_SETTLE, "ui")


## The frames a shot of a view already drawn waits: TEMPORAL_SETTLE when the photographed viewport runs a temporal filter, else none.
func temporal_settle() -> int:
	var view := tree.root
	return TEMPORAL_SETTLE if view.use_taa or view.scaling_3d_mode == Viewport.SCALING_3D_MODE_FSR2 else 0


func shot(name: String, settle: int = SETTLE_FRAMES) -> void:
	await frames(settle)
	var path := "%s/%s.png" % [out_dir, name]
	tree.root.get_texture().get_image().save_png(path)
	print("capture: ", path)


## A shot's hour as the player sets it, the clock and the sky together (the reference's captures set the clock and the sky mode):
## the day's hour by the top bar's sun, the night's by its moon, any other by its clock, the light following the city's clock. Never the
## light pinned behind the mode the bar shows; what runs by the hour (traffic, people, lit windows) reads the clock.
static func at_hour(world: CityWorld, hour: float) -> void:
	CityStore.dispatch(&"set_day_hour", [hour])
	var day: Dictionary = DataTables.game("daylight")
	var mode := "day" if is_equal_approx(hour, float(day.day_hour)) else "night" if is_equal_approx(hour, float(day.night_hour)) else "cycle"
	world.set_sky_mode(mode)
	if mode == "cycle":
		world.follow_clock(hour) # the store's snapshot gives the new hour from the next frame



## The step that starts now, in the log.
func step(what: String) -> void:
	print("step: ", what)


func frames(n: int) -> void:
	for i in n:
		await tree.process_frame
