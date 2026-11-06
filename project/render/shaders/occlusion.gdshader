// What the corners of the town hide from the light, in one full-screen pass that multiplies the lit picture: ground truth
// ambient occlusion (Jimenez et al. 2016, "Practical Real-Time Strategies for Accurate Indirect Occlusion"): in a few
// slices through the view, the horizon each side of the pixel found in the depth, and the visible arc under them cosine
// weighted. It sees the 15 cm of a kerb, which Godot's SSAO does not. A fixed set of slices: the picture has no noise to
// take out, so no denoise pass. Numbers from data/game/daylight.json `occlusion`; render/occlusion_pass.gd lays it.
shader_type spatial;
render_mode unshaded, fog_disabled, depth_test_disabled, depth_draw_never, cull_disabled, shadows_disabled, blend_mul;

#include "res://render/shaders/screen_depth.gdshaderinc"

uniform float reach_m = 0.75; // how far a slice looks
uniform float reach_curve = 1.6; // its steps crowd by the pixel, where a corner's shade is
uniform float thickness_m = 0.8; // what stands further than this from the pixel, along the view, passes behind nothing
uniform float power = 1.05;
uniform float strength = 0.85;

const int SLICES = 4;
const int STEPS = 4;
const float LEAST_PIXELS = 2.0; // a reach shorter than this on the screen finds no corner
const float MOST_SCREEN = 0.15; // of the height: past this a step lands so far off that its edge shows as a band (HBAO's largest radius in pixels)
const float LEAST_SQUARED = 1e-8;

void vertex() {
	POSITION = vec4(VERTEX.xy, 1.0, 1.0);
}

// Raises a horizon to what the point at `offset` from the pixel stands to, the further away the less.
float raised(float horizon, vec3 p, vec3 eye, vec3 offset, float weight, mat4 projection, mat4 inv_projection) {
	vec3 q = p + offset; // the screen's place of q: of the projection, only what x, y and w take
	vec2 clip = vec2(projection[0][0], projection[1][1]) * q.xy + projection[2].xy * q.z;
	vec3 delta = view_at(clip / (projection[2][3] * q.z + projection[3][3]) * 0.5 + 0.5, inv_projection) - p;
	float size = dot(delta, delta);
	if (size < LEAST_SQUARED || abs(delta.z) >= thickness_m) return horizon;
	return horizon + max(0.0, (dot(eye, delta) * inversesqrt(size) - horizon) * weight);
}

float visible(vec3 p, vec3 n, float reach, mat4 projection, mat4 inv_projection) {
	vec3 eye = normalize(-p);
	float sum = 0.0;
	float ring[STEPS]; // how far each step goes: the same in every slice
	for (int j = 0; j < STEPS; j++) ring[j] = reach * pow(float(j + 1) / float(STEPS), reach_curve);
	for (int i = 0; i < SLICES; i++) {
		float a = (float(i) + 0.5) / float(SLICES) * PI;
		vec3 way = vec3(cos(a), sin(a), 0.0);
		vec3 side = normalize(cross(way, eye));
		vec3 across = cross(side, eye);
		vec3 ns = normalize(n - side * dot(n, side));
		vec3 along = cross(ns, side);
		vec2 h = vec2(dot(eye, along), -dot(eye, along));
		for (int j = 0; j < STEPS; j++) {
			vec3 offset = way * ring[j];
			float weight = 2.0 / float(j + 2);
			h.x = raised(h.x, p, eye, offset, weight, projection, inv_projection);
			h.y = raised(h.y, p, eye, -offset, weight, projection, inv_projection);
		}
		vec2 s = sqrt(max(vec2(0.0), 1.0 - h * h));
		float nx = 0.5 * (acos(clamp(h.y, -1.0, 1.0)) - acos(clamp(h.x, -1.0, 1.0)) + s.x * h.x - s.y * h.y);
		float ny = 0.5 * (2.0 - h.x * h.x - h.y * h.y);
		sum += dot(ns, across) * nx + dot(ns, eye) * ny;
	}
	return clamp(sum / float(SLICES), 0.0, 1.0);
}

void fragment() {
	float shade = 1.0;
	float depth = textureLod(depth_texture, SCREEN_UV, 0.0).x;
	vec3 p = view_at(SCREEN_UV, INV_PROJECTION_MATRIX);
	float metre_on_screen = abs(PROJECTION_MATRIX[1][1]) * VIEWPORT_SIZE.y * 0.5 / -p.z; // in pixels
	if (depth > 0.0 && reach_m * metre_on_screen >= LEAST_PIXELS) {
		float reach = min(reach_m, MOST_SCREEN * VIEWPORT_SIZE.y / metre_on_screen);
		vec3 n = view_normal(p, SCREEN_UV, 1.0 / VIEWPORT_SIZE, INV_PROJECTION_MATRIX);
		shade = mix(1.0, pow(visible(p, n, reach, PROJECTION_MATRIX, INV_PROJECTION_MATRIX), power), strength);
	}
	ALBEDO = vec3(shade);
}
