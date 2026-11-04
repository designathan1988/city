## The tone mapping the Environment applies to the 3D image (render/city_world.gd: ACES with its exposure and white), as
## Godot 4.7 computes it (servers/rendering/renderer_rd/shaders/effects/tonemap.glsl tonemap_aces, the white from
## storage/environment_storage.cpp environment_get_tonemap_parameters; drivers/gles3 tonemap_inc.glsl is the same), and
## its inverse: the scene radiance a light needs to show as a display colour (a lit signal lens shown as its colour, as
## the Roadcraft 4.0 draws it untone-mapped). ACES's input matrix mixes the channels, so a bright saturated green or
## amber would need a negative radiance in another channel: such a colour becomes the nearest one ACES shows (least
## CIELAB distance, every channel of the radiance >= 0).
class_name ToneMap
extends RefCounted

const BIAS := 1.8 # tonemap_aces exposure_bias
const A := 0.0245786
const B := 0.000090537
const C := 0.983729
const D := 0.432951
const E := 0.238081
## tonemap_aces rgb_to_rrt and odt_to_rgb, as column bases (the GLSL `color *= m` takes m's vec3s as rows).
const RRT := Basis(Vector3(0.59719, 0.07600, 0.02840), Vector3(0.35458, 0.90834, 0.13383), Vector3(0.04823, 0.01566, 0.83777))
const ODT := Basis(Vector3(1.60475, -0.10208, -0.00327), Vector3(-0.53108, 1.10813, -0.07276), Vector3(-0.07367, -0.00605, 1.07602))
const CURVE_TOP := 0.999 # of the curve's asymptote 1 / C: the highest output the inverse is asked for
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


## What the screen shows (linear, 0..1) for scene radiance `s` under Environment `exposure` and `white`.
static func aces(s: Vector3, exposure: float, white: float) -> Vector3:
	var x := RRT * (s.max(Vector3.ZERO) * exposure * BIAS)
	var out := ODT * Vector3(_curve(x.x), _curve(x.y), _curve(x.z)) / _curve(maxf(white, 1.0) * BIAS)
	return out.clamp(Vector3.ZERO, Vector3.ONE)


## The scene radiance (linear, >= 0) that shows as `srgb` under Environment `exposure` and `white`, or as the nearest
## colour ACES shows.
static func radiance(srgb: Color, exposure: float, white: float) -> Vector3:
	var lin := srgb.srgb_to_linear()
	var goal := Vector3(lin.r, lin.g, lin.b)
	var y := ODT.inverse() * (goal * _curve(maxf(white, 1.0) * BIAS))
	var top := CURVE_TOP / C
	var reachable := y.x >= 0.0 and y.y >= 0.0 and y.z >= 0.0 and maxf(y.x, maxf(y.y, y.z)) <= top
	y = y.clamp(Vector3.ZERO, Vector3.ONE * top)
	var s := RRT.inverse() * Vector3(_curve_inverse(y.x), _curve_inverse(y.y), _curve_inverse(y.z)) / (exposure * BIAS)
	if reachable and s.x >= 0.0 and s.y >= 0.0 and s.z >= 0.0:
		return s
	return _nearest(s.max(Vector3.ZERO), _lab(goal), exposure, white)


static func _nearest(s: Vector3, goal: Vector3, exposure: float, white: float) -> Vector3:
	var best := _lab(aces(s, exposure, white)).distance_squared_to(goal)
	var step := maxf(s.x, maxf(s.y, s.z)) * SEARCH_FIRST
	var end := step * SEARCH_END
	var tries := 0
	while step > end and tries < SEARCH_MAX_TRIES:
		var moved := false
		for axis in 3:
			for sign: float in [1.0, -1.0]:
				var c := s
				c[axis] = maxf(0.0, c[axis] + sign * step)
				var e := _lab(aces(c, exposure, white)).distance_squared_to(goal)
				tries += 1
				if e < best:
					best = e
					s = c
					moved = true
		if not moved:
			step *= 0.5
	return s


static func _curve(x: float) -> float:
	return (x * (x + A) - B) / (x * (C * x + D) + E)


## x >= 0 with _curve(x) = y, 0 <= y < 1 / C: the root of (yC - 1) x^2 + (yD - A) x + (yE + B) = 0.
static func _curve_inverse(y: float) -> float:
	var a := y * C - 1.0
	var b := y * D - A
	var c := y * E + B
	return (-b - sqrt(b * b - 4.0 * a * c)) / (2.0 * a)


## CIELAB (D65) of a linear sRGB colour.
static func _lab(lin: Vector3) -> Vector3:
	var xyz := XYZ * lin / D65
	var f := Vector3(_lab_f(xyz.x), _lab_f(xyz.y), _lab_f(xyz.z))
	return Vector3(LAB_L.x * f.y - LAB_L.y, LAB_AB.x * (f.x - f.y), LAB_AB.y * (f.y - f.z))


static func _lab_f(t: float) -> float:
	return pow(t, 1.0 / 3.0) if t > LAB_EPS else (LAB_KAPPA * t + LAB_L.y) / LAB_L.x
