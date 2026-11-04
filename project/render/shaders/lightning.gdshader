// A bolt's channel: each segment a ribbon of a width in pixels, turned to the screen (VERTEX one end, NORMAL the other
// end, UV.x the side, UV.y the width), brighter than white so it passes the tone curve and feeds the glow at night.
shader_type spatial;
render_mode unshaded, fog_disabled, cull_disabled, shadows_disabled, depth_draw_never, blend_add;

uniform float brightness = 0.0;
uniform vec2 screen = vec2(1600.0, 900.0);
uniform vec3 core = vec3(6.0, 6.4, 8.0);

#include "res://render/shaders/color_space.gdshaderinc"

varying float v_opacity;

void vertex() {
	vec4 a = PROJECTION_MATRIX * VIEW_MATRIX * vec4(VERTEX, 1.0);
	vec4 b = PROJECTION_MATRIX * VIEW_MATRIX * vec4(NORMAL, 1.0);
	vec2 along = normalize((b.xy / b.w - a.xy / a.w) * screen);
	vec2 side = vec2(-along.y, along.x) / screen * 2.0;
	a.xy += side * UV.x * UV.y * a.w * 0.5;
	POSITION = a;
	v_opacity = COLOR.a;
}

void fragment() {
	ALBEDO = linear_out(core * brightness * v_opacity);
}
