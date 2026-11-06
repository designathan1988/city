## A road just laid lights up on the ground and fades (Roadcraft renderer.ts flashRoads, blinkMaterial): over whatever stands on it, added to the picture, once.
class_name RoadFlash
extends MeshInstance3D

const RISE_S := 0.1
const FADE_S := 0.75

var _mesh := ImmediateMesh.new()
var _material := StandardMaterial3D.new()
var _peak := 0.0
var _born := -1.0


func _ready() -> void:
	mesh = _mesh
	cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	_material.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	_material.blend_mode = BaseMaterial3D.BLEND_MODE_ADD
	_material.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
	_material.no_depth_test = true
	_material.cull_mode = BaseMaterial3D.CULL_DISABLED
	_material.albedo_color = ThemeBuilder.color("road_flash")
	_peak = _material.albedo_color.a
	set_process(false)


## Points along the road and its half width, footways and all.
func flash(points: PackedVector3Array, half_width: float) -> void:
	_mesh.clear_surfaces()
	if points.size() < 2:
		return
	_mesh.surface_begin(Mesh.PRIMITIVE_TRIANGLE_STRIP, _material)
	var lift := Vector3.UP * ThemeBuilder.size("preview_lift")
	for i in points.size():
		var along := points[mini(i + 1, points.size() - 1)] - points[maxi(i - 1, 0)]
		var left := Vector3(along.z, 0.0, -along.x).normalized() * half_width
		_mesh.surface_add_vertex(points[i] + left + lift)
		_mesh.surface_add_vertex(points[i] - left + lift)
	_mesh.surface_end()
	_born = 0.0
	_alpha(0.0)
	show()
	set_process(true)


func _alpha(share: float) -> void:
	_material.albedo_color.a = _peak * share


func _process(delta: float) -> void:
	_born += delta
	_alpha(smoothstep(0.0, RISE_S, _born) * (1.0 - smoothstep(RISE_S, FADE_S, _born)))
	if _born > FADE_S:
		hide()
		set_process(false)
