## What the eye takes in and how it adapts: the night's own lights over the air (the full moon, the town's light sent up), the luminance
## the eye is adapted to (metered on the sky's row, render/sky_level_table.gd) and the picture's pre-exposure that follows from it
## (render/eye.gd), given to the camera and to the shaders that pre-expose by hand. render/daylight.gd places the sun and the moon and
## asks for the meter at each hour; the numbers are data/game/daylight.json's.
class_name LightMeter
extends RefCounted

const POGSON := 0.4 # a magnitude is 10^0.4 in brightness (Pogson's ratio)
const DECADE := 10.0 # the base magnitudes are counted in
const COMPATIBILITY := "gl_compatibility"
const HORIZON_ZENITH := 9.0 / 7.0 # the CIE overcast sky's zenith over its level light, Lz = 9 / (7 pi) E (sky_model.gdshaderinc sky_lit)
const HORIZON_THIRDS := 3.0 # its horizon a third of its zenith, (1 + 2 sin 0) / 3 # RenderingServer.get_current_rendering_method() of the renderer that pre-exposes its sky right

var lux := 0.0 # the radiance of a white floor under 1 lx
var pre_exposure := 1.0 # the eye's adaptation, as the picture's pre-exposure
var moon_top := Vector3.ZERO # the full moon's light over the air
var town := Vector3.ZERO # the town's light sent up while its lamps are on, a radiance (fill)
var _d: Dictionary
var _air: Air
var _table: SkyLevelTable
var _camera: CameraAttributesPractical
var _env: Environment
var _sky: ShaderMaterial
var _eye: Eye


func _init(day: Dictionary, air: Air, table: SkyLevelTable, camera: CameraAttributesPractical, env: Environment,
		sky: ShaderMaterial) -> void:
	_d = day
	_air = air
	_table = table
	_camera = camera
	_env = env
	_sky = sky
	_eye = Eye.new(day.eye)
	lux = 1.0 / air.unit_lx
	var moon: Dictionary = day.moon
	moon_top = air.sun_top * _moon_tint(moon) * pow(DECADE, -POGSON * (float(moon.magnitude) - float(moon.sun_magnitude)))
	RenderingServer.global_shader_parameter_set(&"lux", lux)
	RenderingServer.global_shader_parameter_set(&"room_lux", Vector2(day.rooms.home_lux, day.rooms.office_lux) * lux)
	RenderingServer.global_shader_parameter_set(&"pre_exposure", pre_exposure)


func is_ready() -> bool:
	return _table.is_filled()


## The moon's colour: the sun's times its reflectance, which rises with the wavelength (daylight.json moon), of luminance 1.
func _moon_tint(moon: Dictionary) -> Vector3:
	var nm := _air.channel_nm
	var r := Vector3.ONE + (nm - Vector3.ONE * nm.y) * float(moon.reflectance_slope_per_nm)
	return r / ToneMap.luminance(r)


## The sky's row is in: the town's light is what makes the zenith measured over it (daylight.json sky_glow), in the street lamps'
## colour; `day_adapted` (the day hour's light, metered by the caller now that it can be) is the eye's reference.
func fill(day_adapted: Callable) -> void:
	var lamp := Color.html(DataTables.render_table("materials").lamp_glow.params.glow_color).srgb_to_linear()
	var colour := Vector3(lamp.r, lamp.g, lamp.b)
	colour /= ToneMap.luminance(colour)
	var glow: Dictionary = _d.sky_glow
	var zenith: float = float(glow.mag_zero_cd_m2) * pow(DECADE, -POGSON * float(glow.zenith_mag_arcsec2)) * PI * lux
	town = colour * zenith / ToneMap.luminance(_table.town_zenith * colour)
	_eye.set_day(day_adapted.call())


## The luminance the eye adapts to, cd/m^2 (daylight.json eye): a grey card under the light of the level ground, the sun's and the
## moon's own (`sun_lit`, `moon_lit` after the air and the weather, towards `toward_sun` and `toward_moon`) and the sky's, which they
## and the town's light make (the sky's row under the weather's deck, as sky_model.gdshaderinc has it), plus the lit street while
## the lamps are on (`lamps`).
func adapted(toward_sun: Vector3, sun_lit: Vector3, toward_moon: Vector3, moon_lit: Vector3, overcast: float, flash: float,
		lamps: float) -> float:
	var level := ToneMap.luminance(sun_lit) * maxf(toward_sun.y, 0.0) + _lit_sky(toward_sun, _air.sun_top, overcast)
	if moon_lit != Vector3.ZERO:
		level += ToneMap.luminance(moon_lit) * maxf(toward_moon.y, 0.0) + _lit_sky(toward_moon, moon_top, overcast)
	var up := town * lamps
	level += lerpf(ToneMap.luminance(_table.town_level * up), ToneMap.luminance(up) * float(_d.sky_glow.cloud_reflect), overcast)
	level *= 1.0 + flash
	var eye: Dictionary = _d.eye
	return float(eye.grey_card) * level * _air.unit_lx / PI + float(eye.street_cd_m2) * lamps


## The level light of the sky one light over the air makes (the sun's or the moon's, `top`), a luminance in the lights' unit: the clear
## row times that light over the sun's, under the weather's deck (sky_model.gdshaderinc sky_lit: the CIE overcast sky's level light
## is overcast_pass of the light's own on level ground).
func _lit_sky(toward: Vector3, top: Vector3, overcast: float) -> float:
	var clear := ToneMap.luminance(_table.level(toward.y) * top / _air.sun_top)
	var deck: float = _d.overcast_pass * ToneMap.luminance(_air.light_at(toward, 0.0, top)) * maxf(toward.y, 0.0)
	return lerpf(clear, deck, overcast)


## The haze's light, a radiance (the sky at the horizon round the eye, the mean of its row, render/sky_level_table.gd): the sun's
## clear horizon or the overcast deck's (the CIE overcast sky at the horizon, a third of its zenith), the moon's alike, the town's;
## pre-exposed as the picture is. The Environment's fog takes it as its own light (Compatibility reads no radiance map for the haze;
## Forward+'s aerial perspective reads the sky itself and leaves it aside).
func haze(toward_sun: Vector3, toward_moon: Vector3, moon_up: bool, overcast: float, flash: float, lamps: float) -> Vector3:
	var light := _lit_horizon(toward_sun, _air.sun_top, overcast)
	if moon_up:
		light += _lit_horizon(toward_moon, moon_top, overcast)
	var up := town * lamps
	light += (_table.town_horizon * up).lerp(up * float(_d.sky_glow.cloud_reflect), overcast)
	return light * (1.0 + flash) * pre_exposure


func _lit_horizon(toward: Vector3, top: Vector3, overcast: float) -> Vector3:
	var clear := _table.horizon(toward.y) * top / _air.sun_top
	var zenith: float = HORIZON_ZENITH * _d.overcast_pass * ToneMap.luminance(_air.light_at(toward, 0.0, top)) * maxf(toward.y, 0.0)
	return clear.lerp(Vector3.ONE * zenith / HORIZON_THIRDS, overcast)


## The eye adapted to `adapted_cd_m2`: the picture's pre-exposure, given to the camera and to the shaders that pre-expose by hand.
## On Forward+ and Mobile the sky is pre-exposed by its own shader (sky.gdshader) and the background's energy takes the exposure back
## out, because Godot 4.7.2 pre-exposes their sky's radiance map once and its ambient twice: the map is drawn times the background's
## energy b and the camera's exposure e (renderer_rd/environment/sky.cpp, sky.glsl brightness_multiplier), the ambient and the
## reflections read it times b again (render_scene_data_rd.cpp ambient_light_color_energy.a) and times e again
## (IBL_exposure_normalization, over a baked exposure that holds only the buffer's luminance multiplier), while the fog's aerial
## perspective reads the map as it is. With b = 1 / e and the sky's own light times e, the sky drawn, the map and the fog are times e,
## and the ambient e x b^2 x e^2 = e: once. Compatibility's ambient is the map times b (rasterizer_scene_gles3.cpp, scene_gles3.glsl)
## and its map is pre-exposed once, so there b = 1 and the sky shader writes the code its renderer decodes to the light times e.
func expose(adapted_cd_m2: float) -> void:
	hold(_eye.pre_exposure(adapted_cd_m2))


## The picture pre-exposed by `e`, whatever the light (expose; a test holds one of its own). The sky's own uniform, not a global, so
## a change of it draws the sky's radiance map again.
func hold(e: float) -> void:
	pre_exposure = e
	_camera.exposure_multiplier = e
	_env.background_energy_multiplier = 1.0 if RenderingServer.get_current_rendering_method() == COMPATIBILITY else 1.0 / e
	_sky.set_shader_parameter("pre_exposure", e)
	RenderingServer.global_shader_parameter_set(&"pre_exposure", e)
