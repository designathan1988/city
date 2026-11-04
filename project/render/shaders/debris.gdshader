// The pieces a blast throws (wreck shells, planks, slabs, poles, splinters, glass, organs): lit, matte, each its own
// tint in INSTANCE_CUSTOM.rgb (sRGB) and the burnt share in .a (a burning wreck goes black as it burns).
shader_type spatial;
render_mode cull_disabled;

uniform float roughness = 0.9;
uniform float burnt_tint = 0.12; // the share of its colour a wreck keeps when fully burnt

varying flat vec3 v_tint;

#include "res://render/shaders/color_space.gdshaderinc"

void vertex() {
	v_tint = mix(INSTANCE_CUSTOM.rgb, INSTANCE_CUSTOM.rgb * burnt_tint, INSTANCE_CUSTOM.a);
}

void fragment() {
	ALBEDO = srgb_to_linear(v_tint);
	ROUGHNESS = roughness;
	METALLIC = 0.05;
}
