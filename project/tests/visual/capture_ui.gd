## UI capture driven like a player: opens the Roads drawer, picks a profile card, clicks the start and end of a road
## on the map (real mouse events through the viewport, so the HUD and the ToolController handle them), zones the new
## street, lets the simulation grow buildings and traffic, bulldozes, and saves a screenshot of each step.
## Run under xvfb: godot --path project --rendering-driver opengl3 --rendering-method gl_compatibility
##   --resolution 1280x720 res://tests/visual/capture_ui.tscn -- --no-flow --capture-dir=/tmp/x
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const BUILD_FRAMES := 600
const SETTLE_FRAMES := 6
## The new road: from this far inside the town's east edge out to this far past it (metres).
const ROAD_IN_M := 40.0
const ROAD_OUT_M := 300.0
const VIEW := [0.0, 55.0, 560.0] # yaw, pitch (degrees), distance
## The view looks this far in front of the road so the road sits above the drawer.
const VIEW_AHEAD_M := 130.0
const GROW_TICKS := 2400
const ZONE_OFFSET_M := 22.0
const ZONE_STEPS := 4
const PROFILE_CATEGORY := "avenidas"

var out_dir := "user://captures"
var rig: CityCamera
var tools: ToolController
var hud: RoadcraftHud
var failures := 0


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
	DirAccess.make_dir_recursive_absolute(out_dir)
	var world := CityWorld.new()
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
	tools = ToolController.new(rig)
	add_child(tools)
	hud = RoadcraftHud.new()
	add_child(hud)
	await _run()


func _run() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var z: float = (b.min.z + b.max.z) * 0.5
	var a := _ground(b.max.x - ROAD_IN_M, z)
	var e := _ground(b.max.x + ROAD_OUT_M, z)
	rig.jump((a + e) * 0.5 + Vector3(0, 0, VIEW_AHEAD_M), VIEW[0], VIEW[1], VIEW[2])
	await _drawn()
	hud.open_category("roads")
	hud.drawer.catalog.show_category(PROFILE_CATEGORY)
	var card: Control = hud.drawer.catalog._cards_row.get_child(0)
	(card.get_meta(&"button") as Button).emit_signal(&"pressed")
	await _shot("ui_01_gaveta_vias")
	var s0 := CityStore.snapshot.duplicate()
	await _click(a)
	await _hover(e)
	await _shot("ui_02_previa_via")
	await _click(e)
	tools.set_tool(ToolController.Tool.NONE)
	var s1 := CityStore.snapshot.duplicate()
	_check(s1.segments > s0.segments, "via nova: segmentos %d -> %d" % [s0.segments, s1.segments])
	_check(s1.money < s0.money, "cobrou %s" % Formatters.format_money(s0.money - s1.money))
	await _drawn()
	await _shot("ui_03_via_construida")
	hud.open_category("zones")
	hud.drawer.pick_zone(1)
	for side in [-1.0, 1.0]:
		for i in ZONE_STEPS + 1:
			var p := a.lerp(e, float(i) / ZONE_STEPS) + Vector3(0, 0, side * ZONE_OFFSET_M)
			await _click(_ground(p.x, p.z))
	await _shot("ui_04_gaveta_zonas")
	CityStore.step_ticks(GROW_TICKS)
	var s2 := CityStore.snapshot
	print("cresceram ", s2.grown, " prédios; veículos ", s2.vehicles, " pessoas ", s2.people)
	_check(s2.grown > 0, "prédios cresceram na rua nova")
	hud.open_category("zones")
	await _drawn()
	hud.open_popover("sim")
	await _shot("ui_05_crescimento_e_transito")
	hud.open_popover("sim")
	hud.open_category("info")
	await _click(_grown_building(a, e))
	await _shot("ui_06_inspecionar")
	hud.open_category("demolish")
	await _click(e.lerp(a, 0.1))
	await _shot("ui_07_demolir")
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


## A building that grew beside the new road (nearest to its middle).
func _grown_building(a: Vector3, e: Vector3) -> Vector3:
	var mid := (a + e) * 0.5
	var best := mid
	var best_d := INF
	for id in CityStore.query(&"list_buildings"):
		var b: Dictionary = CityStore.query(&"probe_building", [id])
		var d: float = b.position.distance_to(mid)
		if b.position.x > a.x and d < best_d:
			best_d = d
			best = b.position
	return best


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	failures += 0 if ok else 1


func _ground(x: float, z: float) -> Vector3:
	var hit: Dictionary = CityStore.query(&"raycast", [Vector3(x, 2000, z), Vector3.DOWN])
	return hit.position if hit.get("hit", false) else Vector3(x, 0, z)


func _screen(p: Vector3) -> Vector2:
	return rig.camera.unproject_position(p)


func _hover(p: Vector3) -> void:
	var m := InputEventMouseMotion.new()
	m.position = _screen(p)
	m.global_position = m.position
	Input.parse_input_event(m)
	await _frames(2)


func _click(p: Vector3) -> void:
	await _hover(p)
	for pressed in [true, false]:
		var ev := InputEventMouseButton.new()
		ev.button_index = MOUSE_BUTTON_LEFT
		ev.pressed = pressed
		ev.position = _screen(p)
		ev.global_position = ev.position
		Input.parse_input_event(ev)
		await _frames(1)


func _drawn() -> void:
	var waited := 0
	await _frames(2)
	while CityStore.query(&"render_pending") > 0 and waited < BUILD_FRAMES:
		await _frames(1)
		waited += 1


func _shot(name: String) -> void:
	await _frames(SETTLE_FRAMES)
	var path := "%s/%s.png" % [out_dir, name]
	get_viewport().get_texture().get_image().save_png(path)
	print("captura: ", path)


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame
