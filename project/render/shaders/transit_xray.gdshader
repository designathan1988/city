// The metro in its cut while the transport tool is on: drawn through the ground, one material per layer so they stack in order
// (materials.json transit_xray, its layers).
shader_type spatial;
render_mode blend_mix, depth_draw_never, depth_test_disabled;

#include "res://render/shaders/color_space.gdshaderinc"

void fragment() {
	ALBEDO = linear_out(srgb_to_linear(COLOR.rgb));
	ALPHA = COLOR.a;
	ROUGHNESS = UV.x;
	METALLIC = UV.y;
}
