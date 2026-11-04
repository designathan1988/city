// What a cooked person looks like, shared by every shader that draws one (people.gdshader walking, the ragdoll's when the body
// falls): the look codes, the varyings that carry them and the colour of each region. A person keeps the same clothes, hair and
// skin from the street to the ground.
// INSTANCE_CUSTOM g / b (small exact integers, half floats in Compatibility): g = frames + 64 * top, b = bottom + 17 * hair +
// 256 * skin; top / bottom 0 = texture colour, k + 1 = clothes k; hair 0..7 hairs, 8..11 greys; skin 0..7 steps.
// UV2 = (region, tint detail): regions 0 skin, 1 hair, 2 top, 3 bottom, 6 eyes.

uniform vec4 clothes[16];
uniform vec4 hairs[8];
uniform vec4 greys[4];
uniform float tint_mix;
uniform float skin_base;
uniform float skin_step;
uniform float roughness_body;
uniform vec3 eye_color : source_color;

const float GAMMA = 2.2;

varying flat float v_region;
varying float v_detail;
varying flat vec4 v_look; // top, bottom, hair, skin

// The look of an instance (top, bottom, hair, skin) from g and b of its INSTANCE_CUSTOM.
vec4 person_look_code(float custom_g, float custom_b) {
	float code = custom_g + 0.5;
	float looks = custom_b + 0.5;
	return vec4(floor(code / 64.0), floor(mod(mod(looks, 256.0), 17.0)), floor(mod(looks, 256.0) / 17.0), floor(looks / 256.0));
}

// Vertex stage: hand the instance's look and the vertex's region and detail (UV2) to the fragment. A macro because Godot lets only
// vertex() assign a varying, not a function it calls.
#define PERSON_LOOK(custom_g, custom_b, uv2) v_look = person_look_code(custom_g, custom_b); v_region = (uv2).x; v_detail = (uv2).y;

vec3 lin(vec4 c) {
	return pow(c.rgb, vec3(GAMMA));
}

// Fragment stage: the albedo of a pixel whose texture colour is `base`.
vec3 person_albedo(vec3 base) {
	int region = int(v_region + 0.5);
	int top = int(v_look.x + 0.5), bottom = int(v_look.y + 0.5), hair = int(v_look.z + 0.5);
	vec3 c = base;
	if (region == 2 && top > 0) {
		c = mix(c, lin(clothes[top - 1]) * clamp(v_detail, 0.25, 2.2), tint_mix);
	} else if (region == 3 && bottom > 0) {
		c = mix(c, lin(clothes[bottom - 1]) * clamp(v_detail, 0.25, 2.2), tint_mix);
	} else if (region == 1) {
		c = (hair >= 8 ? lin(greys[hair - 8]) : lin(hairs[hair])) * clamp(v_detail, 0.45, 1.6);
	} else if (region == 6) {
		c = eye_color;
	} else if (region == 0) {
		c *= skin_base + skin_step * v_look.w;
	}
	return c;
}
