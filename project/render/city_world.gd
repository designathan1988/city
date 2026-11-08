## Builds the 3D world around the core: Terrain3D (ground override shader), sun, sky, water, and the materials the
## C++ renderers draw with (data/render/materials.json). Colours come from data/render/palette.json.
class_name CityWorld
extends Node3D

const TERRAIN_SHADER := "res://render/shaders/terrain.gdshader"
const SKY_SHADER := "res://render/shaders/sky.gdshader"
## Kept as text so the orphan gate sees every shader the native renderers load by name from materials.json.
const NATIVE_SHADERS := [
	"res://render/shaders/road.gdshader", "res://render/shaders/building.gdshader", "res://render/shaders/building_far.gdshader",
	"res://render/shaders/building_parts.gdshader", "res://render/shaders/props.gdshader", "res://render/shaders/agents.gdshader",
	"res://render/shaders/people.gdshader", "res://render/shaders/people_blob.gdshader", "res://render/shaders/overlay.gdshader", "res://render/shaders/agent_glass.gdshader",
	"res://render/shaders/vegetation.gdshader", "res://render/shaders/lamp_glow.gdshader", "res://render/shaders/vegetation_cards.gdshader", "res://render/shaders/building_ghost.gdshader",
	"res://render/shaders/building_glass.gdshader", "res://render/shaders/building_sheer.gdshader", "res://render/shaders/building_card.gdshader",
	"res://render/shaders/building_wash.gdshader",
	"res://render/shaders/verge.gdshader", "res://render/shaders/people_far.gdshader", "res://render/shaders/people_held.gdshader",
	"res://render/shaders/people_helmet.gdshader", "res://render/shaders/rubble.gdshader", "res://render/shaders/signature.gdshader",
	"res://render/shaders/signature_glass.gdshader",
	"res://render/shaders/transit.gdshader", "res://render/shaders/transit_glass.gdshader", "res://render/shaders/transit_xray.gdshader",
	"res://render/shaders/building_props.gdshader", "res://render/shaders/shadow_caster.gdshader",
	"res://render/shaders/road_far.gdshader", "res://render/shaders/building_fade.gdshader", "res://render/shaders/building_ruin.gdshader", "res://render/shaders/people_body.gdshader", "res://render/shaders/building_glass_fade.gdshader", "res://render/shaders/building_parts_fade.gdshader",
]
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
	add_child(sky)


## The materials of data/render/materials.json, by name (textures loaded from their res:// paths).
func materials() -> Dictionary:
	if _materials.is_empty():
		var table: Dictionary = DataTables.render_table("materials")
		for key in table:
			if key.begins_with("_"):
				continue
			var m := EffectMaterials.make(key, table[key], environment)
			for name in table[key].params:
				m.set_shader_parameter(name, shader_value(table[key].params[name]))
			if key == "road":
				SurfaceMeans.apply(m, table[key].params, shader_value)
			elif key == "verge":
				_set_ground(m)
			elif key == "prop":
				LampRadiance.lenses(m, environment)
			elif key == "agent":
				LampRadiance.vehicles(m, environment)
			_materials[key] = m
			if table[key].has("fade_shader"): # the same material for what is handing over to another level (building_level.gdshaderinc)
				var fade: ShaderMaterial = m.duplicate()
				fade.shader = load(table[key].fade_shader)
				_materials[key + "_fade"] = fade
			for variant: String in ["ruin", "body"]: # the same material for a building coming down in pieces (ruin_pieces.h) and somebody off their feet (body_renderer.h)
				if table[key].has(variant + "_shader"):
					var other: ShaderMaterial = m.duplicate()
					other.shader = load(table[key][variant + "_shader"])
					_materials[key + "_" + variant] = other
			for layer in int(table[key].get("layers", 0)): # the same material drawn in this order (render_priority)
				var over: ShaderMaterial = m.duplicate()
				over.render_priority = layer
				_materials["%s_%d" % [key, layer]] = over
		add_child(SignSheet.new(DataTables.render_table("streets").signs, _materials.prop))
		_materials["water"] = water.own # the lakes and rivers of the core (render/water) wear the sea's material
	return _materials


## The verge wears the terrain's grass: the same table, the same values.
func _set_ground(m: ShaderMaterial) -> void:
	var ground: Dictionary = DataTables.render_table("palette").terrain
	for key in ground:
		if not key.begins_with("_"):
			m.set_shader_parameter(key, shader_value(ground[key]))
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
	for key in palette.terrain:
		if not key.begins_with("_"):
			_terrain_material.set_shader_param(key, shader_value(palette.terrain[key]))
	_terrain_material.set_shader_param("grass_mean", _grass_mean(palette.terrain))
