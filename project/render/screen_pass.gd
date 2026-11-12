## A quad in clip space that multiplies the picture: drawn after what is lit and before the air of sky_view.gd. It needs the
## linear picture, which Compatibility does not have (its picture is already tone mapped in the material pass), so there it stays
## off, as Godot's SSAO does (docs: Environment and post-processing).
extends MeshInstance3D

const SCREEN_QUAD := Vector2(2, 2)


func lay(shader: String) -> ShaderMaterial:
	var material := ShaderMaterial.new()
	material.shader = load(shader)
	material.render_priority = RenderingServer.MATERIAL_RENDER_PRIORITY_MAX - 1
	var quad := QuadMesh.new()
	quad.size = SCREEN_QUAD
	quad.flip_faces = true
	mesh = quad
	material_override = material
	cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	visible = false
	return material


func _ready() -> void:
	RenderingServer.instance_set_ignore_culling(get_instance(), true)


## Whether the renderer in use has the linear picture the pass multiplies.
func available() -> bool:
	return RenderMethod.draws(&"linear_picture")
