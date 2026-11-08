## The sky as drawn: follows the core's weather and placed clouds (CityStore query "landscape"), lays the clouds' puffs for
## atmosphere.gdshader (render/cloud_puffs.gd), carries them on the wind, holds the wind in the shader globals (plants and water read it) and
## tells the daylight how overcast it is. Rain, bolts and thunder are render/weather_view.gd's.
class_name SkyView
extends Node3D

const METRES_A_KM := Units.METRES_PER_KM
## A view this close to level looks at no ground under it.
const LEVEL_VIEW := 0.0015
## Drift of the clouds so far, on the map (x, z), metres; the cloud tool takes it off what it keeps.
## The landscape as it was read, for the rest of the world (CityWorld: the biome, the water).
signal landscape_read(land: Dictionary)

var drift := Vector2.ZERO
var weather := {}
var clouds: Array = []
var clouds_drawn := true # what the graphics tier pays for: the pass of the clouds, their bodies and their shadows

var _d: Dictionary
var _world: CityWorld
var _mat: ShaderMaterial
var _quad: MeshInstance3D
var _seen := -1
var _clouds := CloudPuffs.new() # the puffs laid for the shader (render/cloud_puffs.gd)
var _weather_view: WeatherView
var _air := 0.0
var _fog_field: Dictionary = {}
var _canyon_field: Dictionary = {}
var _fog_seen := -1 # the fog_revision / canyon_revision last applied
var _lamp_night := -1.0 # what the core was last told
var _canyon_seen := -1
## What the clouds were last laid from: nothing is re-sent while the view, the drift, the count and the revision stay.
var _laid_about := Vector2.INF
var _laid_drift := Vector2.INF
var _laid_count := -1
var _laid_revision := -1


func _init(world: CityWorld) -> void:
	_world = world
	add_to_group(&"sky")


func _ready() -> void:
	_d = DataTables.game("sky")
	_mat = ShaderMaterial.new()
	_mat.shader = load("res://render/shaders/atmosphere.gdshader")
	_mat.render_priority = RenderingServer.MATERIAL_RENDER_PRIORITY_MAX
	_mat.set_shader_parameter("noise_volume", _noise())
	_mat.set_shader_parameter("start_noise", load(_d.start_noise))
	_mat.set_shader_parameter("air_gain", DataTables.game("daylight").sky_gain)
	_mat.set_shader_parameter("fog_extinction", _d.fog_extinction_km / METRES_A_KM)
	for key in ["mist_beta", "mist_reach", "shadow_strength"]:
		_mat.set_shader_parameter(key, _d[key])
	var quad := QuadMesh.new()
	quad.size = Vector2(2, 2)
	quad.flip_faces = true
	_quad = MeshInstance3D.new()
	_quad.mesh = quad
	_quad.material_override = _mat
	_quad.extra_cull_margin = _d.quad_cull_m
	_quad.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	_quad.visible = false
	add_child(_quad)
	_weather_view = WeatherView.new(_d)
	add_child(_weather_view)


func _noise() -> NoiseTexture3D:
	var n := FastNoiseLite.new()
	n.noise_type = FastNoiseLite.TYPE_VALUE_CUBIC
	n.frequency = _d.noise_frequency
	n.fractal_octaves = int(_d.noise_octaves)
	var t := NoiseTexture3D.new()
	var side := int(_d.noise_side)
	t.width = side
	t.height = side
	t.depth = side
	t.seamless = true
	t.normalize = true
	t.noise = n
	return t


## The bolt the weather tool calls down.
func strike_at(p: Vector3) -> void:
	_weather_view.strike(p)


## One frame of the sky, driven by CityWorld after its own (the sun and the clock first); false while there is no city to read.
func step(dt: float) -> bool:
	if not CityStore.ready_to_read():
		return false
	var rev: int = CityStore.query(&"sky_revision")
	if rev != _seen:
		_seen = rev
		_read()
	var away := deg_to_rad(float(weather.get("wind_direction", 0.0)))
	var way := Vector2(cos(away), -sin(away))
	drift += way * float(weather.get("wind", 0.0)) * dt
	_lay()
	_light(way)
	_weather_view.step(dt, weather, way)
	_world.daylight.set_weather(_overcast(), _weather_view.flash)
	if _world.daylight.night != _lamp_night:
		_lamp_night = _world.daylight.night
		CityStore.dispatch(&"set_lamp_night", [_lamp_night])
	var fog_rev: int = CityStore.query(&"fog_revision") # the render takes new windows in; read and bound only then
	if fog_rev != _fog_seen:
		_fog_seen = fog_rev
		_fog_field = CityStore.query(&"fog_field")
		_apply_fog()
	var canyon_rev: int = CityStore.query(&"canyon_revision")
	if canyon_rev != _canyon_seen:
		_canyon_seen = canyon_rev
		_canyon_field = CityStore.query(&"canyon_field")
		_apply_canyon()
	return true


func _read() -> void:
	var land: Dictionary = CityStore.query(&"landscape")
	weather = land.weather
	clouds = land.clouds
	_clouds.shape(clouds, _d)
	landscape_read.emit(land)
	# Rain thickens the air: what the player set, or the rain's own mist if that is more.
	var rain: float = weather.get("rain", 0.0)
	_air = maxf(weather.mist, rain * _d.rain_mist)
	_mat.set_shader_parameter("mist", _air)
	_mat.set_shader_parameter("mist_height", maxf(weather.mist_height_m, _d.rain_mist_height_m) if rain > 0.0 else weather.mist_height_m)
	_mat.set_shader_parameter("fog_density", weather.fog_density)


func _overcast() -> float:
	return maxf(float(weather.get("rain", 0.0)) * _d.overcast_rain, minf(1.0, float(weather.get("lightning", 0.0)) / _d.storm_full) * _d.overcast_storm)


func _light(way: Vector2) -> void:
	RenderingServer.global_shader_parameter_set(&"wind_x", way.x)
	RenderingServer.global_shader_parameter_set(&"wind_z", way.y)
	RenderingServer.global_shader_parameter_set(&"wind_ms", float(weather.get("wind", 0.0)))
	RenderingServer.global_shader_parameter_set(&"rain_wet", float(weather.get("rain", 0.0)))
	_mat.set_shader_parameter("wind_dir", way)
	_mat.set_shader_parameter("sun_dir", _world.sun.global_basis.z.normalized())
	var c := _world.sun.light_color
	_mat.set_shader_parameter("sun_light", Vector3(c.r, c.g, c.b) * _world.sun.light_energy * _d.sun_gain)
	# The air takes the sky's colour at the horizon, overcast with it; the pass works in linear light.
	var horizon := _world.daylight.horizon_linear
	_mat.set_shader_parameter("sky_light", Vector3(horizon.r, horizon.g, horizon.b))
	_mat.set_shader_parameter("mist_color", Vector3(horizon.r, horizon.g, horizon.b))
	_mat.set_shader_parameter("dark", _world.daylight.night)


## Each cloud where the wind has carried it, its repeat round the view, thinning towards the edge it wraps round. The arrays are
## sent only when what places them changed.
func _lay() -> void:
	var cam := get_viewport().get_camera_3d()
	var about := Vector2.ZERO
	if cam:
		var ahead := -cam.global_basis.z
		var t := -cam.global_position.y / ahead.y if ahead.y < -LEVEL_VIEW else 0.0
		about = Vector2(cam.global_position.x + ahead.x * t, cam.global_position.z + ahead.z * t)
	var n := mini(clouds.size(), CloudPuffs.SLOTS) if clouds_drawn else 0
	_quad.visible = n > 0 or _air > 0.0 or bool(_fog_field.get("any", false)) or bool(_canyon_field.get("any", false))
	if about == _laid_about and drift == _laid_drift and n == _laid_count and _seen == _laid_revision:
		return
	_laid_about = about
	_laid_drift = drift
	_laid_count = n
	_laid_revision = _seen
	var span: float = _d.cloud_span_m
	for i in n:
		var c: Dictionary = clouds[i]
		var dx := wrapf(c.x + drift.x - about.x, -span * 0.5, span * 0.5)
		var dz := wrapf(c.z + drift.y - about.y, -span * 0.5, span * 0.5)
		var out: float = (maxf(absf(dx), absf(dz)) - span * _d.show_from) / (span * _d.show_span)
		var show := 1.0 - smoothstep(0.0, 1.0, out)
		_clouds.lay_one(i, c, about.x + dx, about.y + dz, show)
	_mat.set_shader_parameter("cloud_count", n)
	_mat.set_shader_parameter("cloud", _clouds.bounds)
	_mat.set_shader_parameter("puff", _clouds.puffs)
	_mat.set_shader_parameter("cloud_look", _clouds.looks)


## The sky's share of the light is what the streets lose (render/shaders/canyon_light.gdshaderinc).
func _apply_canyon() -> void:
	var on: bool = _canyon_field.get("any", false)
	_mat.set_shader_parameter("canyon_sky", _d.canyon_sky_most - _d.canyon_sun_cut * _world.daylight.direct_share if on else 0.0)
	if on:
		_mat.set_shader_parameter("canyon_map", _canyon_field.texture)
		_mat.set_shader_parameter("canyon_window", _canyon_field.window)


func _apply_fog() -> void:
	var on: bool = _fog_field.get("any", false)
	_mat.set_shader_parameter("fog_on", 1.0 if on else 0.0)
	if on:
		_mat.set_shader_parameter("fog_map", _fog_field.texture)
		_mat.set_shader_parameter("fog_slab", _fog_field.slab)
		_mat.set_shader_parameter("fog_window", _fog_field.window)
