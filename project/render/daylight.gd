## Time of day (Roadcraft render/scene.ts setTimeOfDay): sun or moon direction, colour and energy, the ambient
## light, the gradient sky, fog, night glow and the global shader parameter `night` that lamps, windows and the road
## light pools read. All numbers come from data/game/daylight.json.
class_name Daylight
extends RefCounted

var _d: Dictionary
var _sun: DirectionalLight3D
var _env: Environment
var _sky: ShaderMaterial
var night := 0.0


func _init(day: Dictionary, sun: DirectionalLight3D, env: Environment, sky: ShaderMaterial) -> void:
	_d = day
	_sun = sun
	_env = env
	_sky = sky


func _c(key: String) -> Color:
	return Color.html(_d[key])


func set_hour(hour: float) -> void:
	var k: float = (hour - _d.sunrise_h) / _d.day_span_h
	var elev: float = _d.max_elev_deg * sin(PI * k)
	var az: float = _d.az_start_deg + _d.az_span_deg * k
	night = smoothstep(_d.night_from_deg, _d.night_to_deg, elev)
	var day := smoothstep(_d.day_from_deg, _d.day_to_deg, elev)
	if elev > _d.day_from_deg:
		_aim(maxf(elev, _d.sun_min_elev_deg), az)
		_sun.light_color = _c("sun_low").lerp(_c("sun_day"), smoothstep(_d.sun_warm_from_deg, _d.sun_warm_to_deg, elev))
		_sun.light_energy = _d.sun_energy * day
	else:
		_aim(_d.moon_elev_deg, az + _d.moon_offset_deg)
		_sun.light_color = _c("moon")
		_sun.light_energy = _d.moon_energy * night
	var hemi: float = _d.hemi_base + _d.hemi_day_k * day + _d.hemi_night_k * night
	_env.ambient_light_color = _c("hemi_night").lerp(_c("hemi_day"), day)
	_env.ambient_light_energy = hemi * _d.ambient_scale
	_set_sky(elev)
	_env.glow_enabled = night > _d.glow_from_night
	_env.glow_intensity = _d.glow_intensity * night
	RenderingServer.global_shader_parameter_set(&"night", night)


## Points the light from the direction (elevation, azimuth) in degrees: sunrise at +X, noon toward -Z.
func _aim(elev_deg: float, az_deg: float) -> void:
	var e := deg_to_rad(elev_deg)
	var a := deg_to_rad(az_deg)
	var toward := Vector3(cos(e) * sin(a), sin(e), cos(e) * cos(a))
	_sun.basis = Basis.looking_at(-toward, Vector3.UP if absf(toward.y) < _d.vertical_dot else Vector3.FORWARD)


func _set_sky(elev: float) -> void:
	var height := sin(deg_to_rad(elev))
	var light := clampf((height + _d.light_from) / _d.light_span, 0.0, 1.0)
	var warm := maxf(0.0, 1.0 - _d.warm_k * height)
	var zenith := _c("zenith_day").lerp(_c("zenith_night"), 1.0 - light)
	var horizon := _c("horizon_day").lerp(_c("horizon_dusk"), warm * light * _d.dusk_share)
	horizon = horizon.lerp(_c("horizon_night"), 1.0 - light)
	var sun_col := _c("sun_day").lerp(_c("sun_low"), minf(1.0, warm)) * light
	_sky.set_shader_parameter("zenith", zenith)
	_sky.set_shader_parameter("horizon", horizon)
	_sky.set_shader_parameter("ground", _c("sky_ground").lerp(_c("horizon_night"), 1.0 - light))
	_sky.set_shader_parameter("low_zenith", _c("low_zenith"))
	_sky.set_shader_parameter("low_horizon", _c("low_horizon"))
	_sky.set_shader_parameter("low", _d.low_sky * light * (1.0 - minf(1.0, warm)))
	_sky.set_shader_parameter("sun_color", sun_col)
	_sky.set_shader_parameter("gain", _d.sky_gain)
	var fog := horizon.lerp(_c("low_horizon"), _d.low_sky * light * (1.0 - minf(1.0, warm)) * _d.fog_low_share)
	_env.fog_light_color = fog
	_env.fog_density = _d.fog_density
