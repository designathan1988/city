## Player settings (graphics preset, the road tool's snapping), persisted in user://settings.cfg, and the frame cap (Engine.max_fps): the game
## never renders faster than it needs to, and slower while another application has the focus (vsync alone does not hold
## a covered window back: it ran past twice the refresh rate). Presets and caps: data/game/settings.json.
extends Node

const PATH := "user://settings.cfg"
const MSAA_MODES := [Viewport.MSAA_DISABLED, Viewport.MSAA_2X, Viewport.MSAA_4X]

var preset := ""
## The road tool's snapping: one switch for all of it, then each kind (Roadcraft editor/snap.ts RoadSnap).
var road_snap := {"on": true, "angles": true, "grid": true}


func _ready() -> void:
	var cfg := ConfigFile.new()
	cfg.load(PATH)
	preset = cfg.get_value("graphics", "preset", DataTables.game("settings").default)
	for key in road_snap:
		road_snap[key] = cfg.get_value("road_snap", key, road_snap[key])
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
	_store("graphics", "preset", name)


func set_road_snap(key: String, value: bool) -> void:
	road_snap[key] = value
	_store("road_snap", key, value)


## One value written, the others kept.
func _store(section: String, key: String, value: Variant) -> void:
	var cfg := ConfigFile.new()
	cfg.load(PATH)
	cfg.set_value(section, key, value)
	cfg.save(PATH)


func current() -> Dictionary:
	return DataTables.game("settings").presets[preset]


func apply(world: CityWorld, viewport: Viewport) -> void:
	var p := current()
	world.sun.shadow_enabled = p.shadows
	world.daylight.shadow_least_m = p.shadow_distance
	RenderingServer.directional_shadow_atlas_set_size(int(p.shadow_size), true)
	RenderingServer.directional_soft_shadow_filter_set_quality(int(p.shadow_filter) as RenderingServer.ShadowQuality)
	world.environment.ssao_enabled = p.occlusion
	viewport.screen_space_aa = Viewport.SCREEN_SPACE_AA_SMAA if p.smaa else Viewport.SCREEN_SPACE_AA_DISABLED
	viewport.msaa_3d = MSAA_MODES[int(p.msaa)]
	viewport.scaling_3d_scale = p.render_scale
