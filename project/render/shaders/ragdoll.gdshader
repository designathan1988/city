// A person falling (native/src/render/effects/ragdoll_world.cpp): the cooked mesh of the walker, skinned with the matrices of its
// rigid bodies instead of a walk frame. Colour, look codes and the four-bone blend are people_color.gdshaderinc and
// people_skin.gdshaderinc, the walkers' own; what is here is where the pose comes from: a texture with one row per falling person and,
// per bone, the 3 rows of its 3x4 matrix (bind pose -> place, in metres from the instance's origin, which is the pelvis), as
// 16-bit fixed point over [-span_m, span_m]. INSTANCE_CUSTOM = (row, 64 * top, bottom + 17 * hair + 256 * skin, 1 when charred).
shader_type spatial;

#include "res://render/shaders/people_color.gdshaderinc"
#include "res://render/shaders/people_skin.gdshaderinc"

uniform sampler2D bones : filter_nearest, repeat_disable;
uniform float span_m;
uniform vec3 char_color : source_color;

const int ROWS_PER_BONE = 3;

varying flat float v_char;

BonePose body_pose(int bone, int row) {
	return BonePose(fixed_point_row(bones, bone * ROWS_PER_BONE, row, span_m), fixed_point_row(bones, bone * ROWS_PER_BONE + 1, row, span_m),
			fixed_point_row(bones, bone * ROWS_PER_BONE + 2, row, span_m));
}

void vertex() {
	int row = int(INSTANCE_CUSTOM.r + 0.5);
	ivec4 ids = skin_bones(BONE_INDICES);
	vec3 pos = vec3(0.0), nor = vec3(0.0);
	for (int i = 0; i < 4; i++) {
		BonePose pose = body_pose(ids[i], row);
		skin_add(pos, nor, BONE_WEIGHTS[i], pose, pose, 0.0, VERTEX, NORMAL);
	}
	VERTEX = pos;
	NORMAL = normalize(nor);
	v_char = INSTANCE_CUSTOM.a;
	PERSON_LOOK(INSTANCE_CUSTOM.g, INSTANCE_CUSTOM.b, UV2)
}

void fragment() {
	ALBEDO = mix(person_albedo(COLOR.rgb), char_color, v_char);
	ROUGHNESS = roughness_body;
}
