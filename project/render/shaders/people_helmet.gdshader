// A motorcyclist's helmet (Roadcraft riderPoses.helmetShape): a shell fitted to each body's head at the cook, posed by the head's bone of the same clip
// and phase as the rider it is on. Instances as the body's (people_pose.gdshaderinc).
shader_type spatial;
#include "res://render/shaders/contracts/people.gdshaderinc"

#include "res://render/shaders/people_skin.gdshaderinc"
#include "res://render/shaders/people_pose.gdshaderinc"

uniform int head_bone;
uniform vec4 helmet_rows[PEOPLE_BODIES * PEOPLE_HAND_ROWS]; // by variation, three rows each: the shell at rest
uniform vec3 helmet_colours[HELMET_COLOURS];
uniform int colour_count = 1;
uniform float helmet_roughness; // materials.json people_helmet

varying vec3 v_colour;

void vertex() {
	int at = int(INSTANCE_CUSTOM.r + 0.5) * PEOPLE_HAND_ROWS;
	vec4 x = helmet_rows[at], y = helmet_rows[at + 1], z = helmet_rows[at + 2];
	vec4 here = vec4(VERTEX, 1.0);
	mat3 shape = mat3(vec3(x.x, y.x, z.x), vec3(x.y, y.y, z.y), vec3(x.z, y.z, z.z));
	vec3 pos, nor;
	person_pose(INSTANCE_CUSTOM, ivec4(head_bone), vec4(1.0, 0.0, 0.0, 0.0), vec3(dot(x, here), dot(y, here), dot(z, here)),
			normalize(transpose(inverse(shape)) * NORMAL), pos, nor);
	VERTEX = pos;
	NORMAL = normalize(nor);
	v_colour = helmet_colours[int(abs(INSTANCE_CUSTOM.b) + 0.5) % colour_count];
}

void fragment() {
	ALBEDO = v_colour;
	ROUGHNESS = helmet_roughness;
}
