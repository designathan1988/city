// What a person holds (native/src/render/agents/held_things.cpp): a thing with the palm's frame, posed by the same bone of the same clip as the hand, so it never
// leaves it. Instances as the body's (people_pose.gdshaderinc). UV2.x = 1: a thing that hangs or stands by its own weight, its vertices round the fist
// with the body's axes; otherwise in the palm's frame (X along the fingers, Y to the thumb, Z out of the back of the hand). It grows out of the fist as the
// act comes in, as a hand brings it out of a pocket.
shader_type spatial;

#include "res://render/shaders/people_skin.gdshaderinc"
#include "res://render/shaders/people_pose.gdshaderinc"

uniform int hand_bone;
uniform vec3 fist; // in the palm's frame
uniform vec4 palm_rows[96]; // by variation, three rows each: the palm at rest

const float ROUGH = 0.7;

void vertex() {
	int at = int(INSTANCE_CUSTOM.r + 0.5) * 3;
	vec4 x = palm_rows[at], y = palm_rows[at + 1], z = palm_rows[at + 2];
	float grown = act_share(INSTANCE_CUSTOM);
	bool upright = UV2.x > 0.5;
	vec4 here = vec4(upright ? fist : fist + (VERTEX - fist) * grown, 1.0);
	vec4 facing = vec4(NORMAL, 0.0);
	vec3 pos, nor;
	person_pose(INSTANCE_CUSTOM, COLOR.a, ivec4(hand_bone), vec4(1.0, 0.0, 0.0, 0.0), vec3(dot(x, here), dot(y, here), dot(z, here)),
			vec3(dot(x, facing), dot(y, facing), dot(z, facing)), pos, nor);
	VERTEX = upright ? pos + VERTEX * grown : pos;
	NORMAL = upright ? NORMAL : normalize(nor);
}

void fragment() {
	ALBEDO = COLOR.rgb;
	ROUGHNESS = ROUGH;
}
