// How much of the sky a point of the town sees past the buildings round it (a horizon search over the height window of
// render/atmosphere/canyon_field.h, 8 directions and 3 steps), for the atmosphere pass: the bottom of a street is darker than its
// roofs. The sky's share of a pixel's light is canyon_sky. Needs depth_texture.
#include "res://render/shaders/contracts/land.gdshaderinc"

uniform sampler2D canyon_map : repeat_disable, filter_nearest;
uniform vec3 canyon_window = vec3(0.0, 0.0, 1.0); // low corner x, z and side
uniform float canyon_sky = 0.0;

const float CANYON_LEAST_RISE = 0.15; // a wall sees only the sky in front of it
const float CANYON_LIFT_FROM_M = 0.4; // under a roof's own height by this much, a level pixel starts to lose the sky
const float CANYON_LIFT_TO_M = 1.5;
const float CANYON_UNDER = 0.35; // what a level pixel keeps under a solid

vec2 canyon_at(vec2 p) {
	vec2 uv = (p - canyon_window.xy) / canyon_window.z;
	if (uv.x < 0.0 || uv.y < 0.0 || uv.x >= 1.0 || uv.y >= 1.0) return vec2(-9000.0);
	return textureLod(canyon_map, uv, 0.0).rg;
}

// The surface's normal from the depth, each axis from the side that lies on the same surface as the pixel.
vec3 canyon_normal(vec3 p, vec3 eye, vec2 uv, vec2 texel, mat4 inv_projection, mat4 inv_view) {
	vec3 c = view_at(uv, inv_projection);
	vec3 l = view_at(uv - vec2(texel.x, 0.0), inv_projection), r = view_at(uv + vec2(texel.x, 0.0), inv_projection);
	vec3 b = view_at(uv - vec2(0.0, texel.y), inv_projection), t = view_at(uv + vec2(0.0, texel.y), inv_projection);
	vec3 dx = distance(l, c) < distance(r, c) ? c - l : r - c;
	vec3 dy = distance(b, c) < distance(t, c) ? c - b : t - c;
	vec3 n = mat3(inv_view) * normalize(cross(dx, dy));
	return dot(n, eye - p) < 0.0 ? -n : n;
}

// What is left of the pixel's light at world point `p`.
float canyon_keep(vec3 p, vec3 eye, vec2 uv, vec2 texel, mat4 inv_projection, mat4 inv_view) {
	vec2 here = canyon_at(p.xz);
	if (here.y < p.y) return 1.0; // nothing stands over it within reach
	float tallest = here.y - p.y;
	vec3 n = canyon_normal(p, eye, uv, texel, inv_projection, inv_view);
	bool wall = abs(n.y) < 0.6;
	vec2 facing = wall ? normalize(n.xz) : n.xz;
	float sum = 0.0, count = 0.0;
	for (int k = 0; k < 8; k++) {
		float a = (float(k) + 0.5) * 0.7853982;
		vec2 d = vec2(cos(a), sin(a));
		if (wall && dot(d, facing) < CANYON_LEAST_RISE) continue;
		float rise = 0.0;
		for (int s = 0; s < 3; s++) {
			float reach = CANYON_CELL_M * (0.9 + float(s * s) * 2.4);
			if (tallest <= rise * reach) break; // the tallest thing in reach would not raise it, here or further on
			rise = max(rise, (canyon_at(p.xz + d * reach).x - p.y) / reach);
		}
		sum += 1.0 / (1.0 + rise * rise);
		count += 1.0;
	}
	float vis = count > 0.0 ? sum / count : 1.0;
	if (!wall) vis *= mix(1.0, CANYON_UNDER, smoothstep(CANYON_LIFT_FROM_M, CANYON_LIFT_TO_M, here.x - p.y));
	// Light bounced between the walls gives some back (Jimenez et al. 2016, albedo 0.5).
	vis = max(vis, ((vis * 0.6878 - 1.7559) * vis + 2.0679) * vis);
	return mix(1.0, vis, canyon_sky);
}
