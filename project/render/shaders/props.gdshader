// Props, street furniture and trees. COLOR.rgb is sRGB; UV2.x tags each part (native/src/render/props/prop_kit.h
// PropTag): 0 fixed colour, 1 tint (INSTANCE_CUSTOM.rgb, trees and agents), 2 glow at night (lamp glass, windows). The
// signal lenses are signal_lens.gdshader.
shader_type spatial;

global uniform float night;
uniform vec3 glow_color : source_color;
uniform float glow_gain = 6.0;
uniform float roughness = 0.85;

varying flat float v_tag;
varying flat vec3 v_tint;

vec3 to_linear(vec3 c) {
	return mix(c / 12.92, pow((c + 0.055) / 1.055, vec3(2.4)), step(0.04045, c));
}

// COLOR as linear albedo: Compatibility (OUTPUT_IS_SRGB) hands it over already decoded (see road.gdshader).
vec3 vertex_albedo(vec3 c) {
	return OUTPUT_IS_SRGB ? c : to_linear(c);
}

void vertex() {
	v_tag = UV2.x;
	v_tint = INSTANCE_CUSTOM.rgb;
}

void fragment() {
	vec3 c = vertex_albedo(COLOR.rgb);
	if (v_tag > 0.5 && v_tag < 1.5) {
		c *= v_tint;
	}
	ALBEDO = c;
	ROUGHNESS = roughness;
	if (v_tag > 1.5) {
		EMISSION = glow_color * c * night * glow_gain;
	}
}
