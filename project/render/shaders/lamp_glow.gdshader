// The lit air under a street lamp (Roadcraft scenery.ts beam): a cone down from the lens, added to what is behind, an instance
// of render/props/prop_kit.h light(). It is the lit air of rain or storm: `haze` (0 in clear weather, where its vertices collapse
// and it costs no fragment), drawn as Volumetric Light Beam draws one. The light on the ground is render/props/lamp_lights.h.
shader_type spatial;
render_mode blend_add, unshaded, cull_disabled, depth_draw_never, shadows_disabled;

global uniform float night;
uniform vec3 glow_color : source_color;
uniform float beam_gain = 0.4;
uniform float beam_softness = 1.6; // how hard the cone's edges are seen from the side: higher, softer
uniform float beam_fade_near_m = 60.0;
uniform float beam_fade_far_m = 140.0;
uniform float haze = 0.0;

const float HAZE_SHOWN = 0.02;

varying float v_along; // the beam: 0 lens, 1 ground
varying float v_fade;
varying vec3 v_normal_w;
varying vec3 v_to_eye;

#include "res://render/shaders/color_space.gdshaderinc"

void vertex() {
	v_along = 1.0 - UV.y;
	vec3 world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	vec3 s = vec3(length(MODEL_MATRIX[0].xyz), length(MODEL_MATRIX[1].xyz), length(MODEL_MATRIX[2].xyz));
	v_normal_w = mat3(MODEL_MATRIX) * (NORMAL / (s * s)); // a scaled cone's normal: through the inverse transpose
	v_to_eye = CAMERA_POSITION_WORLD - world;
	v_fade = 1.0 - smoothstep(beam_fade_near_m, beam_fade_far_m, length(v_to_eye));
	if (night <= 0.0 || haze <= HAZE_SHOWN) {
		VERTEX = vec3(0.0);
	}
}

void fragment() {
	if (!FRONT_FACING) {
		discard;
	}
	float t = clamp(v_along, 0.0, 1.0);
	float fall = mix(1.0 - t, (1.0 - t) * (1.0 - t), 0.5);
	float facing = abs(dot(normalize(v_normal_w), normalize(v_to_eye)));
	float light = beam_gain * haze * v_fade * fall * pow(facing, beam_softness);
	ALBEDO = linear_out(source_linear(glow_color) * light * night);
	ALPHA = 1.0;
}
