// The glass of the shelters, the station halls and the metro's ways down: see through by its vertex alpha.
shader_type spatial;
render_mode blend_mix, depth_draw_never;

#include "res://render/shaders/color_space.gdshaderinc"

void fragment() {
	ALBEDO = linear_out(srgb_to_linear(COLOR.rgb));
	ALPHA = COLOR.a;
	ROUGHNESS = UV.x;
	METALLIC = UV.y;
}
