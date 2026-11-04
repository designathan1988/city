## Builds the 3D world around the core: Terrain3D (ground override shader), sun, sky, water, and the materials the
## C++ renderers draw with (data/render/materials.json). Colours come from data/render/palette.json.
class_name CityWorld
extends Node3D

const TERRAIN_SHADER := "res://render/shaders/terrain.gdshader"
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
	"res://render/shaders/vegetation_cards.gdshader", "res://render/shaders/puffs.gdshader",
	"res://render/shaders/debris.gdshader", "res://render/shaders/marks.gdshader",
]
const LENSES := 5 # props.gdshader lit_radiance (prop_kit.h Lens::COUNT)

var terrain: Node3D
var sun: DirectionalLight3D
var environment: Environment
var daylight: Daylight
var _materials := {} # kept alive here: the core draws with their RIDs
var _terrain_material: Resource


func _ready() -> void:
	var palette: Dictionary = DataTables.render().palette
	_build_environment(palette)
	_build_terrain(palette)
	_build_water(palette)


## The materials of data/render/materials.json, by name (textures loaded from their res:// paths).
func materials() -> Dictionary:
	if _materials.is_empty():
		var table: Dictionary = DataTables.render().materials
		for key in table:
			if key.begins_with("_"):
				continue
			var m := ShaderMaterial.new()
			m.shader = load(table[key].shader)
			m.render_priority = int(table[key].get("render_priority", 0)) # between transparent layers: the later is drawn over
			for name in table[key].params:
				m.set_shader_parameter(name, _param(table[key].params[name]))
			if key == "road":
				_set_means(m, table[key].params)
			elif key == "prop":
				_set_lens_radiance(m)
			elif key == "marks":
				_set_crater_radiance(m, table[key].display_colors.crater)
			_materials[key] = m
	return _materials


## The mean colour of each surface map, computed once here and sent as a uniform (road.gdshader divides by it): the
## shader no longer samples the smallest mip every pixel.
func _set_means(m: ShaderMaterial, params: Dictionary) -> void:
	for name in params:
		if not name.ends_with("_map"):
			continue
		var col := _mean_color(_param(params[name]))
		m.set_shader_parameter("mean_" + name.trim_suffix("_map"), col)


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


## The crater's colours (the reference game's, as the screen shows them), as the scene radiance that shows as each under the
## Environment's tone mapping (render/tone_map.gd): the unlit crater decal is not darkened by the ACES toe.
func _set_crater_radiance(m: ShaderMaterial, shown: Array) -> void:
	var rad := PackedVector3Array()
	for html: String in shown:
		rad.append(ToneMap.radiance(Color.html(html), environment.tonemap_exposure, environment.tonemap_white))
	m.set_shader_parameter("crater_rad", rad)


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
	if v is Array:
		return PackedFloat32Array(v)
	return v


func _build_environment(_palette: Dictionary) -> void:
	var day: Dictionary = DataTables.game("daylight")
	sun = DirectionalLight3D.new()
	sun.shadow_enabled = true
	sun.directional_shadow_mode = DirectionalLight3D.SHADOW_PARALLEL_4_SPLITS
	sun.directional_shadow_max_distance = day.shadow_max_distance_m
	sun.directional_shadow_split_1 = day.shadow_split_1
	sun.directional_shadow_split_2 = day.shadow_split_2
	sun.directional_shadow_split_3 = day.shadow_split_3
	sun.directional_shadow_blend_splits = true
	sun.directional_shadow_fade_start = day.shadow_fade_start
	sun.shadow_normal_bias = day.shadow_normal_bias
	sun.shadow_bias = day.shadow_bias
	add_child(sun)
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
	environment.adjustment_enabled = day.contrast != 1.0 or day.saturation != 1.0
	environment.adjustment_contrast = day.contrast
	environment.adjustment_saturation = day.saturation
	var we := WorldEnvironment.new()
	we.environment = environment
	add_child(we)
	daylight = Daylight.new(day, sun, environment, sky_mat)
	daylight.set_hour(day.day_hour)


## Pins the light to an hour of the day (Roadcraft "always day" 16:00, "always night" 22:00).
func set_hour(hour: float) -> void:
	daylight.set_hour(hour)


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


func _build_water(palette: Dictionary) -> void:
	var plane := PlaneMesh.new()
	plane.size = Vector2(WATER_EXTENT_M, WATER_EXTENT_M)
	var mat := ShaderMaterial.new()
	mat.shader = load(WATER_SHADER)
	for key in palette.water:
		if key != "sea_level_m" and not key.begins_with("_"):
			mat.set_shader_parameter(key, _param(palette.water[key]))
	# The ripples move the way the wind blows: the vegetation material holds the wind (its single owner).
	var wind: Dictionary = DataTables.render().materials.vegetation.params
	mat.set_shader_parameter("wind_x", wind.wind_dir_x)
	mat.set_shader_parameter("wind_z", wind.wind_dir_z)
	var water := MeshInstance3D.new()
	water.mesh = plane
	water.material_override = mat
	water.position.y = palette.water.sea_level_m
	water.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	add_child(water)
