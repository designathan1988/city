## The air of render/shaders/sky_model.gdshaderinc as values (data/game/daylight.json "air"), handed to every shader that draws the
## sky or looks through the air: the sky's own material and, as shader globals, the passes and surfaces. It mirrors the include's
## sky_sun_at for the sun's own light at the ground (Daylight colours the DirectionalLight3D with it), so the light on the town and
## the sky it comes through are the same air.
class_name Air
extends RefCounted

## The air's shape (tools/build/shader_contracts.json SkyShape), the shaders' own: CityStore.constants.shader.
static func _k() -> Dictionary:
	return CityStore.constants.shader

## Koschmieder (1924): the meteorological visibility is where a black target's contrast falls to 2 %, V = ln(50) / beta.
const KOSCHMIEDER := 3.912
const METRES_A_KM := 1000.0
const GREEN := 1 # the channel the visibility is measured in (550 nm, the eye's peak)

var rayleigh := Vector4.ZERO # beta r g b per m, scale height m
var mie := Vector4.ZERO # extinction per m, single-scattering albedo, phase g, scale height m
var ozone := Vector4.ZERO # zenith optical depth r g b, height of the layer m
var ground := Vector3.ZERO # the land's albedo under the sky, linear
var sun_top := Vector3.ZERO # the sun's light over the air, in the lights' energy unit
var extinction := 0.0 # what the air at the ground takes a metre, at 550 nm: the fog's density


func _init(air: Dictionary) -> void:
	var r: Array = air.rayleigh_beta_per_m
	rayleigh = Vector4(r[0], r[1], r[2], air.rayleigh_height_m)
	extinction = KOSCHMIEDER / (float(air.visibility_km) * METRES_A_KM)
	mie = Vector4(extinction - float(r[GREEN]), air.aerosol_albedo, air.aerosol_g, air.aerosol_height_m)
	var o: Array = air.ozone_beta_per_m
	var thick: float = air.ozone_thickness_m
	ozone = Vector4(o[0] * thick, o[1] * thick, o[2] * thick, air.ozone_height_m)
	var g: Array = air.ground_albedo
	ground = Vector3(g[0], g[1], g[2])
	sun_top = Vector3.ONE * float(air.sun_top_klx) / float(air.energy_unit_klx)


## The shell's air mass (sky_model.gdshaderinc sky_airmass).
static func airmass(mu: float, h: float, scale_h: float) -> float:
	var x: float = (_k().PLANET_RADIUS_M + h) / scale_h
	return sqrt(x * x * mu * mu + 2.0 * x + 1.0) - x * mu


static func ozone_airmass(mu: float, layer_h: float) -> float:
	var c := _k()
	var k: float = c.PLANET_RADIUS_M / (c.PLANET_RADIUS_M + layer_h)
	return 1.0 / sqrt(maxf(1.0 - k * k * (1.0 - mu * mu), c.SKY_LEAST_SLANT))


## The sun's light after the air at height h (sky_model.gdshaderinc sky_sun_at), towards `sun` (towards the sun).
func sun_at(sun: Vector3, h: float) -> Vector3:
	var mu := sun.y
	var r := Vector3(rayleigh.x, rayleigh.y, rayleigh.z) * rayleigh.w * exp(-h / rayleigh.w) * airmass(mu, h, rayleigh.w)
	var m := mie.x * mie.w * exp(-h / mie.w) * airmass(mu, h, mie.w)
	var o := Vector3(ozone.x, ozone.y, ozone.z) * ozone_airmass(mu, ozone.w)
	var c := _k()
	var dip: float = -sqrt(2.0 * maxf(h, 1.0) / c.PLANET_RADIUS_M)
	var t := r + Vector3.ONE * m + o
	var seen := smoothstep(dip - c.SUN_RADIUS_RAD, dip + c.SUN_RADIUS_RAD, mu)
	return sun_top * Vector3(exp(-t.x), exp(-t.y), exp(-t.z)) * seen


## The values on a sky material (its own uniforms: a change re-renders the radiance map) and as the shader globals of the passes.
func publish(sky: ShaderMaterial, sun: Vector3) -> void:
	for target: Callable in [sky.set_shader_parameter, RenderingServer.global_shader_parameter_set]:
		target.call(&"solar_direction", sun)
		target.call(&"solar_top", sun_top)
		target.call(&"air_rayleigh", rayleigh)
		target.call(&"air_mie", mie)
		target.call(&"air_ozone", ozone)
		target.call(&"air_ground", ground)
