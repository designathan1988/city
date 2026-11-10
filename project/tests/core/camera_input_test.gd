## Camera test through real input (Input.parse_input_event, like the player's mouse and keys): middle-drag pan keeps
## the grabbed ground under the cursor, Shift + right drag too, the wheel zooms about the cursor, right drag turns
## about the press point with the near side following the hand, tilt and zoom limits hold, keys glide, a right click
## cancels the road in progress while a right drag does not, and the left button still reaches the tool.
extends TestCase

const TOWN_MAP := 0
const SEED := 1
## Allowed drift of a held point, as a share of the view's height (headless views are tiny).
const PX_TOL_SHARE := 0.005
const STEPS := 8
const STILL_S := 0.15
const SETTLE_LIMIT_S := 12.0
const EPS := 1e-3
const OPEN_M := 600.0 # past the city: nothing for the eye to be pulled clear of

var rig: CityCamera
var tools: ToolController
var cfg: Dictionary


func _ready() -> void:
	rig = CityCamera.new()
	add_child(rig)
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	tools = ToolController.new(rig)
	add_child(tools)
	cfg = rig.cfg
	await frames(2)
	await _pan_tests()
	await _zoom_test()
	await _orbit_tests()
	await _limit_tests()
	await _key_tests()
	await CameraWalkCases.new(self).run()
	await CameraRoadKeyCases.new(self).run()
	finish()


func home() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	rig.jump((b.min + b.max) * 0.5, 35.0, 45.0, 700.0)


func _tol() -> float:
	return screen().y * PX_TOL_SHARE


func screen() -> Vector2:
	return get_viewport().get_visible_rect().size


func _pan_tests() -> void:
	print("view %s, tolerance %.2f px" % [screen(), _tol()])
	for shift in [false, true]:
		home()
		await frames(1)
		var p0 := screen() * Vector2(0.4, 0.6)
		var p1 := screen() * Vector2(0.65, 0.4)
		var g0: Variant = rig.ground_at(p0)
		var button := MOUSE_BUTTON_RIGHT if shift else MOUSE_BUTTON_MIDDLE
		await _move(p0)
		await _button(button, p0, true, shift)
		var worst := 0.0
		for i in STEPS:
			var at := p0.lerp(p1, float(i + 1) / STEPS)
			await _move(at, _mask(button))
			worst = maxf(worst, rig.camera.unproject_position(g0).distance_to(at))
		await get_tree().create_timer(STILL_S).timeout
		await _button(button, p1, false, shift)
		var err := rig.camera.unproject_position(g0).distance_to(p1)
		var what := "Shift+right" if shift else "middle"
		check(worst < _tol(), "%s: the ground held follows the cursor each step (worst %.2f px)" % [what, worst])
		check(err < _tol(), "%s: and stays under the cursor on release (error %.2f px)" % [what, err])


## On open ground: among the buildings the eye is pulled up its line of sight (CameraPull), which moves the picture on purpose.
func _zoom_test() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	rig.jump(b.min - Vector3(OPEN_M, 0.0, OPEN_M), 35.0, 45.0, 700.0)
	await frames(1)
	var at := screen() * Vector2(0.25, 0.3)
	var g0: Variant = rig.ground_at(at)
	var half0 := rig.half
	await _move(at)
	for i in 3:
		await _wheel(MOUSE_BUTTON_WHEEL_UP, at)
	await glide_settled()
	await _pull_settled()
	var err := rig.camera.unproject_position(g0).distance_to(at)
	var want := half0 / exp(3.0 * cfg.wheel_log_per_notch)
	check(rig.pull.shown() < EPS, "wheel: far from buildings the eye is not pulled (%.3f)" % rig.pull.shown())
	check(err < _tol(), "wheel: the point under the cursor stays under it (error %.2f px)" % err)
	check(absf(rig.half - want) < want * 0.01, "wheel: 3 notches zoom in (half height %.1f -> %.1f)" % [half0, rig.half])


func _orbit_tests() -> void:
	home()
	await frames(1)
	var c := screen() * 0.5
	var near := screen() * Vector2(0.5, 0.85)
	var g_near: Variant = rig.ground_at(near)
	var g_pivot: Variant = rig.ground_at(c)
	var yaw0 := rig.yaw
	var dx := 120.0
	await drag(MOUSE_BUTTON_RIGHT, c, c + Vector2(dx, 0), false, true)
	await frames(30) # the spring that spends a turn, well run out
	var turned := angle_difference(yaw0, rig.yaw)
	check(absf(turned + dx * rig.gestures.rad_per_px) < EPS, "right: turns %.3f rad (expected %.3f)" % [turned, -dx * rig.gestures.rad_per_px])
	check(rig.camera.unproject_position(g_pivot).distance_to(c) < _tol(), "right: turns round the ground under the click")
	check(rig.camera.unproject_position(g_near).x > near.x, "right: the near side follows the hand")
	var pitch0 := rig.pitch
	await drag(MOUSE_BUTTON_RIGHT, c, c + Vector2(0, 60), false, true)
	check(rig.pitch > pitch0, "right down: tilts to the view from above (%.1f -> %.1f degrees)" % [rad_to_deg(pitch0), rad_to_deg(rig.pitch)])


func _limit_tests() -> void:
	home()
	await frames(1)
	var c := screen() * 0.5
	await drag(MOUSE_BUTTON_RIGHT, c, c + Vector2(0, 2000), false, true)
	check(absf(rig.pitch - deg_to_rad(cfg.max_tilt_deg)) < EPS, "greatest tilt %.1f degrees" % rad_to_deg(rig.pitch))
	await drag(MOUSE_BUTTON_RIGHT, c, c - Vector2(0, 2000), false, true)
	var lo := CameraProfile.min_tilt(cfg, rig.half)
	check(absf(rig.pitch - lo) < EPS, "least tilt from afar %.1f degrees (limit %.1f)" % [rad_to_deg(rig.pitch), rad_to_deg(lo)])
	for i in 80:
		await _wheel(MOUSE_BUTTON_WHEEL_UP, c)
	await glide_settled()
	check(rig.half >= cfg.view_half_min - EPS and rig.half < cfg.view_half_min * 2.0, "least zoom: half height %.2f m" % rig.half)
	await drag(MOUSE_BUTTON_RIGHT, c, c - Vector2(0, 2000), false, true)
	lo = CameraProfile.min_tilt(cfg, rig.half)
	check(rig.pitch < 0.0 and absf(rig.pitch - lo) < EPS, "in the street it looks up to %.1f degrees" % rad_to_deg(rig.pitch))
	var under: Variant = CityStore.query(&"raycast", [rig.camera.global_position + Vector3.UP * cfg.ray_top, Vector3.DOWN]).get("position")
	check(under != null and rig.camera.global_position.y >= under.y + cfg.ground_clearance - EPS, "eye above the ground (eye %.3f, ground %s)" % [rig.camera.global_position.y, under])
	for i in 120:
		await _wheel(MOUSE_BUTTON_WHEEL_DOWN, c)
	await glide_settled()
	check(rig.half <= cfg.view_half_max + EPS and rig.half > cfg.view_half_max * 0.5, "greatest zoom: half height %.0f m" % rig.half)


func _key_tests() -> void:
	home()
	await frames(1)
	var f0 := rig.focus
	var fwd := Vector3(-sin(rig.yaw), 0, -cos(rig.yaw))
	await hold_key(KEY_W, false, 0.5)
	check((rig.focus - f0).dot(fwd) > 0.0, "W walks forward (%.1f m)" % (rig.focus - f0).dot(fwd))
	var yaw0 := rig.yaw
	await hold_key(KEY_Q, false, 0.3)
	check(angle_difference(yaw0, rig.yaw) > 0.0, "Q turns")
	await glide_settled()
	yaw0 = rig.yaw
	await hold_key(KEY_E, true, 0.05)
	await glide_settled()
	var quarter := angle_difference(yaw0, rig.yaw)
	check(absf(quarter + deg_to_rad(cfg.quarter_turn_deg)) < EPS, "Shift+E: a quarter turn (%.1f degrees)" % rad_to_deg(quarter))


func drag(button: MouseButton, from: Vector2, to: Vector2, shift: bool, still: bool) -> void:
	await _move(from)
	await _button(button, from, true, shift)
	for i in STEPS:
		await _move(from.lerp(to, float(i + 1) / STEPS), _mask(button))
	if still:
		await get_tree().create_timer(STILL_S).timeout
	await _button(button, to, false, shift)


func click(button: MouseButton, at: Vector2) -> void:
	await _move(at)
	await _button(button, at, true, false)
	await _button(button, at, false, false)


func _mask(button: MouseButton) -> int:
	return 1 << (button - 1)


func _move(at: Vector2, mask: int = 0) -> void:
	var m := InputEventMouseMotion.new()
	m.position = at
	m.global_position = at
	m.button_mask = mask
	Input.parse_input_event(m)
	await frames(1)


func _button(button: MouseButton, at: Vector2, pressed: bool, shift: bool) -> void:
	var e := InputEventMouseButton.new()
	e.button_index = button
	e.pressed = pressed
	e.shift_pressed = shift
	e.position = at
	e.global_position = at
	Input.parse_input_event(e)
	await frames(1)


## Waits for the pull to let go, as long as the slowest let-go takes (camera.json: the hold, then the ease from a full pull down
## to EPS): the state is checked, not a fixed time, so a slow frame cannot fail it and a pull that never lets go still does.
func _pull_settled() -> void:
	var limit: float = cfg.pull_hold_s + cfg.pull_out_s * log(1.0 / EPS)
	var waited := 0.0
	while rig.pull.shown() >= EPS and waited < limit:
		await get_tree().process_frame
		waited += get_process_delta_time()


## Waits for the glide to end: the state is checked, not a fixed time, so a machine sharing its cores cannot fail it.
func glide_settled() -> void:
	await frames(1)
	var waited := 0.0
	while rig.motion.moving() and waited < SETTLE_LIMIT_S:
		await get_tree().process_frame
		waited += get_process_delta_time()


func _wheel(button: MouseButton, at: Vector2) -> void:
	await _button(button, at, true, false)
	await _button(button, at, false, false)


func hold_key(code: Key, shift: bool, seconds: float) -> void:
	for pressed in [true, false]:
		var k := InputEventKey.new()
		k.keycode = code
		k.physical_keycode = code
		k.pressed = pressed
		k.shift_pressed = shift
		Input.parse_input_event(k)
		if pressed:
			await get_tree().create_timer(seconds).timeout
	await frames(1)


func frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame
