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
	// the look's own glow as the cooked model gave it, as the screen shows it; the rooms in the scene's light
	vec3 room = windows || painted_windows ? room_light(UV, painted_windows ? c.a : 1.0, mat3(INV_VIEW_MATRIX) * -VIEW, mat3(INV_VIEW_MATRIX) * NORMAL) : vec3(0.0);
	EMISSION = emission_shown(emission) + emission_light(room);
	if (alpha_cut > 0.0) {
		ALPHA = c.a;
		ALPHA_SCISSOR_THRESHOLD = alpha_cut;
	}
}
