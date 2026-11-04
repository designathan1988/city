// Smoke, dust, flame, sparks, grit, blood spray, water and paper of a blast: one billboard each, no per-frame work on the
// CPU. The CPU writes a puff once (position and size in the instance transform, INSTANCE_CUSTOM = born, kind, seed, life);
// the motion, size, colour and fade are all worked out here from its age. Kinds: 0 exhaust 1 dust 2 dark smoke
// 3 concrete dust 4 blood 5 flame 6 spark 7 grit 8 hydrant water 9 paper. The motions are those of the puffs of the
// reference game, written in steps of STEP_M metres.
shader_type spatial;
render_mode unshaded, blend_mix, depth_draw_never, cull_disabled, shadows_disabled;

global uniform float night;
uniform float now = 0.0; // the effects clock, seconds (the same clock the puffs were born on)
uniform vec2 wind_dir = vec2(0.6, 0.8);
uniform vec3 smoke_color : source_color = vec3(0.55, 0.56, 0.58);
uniform vec3 dust_color : source_color = vec3(0.6, 0.53, 0.43);
uniform vec3 dark_color : source_color = vec3(0.16, 0.15, 0.15); // the reference game's dark smoke (exhaust.ts); the pale core of a blast was missing flame sources, not a lighter smoke
uniform vec3 concrete_color : source_color = vec3(0.74, 0.71, 0.66);
uniform vec3 blood_color : source_color = vec3(0.2, 0.008, 0.015);
uniform vec3 grit_color : source_color = vec3(0.3, 0.24, 0.18);
uniform vec3 water_color : source_color = vec3(0.82, 0.9, 0.97);
uniform vec3 spark_color = vec3(4.0, 2.6, 1.0);
uniform vec3 flame_hot = vec3(3.4, 2.6, 1.4);
uniform vec3 flame_warm = vec3(2.4, 0.9, 0.18);
uniform float night_dim = 0.35; // how dark smoke and dust read at night (they are not lit)

const float STEP_M = 0.4;

varying flat float v_kind;
varying flat float v_seed;
varying flat float v_age;
varying flat float v_alpha;

void vertex() {
	float born = INSTANCE_CUSTOM.x, kind = INSTANCE_CUSTOM.y, seed = INSTANCE_CUSTOM.z, life = max(INSTANCE_CUSTOM.w, 0.001);
	float t = now - born;
	float age = t / life;
	v_kind = kind;
	v_seed = seed;
	v_age = age;
	v_alpha = 0.0;
	bool dead = age < 0.0 || age > 1.0;
	float size = length(MODEL_MATRIX[0].xyz);
	vec3 p = vec3(0.0);
	float rise = kind < 0.5 ? 2.4 : kind < 1.5 ? 1.0 : kind < 2.5 ? 3.5 : 1.8;
	vec2 out2 = vec2(sin(seed * 40.0), cos(seed * 40.0));
	bool flung = (kind > 3.5 && kind < 4.5) || (kind > 5.5 && kind < 7.5);
	if (kind > 8.5) { // paper: blown up and out, then drifting down, swinging, carried by the wind
		float up = 9.0 * (1.0 - exp(-t * 2.0)) - 0.55 * t;
		p.y += up + 0.4 * sin(t * 2.3 + seed * 9.0);
		p.xz += out2 * (5.0 + 6.0 * fract(seed * 7.0)) * (1.0 - exp(-t * 1.5)) + wind_dir * 0.9 * t + vec2(sin(t * 1.7 + seed * 13.0), cos(t * 1.3 + seed * 5.0)) * 0.8;
	} else if (kind > 7.5) { // a hydrant jet: straight up, fanning, falling back
		p.y += (30.0 + 6.0 * fract(seed * 13.0)) * clamp(size / 0.8, 0.15, 1.0) * t - 9.8 * t * t;
		p.xz += out2 * (0.6 + 1.6 * fract(seed * 7.0)) * t + wind_dir * 0.8 * t;
	} else if (flung) { // blood, sparks, grit: flung up and out, falling under gravity
		float fling = kind > 5.5 && kind < 6.5 ? 3.0 : 1.0;
		p.y += (5.0 + 10.0 * fract(seed * 17.0)) * fling * t - 12.25 * t * t;
		p.xz += out2 * (3.0 + 8.0 * fract(seed * 7.0)) * fling * t;
	} else if (kind > 4.5) { // flame: bursting out, licking up fast
		p.y += 6.0 * t + 0.3 * sin(t * 9.0 + seed * 6.28);
		p.xz += out2 * 4.0 * t * (1.0 - age * 0.6) + wind_dir * 1.5 * t;
	} else {
		p.y += rise * t * (1.0 - 0.3 * age) + 0.15 * sin(t * 3.0 + seed * 6.28);
		float spread = kind > 2.5 ? 2.2 : kind > 0.5 ? 1.2 : 0.5;
		p.xz += wind_dir * (3.0 * t + 0.6 * t * t) + out2 * spread * t;
	}
	float grow = kind > 8.5 ? 0.0 : kind > 7.5 ? 1.2 : kind > 5.5 ? 0.2 : kind > 4.5 ? 1.6 : kind > 3.5 ? 0.6 : kind > 2.5 ? 3.2 : kind > 1.5 ? 2.6 : 2.0;
	float s = size * (0.5 + grow * age);
	vec3 world = MODEL_MATRIX[3].xyz + p * STEP_M;
	vec4 mv = VIEW_MATRIX * vec4(world, 1.0);
	mv.xy += VERTEX.xy * s;
	POSITION = dead ? vec4(2.0, 2.0, 2.0, 1.0) : PROJECTION_MATRIX * mv;
	float peak = kind > 8.5 ? 1.0 : kind > 7.5 ? 0.55 : kind < 0.5 ? 0.16 : kind < 1.5 ? 0.45 : kind < 2.5 ? 0.5 : kind < 3.5 ? 0.42 : kind < 4.5 ? 0.95 : kind < 5.5 ? 0.8 : 1.0;
	v_alpha = dead ? 0.0 : peak * smoothstep(0.0, 0.06, age) * (1.0 - age) * (1.0 - age * 0.3);
}

void fragment() {
	vec2 d = UV - 0.5;
	float r = length(d) * 2.0;
	float inner = 0.9 + 0.1 * sin(d.x * 9.0 + v_seed * 20.0) * sin(d.y * 7.0 - v_seed * 13.0);
	float soft = exp(-r * r * 3.2) * inner * smoothstep(1.0, 0.7, r);
	float k = v_kind;
	vec3 c = k < 0.5 ? smoke_color : k < 1.5 ? dust_color : k < 2.5 ? dark_color : k < 3.5 ? concrete_color : blood_color;
	if (k > 3.5) soft = smoothstep(1.0, 0.55, r);
	if (k > 4.5 && k < 5.5) { // flame: white-yellow at the heart, orange, red at the edge, brighter than white
		vec2 q = vec2(d.x * (1.6 + 0.8 * (0.5 - d.y)), d.y * 1.1 + 0.08);
		float flick = 0.75 + 0.25 * sin(d.y * 14.0 + v_seed * 40.0 + v_age * 25.0) * sin(d.x * 11.0 - v_seed * 17.0);
		float rf = length(q) * 2.0 / flick;
		vec3 hot = mix(flame_hot, flame_warm, smoothstep(0.0, 0.7, rf * 0.6 + v_age * 0.9));
		c = mix(hot, vec3(0.12, 0.1, 0.09), smoothstep(0.6, 1.0, v_age));
		soft = exp(-rf * rf * 2.2) * smoothstep(1.2, 0.6, rf);
	} else if (k > 5.5 && k < 6.5) {
		c = spark_color;
		soft = smoothstep(1.0, 0.2, r);
	} else if (k > 8.5) { // a sheet of paper, turning
		float turn = abs(sin(v_age * 40.0 + v_seed * 30.0));
		vec2 q = abs(d) * vec2(1.0, 1.0 / max(0.15, turn));
		soft = step(max(q.x, q.y), 0.42);
		c = mix(vec3(0.95, 0.94, 0.9), vec3(0.85, 0.82, 0.74), fract(v_seed * 3.0));
	} else if (k > 7.5) {
		c = water_color;
		soft = exp(-r * r * 2.5);
	} else if (k > 6.5) {
		c = grit_color;
		soft = smoothstep(1.0, 0.5, r);
	}
	bool lit_up = k > 4.5 && k < 6.5;
	c *= (0.85 + 0.3 * (0.5 - d.y)) * (lit_up ? 1.0 : mix(1.0, night_dim, clamp(night, 0.0, 1.0)));
	ALBEDO = c;
	ALPHA = clamp(v_alpha * soft, 0.0, 1.0);
}
