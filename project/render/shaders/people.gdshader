// Pedestrians (native/src/render/agents/people_renderer.cpp, Roadcraft render/people crowd.ts + looks.ts): the cooked
// MakeHuman bodies skinned with the clip palette (3 RGBA float texels per bone and frame = rows of the 3x4 bind -> pose
// matrix, as 16-bit fixed point in RGBA8 texel pairs). Colour, look codes and the four-bone blend are people_color.gdshaderinc
// and people_skin.gdshaderinc, shared with the shader of the body that falls; what is here is only where the clips' pose comes from.
// INSTANCE_CUSTOM (half floats in Compatibility, small exact integers) = (variation, walk frames + 64 * top,
// bottom + 17 * hair + 256 * skin, walk phase 0..1); see people_color.gdshaderinc for the look codes. The instance colour's
// alpha (COLOR.a in vertex()) = the share of the walk in the pose, the rest the idle: a walker slows into standing and out of
// it (Pedestrian::rest).
shader_type spatial;

#include "res://render/shaders/people_color.gdshaderinc"
#include "res://render/shaders/people_skin.gdshaderinc"

uniform sampler2D palette : filter_nearest, repeat_disable;
uniform int block_rows; // a variation's rows of the palette; its walk first, its idle from idle_row
uniform int idle_row;
uniform int idle_frames;
uniform float idle_seconds[32]; // by variation (people_renderer.cpp IDLE_TABLE)
uniform float idle_clock; // the simulation's seconds, for the idles

const int TEXELS = 3;
const float BYTE = 255.0;
const float BODY_SPAN = 2.0; // people_asset.cpp PALETTE_SPAN
const float IDLE_SPREAD = 0.61803; // each person in another place of the idle (the golden ratio spreads whole numbers evenly)

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

void vertex() {
	float code = INSTANCE_CUSTOM.g + 0.5;
	float looks = INSTANCE_CUSTOM.b + 0.5;
	int variation = int(INSTANCE_CUSTOM.r + 0.5);
	int block = variation * block_rows;
	ivec4 bones = skin_bones(BONE_INDICES);
	float walking = COLOR.a;
	vec3 pos = vec3(0.0), nor = vec3(0.0);
	if (walking > 0.0) {
		add_clip(block, int(floor(mod(code, 64.0))), INSTANCE_CUSTOM.a, walking, bones, BONE_WEIGHTS, VERTEX, NORMAL, pos, nor);
	}
	if (walking < 1.0) {
		float phase = idle_clock / idle_seconds[variation] + floor(looks) * IDLE_SPREAD;
		add_clip(block + idle_row, idle_frames, phase, 1.0 - walking, bones, BONE_WEIGHTS, VERTEX, NORMAL, pos, nor);
	}
	VERTEX = pos;
	NORMAL = normalize(nor);
	PERSON_LOOK(INSTANCE_CUSTOM.g, INSTANCE_CUSTOM.b, UV2)
}

void fragment() {
	ALBEDO = person_albedo(COLOR.rgb);
	ROUGHNESS = roughness_body;
}
