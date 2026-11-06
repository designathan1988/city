## Camera test through real input (Input.parse_input_event, like the player's mouse and keys): middle-drag pan keeps
## the grabbed ground under the cursor, Shift + right drag too, the wheel zooms about the cursor, right drag turns
## about the press point with the near side following the hand, tilt and zoom limits hold, keys glide, a right click
## cancels the road in progress while a right drag does not, and the left button still reaches the tool.
## Run: godot --headless --path project res://tests/core/camera_input_test.tscn -- --no-flow
extends Node3D

const TOWN_MAP := 0
const SEED := 1
## Allowed drift of a held point, as a share of the view's height (headless views are tiny).
const PX_TOL_SHARE := 0.005
const STEPS := 8
const STILL_S := 0.15
## The longest a glide may take to end, in the frames' own time: past it the glide never ends, and the check after it says so.
const SETTLE_LIMIT_S := 12.0
const EPS := 1e-3
const OPEN_M := 600.0 # past the city: nothing for the eye to be pulled clear of
const WALK_STEPS := 90
const WALK_VIEW_M := 250.0 # the view over the blocks: well past the street, where the pull ends
const WALK_RIGHT_M := 6.0
const WALK_AHEAD_M := 3.0
const WALK_HOLD_M := 1.0 # the ground under the point looked at moves it this much at most

var failures := 0
var rig: CityCamera
var tools: ToolController
var cfg: Dictionary


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	rig = CityCamera.new()
	add_child(rig)
	_check(CityStore.boot(world.terrain, world.materials()).is_empty(), "boot")
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	tools = ToolController.new(rig)
	add_child(tools)
	cfg = rig.cfg
	await _frames(2)
	await _pan_tests()
	await _zoom_test()
	await _orbit_tests()
	await _limit_tests()
	await _key_tests()
	await _walk_test()
	await CameraRoadKeyCases.new(self).run()
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


func _home() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	rig.jump((b.min + b.max) * 0.5, 35.0, 45.0, 700.0)


func _tol() -> float:
	return _screen().y * PX_TOL_SHARE


func _screen() -> Vector2:
	return get_viewport().get_visible_rect().size


func _pan_tests() -> void:
	print("vista %s, tolerância %.2f px" % [_screen(), _tol()])
	for shift in [false, true]:
		_home()
		await _frames(1)
		var p0 := _screen() * Vector2(0.4, 0.6)
		var p1 := _screen() * Vector2(0.65, 0.4)
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
		var what := "Shift+direito" if shift else "meio"
		_check(worst < _tol(), "%s: o chão agarrado segue o cursor a cada passo (pior %.2f px)" % [what, worst])
		_check(err < _tol(), "%s: e fica sob o cursor ao soltar (erro %.2f px)" % [what, err])


## On open ground: among the buildings the eye is pulled up its line of sight (CameraPull), which moves the picture on purpose.
func _zoom_test() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	rig.jump(b.min - Vector3(OPEN_M, 0.0, OPEN_M), 35.0, 45.0, 700.0)
	await _frames(1)
	var at := _screen() * Vector2(0.25, 0.3)
	var g0: Variant = rig.ground_at(at)
	var half0 := rig.half
	await _move(at)
	for i in 3:
		await _wheel(MOUSE_BUTTON_WHEEL_UP, at)
	await _glide_settled()
	await _pull_settled()
	var err := rig.camera.unproject_position(g0).distance_to(at)
	var want := half0 / exp(3.0 * cfg.wheel_log_per_notch)
	_check(rig.pull.shown < EPS, "roda: longe dos prédios o olho não é puxado (%.3f)" % rig.pull.shown)
	_check(err < _tol(), "roda: o ponto sob o cursor fica sob o cursor (erro %.2f px)" % err)
	_check(absf(rig.half - want) < want * 0.01, "roda: 3 entalhes aproximam (meia altura %.1f -> %.1f)" % [half0, rig.half])


func _orbit_tests() -> void:
	_home()
	await _frames(1)
	var c := _screen() * 0.5
	var near := _screen() * Vector2(0.5, 0.85)
	var g_near: Variant = rig.ground_at(near)
	var g_pivot: Variant = rig.ground_at(c)
	var yaw0 := rig.yaw
	var dx := 120.0
	await _drag(MOUSE_BUTTON_RIGHT, c, c + Vector2(dx, 0), false, true)
	var turned := angle_difference(yaw0, rig.yaw)
	_check(absf(turned + dx * rig.gestures.rad_per_px) < EPS, "direito: gira %.3f rad (esperado %.3f)" % [turned, -dx * rig.gestures.rad_per_px])
	_check(rig.camera.unproject_position(g_pivot).distance_to(c) < _tol(), "direito: gira em torno do chão sob o clique")
	_check(rig.camera.unproject_position(g_near).x > near.x, "direito: o lado perto acompanha a mão")
	var pitch0 := rig.pitch
	await _drag(MOUSE_BUTTON_RIGHT, c, c + Vector2(0, 60), false, true)
	_check(rig.pitch > pitch0, "direito para baixo: inclina para a vista de cima (%.1f -> %.1f graus)" % [rad_to_deg(pitch0), rad_to_deg(rig.pitch)])


func _limit_tests() -> void:
	_home()
	await _frames(1)
	var c := _screen() * 0.5
	await _drag(MOUSE_BUTTON_RIGHT, c, c + Vector2(0, 2000), false, true)
	_check(absf(rig.pitch - deg_to_rad(cfg.max_tilt_deg)) < EPS, "inclinação máxima %.1f graus" % rad_to_deg(rig.pitch))
	await _drag(MOUSE_BUTTON_RIGHT, c, c - Vector2(0, 2000), false, true)
	var lo := CameraProfile.min_tilt(cfg, rig.half)
	_check(absf(rig.pitch - lo) < EPS, "inclinação mínima de longe %.1f graus (limite %.1f)" % [rad_to_deg(rig.pitch), rad_to_deg(lo)])
	for i in 80:
		await _wheel(MOUSE_BUTTON_WHEEL_UP, c)
	await _glide_settled()
	_check(rig.half >= cfg.view_half_min - EPS and rig.half < cfg.view_half_min * 2.0, "zoom mínimo: meia altura %.2f m" % rig.half)
	await _drag(MOUSE_BUTTON_RIGHT, c, c - Vector2(0, 2000), false, true)
	lo = CameraProfile.min_tilt(cfg, rig.half)
	_check(rig.pitch < 0.0 and absf(rig.pitch - lo) < EPS, "na rua olha para cima até %.1f graus" % rad_to_deg(rig.pitch))
	var under: Variant = CityStore.query(&"raycast", [rig.camera.global_position + Vector3.UP * cfg.ray_top, Vector3.DOWN]).get("position")
	_check(under != null and rig.camera.global_position.y >= under.y + cfg.ground_clearance - EPS, "olho acima do chão (olho %.3f, chão %s)" % [rig.camera.global_position.y, under])
	for i in 120:
		await _wheel(MOUSE_BUTTON_WHEEL_DOWN, c)
	await _glide_settled()
	_check(rig.half <= cfg.view_half_max + EPS and rig.half > cfg.view_half_max * 0.5, "zoom máximo: meia altura %.0f m" % rig.half)


## Moving over the buildings at the city's view: the zoom holds. The pull is for the street, and the eye never jumps towards what it crosses.
func _walk_test() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	rig.jump((b.min + b.max) * 0.5, 35.0, 45.0, WALK_VIEW_M)
	await _frames(1)
	var from := rig.camera.global_position.distance_to(rig.focus)
	var worst := 0.0
	for i in WALK_STEPS:
		rig.slide(WALK_RIGHT_M, WALK_AHEAD_M)
		await _frames(1)
		worst = maxf(worst, absf(rig.camera.global_position.distance_to(rig.focus) - from))
	_check(rig.pull.shown < EPS, "andar sobre a cidade: o olho não é puxado (%.3f)" % rig.pull.shown)
	_check(worst < WALK_HOLD_M, "andar sobre a cidade: o zoom fica (variou %.2f m de %.0f m)" % [worst, from])


func _key_tests() -> void:
	_home()
	await _frames(1)
	var f0 := rig.focus
	var fwd := Vector3(-sin(rig.yaw), 0, -cos(rig.yaw))
	await _hold_key(KEY_W, false, 0.5)
	_check((rig.focus - f0).dot(fwd) > 0.0, "W anda para a frente (%.1f m)" % (rig.focus - f0).dot(fwd))
	var yaw0 := rig.yaw
	await _hold_key(KEY_Q, false, 0.3)
	_check(angle_difference(yaw0, rig.yaw) > 0.0, "Q gira")
	await _glide_settled()
	yaw0 = rig.yaw
	await _hold_key(KEY_E, true, 0.05)
	await _glide_settled()
	var quarter := angle_difference(yaw0, rig.yaw)
	_check(absf(quarter + deg_to_rad(cfg.quarter_turn_deg)) < EPS, "Shift+E: um quarto de volta (%.1f graus)" % rad_to_deg(quarter))


func _drag(button: MouseButton, from: Vector2, to: Vector2, shift: bool, still: bool) -> void:
	await _move(from)
	await _button(button, from, true, shift)
	for i in STEPS:
		await _move(from.lerp(to, float(i + 1) / STEPS), _mask(button))
	if still:
		await get_tree().create_timer(STILL_S).timeout
	await _button(button, to, false, shift)


func _click(button: MouseButton, at: Vector2) -> void:
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
	await _frames(1)


func _button(button: MouseButton, at: Vector2, pressed: bool, shift: bool) -> void:
	var e := InputEventMouseButton.new()
	e.button_index = button
	e.pressed = pressed
	e.shift_pressed = shift
	e.position = at
	e.global_position = at
	Input.parse_input_event(e)
	await _frames(1)


## Waits for the pull to let go, as long as the slowest let-go takes (camera.json: the hold, then the ease from a full pull down
## to EPS): the state is checked, not a fixed time, so a slow frame cannot fail it and a pull that never lets go still does.
func _pull_settled() -> void:
	var limit: float = cfg.pull_hold_s + cfg.pull_out_s * log(1.0 / EPS)
	var waited := 0.0
	while rig.pull.shown >= EPS and waited < limit:
		await get_tree().process_frame
		waited += get_process_delta_time()


## Waits for the glide to end: the state is checked, not a fixed time, so a machine sharing its cores cannot fail it.
func _glide_settled() -> void:
	await _frames(1)
	var waited := 0.0
	while rig.motion.moving() and waited < SETTLE_LIMIT_S:
		await get_tree().process_frame
		waited += get_process_delta_time()


func _wheel(button: MouseButton, at: Vector2) -> void:
	await _button(button, at, true, false)
	await _button(button, at, false, false)


func _hold_key(code: Key, shift: bool, seconds: float) -> void:
	for pressed in [true, false]:
		var k := InputEventKey.new()
		k.keycode = code
		k.physical_keycode = code
		k.pressed = pressed
		k.shift_pressed = shift
		Input.parse_input_event(k)
		if pressed:
			await get_tree().create_timer(seconds).timeout
	await _frames(1)


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	if not ok:
		failures += 1
