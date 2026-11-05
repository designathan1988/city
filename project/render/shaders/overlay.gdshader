// Map layers (zone colours, congestion): flat vertex colours over the ground and roads, unshaded, see-through.
shader_type spatial;
render_mode unshaded, blend_mix, cull_disabled, depth_draw_never, shadows_disabled;

uniform float opacity = 0.6;

void fragment() {
	ALBEDO = COLOR.rgb;
	ALPHA = COLOR.a * opacity;
}
