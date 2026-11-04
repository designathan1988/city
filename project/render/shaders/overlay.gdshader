// Map layers (zone colours, congestion): flat vertex colours over the ground and roads, unshaded, see-through.
shader_type spatial;
render_mode unshaded, blend_mix, cull_disabled, depth_draw_never, shadows_disabled;

#include "res://render/shaders/color_space.gdshaderinc"

void fragment() {
	ALBEDO = linear_out(srgb_to_linear(COLOR.rgb)); // the tokens are screen colours
	ALPHA = COLOR.a;
}
