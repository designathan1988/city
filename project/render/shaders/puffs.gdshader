// Smoke, dust, flame, sparks, grit, blood spray, water and paper of a blast: one billboard each, no per-frame work on the
// CPU. The CPU writes a puff once (position and size in the instance transform, INSTANCE_CUSTOM = born, kind, seed, life);
// the motion, size, colour and fade are all worked out here from its age. Kinds: PuffKind (exhaust, dust, dark smoke,
// concrete dust, blood, flame, spark, grit, hydrant water, paper). The motions are those of the puffs of the
// reference game, written in steps of STEP_M metres; every value of them is data/render/materials.json puffs.params.
shader_type spatial;
render_mode unshaded, blend_mix, depth_draw_never, cull_disabled, shadows_disabled;
#include "res://render/shaders/contracts/effects.gdshaderinc"

#include "res://render/shaders/color_space.gdshaderinc"
#include "res://render/shaders/tone_inverse.gdshaderinc"

const float STEP_M = 0.4; // a step of the reference game's motions, metres
const int PUFF_KINDS = PUFF_PAPER + 1; // PuffKind runs 0..PUFF_PAPER
const float LEAST_S = 0.001; // the least life a puff's age divides by
// A puff's independent picks and phases from its seed: seed * k, each k far from a simple ratio of the others (the reference
// game's). SEED_TURN is a turn as it wrote one, not TAU.
const float SEED_OUT = 40.0, SEED_SWING = 9.0, SEED_PICK = 7.0, SEED_DRIFT_X = 13.0, SEED_DRIFT_Z = 5.0, SEED_JET = 13.0;
const float SEED_FLING = 17.0, SEED_TURN = 6.28, SEED_BILLOW_X = 20.0, SEED_BILLOW_Y = 13.0, SEED_FLICK_Y = 40.0;
const float SEED_FLICK_X = 17.0, SEED_SHEET = 30.0;

global uniform float night;
uniform float now = 0.0; // the effects clock, seconds (the same clock the puffs were born on; puffs.cpp)
// Speeds in steps a second, falls in steps a second squared, rates in radians a second; a pick of h is from + over * h.
uniform vec2 wind_dir; // the way the wind carries them
uniform vec3 smoke_color;
uniform vec3 dust_color;
uniform vec3 dark_color; // the reference game's dark smoke (exhaust.ts); the pale core of a blast was missing flame sources, not a lighter smoke
uniform vec3 concrete_color;
uniform vec3 blood_color;
uniform vec3 grit_color;
uniform vec3 water_color;
uniform vec3 spark_color;
uniform vec3 flame_hot;
uniform vec3 flame_warm;
uniform float night_dim; // how dark smoke and dust read at night (they are not lit)
uniform float puff_rise[PUFF_KINDS]; // a cloud's rise, by kind
uniform float puff_spread[PUFF_KINDS]; // a cloud's spread out from where it was born, by kind
uniform float puff_grow[PUFF_KINDS]; // what a puff grows over its life, of its size, by kind
uniform float puff_peak[PUFF_KINDS]; // its most opacity, by kind
uniform vec2 puff_fade; // it fades in over this share of its life, and out slower by this share
uniform vec3 cloud_drift; // a cloud's rise slows by this share over its life; it bobs this far at this rate
uniform vec2 cloud_wind; // the wind carries it at this speed, gathering this much a second
uniform vec3 paper_up; // paper: blown up this far at this rate (1/s), falling back at this speed
uniform vec2 paper_swing; // its swing and the swing's rate
uniform vec3 paper_out; // blown out this far (from and over) at this rate (1/s)
uniform float paper_wind; // the wind's share
uniform vec3 paper_flutter; // its flutter's rates across and along, and how far
uniform vec4 water_jet; // a hydrant's jet: up, from and over; the size of a full jet and the least share of it
uniform float water_fall;
uniform vec3 water_out; // out, from and over; the wind's share
uniform vec4 flung; // blood, sparks, grit: up, from and over; out, from and over
uniform float flung_fall;
uniform vec3 flame_rise; // flame: its rise, its lick and the lick's rate
uniform vec3 flame_out; // out, slowing by this share over its life; the wind's share
uniform vec4 billow; // a cloud's billow: its floor and depth, its waves across and up
uniform vec2 cloud_edge; // its fall-off exp(-k r^2), the radius its edge fades from
uniform float rim_edge; // where a rimmed puff's edge fades from
uniform vec4 flame_shape; // flame: its width at the base, its taper, its height, its lift
uniform vec4 flame_flick; // its flicker: floor and depth, waves up and across
uniform float flame_flick_rate; // the flicker's rate over its life
uniform vec3 flame_heat; // hot to warm: the edge, the radius's share, the age's share
uniform vec3 flame_soot; // the colour it dies to
uniform float flame_dies; // from this share of its life
uniform vec3 flame_edge; // its fall-off exp(-k r^2), the radius its edge fades over, from and to
uniform float spark_edge; // where a spark's edge fades to
uniform vec3 paper_look; // a sheet: its turn over its life, its least share edge-on, its half side
uniform vec3 paper_white; // a sheet's tone between these two
uniform vec3 paper_old;
uniform float water_edge; // a drop's fall-off exp(-k r^2)
uniform vec2 puff_shade; // light from above: from and over

varying flat float v_kind;
varying flat float v_seed;
varying flat float v_age;
varying flat float v_alpha;

void vertex() {
	float born = INSTANCE_CUSTOM.x, kind = INSTANCE_CUSTOM.y, seed = INSTANCE_CUSTOM.z, life = max(INSTANCE_CUSTOM.w, LEAST_S);
	float t = now - born;
	float age = t / life;
	v_kind = kind;
	v_seed = seed;
	v_age = age;
	v_alpha = 0.0;
	bool dead = age < 0.0 || age > 1.0;
	float size = length(MODEL_MATRIX[0].xyz);
	vec3 p = vec3(0.0);
	int k = int(kind + 0.5);
	float rise = puff_rise[k];
	vec2 out2 = vec2(sin(seed * SEED_OUT), cos(seed * SEED_OUT));
	bool flung_kind = k == PUFF_BLOOD || k == PUFF_SPARK || k == PUFF_GRIT;
	if (k == PUFF_PAPER) { // paper: blown up and out, then drifting down, swinging, carried by the wind
		float up = paper_up.x * (1.0 - exp(-t * paper_up.y)) - paper_up.z * t;
		p.y += up + paper_swing.x * sin(t * paper_swing.y + seed * SEED_SWING);
		p.xz += out2 * (paper_out.x + paper_out.y * fract(seed * SEED_PICK)) * (1.0 - exp(-t * paper_out.z)) + wind_dir * paper_wind * t + vec2(sin(t * paper_flutter.x + seed * SEED_DRIFT_X), cos(t * paper_flutter.y + seed * SEED_DRIFT_Z)) * paper_flutter.z;
	} else if (k == PUFF_WATER) { // a hydrant jet: straight up, fanning, falling back
		p.y += (water_jet.x + water_jet.y * fract(seed * SEED_JET)) * clamp(size / water_jet.z, water_jet.w, 1.0) * t - water_fall * t * t;
		p.xz += out2 * (water_out.x + water_out.y * fract(seed * SEED_PICK)) * t + wind_dir * water_out.z * t;
	} else if (flung_kind) { // blood, sparks, grit: flung up and out, falling under gravity
		float fling = k == PUFF_SPARK ? 3.0 : 1.0;
		p.y += (flung.x + flung.y * fract(seed * SEED_FLING)) * fling * t - flung_fall * t * t;
		p.xz += out2 * (flung.z + flung.w * fract(seed * SEED_PICK)) * fling * t;
	} else if (k == PUFF_FLAME) { // flame: bursting out, licking up fast
		p.y += flame_rise.x * t + flame_rise.y * sin(t * flame_rise.z + seed * SEED_TURN);
		p.xz += out2 * flame_out.x * t * (1.0 - age * flame_out.y) + wind_dir * flame_out.z * t;
	} else {
		p.y += rise * t * (1.0 - cloud_drift.x * age) + cloud_drift.y * sin(t * cloud_drift.z + seed * SEED_TURN);
		float spread = puff_spread[k];
		p.xz += wind_dir * (cloud_wind.x * t + cloud_wind.y * t * t) + out2 * spread * t;
	}
	float grow = puff_grow[k];
	float s = size * (0.5 + grow * age);
	vec3 world = MODEL_MATRIX[3].xyz + p * STEP_M;
	vec4 mv = VIEW_MATRIX * vec4(world, 1.0);
	mv.xy += VERTEX.xy * s;
	POSITION = dead ? vec4(2.0, 2.0, 2.0, 1.0) : PROJECTION_MATRIX * mv;
	float peak = puff_peak[k];
	v_alpha = dead ? 0.0 : peak * smoothstep(0.0, puff_fade.x, age) * (1.0 - age) * (1.0 - age * puff_fade.y);
}

void fragment() {
	vec2 d = UV - 0.5;
	float r = length(d) * 2.0;
	float inner = billow.x + billow.y * sin(d.x * billow.z + v_seed * SEED_BILLOW_X) * sin(d.y * billow.w - v_seed * SEED_BILLOW_Y);
	float soft = exp(-r * r * cloud_edge.x) * inner * smoothstep(1.0, cloud_edge.y, r);
	int k = int(v_kind + 0.5);
	vec3 c = k == PUFF_EXHAUST ? smoke_color : k == PUFF_DUST ? dust_color : k == PUFF_SMOKE ? dark_color : k == PUFF_CONCRETE ? concrete_color : blood_color;
	bool cloud = k == PUFF_EXHAUST || k == PUFF_DUST || k == PUFF_SMOKE || k == PUFF_CONCRETE; // soft-edged; the rest have a rim
	if (!cloud) soft = smoothstep(1.0, rim_edge, r);
	if (k == PUFF_FLAME) { // flame: white-yellow at the heart, orange, red at the edge, brighter than white
		vec2 q = vec2(d.x * (flame_shape.x + flame_shape.y * (0.5 - d.y)), d.y * flame_shape.z + flame_shape.w);
		float flick = flame_flick.x + flame_flick.y * sin(d.y * flame_flick.z + v_seed * SEED_FLICK_Y + v_age * flame_flick_rate) * sin(d.x * flame_flick.w - v_seed * SEED_FLICK_X);
		float rf = length(q) * 2.0 / flick;
		vec3 hot = mix(flame_hot, flame_warm, smoothstep(0.0, flame_heat.x, rf * flame_heat.y + v_age * flame_heat.z));
		c = mix(hot, flame_soot, smoothstep(flame_dies, 1.0, v_age));
		soft = exp(-rf * rf * flame_edge.x) * smoothstep(flame_edge.y, flame_edge.z, rf);
	} else if (k == PUFF_SPARK) {
		c = spark_color;
		soft = smoothstep(1.0, spark_edge, r);
	} else if (k == PUFF_PAPER) { // a sheet of paper, turning
		float turn = abs(sin(v_age * paper_look.x + v_seed * SEED_SHEET));
		vec2 q = abs(d) * vec2(1.0, 1.0 / max(paper_look.y, turn));
		soft = step(max(q.x, q.y), paper_look.z);
		c = mix(paper_white, paper_old, fract(v_seed * 3.0));
	} else if (k == PUFF_WATER) {
		c = water_color;
		soft = exp(-r * r * water_edge);
	} else if (k == PUFF_GRIT) {
		c = grit_color;
		soft = smoothstep(1.0, 0.5, r);
	}
	bool lit_up = k == PUFF_FLAME || k == PUFF_SPARK;
	c *= (puff_shade.x + puff_shade.y * (0.5 - d.y)) * (lit_up ? 1.0 : mix(1.0, night_dim, clamp(night, 0.0, 1.0)));
	// Smoke, dust, blood, grit, water and paper are the colour the screen should show (the reference game writes them straight to its
	// frame buffer), so they go through the inverse of the AgX; flame and spark are light, brighter than white: the AgX takes them as
	// they are (their saturated oranges need radiance the tone mapping cannot give back without a hue shift).
	ALBEDO = lit_up ? c : linear_out(display_to_radiance(min(c, vec3(1.0))));
	ALPHA = clamp(v_alpha * soft, 0.0, 1.0);
}
