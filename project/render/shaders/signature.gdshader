// The solid parts of a signature building, cut where the picture says (signature_bodies.cpp).
shader_type spatial;
render_mode cull_disabled;

#include "res://render/shaders/signature_look.gdshaderinc"

void fragment() {
	if (!two_sided && !FRONT_FACING) {
		discard;
	}
	vec4 c = look_colour(UV);
	ALBEDO = c.rgb;
	ROUGHNESS = roughness;
	METALLIC = metallic;
	EMISSION = windows || painted_windows ? emission + room_light(UV, painted_windows ? c.a : 1.0) : emission;
	if (alpha_cut > 0.0) {
		ALPHA = c.a;
		ALPHA_SCISSOR_THRESHOLD = alpha_cut;
	}
}
