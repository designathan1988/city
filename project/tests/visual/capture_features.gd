## Captures of the features that used to be "being built", driven like a player (real mouse and key events through
## the viewport): undo/redo, save and open, the road tabs Melhorar/Dividir/Rotatória, the Relief brush, a park and a
## school drawn from the Build drawer and the map layers. Each step checks the core's numbers and saves a PNG.
## Run under xvfb: godot --path project --rendering-driver opengl3 --rendering-method gl_compatibility
##   --resolution 1280x720 res://tests/visual/capture_features.tscn -- --no-flow --capture-dir=/tmp/x
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const VIEW := [0.0, 55.0, 520.0]
const VIEW_AHEAD_M := 120.0
const ROAD_IN_M := 40.0
const ROAD_OUT_M := 300.0
const HOLD_FRAMES := 90
const MODE_RAISE := 0
const MODE_MESA := 4

var out_dir := "user://captures"
var rig: CityCamera
var world: CityWorld
var tools: ToolController
var hud: RoadcraftHud
var failures := 0
var ui: UiDriver


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
	CityStore.new_map(TOWN_MAP, SEED, true, true)
	ui = UiDriver.new(rig, get_tree(), out_dir)
	tools = ToolController.new(rig)
	add_child(tools)
	hud = RoadcraftHud.new()
	add_child(hud)
	await _run()


func _run() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var z: float = (b.min.z + b.max.z) * 0.5
	var a := ui.ground(b.max.x - ROAD_IN_M, z)
	var e := ui.ground(b.max.x + ROAD_OUT_M, z)
	await _undo_redo(a, e)
	await _road_tabs(a, e)
	await _relief(e)
	await _build(a, e)
	await CaptureFlag.shot(self, a)
	await _layers(a, e)
	await _save_open()
	print("FAILURES: ", failures)
	get_tree().quit(CaptureWait.exit_code(failures))


func _undo_redo(a: Vector3, e: Vector3) -> void:
	rig.jump((a + e) * 0.5 + Vector3(0, 0, VIEW_AHEAD_M), VIEW[0], VIEW[1], VIEW[2])
	await ui.drawn()
	hud.open_category("roads")
	var s0: int = CityStore.snapshot.segments
	await ui.click(a)
	await ui.click(e)
	tools.set_tool(ToolController.Tool.NONE)
	hud.drawer.close()
	await ui.drawn()
	_check(CityStore.snapshot.segments > s0 and CityStore.snapshot.can_undo, "road built, undo active")
	await ui.shot("feat_undo_1_via")
	await ui.key(KEY_Z, true)
	await ui.drawn()
	_check(CityStore.snapshot.segments == s0 and CityStore.snapshot.can_redo, "Ctrl+Z undid the road")
	await ui.shot("feat_undo_2_desfeita")
	await ui.press(hud.top_bar.history_button("redo"))
	await ui.drawn()
	_check(CityStore.snapshot.segments > s0, "Redo by the button")
	await ui.shot("feat_undo_3_refeita")


func _road_tabs(a: Vector3, e: Vector3) -> void:
	var mid := a.lerp(e, 0.6)
	hud.open_category("roads")
	await ui.frames(4)
	await ui.press(hud.drawer._tabs.upgrade)
	hud.drawer.catalog.show_category("avenidas")
	await ui.press(hud.drawer.catalog._cards_row.get_child(0).get_meta(&"button"))
	await ui.hover(mid)
	await ui.shot("feat_upgrade_1_previa")
	var m0: float = CityStore.snapshot.money
	await ui.click(mid)
	await ui.drawn()
	_check(CityStore.snapshot.money < m0, "Upgrade charged the difference")
	await ui.shot("feat_upgrade_2_avenida")
	await ui.press(hud.drawer._tabs.split)
	var s0: int = CityStore.snapshot.segments
	await ui.hover(a.lerp(e, 0.3))
	await ui.click(a.lerp(e, 0.3))
	_check(CityStore.snapshot.segments == s0 + 1, "Split made a node")
	await ui.shot("feat_split")
	await ui.press(hud.drawer._tabs.roundabout)
	var ring := e + Vector3(0, 0, -VIEW_AHEAD_M * 1.5)
	await ui.hover(ring)
	await ui.shot("feat_roundabout_1_previa")
	s0 = CityStore.snapshot.segments
	await ui.click(ring)
	await ui.hover(ring + Vector3(0, 0, 80))
	await ui.drawn()
	_check(CityStore.snapshot.segments > s0, "Roundabout built (%d -> %d)" % [s0, CityStore.snapshot.segments])
	tools.preview.clear()
	rig.jump(ring, 20.0, 50.0, 260.0)
	await ui.drawn()
	await ui.shot("feat_roundabout_2_construida")


## The card of this value in the Landscape panel, as it stands now (a pick draws the panel again).
func _land_card(value: int) -> Button:
	for b: Node in hud.drawer.strip.find_children("*", "Button", true, false):
		if b.get_meta(&"pick", -2) == value:
			return b
	return null


func _relief(e: Vector3) -> void:
	var hill := e + Vector3(260, 0, 140)
	rig.jump(hill + Vector3(-70, 0, 160), 20.0, 40.0, 520.0)
	await ui.drawn()
	hud.open_category("landscape")
	await ui.frames(4)
	await ui.press(_land_card(MODE_MESA))
	var h0 := ui.ground(hill.x, hill.z).y
	await ui.hold(hill, HOLD_FRAMES)
	await ui.press(_land_card(MODE_RAISE))
	await ui.hold(hill + Vector3(-140, 0, 0), HOLD_FRAMES)
	await ui.drawn()
	var h1 := ui.ground(hill.x, hill.z).y
	_check(h1 > h0 + 5.0, "relevo: chapada subiu %.1f -> %.1f m" % [h0, h1])
	await ui.hover(hill + Vector3(-140, 0, 0))
	await ui.shot("feat_terrain_1_chapada")
	await ui.key(KEY_Z, true) # the raised hill beside it
	await ui.drawn()
	await ui.key(KEY_Z, true) # the mesa: two strokes, two undo steps
	await ui.drawn()
	var back := ui.ground(hill.x, hill.z).y
	var drawn: Dictionary = CityStore.query(&"ground_drawn", [hill]) # what the terrain's shader reads there, back from the GPU
	_check(absf(back - h0) < 0.1, "relief undone in the core: %.1f m (was %.1f)" % [back, h0])
	_check(drawn.get("fine", false) and absf(drawn.height - h0) < 0.5, "relief undone in the ground drawn: %s (was %.1f)" % [drawn, h0])
	await ui.shot("feat_terrain_2_desfeito")
	await ui.key(KEY_Z, true, true)
	await ui.key(KEY_Z, true, true)
	await ui.drawn()


func _build(a: Vector3, e: Vector3) -> void:
	var mid := a.lerp(e, 0.8)
	rig.jump(mid + Vector3(0, 0, 60), 0.0, 50.0, 300.0)
	await ui.drawn()
	hud.open_category("build")
	await ui.frames(4)
	await ui.press(hud.drawer._tabs.draw)
	await ui.frames(2)
	var cards := hud.drawer.strip.get_children().filter(func(c: Node) -> bool: return c.has_meta(&"pick"))
	var b0: int = CityStore.snapshot.buildings
	await ui.press(cards[1]) # parque
	await ui.drag(mid + Vector3(-30, 0, 16), mid + Vector3(30, 0, 56), func() -> void: await ui.shot("feat_build_previa"))
	await ui.press(cards[2]) # escola
	await ui.drag(mid + Vector3(-30, 0, -16), mid + Vector3(20, 0, -50))
	await ui.drawn()
	_check(CityStore.snapshot.buildings >= b0 + 2, "park and school drawn (%d -> %d)" % [b0, CityStore.snapshot.buildings])
	await ui.shot("feat_build_parque_escola")
	hud.drawer.close()


func _layers(a: Vector3, e: Vector3) -> void:
	var zoned: Dictionary = CityStore.dispatch(&"paint_zone", [a.lerp(e, 0.5), 80.0, 2])
	_check(zoned.ok, "zoned the road: %s" % zoned)
	CityStore.dispatch(&"set_speed", [1])
	for i in 120:
		await get_tree().process_frame
	rig.jump(a + Vector3(0, 0, 0), 0.0, 60.0, 900.0)
	await ui.drawn()
	hud.open_popover("layers")
	await ui.frames(3)
	var buttons := hud.popover.get_child(0).get_children().filter(func(c: Node) -> bool: return c is Button)
	var specs: Array = DataTables.ui("drawers").popovers.layers.rows
	var rows := [buttons[specs.find_custom(func(r: Dictionary) -> bool: return r.get("action", "") == "congestion")]]
	for r in rows:
		await ui.press(r)
	await ui.frames(70)
	var on: Dictionary = CityStore.query(&"overlays")
	_check(on.zones and on.congestion, "layers on %s, the player's sides %d" % [on, CityStore.snapshot.player_zones])
	await ui.shot("feat_layers")
	for r in rows:
		await ui.press(r)
	hud.open_popover("layers")
	CityStore.dispatch(&"set_speed", [0])


func _save_open() -> void:
	hud.open_popover("menu")
	await ui.frames(3)
	var save_row := _row_with(tr("ui.menu.save"))
	await ui.press(save_row)
	while CityStore.saves.save_busy(): # the save is written on the core's thread
		await ui.frames(1)
	var slot := CityStore.saves.city_slot() # the city's own slot (SaveSlots), given by this first save
	_check(not slot.is_empty() and FileAccess.file_exists(CityStore.saves.save_path(slot)) and not CityStore.snapshot.unsaved,
		"Save wrote the city to %s" % slot)
	var segs: int = CityStore.snapshot.segments
	CityStore.new_map(TOWN_MAP, SEED + 1, false)
	_check(CityStore.saves.city_slot().is_empty(), "the new map did not inherit the saved city's slot")
	await ui.drawn()
	hud.open_popover("menu")
	await ui.frames(3)
	await ui.press(_row_with(tr("action.openMap")))
	await ui.frames(4)
	await ui.shot("feat_save_1_abrir")
	await ui.press(_row_with(slot + "  ·")) # the row of exactly that slot (hud_popover: "<slot>  ·  <date>")
	await ui.drawn()
	_check(CityStore.snapshot.segments == segs and CityStore.saves.city_slot() == slot, "opened the saved city (%d roads)" % segs)
	await ui.shot("feat_save_2_aberto")
	DirAccess.remove_absolute(CityStore.saves.save_path(slot)) # the next run saves to the same name again


func _row_with(text: String) -> Button:
	for c in hud.popover.get_child(0).get_children():
		if c is Button and c.text.begins_with(text):
			return c
	push_error("linha ausente: " + text)
	return Button.new()


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	failures += 0 if ok else 1


