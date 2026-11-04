// The light of a street lamp at night (Roadcraft scenery.ts pool and beam): a pool on the ground and a cone down from the
// lens, added to what is behind. One mesh for both (render/props/prop_kit.h light()): UV2.y 1 the pool, 2 the beam;
// INSTANCE_CUSTOM.a the parts an instance shows. Nothing is drawn by day. The cone is the lit air of rain or storm: `haze`
// (0 in clear weather, where its vertices collapse and it costs no fragment), drawn as Volumetric Light Beam draws one.
shader_type spatial;
render_mode blend_add, unshaded, cull_disabled, depth_draw_never, shadows_disabled;

global uniform float night;
uniform vec3 glow_color : source_color;
uniform float pool_gain = 0.32;
uniform float beam_gain = 0.4;
uniform float beam_softness = 1.6; // how hard the cone's edges are seen from the side: higher, softer
uniform float beam_fade_near_m = 60.0;
uniform float beam_fade_far_m = 140.0;
uniform float haze = 0.0;

const float HAZE_SHOWN = 0.02;
const float BEAM_PART = 1.5; // UV2.y over this: the beam

varying float v_part;
varying float v_along; // the pool: 0 centre, 1 rim; the beam: 0 lens, 1 ground
varying float v_fade;
varying vec3 v_normal_w;
varying vec3 v_to_eye;

#include "res://render/shaders/color_space.gdshaderinc"

void vertex() {
	v_part = UV2.y;
	v_along = v_part < BEAM_PART ? UV.x : 1.0 - UV.y;
	vec3 world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	vec3 s = vec3(length(MODEL_MATRIX[0].xyz), length(MODEL_MATRIX[1].xyz), length(MODEL_MATRIX[2].xyz));
	v_normal_w = mat3(MODEL_MATRIX) * (NORMAL / (s * s)); // a scaled cone's normal: through the inverse transpose
	v_to_eye = CAMERA_POSITION_WORLD - world;
	v_fade = 1.0 - smoothstep(beam_fade_near_m, beam_fade_far_m, length(v_to_eye));
	int shown = int(INSTANCE_CUSTOM.a + 0.5);
	if ((shown & int(v_part + 0.5)) == 0 || (v_part > BEAM_PART && haze <= HAZE_SHOWN)) {
		VERTEX = vec3(0.0);
	}
}

// The pool's fall from its centre (Roadcraft lightLevels.ts): a bright core and a long soft edge.
float pool_fall(float r) {
	if (r < 0.35) {
		return mix(1.0, 0.55, r / 0.35);
	}
	if (r < 0.7) {
		return mix(0.55, 0.15, (r - 0.35) / 0.35);
	}
	return mix(0.15, 0.0, clamp((r - 0.7) / 0.3, 0.0, 1.0));
}

void fragment() {
	float light;
	if (v_part < BEAM_PART) {
		light = pool_gain * pool_fall(v_along);
	} else {
		if (!FRONT_FACING) {
			discard;
		}
		float t = clamp(v_along, 0.0, 1.0);
		float fall = mix(1.0 - t, (1.0 - t) * (1.0 - t), 0.5);
		float facing = abs(dot(normalize(v_normal_w), normalize(v_to_eye)));
		light = beam_gain * haze * v_fade * fall * pow(facing, beam_softness);
	}
	ALBEDO = linear_out(source_linear(glow_color) * light * night);
	ALPHA = 1.0;
}
