// What a person wears (native/src/render/agents/worn_things.cpp): a backpack, an open umbrella. Its parts are laid in the body's axes from a
// joint's rest place on this body, scaled to its height (worn_at), and posed rigidly by the one bone the data gives the thing (worn_bone)
// of the same clip and phase as the body. Instances as the body's (people_pose.gdshaderinc). UV2 = (the thing's index, 1 on a part painted in
// the wearer's colour, picked from the thing's colours by the instance's seed).
shader_type spatial;
#include "res://render/shaders/contracts/people.gdshaderinc"

#include "res://render/shaders/people_skin.gdshaderinc"
#include "res://render/shaders/people_pose.gdshaderinc"

uniform int worn_bone[WORN_THINGS];
uniform vec4 worn_at[PEOPLE_BODIES * WORN_THINGS]; // by variation, then thing: xyz the joint's rest place, w the body's height over the drawn one
uniform vec3 worn_colours[WORN_THINGS * WORN_COLOURS];
uniform int worn_colour_count[WORN_THINGS];
uniform float worn_roughness;

varying vec3 v_colour;

void vertex() {
	int thing = int(UV2.x + 0.5);
	vec4 at = worn_at[int(INSTANCE_CUSTOM.r + 0.5) * WORN_THINGS + thing];
	vec3 pos, nor;
	person_pose(INSTANCE_CUSTOM, COLOR.a, ivec4(worn_bone[thing]), vec4(1.0, 0.0, 0.0, 0.0), at.xyz + VERTEX * at.w, NORMAL, pos, nor);
	VERTEX = pos;
	NORMAL = normalize(nor);
	int paint = int(abs(INSTANCE_CUSTOM.b) + 0.5) % max(worn_colour_count[thing], 1);
	v_colour = UV2.y > 0.5 ? worn_colours[thing * WORN_COLOURS + paint] : COLOR.rgb;
}

void fragment() {
	ALBEDO = v_colour;
	ROUGHNESS = worn_roughness;
}
