// Pedestrians (native/src/render/agents/people_renderer.cpp, Roadcraft render/people crowd.ts + looks.ts): the cooked
// MakeHuman bodies skinned with the clip palette (3 RGBA float texels per bone and frame = rows of the 3x4 bind -> pose
// matrix, as 16-bit fixed point in RGBA8 texel pairs). BONE_INDICES / BONE_WEIGHTS = 4 bones and their weights (sum 1; no
// skeleton is bound, they are plain data here). UV2 = (region, tint detail).
// INSTANCE_CUSTOM (half floats in Compatibility, small exact integers) = (variation, walk frames + 64 * top,
// bottom + 17 * hair + 256 * skin, walk phase 0..1); top / bottom 0 = texture colour, k + 1 = clothes k; hair 0..7
// hairs, 8..11 greys; skin 0..7 steps. The instance colour's alpha (COLOR.a in vertex()) = the share of the walk in the pose,
// the rest the idle: a walker slows into standing and out of it (Pedestrian::rest).
shader_type spatial;

uniform sampler2D palette : filter_nearest, repeat_disable;
uniform vec4 clothes[16];
uniform vec4 hairs[8];
uniform vec4 greys[4];
uniform int block_rows; // a variation's rows of the palette; its walk first, its idle from idle_row
uniform int idle_row;
uniform int idle_frames;
uniform float idle_seconds[32]; // by variation (people_renderer.cpp IDLE_TABLE)
uniform float idle_clock; // the simulation's seconds, for the idles
uniform float tint_mix;
uniform float skin_base;
uniform float skin_step;
uniform float roughness_body;
uniform vec3 eye_color : source_color;

const int TEXELS = 3;
const float BYTE = 255.0;
const float GAMMA = 2.2;
const int LAST_BONE = 22; // 23 bones (people.json "bones")
const float BODY_SPAN = 2.0; // people_asset.cpp PALETTE_SPAN
const float IDLE_SPREAD = 0.61803; // each person in another place of the idle (the golden ratio spreads whole numbers evenly)

varying flat float v_region;
varying float v_detail;
varying flat vec4 v_look; // top, bottom, hair, skin

// One palette row value: 16-bit fixed point over [-BODY_SPAN, BODY_SPAN], high bytes in texel 2x, low bytes in 2x + 1.
vec4 palette_row(int texel, int row) {
	vec4 hi = texelFetch(palette, ivec2(texel * 2, row), 0);
	vec4 lo = texelFetch(palette, ivec2(texel * 2 + 1, row), 0);
	return (hi * 256.0 + lo) * BYTE / 65535.0 * 2.0 * BODY_SPAN - BODY_SPAN;
}

vec3 bone_point(int bone, int row, vec4 p) {
	return vec3(dot(palette_row(bone * TEXELS, row), p), dot(palette_row(bone * TEXELS + 1, row), p), dot(palette_row(bone * TEXELS + 2, row), p));
}

// Adds the pose of a clip (its first row, frames and phase 0..1) at `share`: the linear blend of the four bones.
void add_clip(int first, int frames, float phase, float share, ivec4 bones, vec4 w, vec4 p, vec4 n, inout vec3 pos, inout vec3 nor) {
	float f = fract(phase) * float(frames);
	int f0 = int(floor(f));
	int row0 = first + f0;
	int row1 = first + (f0 + 1 < frames ? f0 + 1 : 0);
	float t = fract(f);
	for (int i = 0; i < 4; i++) {
		pos += share * w[i] * mix(bone_point(bones[i], row0, p), bone_point(bones[i], row1, p), t);
		nor += share * w[i] * mix(bone_point(bones[i], row0, n), bone_point(bones[i], row1, n), t);
	}
}

void vertex() {
	float code = INSTANCE_CUSTOM.g + 0.5;
	float looks = INSTANCE_CUSTOM.b + 0.5;
	v_look = vec4(floor(code / 64.0), floor(mod(mod(looks, 256.0), 17.0)), floor(mod(looks, 256.0) / 17.0), floor(looks / 256.0));
	int variation = int(INSTANCE_CUSTOM.r + 0.5);
	int block = variation * block_rows;
	// unused slots may name any byte: keep every fetch inside the palette (out-of-range reads can be NaN)
	ivec4 bones = min(ivec4(BONE_INDICES), ivec4(LAST_BONE));
	vec4 p = vec4(VERTEX, 1.0);
	vec4 n = vec4(NORMAL, 0.0);
	float walking = COLOR.a;
	vec3 pos = vec3(0.0), nor = vec3(0.0);
	if (walking > 0.0) {
		add_clip(block, int(floor(mod(code, 64.0))), INSTANCE_CUSTOM.a, walking, bones, BONE_WEIGHTS, p, n, pos, nor);
	}
	if (walking < 1.0) {
		float phase = idle_clock / idle_seconds[variation] + floor(looks) * IDLE_SPREAD;
		add_clip(block + idle_row, idle_frames, phase, 1.0 - walking, bones, BONE_WEIGHTS, p, n, pos, nor);
	}
	VERTEX = pos;
	NORMAL = normalize(nor);
	v_region = UV2.x;
	v_detail = UV2.y;
}

vec3 lin(vec4 c) {
	return pow(c.rgb, vec3(GAMMA));
}

void fragment() {
	int region = int(v_region + 0.5);
	int top = int(v_look.x + 0.5), bottom = int(v_look.y + 0.5), hair = int(v_look.z + 0.5);
	vec3 c = COLOR.rgb;
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
	ALBEDO = c;
	ROUGHNESS = roughness_body;
}
