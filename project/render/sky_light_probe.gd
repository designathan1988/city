## The sky's light on level ground, rendered into one texel whenever the hour or the weather moves (render/shaders/sky_level.gdshader:
## the air and weather of the sky itself), and handed to every shader as the global `sky_level_map`. What is lit by the sky outside
## the engine's own ambient reads it: the rooms seen through glass (their daylight factor of it) and the contact shade's share of
## the sun. One owner of the sky's level light, the shader: nothing on the CPU integrates the sky again.
class_name SkyLightProbe
extends SubViewport

const SHADER := "res://render/shaders/sky_level.gdshader"

var material: ShaderMaterial


func _init() -> void:
	size = Vector2i.ONE
	use_hdr_2d = true # the light in linear floats: a byte would round the dusk's light to nothing
	transparent_bg = false
	disable_3d = true
	render_target_update_mode = SubViewport.UPDATE_ONCE
	material = ShaderMaterial.new()
	material.shader = load(SHADER)
	var rect := ColorRect.new()
	rect.size = Vector2.ONE
	rect.material = material
	add_child(rect)


func _ready() -> void:
	RenderingServer.global_shader_parameter_set(&"sky_level_map", get_texture())


## The sky moved: render the texel again on the next frame.
func refresh() -> void:
	render_target_update_mode = SubViewport.UPDATE_ONCE
