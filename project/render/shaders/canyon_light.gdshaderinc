// How much of the sky a point of the town sees past the buildings round it (a horizon search over the height window of
// render/atmosphere/canyon_field.h, 8 directions and 3 steps), for the atmosphere pass: the bottom of a street is darker than its
// roofs. The sky's share of a pixel's light is canyon_sky. Needs depth_texture.

uniform sampler2D canyon_map : repeat_disable, filter_nearest;
uniform vec3 canyon_window = vec3(0.0, 0.0, 1.0); // low corner x, z and side
uniform float canyon_sky = 0.0;

const float CANYON_CELL_M = 4.0;
const float CANYON_LEAST_RISE = 0.15; // a wall sees only the sky in front of it
const float CANYON_LIFT_FROM_M = 0.4; // under a roof's own height by this much, a level pixel starts to lose the sky
const float CANYON_LIFT_TO_M = 1.5;
const float CANYON_UNDER = 0.35; // what a level pixel keeps under a solid

vec2 canyon_at(vec2 p) {
	vec2 uv = (p - canyon_window.xy) / canyon_window.z;
	if (uv.x < 0.0 || uv.y < 0.0 || uv.x >= 1.0 || uv.y >= 1.0) return vec2(-9000.0);
	return textureLod(canyon_map, uv, 0.0).rg;
}

vec3 canyon_world(vec2 uv, mat4 inv_projection, mat4 inv_view) {
	float depth = textureLod(depth_texture, uv, 0.0).x;
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
	vec3 ndc = vec3(uv, depth) * 2.0 - 1.0;
#else
	vec3 ndc = vec3(uv * 2.0 - 1.0, depth);
#endif
	vec4 view = inv_projection * vec4(ndc, 1.0);
	return (inv_view * vec4(view.xyz / view.w, 1.0)).xyz;
}

// The surface's normal from the depth, each axis from the side that lies on the same surface as the pixel.
vec3 canyon_normal(vec3 p, vec3 eye, vec2 uv, vec2 texel, mat4 inv_projection, mat4 inv_view) {
	vec3 l = canyon_world(uv - vec2(texel.x, 0.0), inv_projection, inv_view), r = canyon_world(uv + vec2(texel.x, 0.0), inv_projection, inv_view);
	vec3 b = canyon_world(uv - vec2(0.0, texel.y), inv_projection, inv_view), t = canyon_world(uv + vec2(0.0, texel.y), inv_projection, inv_view);
	vec3 dx = distance(l, p) < distance(r, p) ? p - l : r - p;
	vec3 dy = distance(b, p) < distance(t, p) ? p - b : t - p;
	vec3 n = normalize(cross(dx, dy));
	return dot(n, eye - p) < 0.0 ? -n : n;
}

// What is left of the pixel's light at world point `p`.
float canyon_keep(vec3 p, vec3 eye, vec2 uv, vec2 texel, mat4 inv_projection, mat4 inv_view) {
	if (canyon_at(p.xz).y < p.y) return 1.0; // nothing stands over it within reach
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
			rise = max(rise, (canyon_at(p.xz + d * reach).x - p.y) / reach);
		}
		sum += 1.0 / (1.0 + rise * rise);
		count += 1.0;
	}
	float vis = count > 0.0 ? sum / count : 1.0;
	if (!wall) vis *= mix(1.0, CANYON_UNDER, smoothstep(CANYON_LIFT_FROM_M, CANYON_LIFT_TO_M, canyon_at(p.xz).x - p.y));
	// Light bounced between the walls gives some back (Jimenez et al. 2016, albedo 0.5).
	vis = max(vis, ((vis * 0.6878 - 1.7559) * vis + 2.0679) * vis);
	return mix(1.0, vis, canyon_sky);
}
