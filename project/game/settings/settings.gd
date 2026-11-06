## Player settings, persisted in user://settings.cfg: the graphics preset (auto or a tier) and what was chosen over it, dynamic
## resolution, the frame limit, vsync, full screen, the language, the render switches and the road tool's snapping (Roadcraft
## editor/snap.ts RoadSnap). Everything is applied live.
## auto starts at the tier the GPU gives and the governor moves it. The frame cap (Engine.max_fps) follows the player's
## limit and drops while another application has the focus (vsync alone does not hold a covered window back).
extends Node

## A graphics choice was applied; the panels redraw.
signal changed

const PATH := "user://settings.cfg"
const SECTION := "graphics"

var preset := ""
var road := RoadPrefs.new()
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
var layers: Dictionary = {} # what shows over the map: layer -> on
var zone_tool := false # the zone brush is in hand
var _zoned := false # the player has zoned sides on this city, as last shown
var _world: CityWorld
var _viewport: Viewport
var _quality_governor: FrameGovernor.Quality
var _resolution: FrameGovernor.Resolution
var _focus := true


func _ready() -> void:
	var t: Dictionary = DataTables.game("settings")
	var cfg := ConfigFile.new()
	if not GameFlow.tests_drive: # tests and captures run on the tables' defaults, whatever the player or another clone left in the file
		cfg.load(PATH)
	preset = known_preset(t.tests_preset if GameFlow.tests_drive else cfg.get_value(SECTION, "preset", t.default))
	kept = cfg.get_value(SECTION, "kept", {})
	dynamic_resolution = cfg.get_value(SECTION, "dynamic_resolution", false)
	fps_limit = cfg.get_value(SECTION, "fps_limit", int(t.frame_cap.max_fps))
	vsync = cfg.get_value(SECTION, "vsync", true)
	fullscreen = cfg.get_value(SECTION, "fullscreen", false)
	language = cfg.get_value(SECTION, "language", "")
	switches = RenderSwitches.new(cfg.get_value(SECTION, "switches", {}))
	optimizations = cfg.get_value(SECTION, "optimizations", {})
	layers = cfg.get_value(SECTION, "layers", t.layers.duplicate())
	road.read(cfg)
	hardware = QualityTier.tier_for_gpu(t, RenderingServer.get_video_adapter_name())
	level = QualityTier.level_of(preset, hardware)
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
	cfg.set_value(SECTION, "layers", layers)
	road.write(cfg)
	if not GameFlow.tests_drive:
		cfg.save(PATH)


func set_road_snap(key: String, value: bool) -> void:
	road.snap[key] = value
	_save()


func set_road_keys(keys: Dictionary) -> void:
	road.keys = keys
	_save()


func set_road_walls(on: bool) -> void:
	road.walls = on
	_save()


## What a settings file or a caller names, as a preset of the table: another version's name, or no name, is the default.
func known_preset(name: Variant) -> String:
	var t: Dictionary = DataTables.game("settings")
	return name if name is String and t.presets.has(name) else t.default


## The tier in force, with the choices kept over it and the Effects panel's options.
func current() -> Dictionary:
	return QualityTier.resolved(level, kept, optimizations)


func layer_on(name: String) -> bool:
	return layers.get(name, false)


## A row of the Layers popover: the grid is the ground's, the others the core's overlays.
func set_layer(name: String, on: bool) -> void:
	layers[name] = on
	_show_layer(name)
	_save()


func _show_layer(name: String) -> void:
	if name == "grid":
		switches.set_global(&"map_grid", 1.0 if layer_on(name) else 0.0)
	elif name == "zones":
		CityStore.dispatch(&"show_every_zone", [zone_tool])
		CityStore.dispatch(&"set_overlay", [name, zone_tool or (layer_on(name) and _zoned)])
	else:
		CityStore.dispatch(&"set_overlay", [name, layer_on(name)])


## The zones show with their tool in hand, and, with the layer on, where the player zoned once it is put down (main.ts showLots).
func set_zone_tool(on: bool) -> void:
	zone_tool = on
	_show_layer("zones")


## The play scene's world and viewport: from now on every choice reaches them.
func attach(world: CityWorld, viewport: Viewport) -> void:
	_world = world
	_viewport = viewport
	RenderingServer.viewport_set_measure_render_time(viewport.get_viewport_rid(), true)
	DisplayServer.window_set_vsync_mode(DisplayServer.VSYNC_ENABLED if vsync else DisplayServer.VSYNC_DISABLED)
	set_fullscreen(fullscreen)
	for name: String in layers:
		_show_layer(name)
	apply_now()


func apply_now() -> void:
	if _world == null:
		return
	QualityTier.apply(current(), _world, _viewport, _resolution.scale * switches.resolution)
	switches.apply(_world, _viewport, current())
	changed.emit()


## Auto or a tier; a tier chosen by hand forgets what was chosen over the last one.
func set_preset(next: String) -> void:
	preset = known_preset(next)
	level = QualityTier.level_of(preset, hardware)
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


func _follow_zones() -> void:
	var zoned: bool = CityStore.snapshot.get("player_zones", 0) > 0
	if zoned != _zoned:
		_zoned = zoned
		_show_layer("zones")


func _process(delta: float) -> void:
	_follow_zones()
	if _world == null:
		return
	switches.follow_zoom(_world, _viewport, current())
	if GameFlow.tests_drive:
		return
	var gpu := RenderingServer.viewport_get_measured_render_time_gpu(_viewport.get_viewport_rid())
	var budget := QualityTier.budget_ms(fps_limit)
	if preset == "auto":
		var next := _quality_governor.sample(delta, gpu, budget)
		if next != "" and next != level:
			level = next
			_quality_governor.set_level(next)
			apply_now()
			return
	if dynamic_resolution and _resolution.sample(gpu, budget) > 0.0:
		apply_now()


## What a capture asks for on a world of its own: the preset in `preset`, with no window or layer of the player's.
func apply(world: CityWorld, viewport: Viewport) -> void:
	_world = world
	_viewport = viewport
	level = QualityTier.level_of(preset, hardware)
	apply_now()
