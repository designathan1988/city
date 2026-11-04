## Player settings, persisted in user://settings.cfg: the graphics preset (auto or a tier) and what was chosen over it, dynamic
## resolution, the frame limit, vsync, full screen, the language and the render switches. Everything is applied live.
## auto starts at the tier the GPU gives and the governor moves it. The frame cap (Engine.max_fps) follows the player's
## limit and drops while another application has the focus (vsync alone does not hold a covered window back).
extends Node

## A graphics choice was applied; the panels redraw.
signal changed

const PATH := "user://settings.cfg"
const SECTION := "graphics"
const MS_PER_S := 1000.0

var preset := ""
var kept: Dictionary = {} # shadows, ssao, smaa chosen over the tier
var level := "" # the tier in use
var hardware := ""
var dynamic_resolution := false
var fps_limit := 0
var vsync := true
var fullscreen := false
var language := ""
var switches: RenderSwitches
var optimizations: Dictionary = {} # the Effects panel: option id -> value over the tier
var _world: CityWorld
var _viewport: Viewport
var _quality_governor: FrameGovernor.Quality
var _resolution: FrameGovernor.Resolution
var _focus := true


func _ready() -> void:
	var t: Dictionary = DataTables.game("settings")
	var cfg := ConfigFile.new()
	cfg.load(PATH)
	preset = cfg.get_value(SECTION, "preset", t.default)
	if preset != "auto" and not t.tiers.has(preset):
		preset = t.default
	kept = cfg.get_value(SECTION, "kept", {})
	dynamic_resolution = cfg.get_value(SECTION, "dynamic_resolution", false)
	fps_limit = cfg.get_value(SECTION, "fps_limit", int(t.frame_cap.max_fps))
	vsync = cfg.get_value(SECTION, "vsync", true)
	fullscreen = cfg.get_value(SECTION, "fullscreen", false)
	language = cfg.get_value(SECTION, "language", "")
	switches = RenderSwitches.new(cfg.get_value(SECTION, "switches", {}))
	optimizations = cfg.get_value(SECTION, "optimizations", {})
	hardware = QualityTier.tier_for_gpu(t, RenderingServer.get_video_adapter_name())
	level = hardware if preset == "auto" else preset
	_quality_governor = FrameGovernor.Quality.new(t.governor, t.levels, level, hardware)
	_resolution = FrameGovernor.Resolution.new(t.dynamic_resolution)
	Engine.max_fps = fps_limit


## Tests and captures keep the full cap: what they measure is the frame, and their window seldom has the focus.
func _notification(what: int) -> void:
	if what == NOTIFICATION_APPLICATION_FOCUS_OUT and not GameFlow.tests_drive:
		_focus = false
		Engine.max_fps = int(DataTables.game("settings").frame_cap.background_fps)
	elif what == NOTIFICATION_APPLICATION_FOCUS_IN:
		_focus = true
		Engine.max_fps = fps_limit


func _save() -> void:
	var cfg := ConfigFile.new()
	cfg.set_value(SECTION, "preset", preset)
	cfg.set_value(SECTION, "kept", kept)
	cfg.set_value(SECTION, "dynamic_resolution", dynamic_resolution)
	cfg.set_value(SECTION, "fps_limit", fps_limit)
	cfg.set_value(SECTION, "vsync", vsync)
	cfg.set_value(SECTION, "fullscreen", fullscreen)
	cfg.set_value(SECTION, "language", language)
	cfg.set_value(SECTION, "switches", switches.saved())
	cfg.set_value(SECTION, "optimizations", optimizations)
	if not GameFlow.tests_drive:
		cfg.save(PATH)


## The tier in force, with the choices kept over it.
func current() -> Dictionary:
	var q := QualityTier.merged(DataTables.game("settings").tiers[level], kept)
	for o: Dictionary in DataTables.game("optimizations").options:
		if optimizations.has(o.id):
			q[o.field] = optimizations[o.id]
	return q


## The play scene's world and viewport: from now on every choice reaches them.
func attach(world: CityWorld, viewport: Viewport) -> void:
	_world = world
	_viewport = viewport
	RenderingServer.viewport_set_measure_render_time(viewport.get_viewport_rid(), true)
	DisplayServer.window_set_vsync_mode(DisplayServer.VSYNC_ENABLED if vsync else DisplayServer.VSYNC_DISABLED)
	set_fullscreen(fullscreen)
	apply_now()


func apply_now() -> void:
	if _world == null:
		return
	QualityTier.apply(current(), _world, _viewport, _resolution.scale * switches.resolution)
	switches.apply(_world, _viewport, current())
	changed.emit()


## Auto or a tier; a tier chosen by hand forgets what was chosen over the last one.
func set_preset(next: String) -> void:
	preset = next
	level = hardware if next == "auto" else next
	kept = {}
	_quality_governor.set_level(level)
	_resolution.reset()
	apply_now()
	_save()


## One choice over the tier: shadows (0 none), ssao or smaa.
func keep(key: String, value: Variant) -> void:
	kept[key] = value
	apply_now()
	_save()


func set_dynamic_resolution(on: bool) -> void:
	dynamic_resolution = on
	if not on:
		_resolution.reset()
	apply_now()
	_save()


func set_fps_limit(fps: int) -> void:
	fps_limit = fps
	if _focus:
		Engine.max_fps = fps
	changed.emit()
	_save()


func set_vsync(on: bool) -> void:
	vsync = on
	DisplayServer.window_set_vsync_mode(DisplayServer.VSYNC_ENABLED if on else DisplayServer.VSYNC_DISABLED)
	changed.emit()
	_save()


func set_fullscreen(on: bool) -> void:
	fullscreen = on
	DisplayServer.window_set_mode(DisplayServer.WINDOW_MODE_FULLSCREEN if on else DisplayServer.WINDOW_MODE_WINDOWED)
	changed.emit()
	_save()


func set_language(code: String) -> void:
	language = code
	TranslationServer.set_locale(code)
	_save()


func switches_changed() -> void:
	apply_now()
	_save()


## The GPU's last frame against the budget: the refresh rate or the player's limit.
func _budget_ms() -> float:
	var hz := DisplayServer.screen_get_refresh_rate()
	var fps := float(fps_limit) if fps_limit > 0 else (hz if hz > 0.0 else float(DataTables.game("settings").frame_cap.max_fps))
	return MS_PER_S / fps


func _process(delta: float) -> void:
	if _world == null or GameFlow.tests_drive:
		return
	var gpu := RenderingServer.viewport_get_measured_render_time_gpu(_viewport.get_viewport_rid())
	var budget := _budget_ms()
	if preset == "auto":
		var next := _quality_governor.sample(delta, gpu, budget)
		if next != "" and next != level:
			level = next
			_quality_governor.set_level(next)
			apply_now()
			return
	if dynamic_resolution and _resolution.sample(gpu, budget) > 0.0:
		apply_now()
