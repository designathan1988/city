// Rain: streaks in a box hung over the ground under the view, as render/weather_view.gd lays it. A drop's place is its
// hash in the box plus the fall so far (the lattice), wrapped round the box, never velocity times time: the box may
// move and grow without the drops jumping.
shader_type spatial;
render_mode unshaded, fog_disabled, cull_disabled, shadows_disabled, depth_draw_never, blend_add;

uniform vec3 centre;
uniform vec3 box = vec3(1.0);
uniform vec3 lattice;
uniform vec3 velocity = vec3(0.0, -1.0, 0.0);
uniform float streak; // render/weather_view.gd sets these from data/game/sky.json, the near reach and opacity as it rains
uniform float near_m;
uniform float opacity;
uniform int drops = 0;
uniform vec3 colour : source_color; // sky.json rain_colour
uniform vec4 rain_fade; // the box fades out across from this share of its half, up from this one; a streak's tip keeps 1 less this; the near fade's reach, of near_m

#include "res://render/shaders/color_space.gdshaderinc"

varying float v_fade;

void vertex() {
	float tip = float(VERTEX_ID % 2);
	vec3 p = centre - box * 0.5 + mod(VERTEX * box + lattice, box);
	p -= velocity * streak * tip;
	vec3 q = abs(p - centre) / (box * 0.5);
	v_fade = (1.0 - smoothstep(rain_fade.x, 1.0, max(q.x, q.z))) * (1.0 - smoothstep(rain_fade.y, 1.0, q.y)) * (1.0 - tip * rain_fade.z);
	v_fade *= smoothstep(near_m, near_m * rain_fade.w, distance(p, INV_VIEW_MATRIX[3].xyz));
	POSITION = VERTEX_ID / 2 < drops ? PROJECTION_MATRIX * VIEW_MATRIX * vec4(p, 1.0) : vec4(2.0, 2.0, 2.0, 1.0);
}

void fragment() {
	ALBEDO = linear_out(source_linear(colour) * opacity * v_fade);
}
