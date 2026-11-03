## Tool previews drawn over the world (Roadcraft render/preview.ts): the road ribbon along the planned curve with the
## road's half width, green when it can be built and red when refused, and the zone brush disc. Colours are the
## preview tokens of data/ui/tokens.json; the ribbon ignores depth so buildings never hide it.
class_name PreviewLine
extends MeshInstance3D

const DISC_SEGMENTS := 48

var _mesh := ImmediateMesh.new()
var _ok_mat: StandardMaterial3D
var _bad_mat: StandardMaterial3D


func _ready() -> void:
	mesh = _mesh
	cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	_ok_mat = _material("preview_ok")
	_bad_mat = _material("preview_bad")


func _material(token: String) -> StandardMaterial3D:
	var m := StandardMaterial3D.new()
	m.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	m.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
	m.no_depth_test = true
	m.cull_mode = BaseMaterial3D.CULL_DISABLED
	m.albedo_color = ThemeBuilder.color(token)
	return m


func clear() -> void:
	_mesh.clear_surfaces()


## Points along the road on the ground (lifted by preview_lift here) and its half width.
func show_ribbon(points: PackedVector3Array, half_width: float, ok: bool) -> void:
	_mesh.clear_surfaces()
	if points.size() < 2:
		return
	_mesh.surface_begin(Mesh.PRIMITIVE_TRIANGLE_STRIP, _ok_mat if ok else _bad_mat)
	var lift := Vector3.UP * ThemeBuilder.size("preview_lift")
	for i in points.size():
		var a := points[maxi(i - 1, 0)]
		var b := points[mini(i + 1, points.size() - 1)]
		var d := Vector3(b.x - a.x, 0, b.z - a.z).normalized()
		var left := Vector3(d.z, 0, -d.x) * half_width
		_mesh.surface_add_vertex(points[i] + left + lift)
		_mesh.surface_add_vertex(points[i] - left + lift)
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
