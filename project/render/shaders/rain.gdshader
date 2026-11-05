// Rain: streaks in a box hung over the ground under the view, as render/weather_view.gd lays it. A drop's place is its
// hash in the box plus the fall so far (the lattice), wrapped round the box, never velocity times time: the box may
// move and grow without the drops jumping.
shader_type spatial;
render_mode unshaded, fog_disabled, cull_disabled, shadows_disabled, depth_draw_never, blend_add;

uniform vec3 centre;
uniform vec3 box = vec3(1.0);
uniform vec3 lattice;
uniform vec3 velocity = vec3(0.0, -1.0, 0.0);
uniform float streak = 0.022;
uniform float near_m = 10.0;
uniform float opacity = 0.3;
uniform int drops = 0;
uniform vec3 colour : source_color = vec3(0.62, 0.68, 0.76);

#include "res://render/shaders/color_space.gdshaderinc"

varying float v_fade;

void vertex() {
	float tip = float(VERTEX_ID % 2);
	vec3 p = centre - box * 0.5 + mod(VERTEX * box + lattice, box);
	p -= velocity * streak * tip;
	vec3 q = abs(p - centre) / (box * 0.5);
	v_fade = (1.0 - smoothstep(0.6, 1.0, max(q.x, q.z))) * (1.0 - smoothstep(0.7, 1.0, q.y)) * (1.0 - tip * 0.7);
	v_fade *= smoothstep(near_m, near_m * 2.5, distance(p, INV_VIEW_MATRIX[3].xyz));
	POSITION = VERTEX_ID / 2 < drops ? PROJECTION_MATRIX * VIEW_MATRIX * vec4(p, 1.0) : vec4(2.0, 2.0, 2.0, 1.0);
}

void fragment() {
	ALBEDO = linear_out(source_linear(colour) * opacity * v_fade);
}
