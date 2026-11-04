// Leaf cards, palm fronds and far cards (native/src/render/trees/tree_kit.h): the grey leaf atlas (render/trees/
// leaf_atlas.h) cut at half cover, tinted by each card's tone and the tree's tint, drawn from both
// sides. As the atlas' mips average leaves into gaps, the cover is raised with the mip level so a crown keeps its fill
// far away (Golus, "Anti-aliased Alpha Test: The Esoteric Alpha To Coverage", 2017).
shader_type spatial;
render_mode skip_vertex_transform, cull_disabled;

#include "res://render/shaders/vegetation.gdshaderinc"

const float LEAF_U = 0.0, FROND_U = 0.5, REGION_W = 0.5; // leaf_atlas.h LEAF_REGION_U, FROND_REGION_U, ATLAS_REGION_W
const float CUT = 0.5;
uniform sampler2D leaf_atlas : source_color, filter_linear_mipmap_anisotropic, repeat_disable;
uniform float cover_per_mip = 0.25;
uniform sampler2D tree_albedo : source_color, filter_linear_mipmap, repeat_disable;
uniform sampler2D tree_normal : hint_normal, filter_linear_mipmap, repeat_disable;

#include "res://render/shaders/color_space.gdshaderinc"

void fragment() {
	if (v_fade < 1.0 && dither(FRAGCOORD.xy) > v_fade) {
		discard;
	}
	if (v_part > PART_FAR - 0.5) {
		vec2 at = (v_cell + vec2(UV.x, 1.0 - UV.y)) / vec2(float(picture_frames), float(picture_rows));
		vec4 p = texture(tree_albedo, at);
		vec2 texel = at * vec2(textureSize(tree_albedo, 0));
		float mip = max(0.0, 0.5 * log2(max(dot(dFdx(texel), dFdx(texel)), dot(dFdy(texel), dFdy(texel)))));
		ALPHA = p.a * (1.0 + mip * cover_per_mip);
		ALPHA_SCISSOR_THRESHOLD = CUT;
		ALBEDO = linear_out(source_linear(p.rgb) * v_tint);
		NORMAL = normalize(texture(tree_normal, at).xyz * 2.0 - 1.0);
		ROUGHNESS = roughness;
		SPECULAR = leaf_specular;
	} else {
	bool frond = v_part > PART_FROND - 0.5 && v_part < PART_FROND + 0.5;
	vec2 uv = vec2((frond ? FROND_U : LEAF_U) + clamp(UV.x, 0.0, 1.0) * REGION_W, UV.y);
	vec4 leaf = texture(leaf_atlas, uv);
	vec2 texel = uv * vec2(textureSize(leaf_atlas, 0));
	float mip = max(0.0, 0.5 * log2(max(dot(dFdx(texel), dFdx(texel)), dot(dFdy(texel), dFdy(texel)))));
	ALPHA = leaf.a * (1.0 + mip * cover_per_mip);
	ALPHA_SCISSOR_THRESHOLD = CUT;
	ALBEDO = vertex_albedo(COLOR.rgb, OUTPUT_IS_SRGB) * v_tint * leaf.r;
	ROUGHNESS = roughness;
	SPECULAR = leaf_specular;
	}
}
