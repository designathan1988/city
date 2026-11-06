// The cast (native/src/render/agents/people_renderer.cpp): the cooked bodies skinned with the clip palette (3 RGBA texels per bone and
// frame = rows of the 3x4 bind -> pose matrix, as 16-bit fixed point in RGBA8 texel pairs), coloured by their atlas. The four-bone blend is
// people_skin.gdshaderinc. INSTANCE_CUSTOM = (body, walk frames, a seed, walk phase 0..1). The instance colour's alpha (COLOR.a in
// vertex()) = the share of the walk in the pose, the rest the idle: a walker slows into standing and out of it (Pedestrian::rest); from
// CLIP_CODE_BASE on, the clip of that number alone: somebody in a vehicle. UV2 = (region: 0 cloth, 1 skin, 2 hair, 3 eye; roughness).
shader_type spatial;
render_mode cull_disabled; // the hair's cards and the clothes' open edges are seen from both sides

#include "res://render/shaders/people_skin.gdshaderinc"

uniform sampler2DArray atlas : source_color, filter_linear_mipmap_anisotropic, repeat_disable;
uniform float alpha_test; // the texels of the hair's cards that are drawn
uniform sampler2D palette : filter_nearest, repeat_disable;
uniform int block_rows; // a variation's rows of the palette; its walk first, its idle from idle_row
uniform int idle_row;
uniform int idle_frames;
uniform float idle_seconds[32]; // by variation (people_renderer.cpp IDLE_TABLE)
uniform float idle_clock; // the simulation's seconds, for the idles
uniform int clip_row[16]; // people_renderer.cpp CLIP_TABLE: each clip's first row in a variation's block
uniform int clip_frames[16];

const int TEXELS = 3;
const float BYTE = 255.0;
const float BODY_SPAN = 2.0; // people_asset.cpp PALETTE_SPAN
const float IDLE_SPREAD = 0.61803; // each person in another place of the idle (the golden ratio spreads whole numbers evenly)
const vec3 INSIDE = vec3(0.09, 0.045, 0.045); // skin seen from inside (the cheeks through an open mouth) is the body's dark
const float CLIP_CODE_BASE = 2.0; // people_renderer.cpp: an alpha from here on is a clip, held at the phase in INSTANCE_CUSTOM.a

// One palette row value: 16-bit fixed point over [-BODY_SPAN, BODY_SPAN], high bytes in texel 2x, low bytes in 2x + 1.
vec4 palette_row(int texel, int row) {
	vec4 hi = texelFetch(palette, ivec2(texel * 2, row), 0);
	vec4 lo = texelFetch(palette, ivec2(texel * 2 + 1, row), 0);
	return (hi * 256.0 + lo) * BYTE / 65535.0 * 2.0 * BODY_SPAN - BODY_SPAN;
}

BonePose walk_pose(int bone, int row) {
	return BonePose(palette_row(bone * TEXELS, row), palette_row(bone * TEXELS + 1, row), palette_row(bone * TEXELS + 2, row));
}

// Adds the pose of a clip (its first row, frames and phase 0..1) at `share`: the linear blend of the four bones.
void add_clip(int first, int frames, float phase, float share, ivec4 bones, vec4 weights, vec3 vertex, vec3 normal, inout vec3 pos, inout vec3 nor) {
	float f = fract(phase) * float(frames);
	int f0 = int(floor(f));
	int row0 = first + f0;
	int row1 = first + (f0 + 1 < frames ? f0 + 1 : 0);
	float t = fract(f);
	for (int i = 0; i < 4; i++) {
		skin_add(pos, nor, share * weights[i], walk_pose(bones[i], row0), walk_pose(bones[i], row1), t, vertex, normal);
	}
}

varying flat float v_layer;
varying flat float v_region;
varying float v_rough;

void vertex() {
	float seed = floor(INSTANCE_CUSTOM.b + 0.5);
	int variation = int(INSTANCE_CUSTOM.r + 0.5);
	int block = variation * block_rows;
	ivec4 bones = skin_bones(BONE_INDICES);
	float walking = COLOR.a;
	vec3 pos = vec3(0.0), nor = vec3(0.0);
	if (walking >= CLIP_CODE_BASE - 0.5) {
		int clip = int(walking - CLIP_CODE_BASE + 0.5);
		add_clip(block + clip_row[clip], clip_frames[clip], INSTANCE_CUSTOM.a, 1.0, bones, BONE_WEIGHTS, VERTEX, NORMAL, pos, nor);
		walking = 1.0;
	} else if (walking > 0.0) {
		add_clip(block, int(INSTANCE_CUSTOM.g + 0.5), INSTANCE_CUSTOM.a, walking, bones, BONE_WEIGHTS, VERTEX, NORMAL, pos, nor);
	}
	if (walking < 1.0) {
		float phase = idle_clock / idle_seconds[variation] + seed * IDLE_SPREAD;
		add_clip(block + idle_row, idle_frames, phase, 1.0 - walking, bones, BONE_WEIGHTS, VERTEX, NORMAL, pos, nor);
	}
	VERTEX = pos;
	NORMAL = normalize(nor);
	v_layer = INSTANCE_CUSTOM.r;
	v_region = UV2.x;
	v_rough = UV2.y;
}

void fragment() {
	vec4 texel = texture(atlas, vec3(UV, v_layer));
	vec3 c = texel.rgb * COLOR.rgb;
	if (!FRONT_FACING && v_region > 0.5 && v_region < 1.5) c *= INSIDE;
	ALBEDO = c;
	ALPHA = texel.a;
	ALPHA_SCISSOR_THRESHOLD = alpha_test;
	ROUGHNESS = v_rough;
}
