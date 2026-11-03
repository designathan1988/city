## Builds the 3D world around the core: Terrain3D (ground override shader), sun, sky, water, and the materials the
## C++ renderers draw with (data/render/materials.json). Colours come from data/render/palette.json.
class_name CityWorld
extends Node3D

const TERRAIN_SHADER := "res://render/shaders/terrain.gdshader"
const WATER_SHADER := "res://render/shaders/water.gdshader"
const SKY_SHADER := "res://render/shaders/sky.gdshader"
const WATER_EXTENT_M := 70000.0
const PLACEHOLDER_TEX_PX := 16
## Kept as text so the orphan gate sees every shader the native renderers load by name from materials.json.
const NATIVE_SHADERS := [
	"res://render/shaders/road.gdshader", "res://render/shaders/building.gdshader",
	"res://render/shaders/props.gdshader", "res://render/shaders/agents.gdshader",
	"res://render/shaders/people.gdshader", "res://render/shaders/overlay.gdshader",
]

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
			for name in table[key].params:
				m.set_shader_parameter(name, _param(table[key].params[name]))
			_materials[key] = m
	return _materials


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
		mat.set_shader_param(key, _param(palette.terrain[key]))


func _build_water(palette: Dictionary) -> void:
	var plane := PlaneMesh.new()
	plane.size = Vector2(WATER_EXTENT_M, WATER_EXTENT_M)
	var mat := ShaderMaterial.new()
	mat.shader = load(WATER_SHADER)
	mat.set_shader_parameter("shallow", Color.html(palette.water.shallow))
	mat.set_shader_parameter("deep", Color.html(palette.water.deep))
	var water := MeshInstance3D.new()
	water.mesh = plane
	water.material_override = mat
	water.position.y = palette.water.sea_level_m
	water.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	add_child(water)
