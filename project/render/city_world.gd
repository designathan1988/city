## Builds the 3D world around the core: Terrain3D (ground override shader), sun, sky, water, and the materials the
## C++ renderers draw with (data/render/materials.json). Colours come from data/render/palette.json.
class_name CityWorld
extends Node3D

const TERRAIN_SHADER := "res://render/shaders/terrain.gdshader"
const LUMA := Vector3(0.3, 0.6, 0.1) # the weights terrain_layers.gdshaderinc reads a colour's luma with
const WATER_SHADER := "res://render/shaders/water.gdshader"
const SKY_SHADER := "res://render/shaders/sky.gdshader"
const WATER_EXTENT_M := 70000.0
const PLACEHOLDER_TEX_PX := 16
const MEAN_PX := 64 # side of the shrunk copy whose average becomes a surface map's mean colour
## Kept as text so the orphan gate sees every shader the native renderers load by name from materials.json.
const NATIVE_SHADERS := [
	"res://render/shaders/road.gdshader", "res://render/shaders/building.gdshader",
	"res://render/shaders/building_far.gdshader", "res://render/shaders/building_parts.gdshader",
	"res://render/shaders/props.gdshader", "res://render/shaders/agents.gdshader",
	"res://render/shaders/people.gdshader", "res://render/shaders/overlay.gdshader",
	"res://render/shaders/agent_glass.gdshader", "res://render/shaders/vegetation.gdshader",
	"res://render/shaders/vegetation_cards.gdshader",
]
const WIND_FULL_WAVES_MS := 40.0 # a wind this fast alone makes the roughest water
const LENSES := 5 # props.gdshader lit_radiance (prop_kit.h Lens::COUNT)

var terrain: Node3D
var sun: DirectionalLight3D
var environment: Environment
var daylight: Daylight
var sky: SkyView
var _water_mat: ShaderMaterial
var _materials := {} # kept alive here: the core draws with their RIDs
var _terrain_material: Resource


var sky_mode: String:
	get:
		return daylight.mode


func _ready() -> void:
	var palette: Dictionary = DataTables.render().palette
	_build_environment(palette)
	_build_terrain(palette)
	_build_water(palette)
	sky = SkyView.new(self)
	add_child(sky)


## The materials of data/render/materials.json, by name (textures loaded from their res:// paths).
func materials() -> Dictionary:
	if _materials.is_empty():
		var table: Dictionary = DataTables.render().materials
		for key in table:
			if key.begins_with("_"):
				continue
			var m := ShaderMaterial.new()
			m.shader = load(table[key].shader)
			for name in table[key].params:
				m.set_shader_parameter(name, _param(table[key].params[name]))
			if key == "road":
				_set_means(m, table[key].params)
			elif key == "prop":
				_set_lens_radiance(m)
			_materials[key] = m
		_materials["water"] = _water_mat # the lakes and rivers of the core (render/water) wear the sea's material
	return _materials


## The mean colour of each surface map, computed once here and sent as a uniform (road.gdshader divides by it): the
## shader no longer samples the smallest mip every pixel.
func _set_means(m: ShaderMaterial, params: Dictionary) -> void:
	for name in params:
		if not name.ends_with("_map"):
			continue
		var col := _mean_color(_param(params[name]))
		m.set_shader_parameter("mean_" + name.trim_suffix("_map"), col)
	m.set_shader_parameter("asphalt_reference", Color.html(DataTables.render().streets.surface_colors[0]))


## Each lens lit (data/render/streets.json props.signal): the scene radiance the Environment's tone mapping shows as its
## colour (render/tone_map.gd mirrors Godot's ACES).
func _set_lens_radiance(m: ShaderMaterial) -> void:
	if environment.tonemap_mode != Environment.TONE_MAPPER_ACES:
		push_error("CityWorld: ToneMap só espelha o ACES; as lentes não mostrariam a sua cor")
	var lit := PackedVector3Array()
	lit.resize(LENSES)
	var done := {}
	for part: Dictionary in DataTables.render().streets.props.signal:
		if part.has("lens") and not done.has(int(part.lens)):
			done[int(part.lens)] = true
			lit[int(part.lens)] = ToneMap.radiance(Color.html(part.color), environment.tonemap_exposure,
					environment.tonemap_white)
	m.set_shader_parameter("lit_radiance", lit)


func _mean_color(tex: Texture2D) -> Color:
	if tex == null:
		return Color(1, 1, 1)
	var img := tex.get_image()
	if img == null:
		return Color(1, 1, 1)
	img = img.duplicate()
	if img.is_compressed():
		if img.decompress() != OK:
			return Color(1, 1, 1)
	img.resize(MEAN_PX, MEAN_PX, Image.INTERPOLATE_BILINEAR)
	var sum := Vector3.ZERO
	for y in MEAN_PX:
		for x in MEAN_PX:
			var c := img.get_pixel(x, y)
			sum += Vector3(c.r, c.g, c.b)
	var n := float(MEAN_PX * MEAN_PX)
	return Color(sum.x / n, sum.y / n, sum.z / n)


func _param(v: Variant) -> Variant:
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


## The map's biome tints the grass (0 none, else the biome's index + 1).
func set_map_biome(region: int) -> void:
	_terrain_material.set_shader_param("map_biome", region + 1)


## How rough the water is, how much foam and how fast it runs; the wind's own ripples add to the waves.
func set_water_look(weather: Dictionary) -> void:
	_water_mat.set_shader_parameter("waves", minf(1.0, weather.waves + weather.wind / WIND_FULL_WAVES_MS))
	_water_mat.set_shader_parameter("foam", weather.foam)
	_water_mat.set_shader_parameter("current", weather.current)
	_terrain_material.set_shader_param("gully_auto", weather.gully_auto)


func _build_environment(_palette: Dictionary) -> void:
	var day: Dictionary = DataTables.game("daylight")
	sun = Daylight.make_sun(day)
	add_child(sun)
	var fill := DirectionalLight3D.new()
	fill.shadow_enabled = false
	add_child(fill)
	var sky_mat := ShaderMaterial.new()
	sky_mat.shader = load(SKY_SHADER)
	var sky := Sky.new()
	sky.sky_material = sky_mat
	sky.radiance_size = Sky.RADIANCE_SIZE_128
	environment = Environment.new()
	environment.background_mode = Environment.BG_SKY
	environment.sky = sky
	# The same fill on every renderer: a flat ambient colour (daylight.gd sets colour and energy; no sky share, so
	# Forward+ and Compatibility light the shade alike) and only the specular reflections from the sky. Forward+
	# renders in RGBA16F and Compatibility in RGBA8 with the tonemap applied in the material pass; both support
	# these modes, ACES and fog (docs: Environment and post-processing, Overview of renderers).
	environment.ambient_light_source = Environment.AMBIENT_SOURCE_COLOR
	environment.ambient_light_sky_contribution = 0.0
	environment.reflected_light_source = Environment.REFLECTION_SOURCE_SKY
	environment.tonemap_mode = Environment.TONE_MAPPER_ACES
	environment.tonemap_exposure = day.exposure
	environment.tonemap_white = day.tonemap_white
	environment.ssao_enabled = false
	environment.fog_enabled = true
	environment.fog_sky_affect = 0.0
	environment.glow_hdr_threshold = day.glow_threshold
	environment.glow_blend_mode = Environment.GLOW_BLEND_MODE_ADDITIVE
	FilmLook.grade(environment, day.grade)
	var we := WorldEnvironment.new()
	we.environment = environment
	add_child(we)
	FilmLook.vignette(self, day.vignette)
	daylight = Daylight.new(day, sun, fill, environment, sky_mat)
	add_child(ElementEffects.new())
	daylight.set_hour(day.day_hour)


## Pins the light to an hour of the day (tests and captures; the sky mode owns it in play).
func set_hour(hour: float) -> void:
	daylight.set_hour(hour)


## "day", "night" or "cycle" (render/daylight.gd): the top bar's sun, moon and clock.
func set_sky_mode(mode: String) -> void:
	daylight.set_mode(mode)


func follow_clock(hour: float) -> void:
	daylight.follow_clock(hour)


func _process(_delta: float) -> void:
	if daylight.mode == "cycle" and CityStore.ready_to_read():
		daylight.follow_clock(CityStore.snapshot.hour)


func _build_terrain(palette: Dictionary) -> void:
	terrain = ClassDB.instantiate(&"Terrain3D")
	terrain.name = "Terrain3D"
	var mat: Resource = ClassDB.instantiate(&"Terrain3DMaterial")
	mat.shader_override = load(TERRAIN_SHADER)
	mat.shader_override_enabled = true
	terrain.material = mat
	_terrain_material = mat
	# Terrain3D forces its checkered debug view while the asset list has no texture; the override shader paints
	# its own ground, so one tiny placeholder texture is enough to keep that view off.
	var assets: Resource = ClassDB.instantiate(&"Terrain3DAssets")
	var tex_asset: Resource = ClassDB.instantiate(&"Terrain3DTextureAsset")
	var img := Image.create(PLACEHOLDER_TEX_PX, PLACEHOLDER_TEX_PX, true, Image.FORMAT_RGBA8)
	img.fill(Color.WHITE)
	img.generate_mipmaps()
	tex_asset.albedo_texture = ImageTexture.create_from_image(img)
	assets.set_texture_asset(0, tex_asset)
	terrain.assets = assets
	terrain.collision_mode = 0 # disabled: picking uses the core's raycast
	terrain.free_editor_textures = false
	terrain.cast_shadows = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF # the ground only receives (Roadcraft terrain.ts)
	add_child(terrain)
	# Terrain3D rebuilds its material when it enters the tree and drops parameters set before that (the ground then
	# sampled the shader's white default texture), so the look is applied once the terrain is in the scene.
	for key in palette.terrain:
		if not key.begins_with("_"):
			mat.set_shader_param(key, _param(palette.terrain[key]))
	var grass_mean := _mean_color(_param(palette.terrain.grass_map)).srgb_to_linear()
	mat.set_shader_param("grass_mean", grass_mean.r * LUMA.x + grass_mean.g * LUMA.y + grass_mean.b * LUMA.z)


func _build_water(palette: Dictionary) -> void:
	var plane := PlaneMesh.new()
	plane.size = Vector2(WATER_EXTENT_M, WATER_EXTENT_M)
	var mat := ShaderMaterial.new()
	mat.shader = load(WATER_SHADER)
	for key in palette.water:
		if key != "sea_level_m" and not key.begins_with("_"):
			mat.set_shader_parameter(key, _param(palette.water[key]))
	_water_mat = mat
	var water := MeshInstance3D.new()
	water.mesh = plane
	water.material_override = mat
	water.position.y = palette.water.sea_level_m
	water.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	add_child(water)
