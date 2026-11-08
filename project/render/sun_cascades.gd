## The sun's shadow cascades fitted to the view (Engel, ShaderX5, as the 4.0's sunCascades.ts): the first ends just past the point
## looked at, the others in ratio to the far end, the bias kept the same in texels; the graphics tier sets the cascades' side, the
## filter and the least reach (game/settings/quality.gd). Daylight owns it and hands it the view each frame. Numbers from
## data/game/daylight.json.
class_name SunCascades
extends RefCounted

const BIAS_PERCENT := 100.0 # the engine reads the bias in hundredths
const HALF := 0.5
const MARGIN_TEXELS := 2.0 # a texel of margin on each side of a cascade (renderer_scene_cull.cpp)
## A cascade's side until the quality tier sets it: Godot's default directional shadow atlas, 4096, over the two cascades a side.
const ENGINE_CASCADE_PX := 2048.0

var shadow_side_px := ENGINE_CASCADE_PX
var shadow_filter_radius := 1.0 # the engine's radius for the filter in use
var shadow_least_m := 0.0 # the quality's own reach (game/settings/settings.gd)
var _d: Dictionary
var _sun: DirectionalLight3D
var _first_m := 0.0 # where the first cascade ends
## What the cascades were last fitted to: the eye, its tilt, the ground under it, the lens, the screen and the shadow settings. The fit
## is a function of these and of its own last result, which it reproduces when they repeat: a view at rest is not fitted again.
var _fitted_for := []
## The splits last given to the engine (it keeps them in single precision, so its own copy does not compare).
var _splits := PackedFloat64Array([NAN, NAN, NAN])


func _init(day: Dictionary, sun: DirectionalLight3D) -> void:
	_d = day
	_sun = sun


## Fitted again only when what the fit reads changed.
func follow(camera: Camera3D) -> void:
	var ground := CityCamera.ground_below(camera)
	var view := [camera.global_position, camera.global_basis.z.y, ground, camera.near, camera.far, camera.fov,
			camera.get_viewport().get_visible_rect().size, shadow_side_px, shadow_filter_radius, _sun.shadow_blur, shadow_least_m]
	if view != _fitted_for:
		_fitted_for = view
		_fit(camera, ground)


## The filter in use: its radius is the engine's, and the blur follows it so a soft edge keeps its width whatever the filter
## (shadow_blur at shadow_blur_radius). What a filter changes is its samples: 32 at Soft Ultra, 16 at Soft High, per pixel
## (renderer_scene_render_rd.cpp); at the same width the picture holds to 16 and shows its grain at 8.
func set_filter(radius: float) -> void:
	shadow_filter_radius = radius
	_sun.shadow_blur = _d.shadow_blur * _d.shadow_blur_radius / radius


## A fixed share leaves a texel wider than a sign post up close: the first cascade ends just past the point looked at.
func _fit(camera: Camera3D, ground: float) -> void:
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
