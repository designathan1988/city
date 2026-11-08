// What hangs on a facade and stands on a balcony (native/src/render/buildings/bay_props.h): one mesh a kind as MultiMesh instances.
// INSTANCE_CUSTOM.rgb = the sRGB colour of this one, COLOR.rgb = the linear tone of the part, COLOR.a = 0 on the condenser's case, whose
// front this draws: the fan's hole, its grille and the coil's louvres, a pixel's smoothing on each edge so they do not crawl.
shader_type spatial;
render_mode cull_disabled;
#include "res://render/shaders/contracts/buildings.gdshaderinc"

uniform float wall_gain = 1.0;
uniform float roughness = 0.75;

uniform vec2 ac_case; // the condenser case's front, width and height (bay_props.json, set by BuildingRenderer)
const float AC_FAN_IN = 0.27;
const float AC_COIL_IN = 0.14;

varying vec3 v_model;
varying vec3 v_model_n;
varying flat vec3 v_custom;

#include "res://render/shaders/building_colour.gdshaderinc"

void vertex() {
	v_model = VERTEX;
	v_model_n = NORMAL;
	v_custom = INSTANCE_CUSTOM.rgb;
}

float band(float x, float a, float b, float aa) { return smoothstep(a - aa, a + aa, x) - smoothstep(b - aa, b + aa, x); }

float ac_front(vec3 p) {
	vec2 q = p.xy - vec2(-ac_case.x * 0.5 + AC_FAN_IN, 0.0);
	float d = length(q), aa = max(fwidth(d), 1e-5);
	float hole = 1.0 - smoothstep(0.19 - aa, 0.19 + aa, d);
	float rings = band(d, 0.055, 0.07, aa) + band(d, 0.115, 0.13, aa) + band(d, 0.195 - 0.02, 0.195, aa);
	vec2 r = vec2(q.x + q.y, q.y - q.x) * 0.70710678;
	float ra = max(fwidth(r.x), 1e-5);
	float spokes = max(1.0 - smoothstep(0.006 - ra, 0.006 + ra, abs(r.x)), 1.0 - smoothstep(0.006 - ra, 0.006 + ra, abs(r.y))) * step(d, 0.19);
	float tone = mix(1.0, 0.1, hole);
	tone = mix(tone, 0.82, clamp(rings + spokes, 0.0, 1.0));
	float lx = abs(p.x - (ac_case.x * 0.5 - AC_COIL_IN)), la = max(fwidth(p.x), 1e-5);
	float slats = 0.0;
	for (int k = 0; k < 5; k++) {
		float y = -ac_case.y * 0.5 + 0.09 + float(k) * 0.07;
		slats += band(p.y, y - 0.007, y + 0.007, max(fwidth(p.y), 1e-5));
	}
	slats *= 1.0 - smoothstep(0.1 - la, 0.1 + la, lx);
	return mix(tone, 0.72, clamp(slats, 0.0, 1.0));
}

void fragment() {
	vec3 albedo = to_linear(v_custom) * COLOR.rgb;
	if (COLOR.a < 0.5 * (BAY_PROP_FRONT_MARK + 1.0) && v_model_n.z > 0.5) albedo *= ac_front(v_model);
	ALBEDO = surface_out(albedo * wall_gain);
	ROUGHNESS = roughness;
}
