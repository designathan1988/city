## The light in the real game: play.tscn with its own bar. The sun, moon and clock buttons are pressed, the clock runs at
## the bar's fastest speed and the sky follows it. Run: godot --path project --resolution 1920x1080
## res://tests/visual/capture_clock.tscn -- --capture-dir=/folder
extends Node

const PLAY := "res://game/play.tscn"
const BOOT_FRAMES := 300
const SETTLE_FRAMES := 30
const HOURS_APART := 2.0
const SHOTS := 4
const FASTEST := 3 # the bar's "4x" (ui/roadcraft_hud.gd SPEEDS)
const SUN_BUTTON := 0
const MOON_BUTTON := 1
const CLOCK_BUTTON := 2
const HOURS_A_DAY := 24.0

var out_dir := "user://captures"


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
	DirAccess.make_dir_recursive_absolute(out_dir)
	var play: Node = load(PLAY).instantiate()
	add_child(play)
	await _frames(BOOT_FRAMES)
	var hud: Node = play.find_children("*", "RoadcraftHud", true, false)[0]
	var world: CityWorld = play.find_children("*", "CityWorld", true, false)[0]
	await _press(hud.top_bar._sky_buttons[MOON_BUTTON], world, "modo_noite")
	await _press(hud.top_bar._sky_buttons[SUN_BUTTON], world, "modo_dia")
	await _press(hud.top_bar._sky_buttons[CLOCK_BUTTON], world, "modo_relogio")
	hud.top_bar._speed_buttons[FASTEST].pressed.emit()
	var hour: float = CityStore.snapshot.hour
	for i in SHOTS:
		var next := fmod(hour + HOURS_APART, HOURS_A_DAY)
		while absf(float(CityStore.snapshot.hour) - next) > HOURS_APART * 0.1:
			await get_tree().process_frame
		hour = next
		await _shot("relogio_%d" % i, world)
	print("capture_clock: pronto")
	get_tree().quit(CaptureWait.exit_code(0))


func _press(b: Button, world: CityWorld, name: String) -> void:
	b.pressed.emit()
	await _frames(SETTLE_FRAMES)
	await _shot(name, world)


func _shot(name: String, world: CityWorld) -> void:
	await RenderingServer.frame_post_draw
	get_viewport().get_texture().get_image().save_png(out_dir.path_join(name + ".png"))
	print(name, ": hora ", CityStore.snapshot.hour, " modo ", world.sky_mode, " sol ", world.sun.light_energy, " noite ", world.daylight.night)


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame
