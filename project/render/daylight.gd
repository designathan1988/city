## Time of day: the sun's place, its light after the air (render/air.gd), the sky the air makes of it (render/shaders/sky.gdshader),
## which is also every surface's ambient light and reflections (the Environment reads the sky's radiance map), the air's
## haze over the distance, the moon by night, glow, and the global shader parameters that lamps, windows and the road light pools
## read. All numbers come from data/game/daylight.json.
class_name Daylight
extends RefCounted

const BIAS_PERCENT := 100.0 # the engine reads the bias in hundredths
const HALF := 0.5
const MARGIN_TEXELS := 2.0
const SKY_LIT := 1.0 # the sky's share of the ambient: all of it
const SHADED := 0.0 # what an ambient switched off leaves
const SIDE_BEFORE_TIER_PX := 2048.0 # a cascade's side until the graphics tier gives its own

var _d: Dictionary
var _sun: DirectionalLight3D
var _env: Environment
var _sky: ShaderMaterial
var _probe: SkyLightProbe
var air: Air
var night := 0.0
var shadows_on := true: # what the graphics tier asks of the sun; the moon's shadow goes with the night (_cast)
	set(on):
		shadows_on = on
		_cast()
var shadow_side_px := SIDE_BEFORE_TIER_PX # a cascade's side: game/settings/settings.gd
var shadow_filter_radius := 1.0 # the engine's radius for the filter in use
var solar := Vector3.UP # towards the sun, wherever the light of the night points
var mode := "day"
var shadow_least_m := 0.0 # the quality's own reach (game/settings/settings.gd)
var _first_m := 0.0 # where the first cascade ends
var _clock_shown := -1.0
var _hour := 0.0
var _overcast := 0.0
var _flash := 0.0
## What the cascades were last fitted to: the eye, its tilt, the ground under it, the lens, the screen and the shadow settings. The fit
## is a function of these and of its own last result, which it reproduces when they repeat: a view at rest is not fitted again.
var _fitted_for := []
## The values last given to the engine (it keeps them in single precision, so its own copy does not compare): each is set only
## when it moves.
var _air_density := NAN
var _splits := PackedFloat64Array([NAN, NAN, NAN])
## The Effects panel's switches: bloom and the ambient light drawn or not.
var glow_on := true
var ambient_on := true


func _init(day: Dictionary, sun: DirectionalLight3D, env: Environment, sky: ShaderMaterial, probe: SkyLightProbe) -> void:
	_d = day
	_sun = sun
	_env = env
	_sky = sky
	_probe = probe
	air = Air.new(day.air)
	for m: ShaderMaterial in [_sky, _probe.material]:
		m.set_shader_parameter("night_zenith", _c("zenith_night"))
		m.set_shader_parameter("night_horizon", _c("horizon_night"))
		m.set_shader_parameter("night_gain", _d.sky_gain)
		m.set_shader_parameter("overcast_pass", _d.overcast_pass)
	RenderingServer.global_shader_parameter_set(&"daylight_factor", _d.daylight_factor)


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


## "day" and "night" hold the light at their hours; "cycle" follows the city's clock.
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
	night = smoothstep(_d.night_from_deg, _d.night_to_deg, elev)
	solar = _toward(elev, az)
	if height > -_d.light_from:
		_aim(_sun, maxf(height, _d.low_height) * _d.max_elev_deg, az)
		var lit: Vector3 = air.sun_at(solar, 0.0) * (1.0 - _overcast * _d.overcast_sun)
		var most := maxf(lit.x, maxf(lit.y, lit.z))
		_sun.light_color = Color(lit.x, lit.y, lit.z) / most if most > 0.0 else Color.WHITE
		_sun.light_energy = most
	else:
		_aim(_sun, _d.moon_elev_deg, _d.moon_az_deg)
		_sun.light_color = _c("moon")
		_sun.light_energy = _d.moon_energy * night
	# The sky's radiance map is the ambient, scaled by the background's energy and not by ambient_light_energy (that one scales only
	# the flat colour it mixes out: render_scene_data_rd.cpp, 4.7.2): the switch takes the sky's share away, and the colour is none.
	_env.ambient_light_sky_contribution = SKY_LIT if ambient_on else SHADED
	_env.ambient_light_energy = SHADED
	air.publish(_sky, solar)
	for m: ShaderMaterial in [_sky, _probe.material]: # the sky drawn and the one its level light is taken from: the same weather
		m.set_shader_parameter("dark", night)
		m.set_shader_parameter("overcast", _overcast * _d.overcast_sky)
		m.set_shader_parameter("flash", _flash * _d.flash_sky)
	_probe.refresh()
	_cast()
	_env.glow_enabled = glow_on
	_env.glow_intensity = _d.glow_intensity
	RenderingServer.global_shader_parameter_set(&"night", night)
	RenderingServer.global_shader_parameter_set(&"sun_direction", _sun.global_basis.z)
	var awake := _awake_share(hour)
	RenderingServer.global_shader_parameter_set(&"rooms_awake", awake)
	RenderingServer.global_shader_parameter_set(&"outside_on", _d.outside_all_night + (1.0 - _d.outside_all_night) * awake)


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


## The haze between the eye and what it sees: the air's extinction at the ground (render/air.gd, from the visibility), thinned with
## height as the aerosol is, its colour the sky's own along the view (aerial perspective, the Environment's fog reading the radiance
## map); and the cascades reach past the point looked at, so a far view keeps its shadows: only when that changes by a step.
func follow_view(camera: Camera3D) -> void:
	if camera == null:
		return
	var y: float = maxf(camera.global_position.y, 1.0)
	var scale_h: float = air.mie.w
	var density: float = air.extinction * scale_h / y * (1.0 - exp(-y / scale_h)) # the mean density of the column under the eye
	if density != _air_density:
		_air_density = density
		_env.fog_density = density
	var ground := CityCamera.ground_below(camera)
	var view := [camera.global_position, camera.global_basis.z.y, ground, camera.near, camera.far, camera.fov,
			camera.get_viewport().get_visible_rect().size, shadow_side_px, shadow_filter_radius, _sun.shadow_blur, shadow_least_m]
	if view != _fitted_for:
		_fitted_for = view
		_fit_cascades(camera, ground)


## The filter in use: its radius is the engine's, and the blur follows it so a soft edge keeps its width whatever the filter
## (shadow_blur at shadow_blur_radius). What a filter changes is its samples: 32 at Soft Ultra, 16 at Soft High, per pixel
## (renderer_scene_render_rd.cpp); at the same width the picture holds to 16 and shows its grain at 8.
func set_filter(radius: float) -> void:
	shadow_filter_radius = radius
	_sun.shadow_blur = _d.shadow_blur * _d.shadow_blur_radius / radius


## The sun's cascades fitted to the view (Engel, ShaderX5): the first ends just past the point looked at, the others in ratio to the
## far end. A fixed share leaves a texel wider than a sign post up close.
func _fit_cascades(camera: Camera3D, ground: float) -> void:
	var eye := camera.global_position
	var above: float = maxf(eye.y - ground, 0.0)
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
	_set_split(0, &"directional_shadow_split_1", first / end)
	_set_split(1, &"directional_shadow_split_2", second / end)
	_set_split(2, &"directional_shadow_split_3", sqrt(second * end) / end)
	var bias: float = _d.shadow_bias_m * BIAS_PERCENT / (_first_depth(camera, end) * _sun.shadow_blur * shadow_filter_radius)
	if absf(bias - _sun.shadow_bias) > bias * _d.shadow_reach_step:
		_sun.shadow_bias = bias


## A split of the sun's cascades, given to the light only when it moved.
func _set_split(i: int, property: StringName, share: float) -> void:
	if share != _splits[i]:
		_splits[i] = share
		_sun.set(property, share)


## The first cascade's depth as the engine takes it (renderer_scene_cull.cpp, 4.7.2): the sphere round the slice of the view, a texel
## of margin on each side, and the pancake. The bias of the others grows with theirs, and so stays the same in texels.
func _first_depth(camera: Camera3D, end: float) -> float:
	var near := camera.near
	var far: float = near + _sun.directional_shadow_split_1 * (minf(end, camera.far) - near)
	var screen := camera.get_viewport().get_visible_rect().size
	var across: float = far * tan(deg_to_rad(camera.fov) * HALF) * Vector2(screen.x / screen.y, 1.0).length()
	var radius: float = Vector2(across, (far - near) * HALF).length() * shadow_side_px / (shadow_side_px - MARGIN_TEXELS)
	return 2.0 * radius + _d.shadow_pancake_m


## The direction (elevation, azimuth) in degrees, towards it: noon toward -Z.
static func _toward(elev_deg: float, az_deg: float) -> Vector3:
	var e := deg_to_rad(elev_deg)
	var a := deg_to_rad(az_deg)
	return Vector3(cos(e) * sin(a), sin(e), cos(e) * cos(a))


## Points a light from the direction (elevation, azimuth) in degrees.
func _aim(lamp: DirectionalLight3D, elev_deg: float, az_deg: float) -> void:
	var toward := _toward(elev_deg, az_deg)
	lamp.basis = Basis.looking_at(-toward, Vector3.UP if absf(toward.y) < _d.vertical_dot else Vector3.FORWARD)
