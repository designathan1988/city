## Player settings (graphics preset), persisted in user://settings.cfg, and the frame cap (Engine.max_fps): the game
## never renders faster than it needs to, and slower while another application has the focus (vsync alone does not hold
## a covered window back: it ran past twice the refresh rate). Presets and caps: data/game/settings.json.
extends Node

const PATH := "user://settings.cfg"
const MSAA_MODES := [Viewport.MSAA_DISABLED, Viewport.MSAA_2X, Viewport.MSAA_4X]

var preset := ""


func _ready() -> void:
	var cfg := ConfigFile.new()
	cfg.load(PATH)
	preset = cfg.get_value("graphics", "preset", DataTables.game("settings").default)
	Engine.max_fps = DataTables.game("settings").frame_cap.max_fps


## Tests and captures keep the full cap: what they measure is the frame, and their window seldom has the focus.
func _notification(what: int) -> void:
	var cap: Dictionary = DataTables.game("settings").frame_cap
	if what == NOTIFICATION_APPLICATION_FOCUS_OUT and not GameFlow.tests_drive:
		Engine.max_fps = cap.background_fps
	elif what == NOTIFICATION_APPLICATION_FOCUS_IN:
		Engine.max_fps = cap.max_fps


func set_preset(name: String) -> void:
	preset = name
	var cfg := ConfigFile.new()
	cfg.set_value("graphics", "preset", name)
	cfg.save(PATH)


func current() -> Dictionary:
	return DataTables.game("settings").presets[preset]


func apply(world: CityWorld, viewport: Viewport) -> void:
	var p := current()
	world.sun.shadow_enabled = p.shadows
	world.sun.directional_shadow_max_distance = p.shadow_distance
	viewport.msaa_3d = MSAA_MODES[int(p.msaa)]
	viewport.scaling_3d_scale = p.render_scale
