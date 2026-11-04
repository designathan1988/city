## The interface photographed as the player sees it, in the views the Roadcraft 4.0's photos were taken from: the title screen, the top
## bar, every popover, the drawers with the tool's options, the dialogs and the monitors.
## Run: godot --path project --resolution 1600x900 res://tests/visual/capture_shell.tscn -- --no-flow --capture-dir=<dir>
extends Node3D

const TOWN_MAP := 0
const SEED := 7
const FRAMES := 240
const SETTLE := 8
const VIEW := [45.0, 52.0, 2300.0] # yaw, pitch (degrees), distance: the whole town under the bar

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
	hud.health.toggle()
	await _shot("health")
	hud.health.close()
	hud.profiler.toggle()
	await _shot("profiler")
	hud.profiler.toggle()
	hud.flash(tr("save.done"), true)
	await _shot("toast")
	print("fotos: ", shots)
	GameFlow.quit_now()


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame


func _shot(name: String) -> void:
	await _frames(SETTLE)
	await RenderingServer.frame_post_draw
	get_viewport().get_texture().get_image().save_png("%s/shell_%s.png" % [out_dir, name])
	shots += 1
