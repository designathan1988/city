// The cast (native/src/render/agents/people_renderer.cpp): the cooked bodies skinned with the clip palette (3 RGBA texels per bone and
// frame = rows of the 3x4 bind -> pose matrix, as 16-bit fixed point in RGBA8 texel pairs), coloured by their atlas. The four-bone blend is
// people_skin.gdshaderinc, the pose people_pose.gdshaderinc. UV2 = (region: 0 cloth, 1 skin, 2 hair, 3 eye; roughness).
shader_type spatial;
render_mode cull_disabled; // the hair's cards and the clothes' open edges are seen from both sides

#include "res://render/shaders/people_skin.gdshaderinc"
#include "res://render/shaders/people_pose.gdshaderinc"

uniform sampler2DArray atlas : source_color, filter_linear_mipmap_anisotropic, repeat_disable;
uniform float alpha_test; // the texels of the hair's cards that are drawn

const vec3 INSIDE = vec3(0.09, 0.045, 0.045); // skin seen from inside (the cheeks through an open mouth) is the body's dark

varying flat float v_layer;
varying flat float v_region;
varying float v_rough;

void vertex() {
	vec3 pos, nor;
	person_pose(INSTANCE_CUSTOM, COLOR.a, skin_bones(BONE_INDICES), BONE_WEIGHTS, VERTEX, NORMAL, pos, nor);
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
