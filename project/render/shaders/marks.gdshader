// What a blast leaves on the ground, as flat quads just over it (one MultiMesh): craters (a hole, its broken lip,
// cracks in the paving), soot and ash, blood (a pool that spreads, a splat, drops), and the shock ring. Everything
// is worked out here from the instance: INSTANCE_CUSTOM = kind, seed, born (seconds of the effects clock), life (the ring's).
// Kinds: 0 crater in paving, 1 crater in earth, 2 soot, 3 blood pool, 4 blood splat, 5 blood drops, 6 shock ring.
// Every share, edge and alpha is data/render/materials.json marks.params (by eye: the 4.0's marks); the radius r runs 0..1.
shader_type spatial;
render_mode unshaded, blend_mix, depth_draw_never, cull_disabled, shadows_disabled;
#include "res://render/shaders/contracts/effects.gdshaderinc"

// a fract-dot hash's constants (any numbers far from simple ratios) and the least time a share divides by
const float HASH_X = 123.34, HASH_Y = 456.21, HASH_ADD = 45.32;
const float LEAST_S = 0.001;

global uniform float night;
// The crater's four stops, as scene radiance (what the Environment's tone mapping shows as the reference game's crater colours,
// by CityWorld from data/render/materials.json display_colors): index = 4 * (0 paving, 1 earth) + (0 centre, 1 mid, 2 rim, 3 lip).
const int CRATER_STOPS = 8; // two grounds (paving, earth) of four stops
uniform vec3 crater_rad[CRATER_STOPS];
uniform vec3 blood_deep : source_color;
uniform vec3 blood_thin : source_color;
uniform vec3 soot_color : source_color; // sRGB: Godot makes a source_color linear
uniform vec3 ring_color; // linear
uniform float crack_grey; // linear
uniform float night_dim; // what a mark keeps of its colour at night
uniform float now = 0.0; // the effects clock, seconds (marks.cpp)
uniform float pool_spread_s; // seconds a blood pool takes to reach its size
uniform float pool_start; // the share of its size a pool starts at
uniform float eye_pull_m; // every mark is drawn this far nearer the eye along its ray (no change on screen): the ground's small bumps do not bury it
uniform vec3 wobble; // an edge's wobble: its amplitude, its noise's scale, the seed's scale
uniform vec4 scorch; // the crater's scorch: fades out from, its alpha, the noise's floor and share
uniform float scorch_scale; // the scorch noise's scale
uniform vec2 hole; // the hole fades out between these radii
uniform vec4 lip; // the lip rises between the first two radii and falls between the last two
uniform vec2 hole_stops; // the hole's gradient reaches the rim at this radius, its mid stop at this share of the way
uniform vec4 grit; // the grit: the noise's threshold, its scale, the seed's scale, where it fades in
uniform float grit_share;
uniform vec3 lip_look; // the lip's alpha, the grit's brightness over the lip, the grit's alpha
uniform vec4 crack_wave; // the cracks: their width, how many round, the seed's scale, the bend
uniform vec2 crack_bend; // the bend noise's scale, the cracks' offset
uniform vec4 crack_span; // they rise between two radii, fall from a radius; their alpha
uniform vec4 soot; // the soot: the noise's scale, the seed's scale, fades out from, the alpha's floor
uniform float soot_share; // the alpha's share by the noise
uniform vec4 pool_edge; // a pool's edge between two shares, its deep-to-thin blend between two shares
uniform vec2 pool_look; // its least spread, its alpha
uniform vec4 splat_core; // a splat: its core between two radii, the drops' fade between two radii
uniform vec4 splat_drops; // the drops' threshold, scale, seed scale; the splat's alpha
uniform vec3 drops_noise; // drops: the threshold, the scale, the seed's scale
uniform vec3 drops_fade; // their fade between two radii, the alpha
uniform vec4 ring_edges; // the shock ring rises between two radii, falls between two
uniform float ring_alpha;

varying flat float v_kind;
varying flat float v_seed;
varying flat float v_spread;
varying flat float v_fade;

void vertex() {
	v_kind = INSTANCE_CUSTOM.x;
	v_seed = INSTANCE_CUSTOM.y;
	float age = now - INSTANCE_CUSTOM.z;
	v_spread = clamp(pool_start + age / max(pool_spread_s, LEAST_S), pool_start, 1.0);
	v_fade = 1.0;
	if (int(v_kind + 0.5) == MARK_RING) { // the shock ring races out and fades
		float k = clamp(age / max(INSTANCE_CUSTOM.w, LEAST_S), 0.0, 1.0);
		VERTEX.xz *= 1.0 - (1.0 - k) * (1.0 - k);
		v_fade = 1.0 - k;
	}
	vec3 world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	VERTEX += (inverse(MODEL_MATRIX) * vec4(normalize(CAMERA_POSITION_WORLD - world) * eye_pull_m, 0.0)).xyz;
}

float hash21(vec2 p) {
	p = fract(p * vec2(HASH_X, HASH_Y));
	p += dot(p, p + HASH_ADD);
	return fract(p.x * p.y);
}

float vnoise(vec2 p) {
	vec2 i = floor(p), f = fract(p);
	f = f * f * (3.0 - 2.0 * f);
	return mix(mix(hash21(i), hash21(i + vec2(1.0, 0.0)), f.x), mix(hash21(i + vec2(0.0, 1.0)), hash21(i + vec2(1.0, 1.0)), f.x), f.y);
}

void fragment() {
	vec2 q = (UV - 0.5) * 2.0;
	float r = length(q);
	int k = int(v_kind + 0.5);
	vec3 c = vec3(0.0);
	float a = 0.0;
	float wob = 1.0 + wobble.x * (vnoise(q * wobble.y + v_seed * wobble.z) - 0.5);
	float rn = r / wob;
	if (k == MARK_CRATER_HARD || k == MARK_CRATER_EARTH) { // crater: scorch spreading out, the hole, its lip, grit and (in paving) cracks
		bool hard = k == MARK_CRATER_HARD;
		int base = hard ? 0 : 4;
		vec3 lip_c = crater_rad[base + 3];
		float scorch_a = (1.0 - smoothstep(scorch.x, 1.0, rn)) * scorch.y * (scorch.z + scorch.w * vnoise(q * scorch_scale + v_seed));
		float hole_a = 1.0 - smoothstep(hole.x, hole.y, rn);
		float lip_a = smoothstep(lip.x, lip.y, rn) * (1.0 - smoothstep(lip.z, lip.w, rn));
		float tt = clamp(rn / hole_stops.x, 0.0, 1.0); // the hole's gradient: centre, then mid, then the rim
		float mid = hole_stops.y;
		vec3 inner = tt < mid ? mix(crater_rad[base], crater_rad[base + 1], tt / mid) : mix(crater_rad[base + 1], crater_rad[base + 2], (tt - mid) / (1.0 - mid));
		float grit_a = step(grit.x, vnoise(q * grit.y + v_seed * grit.z)) * smoothstep(1.0, grit.w, rn) * grit_share;
		c = soot_color;
		a = scorch_a;
		c = mix(c, inner, hole_a);
		a = max(a, hole_a);
		c = mix(c, lip_c, lip_a * lip_look.x);
		a = max(a, lip_a * lip_look.x);
		c = mix(c, lip_c * lip_look.y, grit_a);
		a = max(a, grit_a * lip_look.z);
		if (hard) { // cracks running out through the paving
			float ang = atan(q.y, q.x);
			float crack = smoothstep(crack_wave.x, 0.0, abs(sin(ang * crack_wave.y + v_seed * crack_wave.z + crack_wave.w * vnoise(vec2(ang * crack_bend.x, v_seed)))) * rn - crack_bend.y) * smoothstep(crack_span.x, crack_span.y, rn) * (1.0 - smoothstep(crack_span.z, 1.0, rn));
			c = mix(c, vec3(crack_grey), crack);
			a = max(a, crack * crack_span.w);
		}
	} else if (k == MARK_SOOT) { // soot: soft dark blots
		float n = vnoise(q * soot.x + v_seed * soot.y);
		a = (1.0 - smoothstep(soot.z, 1.0, rn)) * (soot.w + soot_share * n);
		c = soot_color;
	} else if (k == MARK_POOL) { // a pool that spreads
		float s = max(v_spread, pool_look.x);
		float blob = 1.0 - smoothstep(pool_edge.x, pool_edge.y, rn / s);
		c = mix(blood_deep, blood_thin, smoothstep(pool_edge.z, pool_edge.w, rn / s));
		a = blob * pool_look.y;
	} else if (k == MARK_SPLAT) { // a splat: a core and drops thrown out of it
		float core = 1.0 - smoothstep(splat_core.x, splat_core.y, rn);
		float thrown = step(splat_drops.x, vnoise(q * splat_drops.y + v_seed * splat_drops.z)) * (1.0 - smoothstep(splat_core.z, splat_core.w, rn));
		a = max(core, thrown) * splat_drops.w;
		c = blood_deep;
	} else if (k == MARK_DROPS) { // a few drops
		float d = step(drops_noise.x, vnoise(q * drops_noise.y + v_seed * drops_noise.z)) * (1.0 - smoothstep(drops_fade.x, drops_fade.y, rn));
		a = d * drops_fade.z;
		c = blood_deep;
	} else { // the shock ring: a bright thin ring racing out
		float w = smoothstep(ring_edges.x, ring_edges.y, r) * (1.0 - smoothstep(ring_edges.z, ring_edges.w, r));
		a = w * ring_alpha * v_fade;
		c = ring_color;
	}
	c *= k == MARK_RING ? 1.0 : mix(1.0, night_dim, clamp(night, 0.0, 1.0));
	ALBEDO = c;
	ALPHA = clamp(a * (k == MARK_RING ? 1.0 : v_fade), 0.0, 1.0);
}
