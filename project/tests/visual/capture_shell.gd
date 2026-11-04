## The interface photographed as the player sees it, in the views the Roadcraft 4.0's photos were taken from: the title screen, the top
## bar, every popover, the drawers with the tool's options, the dialogs and the monitors.
## Run: godot --path project --resolution 1600x900 res://tests/visual/capture_shell.tscn -- --no-flow --capture-dir=<dir>
extends Node3D

const TOWN_MAP := 0
const SEED := 7
const FRAMES := 240
const SETTLE := 8
const VIEW := [45.0, 52.0, 2300.0] # yaw, pitch (degrees), distance: the whole town under the bar
const LONG_FRAME_MS := 80.0
const TIP_WAIT_S := 1.2
const STREET_M := 120.0

var out_dir := "user://captures"
var rig: CityCamera
var hud: RoadcraftHud
var shots := 0


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
	add_child(ToolController.new(rig))
	hud = RoadcraftHud.new()
	add_child(hud)
	Settings.attach(world, get_viewport())
	await _run()


func _run() -> void:
	rig.jump(Vector3.ZERO, VIEW[0], VIEW[1], VIEW[2])
	await _frames(FRAMES)
	var start := StartScreen.new()
	hud.root.add_child(start)
	await _shot("start")
	start.free()
	await _shot("hud")
	for which: String in ["menu", "layers", "sim", "camera", "help"]:
		hud.open_popover(which)
		await _shot("pop_" + which)
		hud.popover.close()
	for cat: String in ["roads", "zones", "build", "landscape", "transit", "demolish"]:
		hud.open_category(cat)
		await _shot("drawer_" + cat)
		hud.open_category(cat)
	hud.open_category("roads")
	hud.drawer.select_tab(hud.drawer.category.tabs[5])
	await _shot("drawer_roundabout")
	hud.open_category("roads")
	hud.open_dialog(GraphicsPanel.new())
	await _shot("dialog_graphics")
	hud.open_dialog(HudDialog.About.new())
	await _shot("dialog_about")
	hud._dialog.close()
	hud.toggle_effects()
	await _shot("effects")
	hud.effects.close()
	await _health()
	hud.profiler.toggle()
	await _shot("profiler")
	hud.profiler.toggle()
	hud.flash(tr("save.done"), true)
	await _shot("toast")
	await _tip()
	await _inspector()
	await _map_layers()
	hud.actions.menu("new_city")
	await _shot("dialog_confirm")
	hud._dialog.close()
	print("fotos: ", shots)
	GameFlow.quit_now()


## The monitor with what really happened: an engine warning through the Logger, a long frame, an edit; each tab.
func _health() -> void:
	push_warning("foto: aviso do motor pelo Logger")
	await _frames(SETTLE)
	Health._slow_frame(LONG_FRAME_MS)
	var road: Dictionary = CityStore.query(&"segment_at", [Vector3.ZERO])
	CityStore.dispatch(&"demolish", [road.get("point", Vector3.ZERO)])
	hud.health.toggle()
	await _shot("health")
	for tab: String in ["world", "state"]:
		hud.health._select(tab)
		await _shot("health_" + tab)
	hud.health._select("health")
	hud.health.close()
	CityStore.dispatch(&"undo")


## The mouse tip of the Layers button, its own element (the toast above is not one).
func _tip() -> void:
	var b: Button = hud.top_bar.action_buttons.layers
	var at := b.get_global_rect().get_center()
	get_viewport().warp_mouse(at)
	var move := InputEventMouseMotion.new()
	move.position = at
	move.global_position = at
	Input.parse_input_event(move)
	await get_tree().create_timer(TIP_WAIT_S).timeout
	await _shot("tip")


## A click on a road and on a junction, the panel the player gets, close enough to see them.
func _inspector() -> void:
	var seg: int = 0
	var info := {}
	while info.get("kind", "") != "road" and seg < CityStore.snapshot.segments:
		var t: Dictionary = CityStore.query(&"segment_traffic", [seg])
		seg += 1
		if not t.is_empty() and int(t.vehicles) > 0:
			info = {"kind": "road"}
	var spot: Vector3 = rig.focus
	rig.jump(spot, VIEW[0], VIEW[1], STREET_M)
	await _frames(SETTLE)
	var road: Dictionary = CityStore.query(&"inspect", [spot])
	road["at"] = spot
	hud.tools.inspected.emit(road)
	await _shot("inspect_road")
	hud.inspector.close()
	rig.jump(Vector3.ZERO, VIEW[0], VIEW[1], VIEW[2])


func _map_layers() -> void:
	Settings.set_layer("grid", true)
	rig.jump(rig.focus, VIEW[0], VIEW[1], STREET_M)
	await _frames(SETTLE)
	await _shot("layer_grid")
	Settings.set_layer("grid", false)
	for style in 4:
		CityStore.dispatch(&"set_marking_style", [style, false])
		await _frames(FRAMES / 2)
		await _shot("paint_%d" % style)
	CityStore.dispatch(&"set_marking_style", [0, false])
	rig.jump(Vector3.ZERO, VIEW[0], VIEW[1], VIEW[2])
	await _frames(FRAMES / 2)


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame


func _shot(name: String) -> void:
	await _frames(SETTLE)
	await RenderingServer.frame_post_draw
	get_viewport().get_texture().get_image().save_png("%s/shell_%s.png" % [out_dir, name])
	shots += 1
