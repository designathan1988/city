// The air over the scene, in one full-screen pass (Godot's "Advanced post-processing": a quad in clip space reading the
// depth texture, drawn after the opaque world): the placed clouds' shadows on what the pixel sees, the mist, the banks
// of painted fog (fog_bank.gdshaderinc) and the clouds' bodies. Each layer takes `keep` of what is behind and adds its own
// light, so the whole pass is one blend over the scene (ALBEDO, ALPHA). render/sky_view.gd lays the clouds and sets the
// uniforms; numbers come from data/game/sky.json.
shader_type spatial;
render_mode unshaded, fog_disabled, depth_test_disabled, depth_draw_never, cull_disabled, shadows_disabled, blend_mix;

#define MAX_CLOUDS 24 // the clouds laid at most: render/cloud_puffs.gd SLOTS
#define PUFFS 7 // a cloud's puffs: render/cloud_puffs.gd LOBES

uniform sampler3D noise_volume : repeat_enable, filter_linear;
uniform sampler2D start_noise : repeat_enable, filter_nearest; // blue noise, one texel a pixel: each ray its own start
uniform int cloud_count = 0;
uniform vec4 cloud[MAX_CLOUDS]; // bounding sphere: x, y, z, size
uniform vec4 puff[MAX_CLOUDS * PUFFS]; // x, y, z, radius
uniform vec4 cloud_look[MAX_CLOUDS]; // base height, thickness (life), how much shows, -
uniform vec3 sun_dir; // set every frame by render/sky_view.gd, before the pass is shown
uniform vec3 sun_light;
uniform float dark = 0.0;
uniform float shadow_strength; // sky.json, set when the pass is made
uniform float mist = 0.0;
uniform float mist_height; // the weather's, set when it is read
uniform float mist_beta;
uniform float mist_reach; // sky.json
uniform float fog_on = 0.0;
// The look of the air: data/game/sky.json atmosphere (render/sky_view.gd sets them), by eye as the 4.0's air. Colours are linear
// light, lengths metres; each vector's parts are said here.
uniform float shadow_depth; // how fast a cloud's heap darkens the ground under it
uniform float mist_most; // the most the mist hides
uniform float cloud_done; // a ray this hidden stops marching the clouds
uniform vec2 cloud_near; // a cloud fades in from the eye between these shares of its size
uniform float cloud_least; // a density under this adds nothing
uniform vec3 cloud_sun_probe; // the sun's light in it: how far towards the sun it looks (of its size), how fast it darkens, the facing's floor
uniform vec2 cloud_facing; // which way the heap faces, between these depths
uniform vec2 cloud_extinction; // the density's extinction and the size it is reckoned on

#include "res://render/shaders/screen_depth.gdshaderinc"
#include "res://render/shaders/color_space.gdshaderinc"
#include "res://render/shaders/cloud_field.gdshaderinc"
#include "res://render/shaders/sky_model.gdshaderinc"
#include "res://render/shaders/air_globals.gdshaderinc"
#include "res://render/shaders/emission_light.gdshaderinc"
#include "res://render/shaders/sky_level_map.gdshaderinc"
#include "res://render/shaders/cloud_light.gdshaderinc"
#include "res://render/shaders/fog_bank.gdshaderinc"

const int CLOUD_STEPS = 16; // the steps through one cloud: a design value, the start's blue noise hides their grain
const int SHADOW_STEPS = 5; // the steps of the sun's ray through a heap
const float NOISE_MEAN = 0.5; // a value noise's mean: what the shadow takes of both noises
const float FAR_KEY = 1e9; // more than any distance to a cloud
const float KEY_TIE = 0.0013; // a cloud's index breaks a tie of distances (metres a slot)
// The far plane's depth, reversed (Forward+) or not (compatibility): a pixel there is sky, at SKY_M.
const float SKY_DEPTH_NEAR = 1e-7, SKY_DEPTH_FAR = 0.999999, SKY_M = 1e7;
const float CLEAR_KEEP = 0.999; // air letting this much through is not drawn (under a level of 255)
const float LEAST_ALPHA = 0.0015; // the least alpha the light is divided by

void vertex() {
	POSITION = vec4(VERTEX.xy, 1.0, 1.0);
}

// How much of the sun's direct light is left at a ground point under the clouds: five steps of the sun's ray through each
// cloud's heap, eroded as its body is, so a shadow is the cloud's heart and not its whole reach.
float shadow_density(int c, vec3 p) {
	float life = cloud_look[c].y;
	return cloud_worn(cloud_shape(heap(c, p), NOISE_MEAN, life), NOISE_MEAN, life) * cloud_look[c].z;
}

float cloud_shade(vec3 hit) {
	float through = 0.0;
	for (int c = 0; c < MAX_CLOUDS; c++) {
		if (c >= cloud_count) break;
		vec2 span = sphere_span(hit, sun_dir, cloud[c]);
		if (span.y <= 0.0) continue;
		float a = max(span.x, 0.0);
		float len = (span.y - a) / float(SHADOW_STEPS);
		for (int i = 0; i < SHADOW_STEPS; i++) {
			through += shadow_density(c, hit + sun_dir * (a + (float(i) + 0.5) * len)) * len / cloud[c].w;
		}
	}
	return 1.0 - shadow_strength * (1.0 - dark) * (1.0 - exp(-through * shadow_depth));
}

// The mist: exponential in height, along the ray to what the pixel sees.
float mist_amount(vec3 ro, vec3 rd, float t, float top) {
	float dist = min(t, mist_reach);
	float y_mid = ro.y + rd.y * dist * 0.5;
	float thick = exp(-max(0.0, min(y_mid, top)) / mist_height);
	return clamp(1.0 - exp(-mist * mist_beta * dist * thick), 0.0, mist_most);
}

// The clouds' bodies, nearest first: `light` the light they add, the result what of the scene shows through.
float cloud_bodies(vec3 ro, vec3 rd, float t_scene, float jitter, float time, out vec3 light) {
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
	float done = -1.0;
	for (int pass = 0; pass < MAX_CLOUDS; pass++) {
		if (pass >= cloud_count || keep < cloud_done) break;
		int best = -1;
		float best_key = FAR_KEY;
		for (int c = 0; c < MAX_CLOUDS; c++) {
			if (c >= cloud_count || spans[c].y <= 0.0) continue;
			float key = max(spans[c].x, 0.0) + float(c) * KEY_TIE;
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
			float density = cloud_density(best, q, time) * smoothstep(size * cloud_near.x, size * cloud_near.y, t);
			if (density < cloud_least) continue;
			// The sun reaching this point: how deep it is on the way towards the sun, and which way the heap faces.
			float here = heap(best, q);
			float towards = clamp(heap(best, q + sun_dir * size * cloud_sun_probe.x), 0.0, 1.0);
			float lit = exp(-towards * cloud_sun_probe.y) * mix(cloud_sun_probe.z, 1.0, smoothstep(cloud_facing.x, cloud_facing.y, here - towards));
			float up = clamp((q.y - cloud_look[best].x) / size, 0.0, 1.0);
			vec3 c = cloud_light(lit, up);
			float alpha = 1.0 - exp(-density * step_len * cloud_extinction.x * cloud_extinction.y / size);
			light += keep * alpha * c;
			keep *= 1.0 - alpha;
			if (keep < cloud_done) break;
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
	bool sky = depth <= SKY_DEPTH_NEAR || depth >= SKY_DEPTH_FAR;
	vec3 rd = normalize(hit - ro);
	float t_scene = sky ? SKY_M : length(hit - ro);
	float keep = 1.0;
	vec3 add = vec3(0.0);
	float jitter = texture(start_noise, FRAGCOORD.xy / vec2(textureSize(start_noise, 0))).r;
	if (!sky && cloud_count > 0) keep *= cloud_shade(hit);
	if (mist > 0.0) {
		float a = mist_amount(ro, rd, t_scene, sky ? mist_height : hit.y);
		add = add * (1.0 - a) + cloud_light(max(sun_dir.y, 0.0), 0.0) * a; // the mist's droplets lit as a cloud's, deep in it
		keep *= 1.0 - a;
	}
	if (fog_on > 0.5) {
		float a = fog_amount(ro, rd, t_scene, jitter, TIME);
		add = add * (1.0 - a) + cloud_light(max(sun_dir.y, 0.0), 0.0) * a; // a fog bank, a cloud on the ground
		keep *= 1.0 - a;
	}
	if (cloud_count > 0) {
		vec3 light;
		float through = cloud_bodies(ro, rd, t_scene, jitter, TIME, light);
		add = add * through + light;
		keep *= through;
	}
	if (keep > CLEAR_KEEP) discard;
	ALPHA = 1.0 - keep;
	ALBEDO = linear_out(add / max(ALPHA, LEAST_ALPHA));
}
