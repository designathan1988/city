## Captures of the Street tab driven like a player (real mouse and keys through the viewport): the tab, a line of poles with
## its preview, each barrier, each piece, a sign that becomes a rule and the street at night. Each step checks the core's
## counts and saves a PNG; the close views look along the footway at a person's height, as the Roadcraft photos do.
## Run: godot --path project --resolution 1600x900 res://tests/visual/capture_street.tscn -- --no-flow --capture-dir=/tmp/x
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const DAY := 16.0
const NIGHT := 22.0
const ITEM_BIN := 3
const ITEM_SIGN := 10
const SIGN_WORDS := "40"
const FENCE := 0
const REACH := 6.0
const LAMPS_ALL := 2
const SCAN_STEPS := 300
const SCAN_ROWS := 40
const SCAN_M := 3.0
const RUN_M := 120.0
const RUN_TRIES := 24
const MIN_RUN_POLES := 4
const VIEW_PITCH := 42.0
const VIEW_DIST := 130.0
const VIEW_UP_M := 26.0
const EYE_M := 1.7
const BACK_M := 14.0
const SIDE_M := 5.0
const LOOK_Y := 3.0
const ITEM_GAP_M := 3.0
const ITEM_STEP_M := 1.0
const BARRIER_LEG_M := 10.0
const BARRIER_GAP_M := 12.0
const BARRIER_FROM_M := 8.0
const TOP_M := 60.0
const ACROSS_TRIES := 40
const ACROSS_ALONG_M := 20.0
const ACROSS_FROM_M := 4.0
const SIGN_GAP_M := 3.0
const CLOSE_SIDE_M := 4.5
const CLOSE_BACK_M := 3.0
const CLOSE_LOOK_M := 0.8
const SIGN_LOOK_M := 2.0
const SIGNS_SIDE_M := 6.0
const SIGNS_ALONG_M := 12.0
const ALONG_BACK_M := 50.0
const ALONG_UP_M := 42.0

var out_dir := "user://captures"
var rig: CityCamera
var tools: ToolController
var hud: RoadcraftHud
var world: CityWorld
var ui: UiDriver
var failures := 0
var spots := {} ## What was placed and where, for the close views.
var _across := Vector3.ZERO ## The footway across the street.


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
	DirAccess.make_dir_recursive_absolute(out_dir)
	world = CityWorld.new()
	add_child(world)
	rig = CityCamera.new()
	add_child(rig)
	var errors := CityStore.boot(world.terrain, world.materials())
	if not errors.is_empty():
		push_error("boot: %s" % errors)
		get_tree().quit(1)
		return
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	world.set_hour(DAY)
	ui = UiDriver.new(rig, get_tree(), out_dir)
	tools = ToolController.new(rig)
	add_child(tools)
	tools.hint.connect(func(text: String, ok: bool) -> void: print("dica: ", text, " ", ok))
	hud = RoadcraftHud.new()
	add_child(hud)
	await _run()


func _run() -> void:
	var a := _footway()
	var b := _run_end(a)
	_check(a != Vector3.INF and b != Vector3.INF, "calçada e trecho")
	if b == Vector3.INF:
		get_tree().quit(1)
		return
	var dir := (b - a).normalized()
	_over((a + b) * 0.5)
	await ui.drawn()
	hud.open_category("landscape")
	await ui.frames(2)
	await ui.press(hud.drawer._tabs["street"])
	await ui.shot("rua_01_aba")
	await _poles(a, b)
	await _items(a, dir)
	await _barriers(a, b)
	await CaptureStreetViews.views(self, a, b)
	print("contagens: ", CityStore.query(&"street_counts"))
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


## The view from over a point, left above the drawer.
func _over(p: Vector3) -> void:
	rig.jump(p + Vector3(0, 0, VIEW_UP_M), 0.0, VIEW_PITCH, VIEW_DIST)


func _footway() -> Vector3:
	var box: Dictionary = CityStore.query(&"city_bounds")
	var mid: Vector3 = (box.min + box.max) * 0.5
	for i in SCAN_STEPS:
		for j in SCAN_ROWS:
			var at: Dictionary = CityStore.query(&"preview_item", [mid + Vector3(i * SCAN_M, 0, j * SCAN_M), ITEM_BIN, 1.0])
			if at.get("ok", false) and _run_end(at.position) != Vector3.INF:
				return at.position
	return Vector3.INF


func _run_end(a: Vector3) -> Vector3:
	for i in RUN_TRIES:
		var end := a + Vector3.RIGHT.rotated(Vector3.UP, TAU * i / RUN_TRIES) * RUN_M
		var plan: Dictionary = CityStore.query(&"preview_pole_run", [a, end, REACH, LAMPS_ALL])
		if plan.get("ok", false) and plan.poles.size() >= MIN_RUN_POLES:
			return end
	return Vector3.INF


## The strip's button that reads `key` in the player's language.
func _card(key: String, under: Node = null) -> Button:
	var text := tr(key).get_slice(" (", 0)
	for c in (under if under else hud.drawer._strip).get_children():
		if c is Button and c.text == text:
			return c
		var inside := _card(key, c)
		if inside:
			return inside
	return null


func _pick(key: String) -> void:
	var b := _card(key)
	_check(b != null, "cartão " + key)
	if b:
		await ui.press(b)


func _poles(a: Vector3, b: Vector3) -> void:
	await _pick("pole.mode.build")
	await ui.shot("rua_02_postes_opcoes")
	await _pick("pole.lamps.all")
	var before: Dictionary = CityStore.query(&"street_counts")
	await ui.hover(a)
	await ui.button(rig.camera.unproject_position(ui.ground(a.x, a.z)), true)
	await ui.hover(a.lerp(b, 0.5))
	await ui.hover(b)
	await ui.shot("rua_03_postes_previa")
	await ui.button(rig.camera.unproject_position(ui.ground(b.x, b.z)), false)
	await ui.drawn()
	var after: Dictionary = CityStore.query(&"street_counts")
	_check(after.poles > before.poles and after.spans > before.spans, "linha pelo mouse %s" % after)
	await ui.shot("rua_04_postes_prontos")
	await ui.key(KEY_ESCAPE, false)


func _items(a: Vector3, dir: Vector3) -> void:
	var list := [
		"streetscape.bench", "streetscape.bin", "streetscape.hydrant", "streetscape.postbox", "streetscape.phone",
		"streetscape.drain", "streetscape.tree", "streetscape.shrub", "streetscape.lamp", "streetscape.sign",
		"streetscape.streetname",
	]
	var before: Dictionary = CityStore.query(&"street_counts")
	var placed := 0
	var walked := ITEM_GAP_M
	for key: String in list:
		await _pick(key)
		var kind: int = tools.params.kind
		var at: Dictionary = {}
		while walked < RUN_M and not at.get("ok", false):
			at = CityStore.query(&"preview_item", [a + dir * walked, kind, REACH])
			walked += ITEM_STEP_M
		if at.get("ok", false):
			var had: int = CityStore.query(&"street_counts").items
			await ui.click(at.position)
			placed += CityStore.query(&"street_counts").items - had
			spots[key] = at.position
			walked += ITEM_GAP_M
		print("peça ", key, ": ", at.get("ok"), " ", at.get("error"), " ", at.get("position"))
	await ui.drawn()
	await ui.shot("rua_05_pecas")
	var after: Dictionary = CityStore.query(&"street_counts")
	_check(after.items == before.items + placed and placed >= list.size() - 2, "peças pelo mouse %d" % placed)
	await CaptureStreetViews.signs(self, a, dir)


## Each barrier on the footway across the street, away from the signs: the points go to its back, or to the kerb.
func _barriers(a: Vector3, b: Vector3) -> void:
	var names := ["barrier.kind.fence", "barrier.kind.wall", "barrier.kind.hedge", "barrier.kind.guardrail", "barrier.kind.railing"]
	var dir := (b - a).normalized()
	var before: Dictionary = CityStore.query(&"street_counts")
	var built := 0
	_over(_across - dir * BARRIER_FROM_M)
	await ui.drawn()
	for i in names.size():
		await _pick(names[i])
		var o := _across - dir * (BARRIER_FROM_M + BARRIER_GAP_M * i)
		await ui.click(o)
		await ui.click(o - dir * BARRIER_LEG_M)
		await ui.key(KEY_ENTER, false)
		spots[names[i]] = o - dir * BARRIER_LEG_M * 0.5
		var now: int = CityStore.query(&"street_counts").barriers
		print("barreira ", names[i], ": ", now - before.barriers - built)
		built = now - before.barriers
	await ui.drawn()
	await ui.shot("rua_06_barreiras")
	_check(built == names.size(), "barreiras pelo mouse %d" % built)


func _check(ok: bool, what: String) -> void:
	print("ok: " if ok else "FALHOU: ", what)
	failures += 0 if ok else 1
