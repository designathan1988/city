// The pieces a blast throws (wreck shells, planks, slabs, poles, splinters, glass, organs): lit, matte, each its own
// tint in INSTANCE_CUSTOM.rgb (sRGB) and the burnt share in .a (a burning wreck goes black as it burns).
shader_type spatial;
render_mode cull_disabled;

uniform float roughness = 0.9;
uniform float burnt_tint = 0.12; // the share of its colour a wreck keeps when fully burnt
uniform vec3 dust : source_color = vec3(0.56, 0.54, 0.5);
uniform float dust_cover = 0.62; // what the concrete's dust takes of a piece's colour, on the mean

varying flat vec3 v_tint;
varying vec3 v_local;

#include "res://render/shaders/lattice_noise.gdshaderinc"
#include "res://render/shaders/color_space.gdshaderinc"

void vertex() {
	v_tint = mix(INSTANCE_CUSTOM.rgb, INSTANCE_CUSTOM.rgb * burnt_tint, INSTANCE_CUSTOM.a);
	v_local = VERTEX;
}

void fragment() {
	// a broken piece is dust over what it was, chipped darker at its faces
	float patches = vnoise(v_local.xz * 7.0 + v_local.y * 5.0), grain = vnoise(v_local.xz * 31.0 + v_local.y * 23.0);
	vec3 c = mix(srgb_to_linear(v_tint), source_linear(dust), dust_cover * smoothstep(0.2, 0.7, patches));
	ALBEDO = linear_out(c * mix(0.62, 1.0, grain));
	ROUGHNESS = roughness;
	METALLIC = 0.05;
}
