## Play scene: the world, the camera rig and the game's shell (ui/roadcraft_hud.gd). A new city is
## generated from a random seed on entry.
extends Node3D

const TOWN_MAP := 0
const SHOT_DELAY_S := 25.0
const SEED_LIMIT := 9999


func _ready() -> void:
	RenderingServer.global_shader_parameter_set(&"night", 0.0)
	var world := CityWorld.new()
	add_child(world)
	Settings.apply(world, get_viewport())
	var cam := CityCamera.new()
	add_child(cam)
	var errors := CityStore.boot(world.terrain, world.materials())
	if not errors.is_empty():
		push_error("Falha ao iniciar: %s" % "\n".join(errors))
		return
	var report: Dictionary = CityStore.new_map(TOWN_MAP, randi_range(1, SEED_LIMIT), true)
	print("cidade: ", report)
	cam.look_at_ground(Vector3.ZERO)
	var tools := ToolController.new(cam)
	add_child(tools)
	var hud := RoadcraftHud.new()
	add_child(hud)
	GameFlow.playing()
	_maybe_screenshot()


## Tests: --shot=PATH saves what the player sees after SHOT_DELAY_S seconds and quits.
func _maybe_screenshot() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--shot="):
			await get_tree().create_timer(SHOT_DELAY_S).timeout
			get_viewport().get_texture().get_image().save_png(arg.trim_prefix("--shot="))
			GameFlow.quit_now()


