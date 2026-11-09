## Tool previews drawn over the world: the road as it will be laid (Roadcraft main.ts drawRoadPreview: a green or red
## outline, the verge, footway, kerb, carriageway and its paint, a ring at each end), a plain ribbon for the other tools,
## and the zone brush disc. Colours and widths are the preview tokens of data/ui/tokens.json; nothing here tests depth,
## so buildings never hide it, and each band's material draws after the one under it. Outlines that stay while the view moves
## (the Lots tool's lots) are one mesh of their own, built when they change, their width in screen pixels kept by its shader.
class_name PreviewLine
extends MeshInstance3D

const DISC_SEGMENTS := 48 # chords of a disc mark
const OUTLINE_SHADER := "res://render/shaders/preview_outline.gdshader"

var _mesh := ImmediateMesh.new()
var _ok_mat: StandardMaterial3D
var _bad_mat: StandardMaterial3D
var _bands: Array[StandardMaterial3D] = []
var _paint: StandardMaterial3D
var _paint_yellow: StandardMaterial3D
var _edge_ok: StandardMaterial3D
var _edge_bad: StandardMaterial3D
var _ring_ok: StandardMaterial3D
var _ring_bad: StandardMaterial3D
var _outlines: MeshInstance3D # show_outlines: one surface for every ring
var _outline_mat: ShaderMaterial
var _shades: Dictionary = {} # show_ribbons: the materials of the colour tokens the tools asked for, by token


func _ready() -> void:
	mesh = _mesh
	cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	_ok_mat = _material("preview_ok")
	_bad_mat = _material("preview_bad")
	_edge_ok = _material("preview_edge_ok")
	_edge_bad = _material("preview_edge_bad")
	for token in ["preview_casing", "preview_footway", "preview_kerb", "preview_asphalt"]:
		_bands.append(_material(token, _bands.size() + 1))
	_paint = _material("preview_paint", _bands.size() + 1)
	_paint_yellow = _material("preview_paint_yellow", _bands.size() + 1)
	_ring_ok = _material("preview_edge_ok", _bands.size() + 2)
	_ring_bad = _material("preview_edge_bad", _bands.size() + 2)
	_outline_mat = ShaderMaterial.new()
	_outline_mat.shader = load(OUTLINE_SHADER)
	_outline_mat.set_shader_parameter("colour", ThemeBuilder.color("preview_ok"))
	_outlines = MeshInstance3D.new()
	_outlines.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	_outlines.material_override = _outline_mat
	_outlines.visible = false
	add_child(_outlines)
	get_viewport().size_changed.connect(_follow_screen)
	_follow_screen()


func _material(token: String, order: int = 0) -> StandardMaterial3D:
	var m := StandardMaterial3D.new()
	m.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	m.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
	m.no_depth_test = true
	m.cull_mode = BaseMaterial3D.CULL_DISABLED
	m.albedo_color = ThemeBuilder.color(token)
	m.render_priority = order
	return m


func clear() -> void:
	_mesh.clear_surfaces()
	clear_outlines()


## Closed rings on the ground, each drawn `half_px` screen pixels to each side of its line at any zoom: one mesh, built here and
## left alone while the view moves (each ring's own surface would cost a draw each, and a mesh holds at most 256 of them).
func show_outlines(rings: Array, half_px: float) -> void:
	var lift := Vector3.UP * ThemeBuilder.size("preview_lift")
	var at := PackedVector3Array()
	var side := PackedVector2Array()
	var index := PackedInt32Array()
	for ring: PackedVector3Array in rings:
		var points := ring.duplicate()
		points.push_back(ring[0])
		var first := at.size()
		for i in points.size():
			var left := _left(points, i)
			at.push_back(points[i] + lift)
			side.push_back(Vector2(left.x, left.z))
			at.push_back(points[i] + lift)
			side.push_back(Vector2(-left.x, -left.z))
		for k in points.size() * 2 - 2: # the strip as triangles: each vertex with the two after it
			index.append(first + k)
			index.append(first + k + 1)
			index.append(first + k + 2)
	var arrays := []
	arrays.resize(Mesh.ARRAY_MAX)
	arrays[Mesh.ARRAY_VERTEX] = at
	arrays[Mesh.ARRAY_TEX_UV] = side
	arrays[Mesh.ARRAY_INDEX] = index
	var mesh := ArrayMesh.new()
	if not index.is_empty():
		mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)
	_outline_mat.set_shader_parameter("half_px", half_px)
	_outlines.mesh = mesh
	_outlines.visible = not index.is_empty()


func clear_outlines() -> void:
	_outlines.mesh = null
	_outlines.visible = false


## The screen's height, what a pixel's width in metres is reckoned from (ToolController.px_at).
func _follow_screen() -> void:
	_outline_mat.set_shader_parameter("screen_h", get_viewport().get_visible_rect().size.y)


## Points along the road on the ground (lifted by preview_lift here) and its half width.
func show_ribbon(points: PackedVector3Array, half_width: float, ok: bool) -> void:
	_mesh.clear_surfaces()
	if points.size() >= 2:
		_band(points, half_width, _ok_mat if ok else _bad_mat)


## The road as it will be laid. plan: the core's plan (points, half_width, asphalt_half). line: "" for an unpainted road,
## else "white" or "yellow". px: metres a screen pixel covers at each end.
func show_road(plan: Dictionary, line: String, ok: bool, px_start: float, px_end: float) -> void:
	_mesh.clear_surfaces()
	var points: PackedVector3Array = plan.points
	if points.size() < 2:
		return
	var verge: float = plan.half_width + ThemeBuilder.size("preview_casing_m")
	_band(points, verge + ThemeBuilder.size("preview_edge_m"), _edge_ok if ok else _edge_bad)
	_band(points, verge, _bands[0])
	_band(points, plan.half_width, _bands[1])
	_band(points, plan.asphalt_half + ThemeBuilder.size("preview_kerb_m"), _bands[2])
	_band(points, plan.asphalt_half, _bands[3])
	if line != "":
		_dashes(points, _paint_yellow if line == "yellow" else _paint)
	var ring := _ring_ok if ok else _ring_bad
	_ring(points[0], px_start, ring)
	_ring(points[-1], px_end, ring)


## The roads a tool is moving: [{points, half_width}], each in the tool's colour or in its own `token` (data/ui/tokens.json).
func show_ribbons(roads: Array) -> void:
	_mesh.clear_surfaces()
	for r in roads:
		_band(r.points, r.half_width, _shade(r.token) if r.has("token") else _ok_mat)


## The material of a colour token, made once.
func _shade(token: String) -> StandardMaterial3D:
	if not _shades.has(token):
		_shades[token] = _material(token)
	return _shades[token]


## A node a press would take.
func show_mark(centre: Vector3, px: float) -> void:
	_mesh.clear_surfaces()
	_ring(centre, px, _ring_ok)


func _band(points: PackedVector3Array, half: float, material: StandardMaterial3D) -> void:
	_mesh.surface_begin(Mesh.PRIMITIVE_TRIANGLE_STRIP, material)
	var lift := Vector3.UP * ThemeBuilder.size("preview_lift")
	for i in points.size():
		var left := _left(points, i) * half
		_mesh.surface_add_vertex(points[i] + left + lift)
		_mesh.surface_add_vertex(points[i] - left + lift)
	_mesh.surface_end()


## The unit vector to the road's left at point `i`, from its two neighbours.
func _left(points: PackedVector3Array, i: int) -> Vector3:
	var a := points[maxi(i - 1, 0)]
	var b := points[mini(i + 1, points.size() - 1)]
	var d := Vector3(b.x - a.x, 0, b.z - a.z).normalized()
	return Vector3(d.z, 0, -d.x)


## The broken line down the road's middle, walked along the points.
func _dashes(points: PackedVector3Array, material: StandardMaterial3D) -> void:
	var dash := ThemeBuilder.size("preview_dash_m")
	var period := dash + ThemeBuilder.size("preview_gap_m")
	var half := ThemeBuilder.size("preview_line_m") * 0.5
	var lift := Vector3.UP * ThemeBuilder.size("preview_lift")
	_mesh.surface_begin(Mesh.PRIMITIVE_TRIANGLES, material)
	var run := 0.0
	for i in points.size() - 1:
		var a := points[i]
		var b := points[i + 1]
		var length := a.distance_to(b)
		var at := 0.0
		while at < length:
			var phase := fmod(run + at, period)
			var step := minf((dash if phase < dash else period) - phase, length - at)
			if phase < dash:
				var left := _left(points, i) * half
				var p := a.lerp(b, at / length) + lift
				var q := a.lerp(b, (at + step) / length) + lift
				for v in [p + left, q + left, q - left, p + left, q - left, p - left]:
					_mesh.surface_add_vertex(v)
			at += step
		run += length
	_mesh.surface_end()


## A ring of preview_ring_px round `centre`, its size held on the screen by `px`.
func _ring(centre: Vector3, px: float, material: StandardMaterial3D) -> void:
	var outer := ThemeBuilder.size("preview_ring_px") * px
	var inner := outer - ThemeBuilder.size("preview_ring_w_px") * px
	var lift := Vector3.UP * ThemeBuilder.size("preview_lift")
	_mesh.surface_begin(Mesh.PRIMITIVE_TRIANGLE_STRIP, material)
	for i in DISC_SEGMENTS + 1:
		var a := TAU * i / DISC_SEGMENTS
		var r := Vector3(cos(a), 0, sin(a))
		_mesh.surface_add_vertex(centre + lift + r * outer)
		_mesh.surface_add_vertex(centre + lift + r * inner)
	_mesh.surface_end()


func show_disc(centre: Vector3, radius: float, ok: bool) -> void:
	_mesh.clear_surfaces()
	_mesh.surface_begin(Mesh.PRIMITIVE_TRIANGLES, _ok_mat if ok else _bad_mat)
	var lift := Vector3.UP * ThemeBuilder.size("preview_lift")
	for i in DISC_SEGMENTS:
		var a := TAU * i / DISC_SEGMENTS
		var b := TAU * (i + 1) / DISC_SEGMENTS
		_mesh.surface_add_vertex(centre + lift)
		_mesh.surface_add_vertex(centre + lift + Vector3(cos(a), 0, sin(a)) * radius)
		_mesh.surface_add_vertex(centre + lift + Vector3(cos(b), 0, sin(b)) * radius)
	_mesh.surface_end()
