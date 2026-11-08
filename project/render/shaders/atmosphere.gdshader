// The air over the scene, in one full-screen pass (Godot's "Advanced post-processing": a quad in clip space reading the
// depth texture, drawn after the opaque world): the placed clouds' shadows on what the pixel sees, the mist (lit as the air
// is, its colour the sky's at the horizon along the view: sky_model.gdshaderinc), the banks of painted fog and the clouds' bodies. Each layer takes `keep` of what is behind and adds its own light, so the whole
// pass is one blend over the scene (ALBEDO, ALPHA). render/sky_view.gd lays the clouds and sets the uniforms; numbers
// come from data/game/sky.json.
shader_type spatial;
render_mode unshaded, fog_disabled, depth_test_disabled, depth_draw_never, cull_disabled, shadows_disabled, blend_mix;

#define MAX_CLOUDS 24
#define PUFFS 7

uniform sampler3D noise_volume : repeat_enable, filter_linear;
uniform sampler2D start_noise : repeat_enable, filter_nearest; // blue noise, one texel a pixel: each ray its own start
uniform sampler2D fog_map : repeat_enable, filter_linear;
uniform int cloud_count = 0;
uniform vec4 cloud[MAX_CLOUDS]; // bounding sphere: x, y, z, size
uniform vec4 puff[168]; // MAX_CLOUDS * PUFFS: x, y, z, radius
uniform vec4 cloud_look[MAX_CLOUDS]; // base height, thickness (life), how much shows, -
uniform vec3 sun_dir = vec3(0.5, 0.8, 0.3);
uniform vec3 sun_light = vec3(3.0, 2.8, 2.5);
uniform float dark = 0.0;
uniform float shadow_strength = 0.62;
uniform float mist = 0.0;
uniform float mist_height = 60.0;
uniform float mist_beta;
uniform float mist_reach = 2400.0;
uniform float air_gain = 1.0; // daylight.json sky_gain: the clouds' and fog banks' authored light (in the sky's colour before it) as light
uniform float fog_on = 0.0;
uniform float fog_density = 1.0;
uniform vec2 fog_slab = vec2(0.0);
uniform vec3 fog_window = vec3(0.0, 0.0, 1.0); // low corner x, z and side
uniform float fog_extinction;
uniform vec2 wind_dir = vec2(0.93, 0.35);

#include "res://render/shaders/screen_depth.gdshaderinc"
#include "res://render/shaders/color_space.gdshaderinc"
#include "res://render/shaders/cloud_field.gdshaderinc"
#include "res://render/shaders/sky_model.gdshaderinc"
#include "res://render/shaders/air_globals.gdshaderinc"
#include "res://render/shaders/emission_light.gdshaderinc"

const int FOG_STEPS = 36; // the grain of the start goes with the length of a step: 2.1 grey levels at 24, measured on a bank
const int CLOUD_STEPS = 16;
const vec3 NIGHT_AIR = vec3(0.03, 0.035, 0.05) * 0.18; // the night's air under the lamps, as the clouds' night fill
const float WINDOW_SLACK = 0.001; // of the fog window's side: the margin of the step range over float rounding (fog_amount)

void vertex() {
	POSITION = vec4(VERTEX.xy, 1.0, 1.0);
}

// How much of the sun's direct light is left at a ground point under the clouds: five steps of the sun's ray through each
// cloud's heap, eroded as its body is, so a shadow is the cloud's heart and not its whole reach.
float shadow_density(int c, vec3 p) {
	float life = cloud_look[c].y;
	float shape = clamp(heap(c, p) * 3.2 - 0.5 * (0.55 + 0.4 * (1.0 - life)) - (1.0 - life) * 0.8, 0.0, 1.0);
	float erode = 0.5 * (0.55 + 0.3 * (1.0 - life));
	return clamp((shape - erode) / (1.0 - erode), 0.0, 1.0) * cloud_look[c].z;
}

float cloud_shade(vec3 hit) {
	float through = 0.0;
	for (int c = 0; c < MAX_CLOUDS; c++) {
		if (c >= cloud_count) break;
		vec2 span = sphere_span(hit, sun_dir, cloud[c]);
		if (span.y <= 0.0) continue;
		float a = max(span.x, 0.0);
		float len = (span.y - a) / 5.0;
		for (int i = 0; i < 5; i++) {
			through += shadow_density(c, hit + sun_dir * (a + (float(i) + 0.5) * len)) * len / cloud[c].w;
		}
	}
	return 1.0 - shadow_strength * (1.0 - dark) * (1.0 - exp(-through * 7.0));
}

// The mist: exponential in height, along the ray to what the pixel sees.
float mist_amount(vec3 ro, vec3 rd, float t, float top) {
	float dist = min(t, mist_reach);
	float y_mid = ro.y + rd.y * dist * 0.5;
	float thick = exp(-max(0.0, min(y_mid, top)) / mist_height);
	return clamp(1.0 - exp(-mist * mist_beta * dist * thick), 0.0, 0.95);
}

// Value noise by hash, no tile to repeat: a wide bank reads no lattice.
float fog_hash(vec3 p) {
	p = fract(p * 0.3183099 + 0.1);
	p *= 17.0;
	return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

float fog_value(vec3 x) {
	vec3 i = floor(x);
	vec3 f = fract(x);
	f = f * f * (3.0 - 2.0 * f);
	return mix(
		mix(mix(fog_hash(i), fog_hash(i + vec3(1.0, 0.0, 0.0)), f.x), mix(fog_hash(i + vec3(0.0, 1.0, 0.0)), fog_hash(i + vec3(1.0, 1.0, 0.0)), f.x), f.y),
		mix(mix(fog_hash(i + vec3(0.0, 0.0, 1.0)), fog_hash(i + vec3(1.0, 0.0, 1.0)), f.x), mix(fog_hash(i + vec3(0.0, 1.0, 1.0)), fog_hash(i + vec3(1.0, 1.0, 1.0)), f.x), f.y),
		f.z);
}

// The billows of a fog bank: 22 m and 9 m across, so the bank breaks into wisps with the ground showing between them.
float fog_noise(vec3 q) {
	vec3 wide = vec3(q.x, q.y * 1.6, q.z) / 22.0;
	vec3 fine = vec3(q.x, q.y * 2.0, q.z) / 9.2;
	return fog_value(wide) * 0.6 + fog_value(wide * 2.1 + 4.1) * 0.15 + fog_value(fine) * 0.25;
}

// The painted fog along the ray: the slab it fills, read from its window; thickness thinning with height over the ground
// the texel keeps, broken into wisps the wind carries (noise_volume moved by a flow of two phases, Vlachos 2010).
float fog_amount(vec3 ro, vec3 rd, float t_scene, float jitter, float time) {
	float t0 = 0.0, t1 = min(t_scene, 12000.0);
	if (abs(rd.y) > 1e-4) {
		float ta = (fog_slab.x - ro.y) / rd.y, tb = (fog_slab.y - ro.y) / rd.y;
		t0 = max(t0, min(ta, tb));
		t1 = min(t1, max(ta, tb));
	} else if (ro.y < fog_slab.x || ro.y > fog_slab.y) {
		return 0.0;
	}
	if (t1 <= t0) return 0.0;
	float step_len = (t1 - t0) / float(FOG_STEPS);
	// The steps keep their places (t0, step_len: the grain is tuned on them); only those that cannot fall inside the window
	// are not visited. The ray's stretch over the window in x and z is taken on the window widened by WINDOW_SLACK of its
	// side, far beyond float rounding, and one step more each way: every step the test in the loop keeps is in the range.
	float tw0 = t0, tw1 = t1;
	for (int k = 0; k < 2; k++) {
		float o = k == 0 ? ro.x : ro.z;
		float d = k == 0 ? rd.x : rd.z;
		float lo = (k == 0 ? fog_window.x : fog_window.y) - WINDOW_SLACK * fog_window.z;
		float hi = lo + (1.0 + 2.0 * WINDOW_SLACK) * fog_window.z;
		if (d == 0.0) {
			if (o < lo || o > hi) return 0.0; // every step stands at this x (or z), outside the window
			continue;
		}
		float ta = (lo - o) / d, tb = (hi - o) / d;
		tw0 = max(tw0, min(ta, tb));
		tw1 = min(tw1, max(ta, tb));
	}
	if (tw1 < tw0) return 0.0;
	int i_first = int(clamp(floor((tw0 - t0) / step_len - jitter) - 1.0, 0.0, float(FOG_STEPS)));
	int i_last = int(clamp(ceil((tw1 - t0) / step_len - jitter) + 1.0, -1.0, float(FOG_STEPS - 1)));
	float keep = 1.0;
	float phase_a = fract(time / 9.0), phase_b = fract(time / 9.0 + 0.5), blend_b = abs(phase_a - 0.5) * 2.0;
	for (int i = i_first; i <= i_last; i++) {
		vec3 q = ro + rd * (t0 + (float(i) + jitter) * step_len);
		vec2 local = (q.xz - fog_window.xy) / fog_window.z;
		if (local.x < 0.0 || local.y < 0.0 || local.x > 1.0 || local.y > 1.0) continue;
		vec4 m = texture(fog_map, q.xz / fog_window.z);
		if (m.r < 0.0035) continue;
		float over = q.y - m.g;
		if (over < -1.6) continue;
		float thin = exp(-max(over, 0.0) / max(m.b, 0.4));
		vec2 flow = wind_dir * m.a * 9.0;
		float na = fog_noise(q - vec3(flow.x, 0.0, flow.y) * phase_a);
		float nb = fog_noise(q - vec3(flow.x, 0.0, flow.y) * phase_b + vec3(15.0, 0.0, 37.0));
		float wisp = smoothstep(0.25, 0.75, mix(na, nb, blend_b) * (0.7 + 0.6 * thin));
		keep *= exp(-m.r * fog_density * thin * wisp * fog_extinction * step_len);
		if (keep < 0.02) break;
	}
	return 1.0 - keep;
}

// The clouds' bodies, nearest first: `light` the light they add, the result what of the scene shows through.
float cloud_bodies(vec3 ro, vec3 rd, float t_scene, float jitter, float time, vec3 sky_light, out vec3 light) {
	light = vec3(0.0);
	float keep = 1.0;
	// A cloud whose entry lies at or past what the pixel sees adds nothing: it is left out here, so the nearest-first
	// selection below never spends a pass on it.
	vec2 spans[MAX_CLOUDS];
	for (int c = 0; c < MAX_CLOUDS; c++) {
		spans[c] = vec2(-1.0);
		if (c < cloud_count) {
			vec2 span = sphere_span(ro, rd, cloud[c]);
			if (max(span.x, 0.0) < t_scene) spans[c] = span;
		}
	}
	vec3 base_shade = mix(vec3(0.22, 0.24, 0.3), vec3(0.02, 0.025, 0.035), dark);
	vec3 fill = mix(sky_light * 0.55 + vec3(0.12), vec3(0.03, 0.035, 0.05), dark);
	float done = -1.0;
	for (int pass = 0; pass < MAX_CLOUDS; pass++) {
		if (pass >= cloud_count || keep < 0.03) break;
		int best = -1;
		float best_key = 1e9;
		for (int c = 0; c < MAX_CLOUDS; c++) {
			if (c >= cloud_count || spans[c].y <= 0.0) continue;
			float key = max(spans[c].x, 0.0) + float(c) * 0.0013;
			if (key > done && key < best_key) { best_key = key; best = c; }
		}
		if (best < 0) break;
		done = best_key;
		float size = cloud[best].w;
		float t0 = max(spans[best].x, 0.0), t1 = min(spans[best].y, t_scene);
		if (t1 <= t0) continue;
		float step_len = (t1 - t0) / float(CLOUD_STEPS);
		for (int i = 0; i < CLOUD_STEPS; i++) {
			float t = t0 + (float(i) + jitter) * step_len;
			vec3 q = ro + rd * t;
			float density = cloud_density(best, q, time) * smoothstep(size * 0.52, size * 1.3, t);
			if (density < 0.01) continue;
			// The sun reaching this point: how deep it is on the way towards the sun, and which way the heap faces.
			float here = heap(best, q);
			float towards = clamp(heap(best, q + sun_dir * size * 0.18), 0.0, 1.0);
			float lit = exp(-towards * 2.6) * mix(0.55, 1.0, smoothstep(-0.05, 0.1, here - towards));
			float up = clamp((q.y - cloud_look[best].x) / size, 0.0, 1.0);
			vec3 c = mix(base_shade, fill, 0.25 + 0.75 * up) * 0.85 + unshaded_light(sun_light) * 0.36 * lit * (1.0 - dark);
			float alpha = 1.0 - exp(-density * step_len * 0.015 * 200.0 / size);
			light += keep * alpha * c;
			keep *= 1.0 - alpha;
			if (keep < 0.03) break;
		}
	}
	return keep;
}

void fragment() {
	float depth = texture(depth_texture, SCREEN_UV).x;
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
	vec3 ndc = vec3(SCREEN_UV, depth) * 2.0 - 1.0;
#else
	vec3 ndc = vec3(SCREEN_UV * 2.0 - 1.0, depth);
#endif
	vec4 view = INV_PROJECTION_MATRIX * vec4(ndc, 1.0);
	view.xyz /= view.w;
	vec3 ro = INV_VIEW_MATRIX[3].xyz;
	vec3 hit = (INV_VIEW_MATRIX * vec4(view.xyz, 1.0)).xyz;
	bool sky = depth <= 1e-7 || depth >= 0.999999;
	vec3 rd = normalize(hit - ro);
	float t_scene = sky ? 1e7 : length(hit - ro);
	float keep = 1.0;
	vec3 add = vec3(0.0);
	float jitter = texture(start_noise, FRAGCOORD.xy / vec2(textureSize(start_noise, 0))).r;
	if (!sky && cloud_count > 0) keep *= cloud_shade(hit);
	vec3 level = normalize(vec3(rd.x, 0.0, rd.z) + vec3(1e-5, 0.0, 0.0));
	vec3 horizon = unshaded_light(sky_radiance(air_now(), level, max(ro.y, 1.0))) + NIGHT_AIR * dark; // what the air sends along a level view, pre-exposed as the scene is
	if (mist > 0.0) {
		float a = mist_amount(ro, rd, t_scene, sky ? mist_height : hit.y);
		add = add * (1.0 - a) + horizon * a;
		keep *= 1.0 - a;
	}
	if (fog_on > 0.5) {
		float a = fog_amount(ro, rd, t_scene, jitter, TIME);
		vec3 fog_light = mix(vec3(0.86, 0.9, 0.95), vec3(1.0, 0.95, 0.84), pow(max(dot(rd, sun_dir), 0.0), 8.0)) * (0.72 + 0.28 * max(sun_dir.y, 0.0));
		add = add * (1.0 - a) + fog_light * (1.0 - 0.85 * dark) * air_gain * a;
		keep *= 1.0 - a;
	}
	if (cloud_count > 0) {
		vec3 light;
		float through = cloud_bodies(ro, rd, t_scene, jitter, TIME, horizon / air_gain, light);
		add = add * through + light * air_gain;
		keep *= through;
	}
	if (keep > 0.999) discard;
	ALPHA = 1.0 - keep;
	ALBEDO = linear_out(add / max(ALPHA, 0.0015));
}
