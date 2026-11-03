## Player settings (graphics preset), persisted in user://settings.cfg. Presets: data/game/settings.json.
extends Node

const PATH := "user://settings.cfg"
const MSAA_MODES := [Viewport.MSAA_DISABLED, Viewport.MSAA_2X, Viewport.MSAA_4X]

var preset := ""


func _ready() -> void:
	var cfg := ConfigFile.new()
	cfg.load(PATH)
	preset = cfg.get_value("graphics", "preset", DataTables.game("settings").default)


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
