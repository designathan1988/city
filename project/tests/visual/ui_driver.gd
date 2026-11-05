## Real input for the visual captures: mouse moves, clicks, holds and drags over ground points or controls, Ctrl keys,
## waiting until the render has caught up and saving the viewport. Events go through Input like a player's.
class_name UiDriver
extends RefCounted

const BUILD_FRAMES := 600
const SETTLE_FRAMES := 8

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
	await hover(p)
	var at := rig.camera.unproject_position(ground(p.x, p.z))
	await button(at, true)
	await button(at, false)


func press(b: Control) -> void:
	var at := b.get_global_rect().get_center()
	await move(at)
	await button(at, true)
	await button(at, false)


func hold(p: Vector3, frames: int) -> void:
	await hover(p)
	var at := rig.camera.unproject_position(ground(p.x, p.z))
	await button(at, true)
	await frames(frames)
	await button(at, false)


func drag(p0: Vector3, p1: Vector3) -> void:
	await hover(p0)
	await button(rig.camera.unproject_position(ground(p0.x, p0.z)), true)
	await hover(p0.lerp(p1, 0.5))
	await hover(p1)
	await shot("feat_build_previa")
	await button(rig.camera.unproject_position(ground(p1.x, p1.z)), false)


func key(code: Key, ctrl: bool, shift: bool = false) -> void:
	for pressed in [true, false]:
		var k := InputEventKey.new()
		k.keycode = code
		k.ctrl_pressed = ctrl
		k.shift_pressed = shift
		k.pressed = pressed
		Input.parse_input_event(k)
		await frames(1)


func drawn() -> void:
	var waited := 0
	await frames(2)
	while CityStore.query(&"render_pending") > 0 and waited < BUILD_FRAMES:
		await frames(1)
		waited += 1


func shot(name: String) -> void:
	await frames(SETTLE_FRAMES)
	var path := "%s/%s.png" % [out_dir, name]
	tree.root.get_texture().get_image().save_png(path)
	print("captura: ", path)


func frames(n: int) -> void:
	for i in n:
		await tree.process_frame
