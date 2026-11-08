## Time of day: the sun's place, its light after the air (render/air.gd), the sky the air makes of it (render/shaders/sky.gdshader),
## which is also every surface's ambient light and reflections (the Environment reads the sky's radiance map), the air's
## haze over the distance, the moon by night and the town's own light in its sky, the eye's adaptation to all of it (render/eye.gd,
## metered here), glow, and the global shader parameters that lamps, windows and the road light pools read. All numbers come from
## data/game/daylight.json.
class_name Daylight
extends RefCounted

const SKY_LIT := 1.0 # the sky's share of the ambient: all of it
const SHADED := 0.0 # what an ambient switched off leaves

var _d: Dictionary
var _sun: DirectionalLight3D
var _env: Environment
var _sky: ShaderMaterial
var _probe: SkyLightProbe
var air: Air
var meter: LightMeter # the night's lights, the eye's meter and its pre-exposure
var lux: float:
	get:
		return meter.lux
var pre_exposure: float:
	get:
		return meter.pre_exposure
var _moon_dir := Vector3.UP # towards the moon
var night := 0.0
var shadows_on := true: # what the graphics tier asks of the sun; the moon's shadow goes with the night (_cast)
	set(on):
		shadows_on = on
		_cast()
var cascades: SunCascades # the sun's shadow cascades fitted to the view
var solar := Vector3.UP # towards the sun, wherever the light of the night points
var mode := "day"
var _clock_shown := -1.0
var _hour := 0.0
var _overcast := 0.0
var _flash := 0.0
## The values last given to the engine (it keeps them in single precision, so its own copy does not compare): each is set only
## when it moves.
var _air_density := NAN
## The Effects panel's switches: bloom and the ambient light drawn or not.
var glow_on := true
var ambient_on := true


func _init(day: Dictionary, sun: DirectionalLight3D, env: Environment, sky: ShaderMaterial, probe: SkyLightProbe,
		table: SkyLevelTable, camera: CameraAttributesPractical) -> void:
	_d = day
	_sun = sun
	_env = env
	_sky = sky
	_probe = probe
	air = Air.new(day.air)
	cascades = SunCascades.new(day, sun)
	meter = LightMeter.new(day, air, table, camera, env, sky)
	_moon_dir = _toward(day.moon_elev_deg, day.moon_az_deg)
	_sky.set_shader_parameter("tone_exposure", env.tonemap_exposure)
	for m: ShaderMaterial in [_sky, _probe.material]:
		m.set_shader_parameter("moon_dir", _moon_dir)
		m.set_shader_parameter("cloud_reflect", day.sky_glow.cloud_reflect)
		m.set_shader_parameter("overcast_pass", _d.overcast_pass)
	RenderingServer.global_shader_parameter_set(&"daylight_factor", _d.daylight_factor)
	table.filled.connect(_on_table)


## The sky's row is read (render/sky_level_table.gd): the meter takes the town's light and the day hour's clear light as the eye's
## reference, and the light is metered again.
func _on_table() -> void:
	meter.fill(func() -> float:
		var day := _place(_d.day_hour)
		var toward: Vector3 = day[0]
		return meter.adapted(toward, air.sun_at(toward, 0.0), _moon_dir, Vector3.ZERO, 0.0, 0.0, day[3]))
	relight()


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


## The sun at a clock time: towards it, its height (the sine of the arc's angle), its azimuth in degrees and how dark it is.
func _place(hour: float) -> Array:
	var k: float = (hour - _d.sunrise_h) / _d.day_span_h
	var height := sin(PI * k)
	var elev: float = _d.max_elev_deg * height
	var az: float = _d.az_start_deg + _d.az_span_deg * clampf(k, 0.0, 1.0)
	return [_toward(elev, az), height, az, smoothstep(_d.night_from_deg, _d.night_to_deg, elev)]


func set_hour(hour: float) -> void:
	_hour = hour
	var place := _place(hour)
	solar = place[0]
	var height: float = place[1]
	night = place[3]
	var dim: float = 1.0 - _overcast * _d.overcast_sun # what the weather leaves of a disc's own light
	var sun_lit: Vector3 = air.sun_at(solar, 0.0) * dim
	var moon_up: bool = height <= -_d.light_from # the moon lights the town once the sun is gone
	var moon_lit: Vector3 = air.light_at(_moon_dir, 0.0, meter.moon_top) * dim if moon_up else Vector3.ZERO
	if moon_up:
		_aim(_sun, _d.moon_elev_deg, _d.moon_az_deg)
		_shine(moon_lit)
	else:
		_aim(_sun, maxf(height, _d.low_height) * _d.max_elev_deg, place[2])
		_shine(sun_lit)
	# The sky's radiance map is the ambient, scaled by the background's energy and not by ambient_light_energy (that one scales only
	# the flat colour it mixes out: render_scene_data_rd.cpp, 4.7.2): the switch takes the sky's share away, and the colour is none.
	_env.ambient_light_sky_contribution = SKY_LIT if ambient_on else SHADED
	_env.ambient_light_energy = SHADED
	air.publish(_sky, solar)
	for m: ShaderMaterial in [_sky, _probe.material]: # the sky drawn and the one its level light is taken from: the same weather
		m.set_shader_parameter("moon_top", meter.moon_top if moon_up else Vector3.ZERO)
		m.set_shader_parameter("city_up", meter.town * night)
		m.set_shader_parameter("overcast", _overcast * _d.overcast_sky)
		m.set_shader_parameter("flash", _flash * _d.flash_sky)
	_probe.refresh()
	if meter.is_ready():
		meter.expose(meter.adapted(solar, sun_lit, _moon_dir, moon_lit, _overcast * _d.overcast_sky, _flash * _d.flash_sky, night))
		_haze_light(meter.haze(solar, _moon_dir, moon_up, _overcast * _d.overcast_sky, _flash * _d.flash_sky, night))
	_cast()
	_env.glow_enabled = glow_on
	_env.glow_intensity = _d.glow_intensity
	RenderingServer.global_shader_parameter_set(&"night", night)
	RenderingServer.global_shader_parameter_set(&"sun_direction", _sun.global_basis.z)
	var awake := _by_hour(_d.awake_by_hour, hour)
	RenderingServer.global_shader_parameter_set(&"rooms_awake", awake)
	RenderingServer.global_shader_parameter_set(&"offices_lit", _by_hour(_d.office_lit_by_hour, hour))
	RenderingServer.global_shader_parameter_set(&"outside_on", _d.outside_all_night + (1.0 - _d.outside_all_night) * awake)


## The fog's own light: its colour (sRGB, as the engine decodes it) and its energy, from a radiance.
func _haze_light(light: Vector3) -> void:
	var most := maxf(light.x, maxf(light.y, light.z))
	if most <= 0.0:
		_env.fog_light_energy = 0.0
		return
	_env.fog_light_color = Color(light.x / most, light.y / most, light.z / most).linear_to_srgb()
	_env.fog_light_energy = most


## The directional light shines with `lit`, its colour and its energy.
func _shine(lit: Vector3) -> void:
	var most := maxf(lit.x, maxf(lit.y, lit.z))
	_sun.light_color = Color(lit.x, lit.y, lit.z) / most if most > 0.0 else Color.WHITE
	_sun.light_energy = most


## Under the street lamps no one can see the moon's shadow, and it is the whole city drawn again for its maps: it fades with the dusk and the pass goes.
func _cast() -> void:
	if _sun == null:
		return
	_sun.shadow_opacity = 1.0 - minf(night / _d.moon_casts_until, 1.0)
	_sun.shadow_enabled = shadows_on and night < _d.moon_casts_until


## A share by the clock, between a table's hourly figures from midnight: the people awake, the offices lit.
func _by_hour(table: Array, hour: float) -> float:
	var h := fposmod(hour, float(table.size()))
	var i := int(h)
	return lerpf(table[i], table[(i + 1) % table.size()], h - float(i))


## The haze between the eye and what it sees: the air's extinction at the ground (render/air.gd, from the visibility), thinned with
## height as the aerosol is, its colour the sky's own along the view (aerial perspective, the Environment's fog reading the radiance
## map); and the sun's cascades fitted to the view (render/sun_cascades.gd).
func follow_view(camera: Camera3D) -> void:
	if camera == null:
		return
	var y: float = maxf(camera.global_position.y, 1.0)
	var scale_h: float = air.mie.w
	var density: float = air.extinction * scale_h / y * (1.0 - exp(-y / scale_h)) # the mean density of the column under the eye
	if density != _air_density:
		_air_density = density
		_env.fog_density = density
	cascades.fit(camera)


## The direction (elevation, azimuth) in degrees, towards it: noon toward -Z.
static func _toward(elev_deg: float, az_deg: float) -> Vector3:
	var e := deg_to_rad(elev_deg)
	var a := deg_to_rad(az_deg)
	return Vector3(cos(e) * sin(a), sin(e), cos(e) * cos(a))


## Points a light from the direction (elevation, azimuth) in degrees.
func _aim(lamp: DirectionalLight3D, elev_deg: float, az_deg: float) -> void:
	var toward := _toward(elev_deg, az_deg)
	lamp.basis = Basis.looking_at(-toward, Vector3.UP if absf(toward.y) < _d.vertical_dot else Vector3.FORWARD)
