## Time of day (Roadcraft render/scene.ts setTimeOfDay): sun or moon direction, colour and energy, the ambient
## light, the gradient sky, fog, night glow and the global shader parameter `night` that lamps, windows and the road
## light pools read. All numbers come from data/game/daylight.json.
class_name Daylight
extends RefCounted

var _d: Dictionary
var _sun: DirectionalLight3D
var _fill: DirectionalLight3D
var _env: Environment
var _sky: ShaderMaterial
var night := 0.0
var mode := "day"
var _clock_shown := -1.0
var _hour := 0.0
var _overcast := 0.0
var _flash := 0.0


func _init(day: Dictionary, sun: DirectionalLight3D, fill: DirectionalLight3D, env: Environment, sky: ShaderMaterial) -> void:
	_d = day
	_sun = sun
	_fill = fill
	_env = env
	_sky = sky


## The sun with its shadows: four cascades, blended where they meet.
static func make_sun(day: Dictionary) -> DirectionalLight3D:
	var sun := DirectionalLight3D.new()
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
	return sun


func _c(key: String) -> Color:
	return Color.html(_d[key])


## How overcast it is (0..1, rain and storm) and how bright a bolt's flash is now: the light follows at once.
func set_weather(overcast: float, flash: float) -> void:
	if is_equal_approx(overcast, _overcast) and is_equal_approx(flash, _flash):
		return
	_overcast = overcast
	_flash = flash
	set_hour(_hour)


## "day" and "night" hold the light at their hours, as the 4.0's top bar does; "cycle" follows the city's clock.
func set_mode(sky: String) -> void:
	mode = sky
	_clock_shown = -1.0
	if sky == "cycle":
		follow_clock(CityStore.snapshot.get("hour", _d.day_hour))
	else:
		set_hour(_d.day_hour if sky == "day" else _d.night_hour)


## The city's clock moved: the light follows in "cycle", a step at a time, so nothing is redone in a frame it did not move.
func follow_clock(hour: float) -> void:
	if mode == "cycle" and absf(hour - _clock_shown) >= _d.clock_step_h:
		_clock_shown = hour
		set_hour(hour)


func set_hour(hour: float) -> void:
	_hour = hour
	var k: float = (hour - _d.sunrise_h) / _d.day_span_h
	var height := sin(PI * k)
	var elev: float = _d.max_elev_deg * height
	var az: float = _d.az_start_deg + _d.az_span_deg * clampf(k, 0.0, 1.0)
	var light := clampf((height + _d.light_from) / _d.light_span, 0.0, 1.0)
	var clear: float = 1.0 - _overcast * _d.overcast_sun
	night = smoothstep(_d.night_from_deg, _d.night_to_deg, elev)
	if height > -_d.light_from:
		var aim: float = maxf(height, _d.low_height) * _d.max_elev_deg
		_aim(_sun, aim, az)
		_sun.light_color = _c("sun_day").lerp(_c("sun_low"), minf(1.0, maxf(0.0, 1.0 - _d.warm_k * height)))
		_sun.light_energy = _d.sun_energy * lerpf(_d.sun_low_share, 1.0, minf(1.0, height * _d.sun_rise_k + _d.sun_rise_from)) * light * clear
		_set_fill(aim, az, light * clear)
	else:
		_aim(_sun, _d.moon_elev_deg, az + _d.moon_offset_deg)
		_sun.light_color = _c("moon")
		_sun.light_energy = _d.moon_energy * night
		_fill.visible = false
	var hemi: float = _d.hemi_day_k * light + _d.hemi_night_k * night
	_env.ambient_light_color = _c("hemi_night").lerp(_c("hemi_day"), light)
	_env.ambient_light_energy = hemi * _d.ambient_scale * (1.0 + _flash * _d.flash_ambient) * (1.0 - _overcast * _d.overcast_ambient)
	_set_sky(elev)
	_env.glow_enabled = night > _d.glow_from_night
	_env.glow_intensity = _d.glow_intensity * night
	RenderingServer.global_shader_parameter_set(&"night", night)


## The fill: a light round from the sun and lower, that keeps the side away from it readable, never more than a share of
## the sun on level ground (three-point lighting).
func _set_fill(sun_elev: float, sun_az: float, light: float) -> void:
	var low := sin(deg_to_rad(_d.fill_elev_deg))
	var key: float = _sun.light_energy * sin(deg_to_rad(sun_elev))
	_fill.light_energy = minf(_d.fill_energy * light, _d.fill_key_share * key / low)
	_fill.light_color = _c("fill")
	_fill.visible = _fill.light_energy > 0.0
	_aim(_fill, _d.fill_elev_deg, sun_az - _d.fill_turn_deg)


## Points a light from the direction (elevation, azimuth) in degrees: noon toward -Z.
func _aim(lamp: DirectionalLight3D, elev_deg: float, az_deg: float) -> void:
	var e := deg_to_rad(elev_deg)
	var a := deg_to_rad(az_deg)
	var toward := Vector3(cos(e) * sin(a), sin(e), cos(e) * cos(a))
	lamp.basis = Basis.looking_at(-toward, Vector3.UP if absf(toward.y) < _d.vertical_dot else Vector3.FORWARD)


func _set_sky(elev: float) -> void:
	var height := sin(deg_to_rad(elev))
	var light := clampf((height + _d.light_from) / _d.light_span, 0.0, 1.0)
	var warm := maxf(0.0, 1.0 - _d.warm_k * height)
	var zenith := _c("zenith_day").lerp(_c("zenith_night"), 1.0 - light)
	var horizon := _c("horizon_day").lerp(_c("horizon_dusk"), warm * light * _d.dusk_share)
	horizon = horizon.lerp(_c("horizon_night"), 1.0 - light)
	var sun_col := _c("sun_day").lerp(_c("sun_low"), minf(1.0, warm)) * light
	var grey: Color = _c("overcast_grey") * light
	zenith = zenith.lerp(grey, _overcast * _d.overcast_sky)
	horizon = horizon.lerp(grey, _overcast * _d.overcast_sky)
	_sky.set_shader_parameter("zenith", zenith)
	_sky.set_shader_parameter("horizon", horizon)
	_sky.set_shader_parameter("ground", _c("sky_ground").lerp(_c("horizon_night"), 1.0 - light))
	_sky.set_shader_parameter("low_zenith", _c("low_zenith"))
	_sky.set_shader_parameter("low_horizon", _c("low_horizon"))
	_sky.set_shader_parameter("low", _d.low_sky * light)
	_sky.set_shader_parameter("sun_color", sun_col)
	_sky.set_shader_parameter("gain", _d.sky_gain * (1.0 + _flash * _d.flash_sky))
	var fog := horizon.lerp(_c("low_horizon"), _d.low_sky * light * _d.fog_low_share)
	_env.fog_light_color = fog
	_env.fog_density = _d.fog_density
