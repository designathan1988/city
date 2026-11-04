// Tree bodies: trunk, limbs and the crown heart under the leaf cards (native/src/render/trees/tree_kit.h), flat faces
// each in its own tone, bent by the wind and faded against the far cards (vegetation.gdshaderinc).
shader_type spatial;
render_mode skip_vertex_transform, cull_back;

#include "res://render/shaders/vegetation.gdshaderinc"

void fragment() {
	if (v_fade < 1.0 && dither(FRAGCOORD.xy) > v_fade) {
		discard;
	}
	ALBEDO = vertex_albedo(COLOR.rgb, OUTPUT_IS_SRGB) * v_tint;
	ROUGHNESS = roughness;
}
