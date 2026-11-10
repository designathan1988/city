## Play scene: the world, the camera rig and the game's shell (ui/roadcraft_hud.gd). A new city is
## generated from a random seed on entry.
extends Node3D

const TOWN_MAP := 0
const SHOT_DELAY_S := 25.0
const LIVE_DRIVER := "res://tests/visual/live_driver.gd"


func _ready() -> void:
	SentParams.send_global(&"night", 0.0)
	var world := CityWorld.new()
	add_child(world)
	var cam := CityCamera.new()
	add_child(cam)
	var errors := CityStore.boot(world.terrain, world.materials())
	if not errors.is_empty():
		push_error("Falha ao iniciar: %s" % "\n".join(errors))
		return
	cam.look_at_ground(Vector3.ZERO)
	var saved := _open_last()
	if not saved:
		print("cidade: ", CityStore.new_map(TOWN_MAP, randi_range(1, int(DataTables.game("start").seed_max)), true))
	Settings.attach(world, get_viewport())
	var tools := ToolController.new(cam)
	add_child(tools)
	var hud := RoadcraftHud.new()
	add_child(hud)
	add_child(InteriorController.new())
	_title(hud, saved)
	_maybe_live()
	_maybe_screenshot()


## -- --live=<dir>: the window takes steps from outside while the player watches (tests/visual/live_driver.gd; no export carries it).
func _maybe_live() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--live="):
			add_child(load(LIVE_DRIVER).new())


## The city of the last session, if it reads; one that does not goes aside and the game starts afresh.
func _open_last() -> bool:
	var slot := SaveSlots.last()
	if slot.is_empty() or GameFlow.tests_drive:
		return false
	if CityStore.saves.load_game(slot).get("ok", false):
		return true
	CityStore.saves.quarantine(slot)
	return false


## The title screen over the game; a run driven by a test goes straight to the game.
func _title(hud: RoadcraftHud, saved: bool) -> void:
	if GameFlow.tests_drive or OS.get_cmdline_user_args().size() > 0:
		GameFlow.playing()
		return
	GameFlow.menu()
	var start := StartScreen.new()
	start.saved = saved
	start.chosen.connect(_on_title.bind(hud))
	start.city_chosen.connect(hud.actions.new_city_at)
	hud.root.add_child(start)


func _on_title(action: String, hud: RoadcraftHud) -> void:
	GameFlow.playing()
	match action:
		"new": hud.actions.new_city("new_city")
		"open": hud.open_popover("files")
		"graphics", "about": hud.actions.menu(action)


## Tests: --shot=PATH saves what the player sees after SHOT_DELAY_S seconds and quits.
func _maybe_screenshot() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--shot="):
			await get_tree().create_timer(SHOT_DELAY_S).timeout
			get_viewport().get_texture().get_image().save_png(arg.trim_prefix("--shot="))
			GameFlow.quit_now()


