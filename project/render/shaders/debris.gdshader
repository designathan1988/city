// The pieces a blast throws (wreck shells, planks, slabs, poles, splinters, glass, organs): lit, matte, each its own
// tint in INSTANCE_CUSTOM.rgb (sRGB) and the burnt share in .a (a burning wreck goes black as it burns).
shader_type spatial;
render_mode cull_disabled;
#include "res://render/shaders/contracts/effects.gdshaderinc"

uniform float roughness = 0.9;
uniform float burnt_tint = 0.12; // the share of its colour a wreck keeps when fully burnt
uniform vec3 dust : source_color = vec3(0.56, 0.54, 0.5);
uniform float dust_cover = 0.62; // what the concrete's dust takes of a piece's colour, on the mean

varying flat vec3 v_tint;
varying flat float v_dust;
varying flat float v_char;
varying vec3 v_local;

#include "res://render/shaders/lattice_noise.gdshaderinc"
#include "res://render/shaders/color_space.gdshaderinc"

void vertex() {
	v_dust = step(DEBRIS_DUSTY - 0.5, INSTANCE_CUSTOM.a); // a piece of a building
	v_char = INSTANCE_CUSTOM.a > DEBRIS_CHARRED - 0.5 && v_dust < 0.5 ? 1.0 : 0.0;
	v_tint = mix(INSTANCE_CUSTOM.rgb, INSTANCE_CUSTOM.rgb * burnt_tint, v_char);
	v_local = VERTEX;
}

void fragment() {
	// a broken piece is dust over what it was, chipped darker at its faces
	float patches = vnoise(v_local.xz * 7.0 + v_local.y * 5.0), grain = vnoise(v_local.xz * 31.0 + v_local.y * 23.0);
	vec3 c = mix(srgb_to_linear(v_tint), source_linear(dust), v_dust * dust_cover * smoothstep(0.2, 0.7, patches));
	c *= mix(1.0, mix(0.62, 1.0, grain), v_dust);
	c *= mix(1.0, mix(0.45, 1.9, patches * patches), v_char); // burnt paint: soot over bare metal
	ALBEDO = linear_out(c);
	ROUGHNESS = roughness;
	METALLIC = 0.05;
}
