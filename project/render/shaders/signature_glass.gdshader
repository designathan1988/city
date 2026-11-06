// The see-through parts of a signature building: glass rails, and the pictures laid over a wall (signature_bodies.cpp).
shader_type spatial;
render_mode blend_mix, depth_draw_never, cull_disabled;

#include "res://render/shaders/signature_look.gdshaderinc"

void fragment() {
	vec4 c = look_colour(UV);
	ALBEDO = c.rgb;
	ALPHA = c.a;
	ROUGHNESS = roughness;
	METALLIC = metallic;
	EMISSION = emission;
}
