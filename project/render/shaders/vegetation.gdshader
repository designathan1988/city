// Tree bodies: trunk, limbs and the crown heart under the leaf cards (native/src/render/trees/tree_kit.h), flat faces
// each in its own tone, bent by the wind and faded against the far cards (vegetation.gdshaderinc).
shader_type spatial;
render_mode skip_vertex_transform, cull_back;
#include "res://render/shaders/contracts/trees.gdshaderinc"

#include "res://render/shaders/vegetation.gdshaderinc"
#include "res://render/shaders/lattice_noise.gdshaderinc"

uniform float bark_ridges; // furrows round a trunk (materials.json vegetation, as the rest)
uniform float bark_run_m; // how long a furrow runs
uniform float bark_deep; // the light left at the bottom of a furrow
uniform float bark_far_m; // past this a furrow is under a pixel
uniform vec4 bark_grain; // the furrows' coarse noise's share, the fine one's; a furrow's edges in the noise

// The furrows of the bark, long and narrow, broken by a second finer layer: what is left of the light at this point.
float bark(vec3 p, float dist) {
	float round_it = atan(p.x, p.z) / TAU * bark_ridges;
	float n = vnoise(vec2(round_it, p.y / bark_run_m)) * bark_grain.x + vnoise(vec2(round_it * 3.0, p.y / bark_run_m * 3.0)) * bark_grain.y;
	float furrow = mix(bark_deep, 1.0, smoothstep(bark_grain.z, bark_grain.w, n));
	return mix(furrow, (1.0 + bark_deep) * 0.5, smoothstep(bark_far_m * 0.5, bark_far_m, dist));
}

void fragment() {
	ALBEDO = vertex_albedo(COLOR.rgb, OUTPUT_IS_SRGB) * v_tint;
	if (v_part > PART_BARK - 0.5) {
		ALBEDO *= bark(v_model, length(VERTEX));
	}
	ROUGHNESS = roughness;
	SPECULAR = v_part > PART_BARK - 0.5 ? leaf_specular : leaf_sheen(NORMAL, VIEW);
}
