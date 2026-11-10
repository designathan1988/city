## The sun's shadow cascades fitted to the view (render/daylight.gd owns the sun and calls `fit` each frame): how far they reach, where
## each ends, the bias and the fade, from the eye, its tilt, the lens and the ground the picture shows. A fit is a function of those and
## of its own last result: a view at rest is not fitted again. Numbers: data/game/daylight.json.
class_name SunCascades
extends RefCounted

const BIAS_PERCENT := 100.0 # the engine reads the bias in hundredths
const HALF := 0.5
const MARGIN_TEXELS := 2.0
const TOP_EDGE := 1.0 # of the picture, as a sign on the lens's half height (_ground_depth)
const BOTTOM_EDGE := -1.0
const ONE_OF_FOUR := 0.25 # a cascade of the four, as a power of the ratio they span

var side_px := 2048.0 # a cascade's side: game/settings/settings.gd
var filter_radius := 1.0 # the engine's radius for the filter in use
var least_m := 0.0 # the quality's own reach (game/settings/settings.gd)
var _d: Dictionary
var _sun: DirectionalLight3D
var _first_m := 0.0 # where the first cascade ends
var _fitted_for := [] # what they were last fitted to
var _splits := PackedFloat64Array([NAN, NAN, NAN]) # as last given to the light (it keeps them in single precision)


func _init(day: Dictionary, sun: DirectionalLight3D) -> void:
	_d = day
	_sun = sun


## The cascades for this view, when the eye, its tilt, the ground under it, the lens, the screen or the shadow settings changed.
func fit(camera: Camera3D) -> void:
	var ground := CityCamera.ground_below(camera)
	var view := [camera.global_position, camera.global_basis.z.y, ground, camera.near, camera.far, camera.fov,
			camera.get_viewport().get_visible_rect().size, side_px, filter_radius, _sun.shadow_blur, least_m]
	if view != _fitted_for:
		_fitted_for = view
		_fit(camera, ground)


## The filter in use: its radius is the engine's, and the blur follows it so a soft edge keeps its width whatever the filter
## (shadow_blur at shadow_blur_radius). What a filter changes is its samples: 32 at Soft Ultra, 16 at Soft High, per pixel
## (renderer_scene_render_rd.cpp); at the same width the picture holds to 16 and shows its grain at 8.
func set_filter(radius: float) -> void:
	filter_radius = radius
	_sun.shadow_blur = _d.shadow_blur * _d.shadow_blur_radius / radius


## Engel, ShaderX5, as the 4.0's sunCascades.ts: the first ends just past the point looked at, the others in ratio to the far end. A
## fixed share leaves a texel wider than a sign post up close.
## The far end is what the view shows: where the picture ends on the ground before the horizon (looking down) the shadows end with it,
## a step past for the stepping and one for the fade, which then starts where the picture ends; a cascade past that drew the city's
## casters for pixels no one sees, and stretched the texels of the others. With the horizon in the picture the reach is the rule's and
## the tier's, and it fades out by the data's share.
func _fit(camera: Camera3D, ground: float) -> void:
	var eye := camera.global_position
	var above: float = maxf(eye.y - ground, 0.0)
	var look: float = above / maxf(_d.shadow_view_least_dip, camera.global_basis.z.y)
	var depth: float = clampf(maxf(look + _d.shadow_reach_m, look * _d.shadow_past_look), least_m, _d.shadow_depth_max_m)
	var past: float = 1.0 + _d.shadow_reach_step
	var shown: float = _ground_depth(camera, ground, TOP_EDGE) * past * past
	var ends_in_view := shown < depth
	if ends_in_view:
		depth = shown
	var fade: float = 1.0 / past if ends_in_view else _d.shadow_fade_start
	if not is_equal_approx(_sun.directional_shadow_fade_start, fade):
		_sun.directional_shadow_fade_start = fade
	if absf(depth - _sun.directional_shadow_max_distance) > depth * _d.shadow_reach_step:
		_sun.directional_shadow_max_distance = depth
	var end: float = _sun.directional_shadow_max_distance
	# The first cascade's share of the reach, but never short of the nearest ground the view shows (where its bottom edge lands): from
	# above it would cover only air; it ends one cascade of the four, in ratio, into what is shown.
	var cap: float = end * _d.shadow_first_share
	var nearest := _ground_depth(camera, ground, BOTTOM_EDGE)
	if nearest < end:
		cap = maxf(cap, nearest * pow(end / nearest, ONE_OF_FOUR))
	var fitted: float = minf(maxf(look * _d.shadow_first_past_look, _d.shadow_first_least_m), cap)
	# by steps, as the far end: a slice that changes size with every metre of the ground under the eye changes the width of the
	# cascade's texels with it, and every edge of every shadow crawls while the camera moves
	if absf(fitted - _first_m) > fitted * _d.shadow_reach_step or _first_m > cap:
		_first_m = fitted
	var first: float = _first_m
	var second: float = sqrt(first * end)
	_set_split(0, &"directional_shadow_split_1", first / end)
	_set_split(1, &"directional_shadow_split_2", second / end)
	_set_split(2, &"directional_shadow_split_3", sqrt(second * end) / end)
	var bias: float = _d.shadow_bias_m * BIAS_PERCENT / (_first_depth(camera, end) * _sun.shadow_blur * filter_radius)
	if absf(bias - _sun.shadow_bias) > bias * _d.shadow_reach_step:
		_sun.shadow_bias = bias


## How deep into the view the ground under one of its edges lies (TOP_EDGE: the farthest the picture shows; BOTTOM_EDGE: the nearest):
## where the edge meets the ground (every point of it falls the same height a metre of depth), the lower of the ground under the eye
## and the ground where it lands; INF when the edge looks at the horizon or over it.
func _ground_depth(camera: Camera3D, ground: float, edge: float) -> float:
	var rise: float = tan(deg_to_rad(camera.fov) * HALF) * edge # of the edge, a metre of depth
	var fall: float = camera.global_basis.z.y - camera.global_basis.y.y * rise
	if fall <= 0.0:
		return INF
	var eye := camera.global_position
	var depth: float = maxf(eye.y - ground, 0.0) / fall
	var lands: Vector3 = eye + (camera.global_basis.y * rise - camera.global_basis.z) * depth
	var there: float = CityStore.query(&"height_at", [lands.x, lands.z])
	return maxf(eye.y - minf(ground, there), 0.0) / fall


## A split given to the light only when it moved.
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
	var radius: float = Vector2(across, (far - near) * HALF).length() * side_px / (side_px - MARGIN_TEXELS)
	return 2.0 * radius + _d.shadow_pancake_m
