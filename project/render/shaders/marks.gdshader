// What a blast leaves on the ground, as flat quads just over it (one MultiMesh): craters (a hole, its broken lip,
// cracks in the paving), soot and ash, blood (a pool that spreads, a splat, drops), and the shock ring. Everything
// is worked out here from the instance: INSTANCE_CUSTOM = kind, seed, born (seconds of the effects clock), life (the ring's).
// Kinds: 0 crater in paving, 1 crater in earth, 2 soot, 3 blood pool, 4 blood splat, 5 blood drops, 6 shock ring.
shader_type spatial;
render_mode unshaded, blend_mix, depth_draw_never, cull_disabled, shadows_disabled;

global uniform float night;
// The crater's four stops, as scene radiance (what the Environment's tone mapping shows as the reference game's crater colours,
// by CityWorld from data/render/materials.json display_colors): index = 4 * (0 paving, 1 earth) + (0 centre, 1 mid, 2 rim, 3 lip).
uniform vec3 crater_rad[8];
uniform vec3 soot_color : source_color = vec3(0.078, 0.067, 0.055);
uniform vec3 blood_deep : source_color = vec3(0.376, 0.024, 0.035);
uniform vec3 blood_thin : source_color = vec3(0.52, 0.047, 0.063);
uniform vec3 ring_color = vec3(1.0, 0.945, 0.816);
uniform float night_dim = 0.4;
uniform float now = 0.0; // the effects clock, seconds
uniform float pool_spread_s = 6.0; // seconds a blood pool takes to reach its size
uniform float eye_pull_m = 0.5; // every mark is drawn this far nearer the eye along its ray (no change on screen): the ground's small bumps do not bury it

varying flat float v_kind;
varying flat float v_seed;
varying flat float v_spread;
varying flat float v_fade;

void vertex() {
	v_kind = INSTANCE_CUSTOM.x;
	v_seed = INSTANCE_CUSTOM.y;
	float age = now - INSTANCE_CUSTOM.z;
	v_spread = clamp(0.2 + age / max(pool_spread_s, 0.001), 0.2, 1.0);
	v_fade = 1.0;
	if (v_kind > 5.5) { // the shock ring races out and fades
		float k = clamp(age / max(INSTANCE_CUSTOM.w, 0.001), 0.0, 1.0);
		VERTEX.xz *= 1.0 - (1.0 - k) * (1.0 - k);
		v_fade = 1.0 - k;
	}
	vec3 world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	VERTEX += (inverse(MODEL_MATRIX) * vec4(normalize(CAMERA_POSITION_WORLD - world) * eye_pull_m, 0.0)).xyz;
}

float hash21(vec2 p) {
	p = fract(p * vec2(123.34, 456.21));
	p += dot(p, p + 45.32);
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
	float k = v_kind;
	vec3 c = vec3(0.0);
	float a = 0.0;
	float wob = 1.0 + 0.22 * (vnoise(q * 3.0 + v_seed * 17.0) - 0.5);
	float rn = r / wob;
	if (k < 1.5) { // crater: scorch spreading out, the hole, its lip, grit and (in paving) cracks
		bool hard = k < 0.5;
		int base = hard ? 0 : 4;
		vec3 lip_c = crater_rad[base + 3];
		float scorch = (1.0 - smoothstep(0.35, 1.0, rn)) * 0.55 * (0.6 + 0.4 * vnoise(q * 6.0 + v_seed));
		float hole = 1.0 - smoothstep(0.5, 0.56, rn);
		float lip = smoothstep(0.42, 0.52, rn) * (1.0 - smoothstep(0.56, 0.62, rn));
		float tt = clamp(rn / 0.58, 0.0, 1.0); // the hole's gradient: centre, then mid at 0.7 of the way, then the rim
		vec3 inner = tt < 0.7 ? mix(crater_rad[base], crater_rad[base + 1], tt / 0.7) : mix(crater_rad[base + 1], crater_rad[base + 2], (tt - 0.7) / 0.3);
		float grit = step(0.72, vnoise(q * 22.0 + v_seed * 5.0)) * smoothstep(1.0, 0.3, rn) * 0.7;
		c = soot_color;
		a = scorch;
		c = mix(c, inner, hole);
		a = max(a, hole);
		c = mix(c, lip_c, lip * 0.9);
		a = max(a, lip * 0.9);
		c = mix(c, lip_c * 1.1, grit);
		a = max(a, grit * 0.8);
		if (hard) { // cracks running out through the paving
			float ang = atan(q.y, q.x);
			float crack = smoothstep(0.06, 0.0, abs(sin(ang * 5.5 + v_seed * 9.0 + 2.0 * vnoise(vec2(ang * 3.0, v_seed)))) * rn - 0.02) * smoothstep(0.5, 0.58, rn) * (1.0 - smoothstep(0.78, 1.0, rn));
			c = mix(c, vec3(0.012), crack);
			a = max(a, crack * 0.85);
		}
	} else if (k < 2.5) { // soot: soft dark blots
		float n = vnoise(q * 4.0 + v_seed * 11.0);
		a = (1.0 - smoothstep(0.1, 1.0, rn)) * (0.25 + 0.4 * n);
		c = soot_color;
	} else if (k < 3.5) { // a pool that spreads
		float s = max(v_spread, 0.05);
		float blob = 1.0 - smoothstep(0.6, 0.72, rn / s);
		c = mix(blood_deep, blood_thin, smoothstep(0.3, 0.7, rn / s));
		a = blob * 0.97;
	} else if (k < 4.5) { // a splat: a core and drops thrown out of it
		float core = 1.0 - smoothstep(0.2, 0.3, rn);
		float drops = step(0.78, vnoise(q * 14.0 + v_seed * 7.0)) * (1.0 - smoothstep(0.35, 0.95, rn));
		a = max(core, drops) * 0.95;
		c = blood_deep;
	} else if (k < 5.5) { // a few drops
		float drops = step(0.7, vnoise(q * 9.0 + v_seed * 3.0)) * (1.0 - smoothstep(0.1, 0.9, rn));
		a = drops * 0.95;
		c = blood_deep;
	} else { // the shock ring: a bright thin ring racing out
		float w = smoothstep(0.86, 0.93, r) * (1.0 - smoothstep(0.97, 1.0, r));
		a = w * 0.55 * v_fade;
		c = ring_color;
	}
	c *= k > 5.5 ? 1.0 : mix(1.0, night_dim, clamp(night, 0.0, 1.0));
	ALBEDO = c;
	ALPHA = clamp(a * (k > 5.5 ? 1.0 : v_fade), 0.0, 1.0);
}
