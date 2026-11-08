## The tone mapping the Environment applies to the 3D image (render/city_world.gd: AgX with its exposure, white and contrast), as
## Godot 4.7 computes it on an SDR output (servers/rendering/renderer_rd/shaders/effects/tonemap.glsl tonemap_agx and allenwp_curve,
## the curve's parameters from storage/environment_storage.cpp environment_get_tonemap_parameters; drivers/gles3 tonemap_inc.glsl is
## the same), and its inverse: the scene radiance a light needs to show as a display colour (a lit signal lens shown in its signal
## colour, whatever the tone mapping does to bright light). AgX's inset matrix mixes the channels, so a bright saturated green or
## amber would need a negative radiance in another channel: such a colour becomes the nearest one AgX shows (least CIELAB distance,
## every channel of the radiance >= 0).
class_name ToneMap
extends RefCounted

## Linear sRGB (D65) to CIE XYZ, as column bases, and the D65 white; CIELAB's epsilon and kappa (CIE 15:2004).
const XYZ := Basis(Vector3(0.4124564, 0.2126729, 0.0193339), Vector3(0.3575761, 0.7151522, 0.1191920),
		Vector3(0.1804375, 0.0721750, 0.9503041))
const D65 := Vector3(0.95047, 1.0, 1.08883)
const LAB_EPS := 216.0 / 24389.0
const LAB_KAPPA := 24389.0 / 27.0
const LAB_L := Vector2(116.0, 16.0) # L* = 116 f(Y) - 16
const LAB_AB := Vector2(500.0, 200.0)
## The nearest colour: steps on each channel of the radiance, from a quarter of its largest, halved while none brings
## the colour closer, down to a hundred-thousandth of it.
const SEARCH_FIRST := 0.25
const SEARCH_END := 1e-5
const SEARCH_MAX_TRIES := 4000
## An SDR output's top, the engine's output_max_value (HDR output is not asked for: project.godot).
const OUTPUT_MAX := 1.0
## The least white AgX takes (environment_storage.cpp environment_get_white).
const ENGINE_LEAST_WHITE := 2.0
## The engine's tonemap_agx once for this and tone_inverse.gdshaderinc (tools/build/shader_contracts.json AgxCurve, AGX_INSET_*,
## AGX_OUTSET_*: CityStore.constants.shader): its crossover, its matrices as column bases (the GLSL mat3(...) takes them as
## columns), the highest share of the output's top the inverse is asked for. Read at first use.
static var _agx := {}


static func _k() -> Dictionary:
	if _agx.is_empty():
		var s: Dictionary = CityStore.constants.shader
		_agx = {cross = s.AGX_CROSSOVER, top = s.AGX_TOP, inset = Basis(s.AGX_INSET_0, s.AGX_INSET_1, s.AGX_INSET_2),
				outset = Basis(s.AGX_OUTSET_0, s.AGX_OUTSET_1, s.AGX_OUTSET_2)}
	return _agx


## The allenwp curve's parameters for a white and a contrast, as environment_get_tonemap_parameters computes them: contrast, toe a,
## slope and w. tone_inverse.gdshaderinc takes them as its tone_curve.
static func curve_params(white: float, contrast: float) -> Vector4:
	var c: float = _k().cross
	var shoulder_max := OUTPUT_MAX - c
	var toe_a := (1.0 / c - 1.0) * pow(c, contrast)
	var denom := pow(c, contrast) + toe_a
	var slope := contrast * pow(c, contrast - 1.0) * toe_a / (denom * denom)
	var w := maxf(white, ENGINE_LEAST_WHITE) * OUTPUT_MAX - c
	return Vector4(contrast, toe_a, slope, w * w / shoulder_max * slope)


## The parameters of an Environment's AgX.
static func params_of(environment: Environment) -> Vector4:
	return curve_params(environment.tonemap_agx_white, environment.tonemap_agx_contrast)


## What the screen shows (linear, 0..1) for scene radiance `s` under the Environment's exposure and AgX, the picture pre-exposed by
## `pre` (render/eye.gd: what the lights, the sky and their emission were multiplied by before it).
static func shown(s: Vector3, environment: Environment, pre := 1.0) -> Vector3:
	return agx(s * pre, environment.tonemap_exposure, params_of(environment))


## The luminance of a linear sRGB colour: XYZ's Y.
static func luminance(lin: Vector3) -> float:
	return lin.dot(Vector3(XYZ.x.y, XYZ.y.y, XYZ.z.y))


static func agx(s: Vector3, exposure: float, p: Vector4) -> Vector3:
	var k := _k()
	var x: Vector3 = k.inset * (s.max(Vector3.ZERO) * exposure)
	var y := Vector3(_curve(x.x, p), _curve(x.y, p), _curve(x.z, p)).min(Vector3.ONE * OUTPUT_MAX)
	return (k.outset * y).clamp(Vector3.ZERO, Vector3.ONE)


## The scene radiance (linear, >= 0) that shows as `srgb` under the Environment's exposure and AgX, or as the nearest colour AgX
## shows.
static func radiance(srgb: Color, environment: Environment) -> Vector3:
	var exposure := environment.tonemap_exposure
	var p := params_of(environment)
	var lin := srgb.srgb_to_linear()
	var goal := Vector3(lin.r, lin.g, lin.b)
	var k := _k()
	var outset: Basis = k.outset
	var inset: Basis = k.inset
	var y := outset.inverse() * goal
	var top: float = k.top * OUTPUT_MAX
	var reachable := y.x >= 0.0 and y.y >= 0.0 and y.z >= 0.0 and maxf(y.x, maxf(y.y, y.z)) <= top
	y = y.clamp(Vector3.ZERO, Vector3.ONE * top)
	var s: Vector3 = inset.inverse() * Vector3(_curve_inverse(y.x, p), _curve_inverse(y.y, p), _curve_inverse(y.z, p)) / exposure
	if reachable and s.x >= 0.0 and s.y >= 0.0 and s.z >= 0.0:
		return s
	return _nearest(s.max(Vector3.ZERO), _lab(goal), exposure, p)


static func _nearest(s: Vector3, goal: Vector3, exposure: float, p: Vector4) -> Vector3:
	var best := _lab(agx(s, exposure, p)).distance_squared_to(goal)
	var step := maxf(s.x, maxf(s.y, s.z)) * SEARCH_FIRST
	var end := step * SEARCH_END
	var tries := 0
	while step > end and tries < SEARCH_MAX_TRIES:
		var moved := false
		for axis in 3:
			for sign: float in [1.0, -1.0]:
				var c := s
				c[axis] = maxf(0.0, c[axis] + sign * step)
				var e := _lab(agx(c, exposure, p)).distance_squared_to(goal)
				tries += 1
				if e < best:
					best = e
					s = c
					moved = true
		if not moved:
			step *= 0.5
	return s


## allenwp_curve: a sigmoid power toe under the crossover, a Reinhard-like shoulder over it.
static func _curve(x: float, p: Vector4) -> float:
	var c: float = _k().cross
	if x < c:
		var t := pow(x, p.x)
		return t / (t + p.y)
	var s := x - c
	var slope_s := p.z * s
	return slope_s * (1.0 + s / p.w) / (1.0 + slope_s / (OUTPUT_MAX - c)) + c


## x >= 0 with _curve(x) = y, 0 <= y < OUTPUT_MAX. The toe: t = a y / (1 - y), x = t^(1 / contrast). The shoulder, u = y - crossover
## and m the shoulder's top: (slope / w) s^2 + slope (1 - u / m) s - u = 0, its root s >= 0.
static func _curve_inverse(y: float, p: Vector4) -> float:
	var c: float = _k().cross
	if y < c:
		return pow(p.y * y / (1.0 - y), 1.0 / p.x)
	var u := y - c
	var a := p.z / p.w
	var b := p.z * (1.0 - u / (OUTPUT_MAX - c))
	return (-b + sqrt(b * b + 4.0 * a * u)) / (2.0 * a) + c


## CIELAB (D65) of a linear sRGB colour.
static func _lab(lin: Vector3) -> Vector3:
	var xyz := XYZ * lin / D65
	var f := Vector3(_lab_f(xyz.x), _lab_f(xyz.y), _lab_f(xyz.z))
	return Vector3(LAB_L.x * f.y - LAB_L.y, LAB_AB.x * (f.x - f.y), LAB_AB.y * (f.y - f.z))


static func _lab_f(t: float) -> float:
	return pow(t, 1.0 / 3.0) if t > LAB_EPS else (LAB_KAPPA * t + LAB_L.y) / LAB_L.x
