// How much of the sky a point of the town sees past the buildings round it (a horizon search over the height window of
// render/atmosphere/canyon_field.h, 8 directions and 3 steps): a surface's ambient occlusion (AO), which the engine takes off the
// sky's light alone, so a street's floor and walls lose the sky and keep the sun, shadows and all (World AO, St-Amour 2013,
// Rendering Assassin's Creed III). render/sky_view.gd sets the globals; canyon_on is 0 while no field is laid.
#include "res://render/shaders/contracts/land.gdshaderinc"
const float CANYON_NONE = -9000.0; // a height read off the field: none there (under any ground)
const float CANYON_WALL = 0.6; // a normal leaning more than this off up is a wall's
const int CANYON_DIRECTIONS = 8; // the directions round a point it looks for what rises
const float CANYON_REACH_FIRST = 0.9, CANYON_REACH_GROWTH = 2.4; // its steps out, in cells: the first, then by the step squared
// The light bounced between the walls, Jimenez et al.'s multi-bounce fit ("Practical Realtime Strategies for Accurate Indirect
// Occlusion", 2016) for albedo 0.5: a = 2.0404 * 0.5 - 0.3324, b = 4.7951 * 0.5 - 0.6417, c = 2.7552 * 0.5 + 0.6903.
const float BOUNCE_A = 0.6878, BOUNCE_B = 1.7559, BOUNCE_C = 2.0679;

global uniform sampler2D canyon_map : filter_nearest, repeat_disable;
global uniform vec3 canyon_window; // low corner x, z and side
global uniform float canyon_on;

const float CANYON_LEAST_RISE = 0.15; // a wall sees only the sky in front of it
const float CANYON_LIFT_FROM_M = 0.4; // under a roof's own height by this much, a level surface starts to lose the sky
const float CANYON_LIFT_TO_M = 1.5;
const float CANYON_UNDER = 0.35; // what a level surface keeps under a solid
const int CANYON_STEPS = 3; // its steps out along each direction

vec2 canyon_at(vec2 p) {
	vec2 uv = (p - canyon_window.xy) / canyon_window.z;
	if (uv.x < 0.0 || uv.y < 0.0 || uv.x >= 1.0 || uv.y >= 1.0) return vec2(CANYON_NONE);
	return textureLod(canyon_map, uv, 0.0).rg;
}

// The share of the sky a surface at world point `p` facing `n` (world) sees.
float canyon_sky_seen(vec3 p, vec3 n) {
	if (canyon_on < 0.5) return 1.0;
	vec2 here = canyon_at(p.xz);
	if (here.y < p.y) return 1.0; // nothing stands over it within reach
	float tallest = here.y - p.y;
	bool wall = abs(n.y) < CANYON_WALL;
	vec2 facing = wall ? normalize(n.xz) : n.xz;
	float sum = 0.0, count = 0.0;
	for (int k = 0; k < CANYON_DIRECTIONS; k++) {
		float a = (float(k) + 0.5) * (TAU / float(CANYON_DIRECTIONS));
		vec2 d = vec2(cos(a), sin(a));
		if (wall && dot(d, facing) < CANYON_LEAST_RISE) continue;
		float rise = 0.0;
		for (int s = 0; s < CANYON_STEPS; s++) {
			float reach = CANYON_CELL_M * (CANYON_REACH_FIRST + float(s * s) * CANYON_REACH_GROWTH);
			if (tallest <= rise * reach) break; // the tallest thing in reach would not raise it, here or further on
			rise = max(rise, (canyon_at(p.xz + d * reach).x - p.y) / reach);
		}
		sum += 1.0 / (1.0 + rise * rise);
		count += 1.0;
	}
	float vis = count > 0.0 ? sum / count : 1.0;
	if (!wall) vis *= mix(1.0, CANYON_UNDER, smoothstep(CANYON_LIFT_FROM_M, CANYON_LIFT_TO_M, here.x - p.y));
	// Light bounced between the walls gives some back (Jimenez et al. 2016, albedo 0.5).
	return max(vis, ((vis * BOUNCE_A - BOUNCE_B) * vis + BOUNCE_C) * vis);
}
