## The occlusion pass of render/shaders/occlusion.gdshader: a quad in clip space, drawn after what is lit and before the air
## of sky_view.gd. It multiplies the linear picture, which Compatibility does not have (its picture is already tone mapped in
## the material pass), so there it stays off, as Godot's SSAO does (docs: Environment and post-processing).
extends MeshInstance3D

const SCREEN_QUAD := Vector2(2, 2)
const WITH_LINEAR_PICTURE := "forward_plus"


func setup(table: Dictionary) -> void:
	var material := ShaderMaterial.new()
	material.shader = load("res://render/shaders/occlusion.gdshader")
	material.render_priority = RenderingServer.MATERIAL_RENDER_PRIORITY_MAX - 1
	for key in ["reach_m", "reach_curve", "thickness_m", "power", "strength"]:
		material.set_shader_parameter(key, table[key])
	var quad := QuadMesh.new()
	quad.size = SCREEN_QUAD
	quad.flip_faces = true
	mesh = quad
	material_override = material
	cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	visible = false


func _ready() -> void:
	RenderingServer.instance_set_ignore_culling(get_instance(), true)


func turn(on: bool) -> void:
	visible = on and RenderingServer.get_current_rendering_method() == WITH_LINEAR_PICTURE
