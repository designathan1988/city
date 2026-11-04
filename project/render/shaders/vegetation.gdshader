// Tree bodies: trunk, limbs and the crown heart under the leaf cards (native/src/render/trees/tree_kit.h), flat faces
// each in its own tone, bent by the wind and faded against the far cards (vegetation.gdshaderinc).
shader_type spatial;
render_mode skip_vertex_transform, cull_back;

#include "res://render/shaders/vegetation.gdshaderinc"

void vertex() {
	float fade;
	tree_vertex(VERTEX, NORMAL, UV, UV2, COLOR.a, INSTANCE_CUSTOM, MODEL_MATRIX, VIEW_MATRIX, MAIN_CAM_INV_VIEW_MATRIX, TIME, fade);
	v_tint = INSTANCE_CUSTOM.rgb;
	v_fade = fade;
	v_part = UV2.x;
}

void fragment() {
	if (v_fade < 1.0 && dither(FRAGCOORD.xy) > v_fade) {
		discard;
	}
	ALBEDO = vertex_albedo(COLOR.rgb, OUTPUT_IS_SRGB) * v_tint;
	ROUGHNESS = roughness;
}
