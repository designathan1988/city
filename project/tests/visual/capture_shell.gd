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
const PHOTO_HOUR := 12.0
const CLOCK_BUTTON := 2
const INSPECT_PITCH := 75.0
const PAIRED_TABS := ["build_draw", "landscape_ground", "landscape_nature", "landscape_sky"]
const SEARCH_M := 600.0
const SEARCH_STEP_M := 3.0

var out_dir := "user://captures"
var rig: CityCamera
var hud: RoadcraftHud
var driver: UiDriver
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
	driver = UiDriver.new(rig, get_tree(), out_dir)
	await _run()


## The bar's clock button and its fastest speed, up to the hour the reference's photos were taken at: same clock, same sun.
func _to_noon() -> void:
	hud.top_bar._sky_buttons[CLOCK_BUTTON].pressed.emit()
	CityStore.dispatch(&"set_speed", [CityStore.code(&"SimSpeed", &"FASTEST")])
	while float(CityStore.snapshot.hour) < PHOTO_HOUR:
		await get_tree().process_frame
	CityStore.dispatch(&"set_speed", [CityStore.code(&"SimSpeed", &"PAUSED")])


func _run() -> void:
	rig.jump(Vector3.ZERO, VIEW[0], VIEW[1], VIEW[2])
	await _frames(FRAMES)
	var start := StartScreen.new()
	hud.root.add_child(start)
	await _shot("start")
	await _to_noon() # the reference's title screen is at the opening hour, the rest at noon
	start.free()
	await _shot("hud")
	for which: String in ["menu", "layers", "sim", "camera", "help"]:
		hud.open_popover(which)
		await _shot("pop_" + which)
		hud.popover.close()
	for cat: String in ["roads", "zones", "build", "landscape", "transit", "demolish"]:
		hud.open_category(cat)
		await _shot("drawer_" + cat)
		await _tabs(cat)
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
	await _switch_shadow()
	await _map_layers()
	hud.actions.menu("new_city")
	await _shot("dialog_confirm")
	hud._dialog.close()
	print("fotos: ", shots)
	GameFlow.quit_now()


## The monitor with what really happened: an engine warning through the Logger, a long frame, an edit; each tab.
func _health() -> void:
	push_warning("photo: an engine warning through the Logger")
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
	hud.open_category("info")
	for kind: String in ["road", "node", "signal"]:
		var spot := _nearest(kind)
		rig.jump(spot, VIEW[0], INSPECT_PITCH, STREET_M) # from above: a click on the middle of the screen falls on the road, not on a tower
		await _frames(FRAMES / 2)
		await driver.click(spot)
		await _shot("inspect_" + kind)
		hud.inspector.close()
	hud.open_category("info")
	rig.jump(Vector3.ZERO, VIEW[0], VIEW[1], VIEW[2])


## The point of that kind nearest the middle of the town, ring by ring.
func _nearest(kind: String) -> Vector3:
	for ring in range(0, int(SEARCH_M / SEARCH_STEP_M)):
		for i in ring * 8 + 1:
			var a := TAU * i / (ring * 8 + 1)
			var p := Vector3(cos(a), 0.0, sin(a)) * ring * SEARCH_STEP_M
			var info: Dictionary = CityStore.query(&"inspect", [p])
			var found: String = info.get("kind", "")
			if found == "node" and CityStore.query(&"node_traffic", [info.id]).has("cycle_s"):
				found = "signal"
			if found == kind:
				return p
	return Vector3.ZERO


## A layer's switch before and after: what it takes away goes with its shadow (the caster mask follows the camera's).
func _switch_shadow() -> void:
	rig.jump(_nearest("node"), VIEW[0], VIEW[1], STREET_M)
	await _frames(FRAMES / 2)
	await _shot("switch_on")
	Settings.switches.set_on("buildings", false)
	Settings.switches_changed()
	await _frames(SETTLE)
	await _shot("switch_off")
	Settings.switches.set_on("buildings", true)
	Settings.switches_changed()
	rig.jump(Vector3.ZERO, VIEW[0], VIEW[1], VIEW[2])


func _map_layers() -> void:
	var junction := _nearest("node") # the grid is seen over a junction, the same one the shadow shots stand on
	Settings.set_layer("grid", true)
	rig.jump(junction, VIEW[0], VIEW[1], STREET_M)
	await _frames(SETTLE)
	await _shot("layer_grid")
	Settings.set_layer("grid", false)
	rig.jump(Vector3.ZERO, VIEW[0], VIEW[1], VIEW[2])
	await _frames(FRAMES / 2)


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame


## The drawer's other tabs that have a photo of the 4.0.
func _tabs(cat: String) -> void:
	for t: Dictionary in hud.drawer.category.tabs:
		if PAIRED_TABS.has("%s_%s" % [cat, t.id]):
			hud.drawer.select_tab(t)
			await _shot("drawer_%s_%s" % [cat, t.id])


## The clock stands still between the photos; it runs for each, so the bar shows the game at 1x.
func _shot(name: String) -> void:
	CityStore.dispatch(&"set_speed", [CityStore.code(&"SimSpeed", &"NORMAL")])
	await _frames(SETTLE)
	await RenderingServer.frame_post_draw
	get_viewport().get_texture().get_image().save_png("%s/shell_%s.png" % [out_dir, name])
	CityStore.dispatch(&"set_speed", [CityStore.code(&"SimSpeed", &"PAUSED")])
	print("hora %s %.3f" % [name, CityStore.snapshot.hour])
	print("view %s: eye %s, city %s, %d vehicles, %d people" % [name, rig.camera.global_position, CityStore.query(&"city_bounds"),
		CityStore.snapshot.vehicles, CityStore.snapshot.people])
	shots += 1
