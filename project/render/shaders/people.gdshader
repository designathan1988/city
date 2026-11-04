// Pedestrians (native/src/render/agents/people_renderer.cpp, Roadcraft render/people crowd.ts + looks.ts): the cooked
// MakeHuman bodies skinned with the walk palette (3 RGBA float texels per bone and frame = rows of the 3x4 bind -> pose
// matrix, as 16-bit fixed point in RGBA8 texel pairs). Colour, look codes and the four-bone blend are people_color.gdshaderinc
// and people_skin.gdshaderinc, shared with the shader of the body that falls; what is here is only where the walk's pose comes from.
// INSTANCE_CUSTOM (half floats in Compatibility, small exact integers) = (first palette row, frames + 64 * top,
// bottom + 17 * hair + 256 * skin, walk phase 0..1); see people_color.gdshaderinc for the look codes.
shader_type spatial;

#include "res://render/shaders/people_color.gdshaderinc"
#include "res://render/shaders/people_skin.gdshaderinc"

uniform sampler2D palette : filter_nearest, repeat_disable;

const int TEXELS = 3;
const float BODY_SPAN = 2.0; // people_asset.cpp PALETTE_SPAN

// One palette row value: 16-bit fixed point over [-BODY_SPAN, BODY_SPAN].
vec4 palette_row(int texel, int row) {
	return fixed_point_row(palette, texel, row, BODY_SPAN);
}

BonePose walk_pose(int bone, int row) {
	return BonePose(palette_row(bone * TEXELS, row), palette_row(bone * TEXELS + 1, row), palette_row(bone * TEXELS + 2, row));
}

void vertex() {
	float frames = floor(mod(INSTANCE_CUSTOM.g + 0.5, 64.0));
	float f = fract(INSTANCE_CUSTOM.a) * frames;
	int f0 = int(floor(f));
	int row0 = int(INSTANCE_CUSTOM.r + 0.5) + f0;
	int row1 = int(INSTANCE_CUSTOM.r + 0.5) + (f0 + 1 < int(frames) ? f0 + 1 : 0);
	float t = fract(f);
	ivec4 bones = skin_bones(BONE_INDICES);
	vec3 pos = vec3(0.0), nor = vec3(0.0);
	for (int i = 0; i < 4; i++) {
		skin_add(pos, nor, BONE_WEIGHTS[i], walk_pose(bones[i], row0), walk_pose(bones[i], row1), t, VERTEX, NORMAL);
	}
	VERTEX = pos;
	NORMAL = normalize(nor);
	PERSON_LOOK(INSTANCE_CUSTOM.g, INSTANCE_CUSTOM.b, UV2)
}

void fragment() {
	ALBEDO = person_albedo(COLOR.rgb);
	ROUGHNESS = roughness_body;
}
