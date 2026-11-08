// The pieces a blast throws (wreck shells, planks, slabs, poles, splinters, glass, organs): lit, matte, each its own
// tint in INSTANCE_CUSTOM.rgb (sRGB) and the burnt share in .a (a burning wreck goes black as it burns).
shader_type spatial;
render_mode cull_disabled;
#include "res://render/shaders/contracts/effects.gdshaderinc"

// The pieces' look: data/render/materials.json debris.params, by eye as the 4.0's debris; each vector's parts are said here.
uniform float roughness;
uniform float burnt_tint; // the share of its colour a wreck keeps when fully burnt
uniform vec3 dust : source_color;
uniform float dust_cover; // what the concrete's dust takes of a piece's colour, on the mean
uniform vec4 debris_noise; // the dust's patches across and up a piece, its grain across and up (a metre's share)
uniform vec2 dust_patches; // the dust lies between these edges of its patches' noise
uniform float chip_shade; // a dusty piece's chipped faces, darker down to this by the grain
uniform vec2 char_shade; // a burnt piece: soot to bare metal, from and to over its patches squared
uniform float debris_metal;

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
	float patches = vnoise(v_local.xz * debris_noise.x + v_local.y * debris_noise.y), grain = vnoise(v_local.xz * debris_noise.z + v_local.y * debris_noise.w);
	vec3 c = mix(srgb_to_linear(v_tint), source_linear(dust), v_dust * dust_cover * smoothstep(dust_patches.x, dust_patches.y, patches));
	c *= mix(1.0, mix(chip_shade, 1.0, grain), v_dust);
	c *= mix(1.0, mix(char_shade.x, char_shade.y, patches * patches), v_char); // burnt paint: soot over bare metal
	ALBEDO = linear_out(c);
	ROUGHNESS = roughness;
	METALLIC = debris_metal;
}
