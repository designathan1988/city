## Time of day (Roadcraft render/scene.ts setTimeOfDay): sun or moon direction, colour and energy, the ambient
## light, the gradient sky, fog, night glow and the global shader parameter `night` that lamps, windows and the road
## light pools read. All numbers come from data/game/daylight.json.
class_name Daylight
extends RefCounted

const BIAS_PERCENT := 100.0 # the engine reads the bias in hundredths
const HALF := 0.5
const MARGIN_TEXELS := 2.0
const LEAST_LIGHT := 0.001 # a share of no light at all stays a number

var _d: Dictionary
var _sun: DirectionalLight3D
var _fill: DirectionalLight3D
var _top: DirectionalLight3D
var _under: DirectionalLight3D
var _env: Environment
var _sky: ShaderMaterial
var night := 0.0
var shadows_on := true: # what the graphics tier asks of the sun; the moon's shadow goes with the night (_cast)
	set(on):
		shadows_on = on
		_cast()
var shadow_side_px := 2048.0 # a cascade's side: game/settings/settings.gd
var shadow_filter_radius := 1.0 # the engine's radius for the filter in use
var direct_share := 0.0 # the sun's or moon's share of the light on level ground
var mode := "day"
var shadow_least_m := 0.0 # the quality's own reach (game/settings/settings.gd)
var _first_m := 0.0 # where the first cascade ends
var _clock_shown := -1.0
var _hour := 0.0
var _overcast := 0.0
var _dark := 0.0
var _flash := 0.0
var revision := 0 # prepared light/sky appearance, including weather and flashes
var _fog_key: Array = []
var _cascade_key: Array = []
## The Effects panel's switches: bloom and the ambient light drawn or not.
var glow_on := true
var ambient_on := true


func _init(day: Dictionary, sun: DirectionalLight3D, fill: DirectionalLight3D, top: DirectionalLight3D, under: DirectionalLight3D, env: Environment, sky: ShaderMaterial) -> void:
	_d = day
	_sun = sun
	_fill = fill
	_top = top
	_under = under
	_env = env
	_sky = sky


func _c(key: String) -> Color:
	return Color.html(_d[key])


## The same hour again, after a switch changed what the light draws.
func relight() -> void:
	set_hour(_hour)


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
	revision += 1
	_hour = hour
	var k: float = (hour - _d.sunrise_h) / _d.day_span_h
	var height := sin(PI * k)
	var elev: float = _d.max_elev_deg * height
	var az: float = _d.az_start_deg + _d.az_span_deg * clampf(k, 0.0, 1.0)
	var light := clampf((height + _d.light_from) / _d.light_span, 0.0, 1.0)
	var clear: float = 1.0 - _overcast * _d.overcast_sun
	_dark = 1.0 - light
	night = smoothstep(_d.night_from_deg, _d.night_to_deg, elev)
	if height > -_d.light_from:
		var aim: float = maxf(height, _d.low_height) * _d.max_elev_deg
		_aim(_sun, aim, az)
		_sun.light_color = _c("sun_day").lerp(_c("sun_low"), minf(1.0, maxf(0.0, 1.0 - _d.warm_k * height)))
		_sun.light_energy = _d.sun_energy * lerpf(_d.sun_low_share, 1.0, minf(1.0, height * _d.sun_rise_k + _d.sun_rise_from)) * light * clear
		_set_fill(aim, az, light * (1.0 - _overcast * _d.overcast_fill))
	else:
		_aim(_sun, _d.moon_elev_deg, _d.moon_az_deg)
		_sun.light_color = _c("moon")
		_sun.light_energy = _d.moon_energy * night
		_fill.visible = false
	var hemi: float = _d.hemi_day_k * light + _d.hemi_night_k * night
	_env.ambient_light_color = _c("hemi_night").lerp(_c("hemi_day"), light)
	var sky: float = _d.ambient_scale * (1.0 + _flash * _d.flash_ambient) * (1.0 - _overcast * _d.overcast_ambient)
	_env.ambient_light_energy = hemi * sky if ambient_on else 0.0
	_top.light_color = _c("sky_top")
	_top.light_energy = _d.sky_top_energy * light * sky if ambient_on else 0.0
	_top.visible = _top.light_energy > 0.0
	_under.light_color = _c("ground_under")
	_under.light_energy = _d.ground_under_k * _env.ambient_light_energy
	_under.visible = _under.light_energy > 0.0
	var direct: float = _sun.light_energy * maxf(_sun.basis.z.y, 0.0)
	var fill: float = _fill.light_energy * maxf(_fill.basis.z.y, 0.0) if _fill.visible else 0.0
	direct_share = direct / maxf(direct + fill + _env.ambient_light_energy + _top.light_energy, LEAST_LIGHT)
	_set_sky(elev)
	_cast()
	_env.glow_enabled = glow_on and night > _d.glow_from_night
	_env.glow_intensity = _d.glow_intensity * night
	RenderingServer.global_shader_parameter_set(&"night", night)
	RenderingServer.global_shader_parameter_set(&"sun_direction", _sun.global_basis.z)
	var awake := _awake_share(hour)
	RenderingServer.global_shader_parameter_set(&"rooms_awake", awake)
	RenderingServer.global_shader_parameter_set(&"outside_on", _d.outside_all_night + (1.0 - _d.outside_all_night) * awake)
	RenderingServer.global_shader_parameter_set(&"room_daylight", (1.0 - night) * _d.room_daylight)


## Under the street lamps no one can see the moon's shadow, and it is the whole city drawn again for its maps: it fades with the dusk and the pass goes.
func _cast() -> void:
	if _sun == null:
		return
	_sun.shadow_opacity = 1.0 - minf(night / _d.moon_casts_until, 1.0)
	_sun.shadow_enabled = shadows_on and night < _d.moon_casts_until


## The share of people awake at a clock time, between the survey's hourly figures: the rooms lit at night.
func _awake_share(hour: float) -> float:
	var table: Array = _d.awake_by_hour
	var h := fposmod(hour, float(table.size()))
	var i := int(h)
	return lerpf(table[i], table[(i + 1) % table.size()], h - float(i))


## The cascades reach past the point looked at, so a far view keeps its shadows: only when that changes by a step.
func follow_view(camera: Camera3D) -> void:
	if camera == null:
		return
	var fog_key := [camera.global_position.y, _dark]
	if fog_key != _fog_key:
		_fog_key = fog_key
		var low := 1.0 - smoothstep(_d.low_from_m, _d.low_to_m, camera.global_position.y)
		_env.fog_light_color = _c("air_high").lerp(_c("low_horizon"), low) * (1.0 - _d.air_night_cut * _dark)
		var y: float = maxf(camera.global_position.y, 1.0)
		_env.fog_density = _d.fog_density * _d.air_height_m / y * (1.0 - exp(-y / _d.air_height_m))
	if not _sun.shadow_enabled:
		_cascade_key.clear() # enabling again must fit even if the view did not move
		return
	var inputs: Array = CityStore.query(&"camera_inputs")
	var cascade_key := [CityCamera.projection_key(camera, camera.get_viewport().get_visible_rect()), inputs[0], inputs[1],
		shadow_least_m, shadow_side_px, shadow_filter_radius, _sun.shadow_blur]
	if cascade_key != _cascade_key:
		_cascade_key = cascade_key
		_fit_cascades(camera)


## The filter in use: its radius is the engine's, and the blur follows it so a soft edge keeps its width whatever the filter
## (shadow_blur at shadow_blur_radius). What a filter changes is its samples: 32 at Soft Ultra, 16 at Soft High, per pixel
## (renderer_scene_render_rd.cpp); at the same width the picture holds to 16 and shows its grain at 8.
func set_filter(radius: float) -> void:
	shadow_filter_radius = radius
	_sun.shadow_blur = _d.shadow_blur * _d.shadow_blur_radius / radius


## The sun's cascades fitted to the view (Engel, ShaderX5, as the 4.0's sunCascades.ts): the first ends just past the
## point looked at, the others in ratio to the far end. A fixed share leaves a texel wider than a sign post up close.
func _fit_cascades(camera: Camera3D) -> void:
	var eye := camera.global_position
	var above: float = maxf(eye.y - CityStore.query(&"height_at", [eye.x, eye.z]), 0.0)
	var look: float = above / maxf(_d.shadow_view_least_dip, camera.global_basis.z.y)
	var depth: float = clampf(maxf(look + _d.shadow_reach_m, look * _d.shadow_past_look), shadow_least_m, _d.shadow_depth_max_m)
	if absf(depth - _sun.directional_shadow_max_distance) > depth * _d.shadow_reach_step:
		_sun.directional_shadow_max_distance = depth
	var end: float = _sun.directional_shadow_max_distance
	var fitted: float = minf(maxf(look * _d.shadow_first_past_look, _d.shadow_first_least_m), end * _d.shadow_first_share)
	# by steps, as the far end: a slice that changes size with every metre of the ground under the eye changes the width of the
	# cascade's texels with it, and every edge of every shadow crawls while the camera moves
	if absf(fitted - _first_m) > fitted * _d.shadow_reach_step or _first_m > end * _d.shadow_first_share:
		_first_m = fitted
	var first: float = _first_m
	var second: float = sqrt(first * end)
	_sun.directional_shadow_split_1 = first / end
	_sun.directional_shadow_split_2 = second / end
	_sun.directional_shadow_split_3 = sqrt(second * end) / end
	var bias: float = _d.shadow_bias_m * BIAS_PERCENT / (_first_depth(camera, end) * _sun.shadow_blur * shadow_filter_radius)
	if absf(bias - _sun.shadow_bias) > bias * _d.shadow_reach_step:
		_sun.shadow_bias = bias


## The first cascade's depth as the engine takes it (renderer_scene_cull.cpp, 4.7.2): the sphere round the slice of the view, a texel
## of margin on each side, and the pancake. The bias of the others grows with theirs, and so stays the same in texels.
func _first_depth(camera: Camera3D, end: float) -> float:
	var near := camera.near
	var far: float = near + _sun.directional_shadow_split_1 * (minf(end, camera.far) - near)
	var screen := camera.get_viewport().get_visible_rect().size
	var across: float = far * tan(deg_to_rad(camera.fov) * HALF) * Vector2(screen.x / screen.y, 1.0).length()
	var radius: float = Vector2(across, (far - near) * HALF).length() * shadow_side_px / (shadow_side_px - MARGIN_TEXELS)
	return 2.0 * radius + _d.shadow_pancake_m


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
	_sky.set_shader_parameter("low", _d.low_sky)
	_sky.set_shader_parameter("dark", 1.0 - light)
	_sky.set_shader_parameter("low_from_m", _d.low_from_m)
	_sky.set_shader_parameter("low_to_m", _d.low_to_m)
	_sky.set_shader_parameter("sun_color", sun_col)
	_sky.set_shader_parameter("gain", _d.sky_gain * (1.0 + _flash * _d.flash_sky))
