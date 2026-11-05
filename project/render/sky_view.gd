## The sky as drawn: follows the core's weather and placed clouds (CityStore query "landscape"), lays the clouds' puffs for
## atmosphere.gdshader, carries them on the wind, holds the wind in the shader globals (plants and water read it) and
## tells the daylight how overcast it is. Rain, bolts and thunder are render/weather_view.gd's.
class_name SkyView
extends Node3D

const METRES_A_KM := 1000.0
const SLOTS := 24
const LOBES := 7
## A view this close to level looks at no ground under it.
const LEVEL_VIEW := 0.0015
## How far a cloud's bound reaches, and its centre, in its own size.
const BOUND_REACH := 1.12
## The hash of a puff's jitter: the same puffs every visit (a cloud's id seeds it).
const ID_BASE := 1000003
const ID_STEP := 7919
const PUFF_SEED := 11
const SALT_X := 6
const SALT_Z := 7
const SALT_R := 8

## Drift of the clouds so far, on the map (x, z), metres; the cloud tool takes it off what it keeps.
var drift := Vector2.ZERO
var weather := {}
var clouds: Array = []

var _d: Dictionary
var _world: CityWorld
var _mat: ShaderMaterial
var _quad: MeshInstance3D
var _seen := -1
var _bounds := PackedVector4Array()
var _puffs := PackedVector4Array()
var _looks := PackedVector4Array()
var _weather_view: WeatherView
var _fog_field: Dictionary = {}


func _init(world: CityWorld) -> void:
	_world = world
	add_to_group(&"sky")


func _ready() -> void:
	_d = DataTables.game("sky")
	_bounds.resize(SLOTS)
	_puffs.resize(SLOTS * LOBES)
	_looks.resize(SLOTS)
	_mat = ShaderMaterial.new()
	_mat.shader = load("res://render/shaders/atmosphere.gdshader")
	_mat.render_priority = RenderingServer.MATERIAL_RENDER_PRIORITY_MAX
	_mat.set_shader_parameter("noise_volume", _noise())
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


func _process(dt: float) -> void:
	if not CityStore.ready_to_read():
		return
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
	_fog_field = CityStore.query(&"fog_field", [get_viewport().get_camera_3d().global_position if get_viewport().get_camera_3d() else Vector3.ZERO])
	_apply_fog()


func _read() -> void:
	var land: Dictionary = CityStore.query(&"landscape")
	weather = land.weather
	clouds = land.clouds
	_world.set_map_biome(land.biome)
	_world.set_water_look(weather)
	_mat.set_shader_parameter("mist", weather.mist)
	_mat.set_shader_parameter("mist_height", weather.mist_height_m)
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
	var fog := _world.environment.fog_light_color
	_mat.set_shader_parameter("sky_light", Vector3(fog.r, fog.g, fog.b))
	_mat.set_shader_parameter("mist_color", Vector3(fog.r, fog.g, fog.b))
	_mat.set_shader_parameter("dark", _world.daylight.night)


## 0..1 hash of an integer and a salt.
func _hash(i: int, salt: int) -> float:
	var h := ((i + 1) * 374761393) ^ (salt * 668265263)
	h = ((h ^ (h >> 13)) * 1274126177) & 0xFFFFFFFF
	return float((h ^ (h >> 16)) & 0xFFFFFFFF) / 4294967296.0


## Each cloud where the wind has carried it, its repeat round the view, thinning towards the edge it wraps round.
func _lay() -> void:
	var cam := get_viewport().get_camera_3d()
	var about := Vector2.ZERO
	if cam:
		var ahead := -cam.global_basis.z
		var t := -cam.global_position.y / ahead.y if ahead.y < -LEVEL_VIEW else 0.0
		about = Vector2(cam.global_position.x + ahead.x * t, cam.global_position.z + ahead.z * t)
	var span: float = _d.cloud_span_m
	var n := mini(clouds.size(), SLOTS)
	for i in n:
		var c: Dictionary = clouds[i]
		var dx := wrapf(c.x + drift.x - about.x, -span * 0.5, span * 0.5)
		var dz := wrapf(c.z + drift.y - about.y, -span * 0.5, span * 0.5)
		var out: float = (maxf(absf(dx), absf(dz)) - span * _d.show_from) / (span * _d.show_span)
		var show := 1.0 - smoothstep(0.0, 1.0, out)
		_lay_one(i, c, about.x + dx, about.y + dz, show)
	_mat.set_shader_parameter("cloud_count", n)
	_mat.set_shader_parameter("cloud", _bounds)
	_mat.set_shader_parameter("puff", _puffs)
	_mat.set_shader_parameter("cloud_look", _looks)
	_quad.visible = n > 0 or float(weather.get("mist", 0.0)) > 0.0 or bool(_fog_field.get("any", false))


func _lay_one(i: int, c: Dictionary, x: float, z: float, show: float) -> void:
	var yaw: float = c.yaw
	var size: float = c.size
	var cs := cos(yaw)
	var sn := sin(yaw)
	var id := ID_BASE + int(c.id) * ID_STEP
	_bounds[i] = Vector4(x, c.height + size * 0.5, z, size * BOUND_REACH)
	_looks[i] = Vector4(c.height, c.density, show, 0.0)
	var shape: Array = _d.puffs
	for k in LOBES:
		var p: Array = shape[k]
		var ja: float = p[0] + (_hash(id * PUFF_SEED + k, SALT_X) - 0.5) * _d.puff_jitter
		var jb: float = p[2] + (_hash(id * PUFF_SEED + k, SALT_Z) - 0.5) * _d.puff_jitter
		var jr: float = p[3] * (_d.puff_least + _d.puff_vary * _hash(id * PUFF_SEED + k, SALT_R))
		_puffs[i * LOBES + k] = Vector4(x + (ja * cs - jb * sn) * size, c.height + p[1] * size, z + (ja * sn + jb * cs) * size, jr * size)


func _apply_fog() -> void:
	var on: bool = _fog_field.get("any", false)
	_mat.set_shader_parameter("fog_on", 1.0 if on else 0.0)
	if on:
		_mat.set_shader_parameter("fog_map", _fog_field.texture)
		_mat.set_shader_parameter("fog_slab", _fog_field.slab)
		_mat.set_shader_parameter("fog_window", _fog_field.window)
