## The outline of the block the Build tools have picked: its plan drawn at its foot and at its top, and the corners between them.
class_name SelectionBox
extends MeshInstance3D

var _lines := ImmediateMesh.new()


func _init() -> void:
	mesh = _lines
	var m := StandardMaterial3D.new()
	m.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	m.albedo_color = ThemeBuilder.color("accent")
	m.no_depth_test = true
	material_override = m
	cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF


func clear() -> void:
	_lines.clear_surfaces()


## `ring`: the foot's corners, then the top's, in the same order (CityStore query "body_outline").
func show_block(ring: PackedVector3Array) -> void:
	clear()
	var n := ring.size() / 2
	if n < 3:
		return
	_lines.surface_begin(Mesh.PRIMITIVE_LINES)
	for i in n:
		var next := (i + 1) % n
		for pair: Array in [[i, next], [i + n, next + n], [i, i + n]]:
			_lines.surface_add_vertex(ring[pair[0]])
			_lines.surface_add_vertex(ring[pair[1]])
	_lines.surface_end()
