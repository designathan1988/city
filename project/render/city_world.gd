## Builds the 3D world around the core: Terrain3D (ground override shader), sun, sky, water; keeps the materials the C++
## renderers draw with (WorldMaterials makes them) and fans the landscape the sky reads out to the land and the water.
## Colours come from data/render/palette.json.
class_name CityWorld
extends Node3D

const TERRAIN_SHADER := "res://render/shaders/terrain.gdshader"
const SKY_SHADER := "res://render/shaders/sky.gdshader"
const WIND_FULL_WAVES_MS := 40.0 # a wind this fast alone makes the roughest water

var terrain: Node3D
var sun: DirectionalLight3D
var environment: Environment
var daylight: Daylight
var sky: SkyView
var occlusion: MeshInstance3D = preload("res://render/occlusion_pass.gd").new() # the corners of the town: the one owner of the screen-space occlusion
var contact: MeshInstance3D = preload("res://render/contact_pass.gd").new() # the sun's shade where things touch
var _materials := {} # kept alive here: the core draws with their RIDs
var _dither := 0.0
var _terrain_material: Resource


var marks: ChangeMarks # F8
var water: CityWater
var _mesh_size := 0
var _terrain_target: Node3D # what the terrain's clipmap centres on (_follow_view)
var _sky_table: Dictionary # the contact shade's share of the sun (data/game/sky.json)
var sky_mode: String:
	get:
		return daylight.mode


func _ready() -> void:
	add_to_group(&"world")
	var palette: Dictionary = DataTables.render_table("palette")
	_build_environment(palette)
	_build_terrain(palette)
	water = CityWater.new(palette)
	add_child(water)
	marks = ChangeMarks.new()
	add_child(marks)
	sky = SkyView.new(self)
	sky.landscape_read.connect(_take_landscape)
	add_child(sky)
	_sky_table = DataTables.game("sky")


## The materials of data/render/materials.json, by name (WorldMaterials), kept alive here, with the sign sheet the props read.
func materials() -> Dictionary:
	if _materials.is_empty():
		_materials = WorldMaterials.make_all(environment, _set_ground)
		add_child(SignSheet.new(DataTables.render_table("streets").signs, _materials.prop))
		_materials["water"] = water.own # the lakes and rivers of the core (render/water) wear the sea's material
	return _materials


## The verge wears the terrain's grass: the same table, the same values.
func _set_ground(m: ShaderMaterial) -> void:
	var ground: Dictionary = DataTables.render_table("palette").terrain
	var kinds := WorldMaterials.uniform_kinds(m.shader)
	for key in ground:
		if not key.begins_with("_"):
			m.set_shader_parameter(key, WorldMaterials.typed(kinds, key, ground[key]))
	m.set_shader_parameter("grass_mean", _grass_mean(ground))


func _grass_mean(ground: Dictionary) -> float:
	var mean := SurfaceMeans.mean_color(shader_value(ground.grass_map)).srgb_to_linear()
	var luma: Vector3 = CityStore.constants.shader.LUMA # the weights terrain_layers.gdshaderinc reads a colour's luma with
	return mean.r * luma.x + mean.g * luma.y + mean.b * luma.z


static func shader_value(v: Variant) -> Variant:
	if v is String and v.begins_with("res://"):
		return load(v)
	if v is String and v.begins_with("#"):
		return Color.html(v)
	if v is Array and not v.is_empty() and v[0] is String:
		var colours := PackedColorArray()
		for c: String in v:
			colours.append(Color.html(c))
		return colours
	if v is Array:
		return PackedFloat32Array(v)
	return v


## What the landscape says of the land and the water, as the sky read it; every map's first read (a new map resets nature, an
## opened one moves its revision) gives the terrain the map's half side, where its edge stands.
func _take_landscape(land: Dictionary) -> void:
	set_map_biome(land.biome)
	set_water_look(land.weather)
	_terrain_material.set_shader_param("map_half", float(CityStore.query(&"city_bounds").half))


## The map's biome tints the grass (0 none, else the biome's index + 1).
func set_map_biome(region: int) -> void:
	_terrain_material.set_shader_param("map_biome", region + 1)
	if _materials.has("verge"):
		_materials.verge.set_shader_parameter("map_biome", region + 1)


## How rough the water is, how much foam and how fast it runs; the wind's own ripples add to the waves.
func set_water_look(weather: Dictionary) -> void:
	water.own.set_shader_parameter("waves", minf(1.0, weather.waves + weather.wind / WIND_FULL_WAVES_MS))
	water.own.set_shader_parameter("foam", weather.foam)
	water.own.set_shader_parameter("current", weather.current)
	_terrain_material.set_shader_param("gully_auto", weather.gully_auto)


func _build_environment(_palette: Dictionary) -> void:
	var day: Dictionary = DataTables.game("daylight")
	sun = DaylightRig.sun(day)
	add_child(sun)
	var fill := DaylightRig.plain()
	add_child(fill)
	var sky_top := DaylightRig.top()
	add_child(sky_top)
	var under := DaylightRig.under()
	add_child(under)
	var sky_mat := ShaderMaterial.new()
	sky_mat.shader = load(SKY_SHADER)
	var sky := Sky.new()
	sky.sky_material = sky_mat
	sky.radiance_size = Sky.RADIANCE_SIZE_128
	environment = Environment.new()
	environment.background_mode = Environment.BG_SKY
	environment.sky = sky
	# The same shade on every renderer: a flat ambient colour and the sky's top light (daylight.gd), no sky share;
	# only the specular reflections come from the sky. Forward+ renders in RGBA16F and Compatibility in RGBA8 with the
	# tonemap in the material pass; both support these modes (docs: Environment and post-processing).
	environment.ambient_light_source = Environment.AMBIENT_SOURCE_COLOR
	environment.ambient_light_sky_contribution = 0.0
	environment.reflected_light_source = Environment.REFLECTION_SOURCE_SKY
	environment.tonemap_mode = Environment.TONE_MAPPER_ACES
	environment.tonemap_exposure = day.exposure
	environment.tonemap_white = day.tonemap_white
	environment.fog_enabled = true
	environment.fog_sky_affect = 0.0
	environment.fog_light_energy = day.sky_gain
	environment.glow_hdr_threshold = day.glow_threshold
	environment.glow_blend_mode = Environment.GLOW_BLEND_MODE_ADDITIVE
	environment.ssil_radius = day.ssil.radius_m
	environment.ssil_intensity = day.ssil.intensity
	FilmLook.grade(environment, day.grade)
	var we := WorldEnvironment.new()
	we.environment = environment
	add_child(we)
	occlusion.setup(day.occlusion)
	add_child(occlusion)
	contact.setup()
	add_child(contact)
	FilmLook.vignette(self, day.vignette)
	daylight = Daylight.new(day, sun, fill, sky_top, under, environment, sky_mat)
	add_child(ElementEffects.new())
	add_child(ShoreView.new(self))
	daylight.set_hour(day.day_hour)


## Pins the light to an hour of the day (tests and captures; the sky mode owns it in play).
func set_hour(hour: float) -> void:
	daylight.set_hour(hour)


## "day", "night" or "cycle" (render/daylight.gd): the top bar's sun, moon and clock.
func set_sky_mode(mode: String) -> void:
	daylight.set_mode(mode)


func follow_clock(hour: float) -> void:
	daylight.follow_clock(hour)


## The clock of the dissolves' noise back to its start: with the game's time stopped, the same picture every run.
func still_dither() -> void:
	_dither = 0.0
	RenderingServer.global_shader_parameter_set(&"dither_clock", 0.0)


func _process(_delta: float) -> void:
	if _delta > 0.0:
		_dither = fmod(_dither + _delta, 1.0)
		RenderingServer.global_shader_parameter_set(&"dither_clock", _dither)
	if daylight.mode == "cycle" and CityStore.ready_to_read():
		daylight.follow_clock(CityStore.snapshot.hour)
	var cam := get_viewport().get_camera_3d()
	daylight.follow_view(cam)
	_follow_view(cam)
	if sky.step(_delta):
		var toward: Vector3 = sun.global_basis.z
		contact.follow(toward, _sky_table.contact_most * daylight.direct_share * clampf(toward.y * _sky_table.contact_sun_gain, 0.0, 1.0))


## The terrain's clipmap and LODs centre on the camera that draws the city, or on the world's origin while none does (a test of the
## core). Terrain3D 1.1 falls back to the viewport's camera only while it has no target, and with neither it errs and stops its
## physics process, the LODs frozen (src/terrain_3d.cpp _grab_camera); a target set here never leaves it without one.
func _follow_view(cam: Camera3D) -> void:
	var want: Node3D = cam if cam else self
	if want != _terrain_target:
		_terrain_target = want
		terrain.clipmap_target = want


## The Optimisations tab: the terrain's nearest meshes divided (Terrain3D mesh_size: lod0 has 4 mesh_size + 2 quads a side).
func set_terrain_lod(divide: int) -> void:
	var size: int = _mesh_size / maxi(1, divide)
	if terrain.get("mesh_size") != size:
		terrain.set("mesh_size", size)


func _build_terrain(palette: Dictionary) -> void:
	_terrain_material = ClassDB.instantiate(&"Terrain3DMaterial")
	terrain = CityTerrain.make(_terrain_material, load(TERRAIN_SHADER))
	_follow_view(null) # a target before its first physics frame, even with no camera yet
	add_child(terrain)
	_mesh_size = terrain.get("mesh_size")
	# Terrain3D drops parameters set before it enters the tree: the look is applied once the terrain is in the scene
	var kinds := WorldMaterials.uniform_kinds(load(TERRAIN_SHADER))
	for key in palette.terrain:
		if not key.begins_with("_"):
			_terrain_material.set_shader_param(key, WorldMaterials.typed(kinds, key, palette.terrain[key]))
	_terrain_material.set_shader_param("grass_mean", _grass_mean(palette.terrain))
	_terrain_material.set_shader_param("gully_auto", DataTables.catalog_table("nature").weather.gully_auto.default) # until the weather is read
	_terrain_material.set_shader_param("map_half", float(DataTables.maps()[0].tiles) * DataTables.tuning("world_tile_m") / 2.0) # the first map's, until a map is read
