## The weather as drawn: rain streaks in a box over the ground under the view, bolts of lightning and their flash, and
## the thunder that follows (render/thunder.gd). All numbers come from data/game/sky.json.
class_name WeatherView
extends Node3D

## A bolt's channel: midpoint displacement generations of the main channel and of a branch.
const MAIN_GENERATIONS := 7
const BRANCH_GENERATIONS := 5
const LEVEL_VIEW := 0.0015

## How bright the flash is now (0 none .. about 1): the daylight lifts the sky by it.
var flash := 0.0

var _d: Dictionary
var _rain_mat: ShaderMaterial
var _rain: MeshInstance3D
var _box := Vector3.ONE
var _centre := Vector3.ZERO
var _lattice := Vector3.ZERO
var _started := false
var _bolt_mat: ShaderMaterial
var _bolts: Array = [] # { age, node }
var _thunder: Thunder
var _rng := RandomNumberGenerator.new()
var _thunder_volume := 0.0


func _init(d: Dictionary) -> void:
	_d = d


func _ready() -> void:
	_rng.seed = int(_d.rng_seed)
	_rain_mat = ShaderMaterial.new()
	_rain_mat.shader = load("res://render/shaders/rain.gdshader")
	_rain_mat.set_shader_parameter("streak", _d.rain_streak_s)
	_rain = MeshInstance3D.new()
	_rain.mesh = _rain_mesh()
	_rain.material_override = _rain_mat
	_rain.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	_rain.custom_aabb = AABB(Vector3.ONE * -_d.rain_aabb_m, Vector3.ONE * _d.rain_aabb_m * 2.0)
	_rain.visible = false
	add_child(_rain)
	_bolt_mat = ShaderMaterial.new()
	_bolt_mat.shader = load("res://render/shaders/lightning.gdshader")
	_thunder = Thunder.new(_d)
	add_child(_thunder)


## Two vertices a drop: its seed in the box, the same for both (the shader tells head from tail by VERTEX_ID).
func _rain_mesh() -> ArrayMesh:
	var n := int(_d.rain_max_drops)
	var pts := PackedVector3Array()
	pts.resize(n * 2)
	var r := RandomNumberGenerator.new()
	r.seed = int(_d.rng_seed)
	for i in n:
		var p := Vector3(r.randf(), r.randf(), r.randf())
		pts[i * 2] = p
		pts[i * 2 + 1] = p
	var arrays := []
	arrays.resize(Mesh.ARRAY_MAX)
	arrays[Mesh.ARRAY_VERTEX] = pts
	var m := ArrayMesh.new()
	m.add_surface_from_arrays(Mesh.PRIMITIVE_LINES, arrays)
	return m


func step(dt: float, weather: Dictionary, way: Vector2) -> void:
	var cam := get_viewport().get_camera_3d()
	if cam == null or weather.is_empty():
		return
	var ground := _ground_under(cam)
	var span := 2.0 * cam.global_position.distance_to(ground) * tan(deg_to_rad(cam.fov) * 0.5) * float(get_viewport().get_visible_rect().size.aspect())
	var rain: float = weather.rain
	_rain.visible = rain > _d.rain_min
	if _rain.visible:
		_advance(ground, span, way * float(weather.wind), dt)
		_rain_mat.set_shader_parameter("drops", roundi(_d.rain_max_drops * minf(1.0, rain)))
		_rain_mat.set_shader_parameter("opacity", _d.rain_opacity + _d.rain_opacity_full * rain)
	var rate: float = weather.lightning
	if rate > 0.0 and _rng.randf() < dt * rate / 60.0:
		var a := _rng.randf() * TAU
		var r: float = _rng.randf() * span * _d.strike_reach
		strike(ground + Vector3(cos(a) * r, 0.0, sin(a) * r))
	_age(dt)
	_bolt_mat.set_shader_parameter("brightness", flash)
	_bolt_mat.set_shader_parameter("screen", get_viewport().get_visible_rect().size)
	_thunder_volume = weather.thunder


func _ground_under(cam: Camera3D) -> Vector3:
	var ahead := -cam.global_basis.z
	var y: float = CityStore.query(&"height_at", [cam.global_position.x, cam.global_position.z])
	var t := (y - cam.global_position.y) / ahead.y if ahead.y < -LEVEL_VIEW else float(_d.rain_look_m)
	return cam.global_position + ahead * minf(t, _d.rain_look_m)


## The rain one frame on: the box over the ground as wide as the view, the lattice's origin carried into it so nothing
## jumps; a fall fast enough to cross the view in about a second at any zoom, the wind's slant kept in proportion.
func _advance(ground: Vector3, span: float, wind: Vector2, dt: float) -> void:
	var side := clampf(span * _d.rain_box_share, _d.rain_box_least_m, _d.rain_box_most_m)
	var tall: float = side * _d.rain_box_tall
	var c := Vector3(ground.x, ground.y + tall * _d.rain_box_lift, ground.z)
	var o := Vector3.ZERO
	if _started:
		o = _centre - _box * 0.5 + _lattice
		o.x = c.x + (o.x - c.x) * (side / _box.x)
		o.y = ground.y + (o.y - ground.y) * (tall / _box.y)
		o.z = c.z + (o.z - c.z) * (side / _box.z)
	var fall := maxf(_d.rain_fall_ms, tall * _d.rain_fall_share)
	var scale: float = fall / _d.rain_fall_ms
	var length := wind.length()
	var across := minf(_d.rain_slant * fall, length * scale)
	var k := across / length if length > 1e-6 else 0.0
	var v := Vector3(wind.x * k, -fall, wind.y * k)
	o += v * dt
	_box = Vector3(side, tall, side)
	_centre = c
	_lattice = Vector3(fposmod(o.x - (c.x - side * 0.5), side), fposmod(o.y - (c.y - tall * 0.5), tall), fposmod(o.z - (c.z - side * 0.5), side))
	_started = true
	_rain_mat.set_shader_parameter("centre", _centre)
	_rain_mat.set_shader_parameter("box", _box)
	_rain_mat.set_shader_parameter("lattice", _lattice)
	_rain_mat.set_shader_parameter("velocity", v)
	_rain_mat.set_shader_parameter("near_m", side * _d.rain_near_share)


## A bolt from the cloud base to `at`.
func strike(at: Vector3) -> void:
	if _bolts.size() >= int(_d.max_strikes):
		_end(0)
	var top: Vector3 = at + Vector3(_rng.randf_range(-1.0, 1.0) * _d.bolt_lean_m, _d.bolt_height_m, _rng.randf_range(-1.0, 1.0) * _d.bolt_lean_m)
	var node := MeshInstance3D.new()
	node.mesh = _bolt_mesh(top, at)
	node.material_override = _bolt_mat
	node.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	node.extra_cull_margin = _d.bolt_height_m
	add_child(node)
	_bolts.append({"age": 0.0, "node": node})
	var cam := get_viewport().get_camera_3d()
	if cam:
		_thunder.play(cam.global_position.distance_to(at), _thunder_volume)


func _split(a: Vector3, b: Vector3, kick: float, generations: int) -> PackedVector3Array:
	var pts := PackedVector3Array([a, b])
	var offset := kick
	for g in generations:
		var next := PackedVector3Array([pts[0]])
		for i in range(1, pts.size()):
			var p := pts[i - 1]
			var q := pts[i]
			var along := (q - p).normalized()
			var side := Vector3(_rng.randf() - 0.5, _rng.randf() - 0.5, _rng.randf() - 0.5).cross(along).normalized()
			next.append((p + q) * 0.5 + side * _rng.randf_range(-1.0, 1.0) * offset)
			next.append(q)
		pts = next
		offset *= 0.5
	return pts


func _bolt_mesh(from: Vector3, to: Vector3) -> ArrayMesh:
	var length := from.distance_to(to)
	var channel := _split(from, to, length * _d.bolt_kick, MAIN_GENERATIONS)
	var arrays := _BoltArrays.new()
	arrays.ribbon(channel, _d.bolt_core_px, 1.0)
	arrays.ribbon(channel, _d.bolt_glow_px, _d.bolt_glow_opacity)
	var count := int(_d.bolt_branches_min) + _rng.randi_range(0, int(_d.bolt_branches_more))
	for b in count:
		var f := _rng.randf_range(_d.bolt_branch_from, _d.bolt_branch_to)
		var start := channel[int(f * (channel.size() - 1))]
		var down := (to - from).normalized()
		var side := Vector3(_rng.randf() - 0.5, 0.0, _rng.randf() - 0.5).normalized()
		var dir := (down * cos(_d.bolt_branch_rad) + side * sin(_d.bolt_branch_rad)).normalized()
		var reach: float = length * (1.0 - f) * _rng.randf_range(_d.bolt_branch_least, _d.bolt_branch_most)
		arrays.ribbon(_split(start, start + dir * reach, reach * _d.bolt_branch_kick, BRANCH_GENERATIONS), _d.bolt_branch_px, _d.bolt_branch_opacity)
	return arrays.mesh()


## A stroke, a dim, the return stroke, then the glow dies away.
func _age(dt: float) -> void:
	flash = 0.0
	for i in range(_bolts.size() - 1, -1, -1):
		var s: Dictionary = _bolts[i]
		s.age += dt
		if s.age >= _d.strike_life_s:
			_end(i)
			continue
		var t: float = s.age
		var life: float = _d.strike_life_s
		var bright: float = 1.0 if t < _d.stroke_s else (_d.dim_level if t < _d.dim_s else (_d.return_level if t < _d.return_s else maxf(0.0, 1.0 - (t - _d.return_s) / (life - _d.return_s)) * _d.glow_level))
		flash = maxf(flash, bright)


func _end(i: int) -> void:
	_bolts[i].node.queue_free()
	_bolts.remove_at(i)


## The triangles of a bolt's ribbons (lightning.gdshader's layout), gathered as arrays.
class _BoltArrays:
	var pos := PackedVector3Array()
	var other := PackedVector3Array()
	var uv := PackedVector2Array()
	var col := PackedColorArray()
	var idx := PackedInt32Array()

	func ribbon(pts: PackedVector3Array, width: float, opacity: float) -> void:
		for i in range(1, pts.size()):
			var at := pos.size()
			for corner in [[pts[i - 1], pts[i], -1.0], [pts[i - 1], pts[i], 1.0], [pts[i], pts[i - 1], 1.0], [pts[i], pts[i - 1], -1.0]]:
				pos.append(corner[0])
				other.append(corner[1])
				uv.append(Vector2(corner[2], width))
				col.append(Color(1, 1, 1, opacity))
			idx.append_array(PackedInt32Array([at, at + 1, at + 2, at, at + 2, at + 3]))

	func mesh() -> ArrayMesh:
		var arrays := []
		arrays.resize(Mesh.ARRAY_MAX)
		arrays[Mesh.ARRAY_VERTEX] = pos
		arrays[Mesh.ARRAY_NORMAL] = other
		arrays[Mesh.ARRAY_TEX_UV] = uv
		arrays[Mesh.ARRAY_COLOR] = col
		arrays[Mesh.ARRAY_INDEX] = idx
		var m := ArrayMesh.new()
		m.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)
		return m
