// The sun's contact shadows: a few steps towards the sun along the depth, for the thin shade the shadow map does not resolve where
// a thing meets what it stands on. What a shade takes is the sun's share of the light on level ground, the sky's worked out once a
// draw from the same air the sky is drawn with (sky_model.gdshaderinc). Only where the sun does reach, or a shadow would be shaded twice;
// and a full-screen pass cannot read the shadow atlas, so this one is lit: LIGHT_VERTEX sets the pixel's place for light and
// shadows without moving the fragment, and ATTENUATION is then the sun's own shadow at that place. render/contact_pass.gd lays it.
shader_type spatial;
render_mode fog_disabled, depth_test_disabled, depth_draw_never, cull_disabled, ambient_light_disabled, specular_disabled, blend_mul;

#include "res://render/shaders/screen_depth.gdshaderinc"
#include "res://render/shaders/sky_model.gdshaderinc"
#include "res://render/shaders/air_globals.gdshaderinc"

uniform vec3 sun_direction = vec3(0.0, 1.0, 0.0); // towards the sun, in the world
uniform float strength = 0.0;
uniform float sun_level = 0.0; // the sun's light on level ground, in the lights' unit (render/contact_pass.gd)

const int STEPS = 8;
const float RAY_M = 1.2;
const float OFF_M = 0.04; // off the surface, or a pixel shades itself
const float SAME_SURFACE_M = 0.03, TOUCHING_M = 0.8; // what stands in front by less is the same surface, by more is not touching
const float FADE_FROM_M = 70.0, END_M = 120.0;
const float LEAST_SUN = 0.05;

varying float shade;
varying float v_sun_share;

void vertex() {
	POSITION = vec4(VERTEX.xy, 1.0, 1.0);
	float sky = dot(sky_level_light(air_now(), 0.0), vec3(0.2126, 0.7152, 0.0722));
	v_sun_share = sun_level / max(sun_level + sky, 1e-4);
}

// Whether a surface stands between the pixel and the sun, within the steps.
bool touching(vec3 p, vec3 n, vec3 sun, mat4 projection, mat4 inv_projection) {
	for (int i = 1; i <= STEPS; i++) {
		vec3 q = p + n * OFF_M + sun * (float(i) / float(STEPS) * RAY_M);
		vec4 clip = projection * vec4(q, 1.0);
		vec2 at = clip.xy / clip.w * 0.5 + 0.5;
		if (at.x < 0.0 || at.y < 0.0 || at.x > 1.0 || at.y > 1.0) return false;
		float near_q = length(q), near_s = length(view_at(at, inv_projection));
		if (near_s < near_q - SAME_SURFACE_M && near_q - near_s < TOUCHING_M) return true;
	}
	return false;
}

void fragment() {
	vec3 p = view_at(SCREEN_UV, INV_PROJECTION_MATRIX);
	float far = length(p);
	if (!(far < END_M)) discard;
	vec3 sun = mat3(VIEW_MATRIX) * sun_direction;
	vec3 n = view_normal(p, SCREEN_UV, 1.0 / VIEWPORT_SIZE, INV_PROJECTION_MATRIX);
	if (dot(n, sun) <= LEAST_SUN || !touching(p, n, sun, PROJECTION_MATRIX, INV_PROJECTION_MATRIX)) discard;
	shade = strength * v_sun_share * (1.0 - smoothstep(FADE_FROM_M, END_M, far));
	ALBEDO = vec3(1.0);
	SPECULAR = 0.0;
	LIGHT_VERTEX = p;
}

// The sun is the one directional light: its shade over the pixel.
void light() {
	if (LIGHT_IS_DIRECTIONAL) DIFFUSE_LIGHT = max(DIFFUSE_LIGHT, vec3(1.0 - shade * ATTENUATION));
}
