// What hangs on a facade and stands on a balcony (native/src/render/buildings/bay_props.h): one mesh a kind as MultiMesh instances.
// INSTANCE_CUSTOM.rgb = the sRGB colour of this one, COLOR.rgb = the linear tone of the part, COLOR.a = 0 on the condenser's case, whose
// front this draws: the fan's hole, its grille and the coil's louvres, a pixel's smoothing on each edge so they do not crawl.
shader_type spatial;
render_mode cull_disabled;
#include "res://render/shaders/contracts/buildings.gdshaderinc"

uniform float wall_gain = 1.0;
uniform float roughness; // materials.json building_props

uniform vec2 ac_case; // the condenser case's front, width and height (bay_props.json, set by BuildingRenderer)
// The condenser's front as drawn on its case, metres (by eye, a split unit's): the fan's middle and the coil's, in from the
// case's sides; the fan's hole, its three rings (inner and outer radii), its spokes' half width; the coil's five slats, the
// first's height over the bottom, their pitch and half height, their reach out from the coil's middle; the tones of the hole,
// the grille and the slats.
const float AC_FAN_IN = 0.27, AC_COIL_IN = 0.14;
const float AC_FAN_R = 0.19, AC_RING1_IN = 0.055, AC_RING1_OUT = 0.07, AC_RING2_IN = 0.115, AC_RING2_OUT = 0.13, AC_RING3_W = 0.02, AC_RING3_OUT = 0.195, AC_SPOKE_HALF = 0.006;
const int AC_SLATS = 5;
const float AC_SLAT_FIRST = 0.09, AC_SLAT_PITCH = 0.07, AC_SLAT_HALF = 0.007, AC_SLAT_REACH = 0.1;
const float AC_HOLE_TONE = 0.1, AC_GRILLE_TONE = 0.82, AC_SLAT_TONE = 0.72;
const float HALF_SQRT2 = 0.70710678; // a turn of 45 degrees
const float LEAST_AA = 1e-5; // a pixel's width never under this

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
	float d = length(q), aa = max(fwidth(d), LEAST_AA);
	float hole = 1.0 - smoothstep(AC_FAN_R - aa, AC_FAN_R + aa, d);
	float rings = band(d, AC_RING1_IN, AC_RING1_OUT, aa) + band(d, AC_RING2_IN, AC_RING2_OUT, aa) + band(d, AC_RING3_OUT - AC_RING3_W, AC_RING3_OUT, aa);
	vec2 r = vec2(q.x + q.y, q.y - q.x) * HALF_SQRT2;
	float ra = max(fwidth(r.x), LEAST_AA);
	float spokes = max(1.0 - smoothstep(AC_SPOKE_HALF - ra, AC_SPOKE_HALF + ra, abs(r.x)), 1.0 - smoothstep(AC_SPOKE_HALF - ra, AC_SPOKE_HALF + ra, abs(r.y))) * step(d, AC_FAN_R);
	float tone = mix(1.0, AC_HOLE_TONE, hole);
	tone = mix(tone, AC_GRILLE_TONE, clamp(rings + spokes, 0.0, 1.0));
	float lx = abs(p.x - (ac_case.x * 0.5 - AC_COIL_IN)), la = max(fwidth(p.x), LEAST_AA);
	float slats = 0.0;
	for (int k = 0; k < AC_SLATS; k++) {
		float y = -ac_case.y * 0.5 + AC_SLAT_FIRST + float(k) * AC_SLAT_PITCH;
		slats += band(p.y, y - AC_SLAT_HALF, y + AC_SLAT_HALF, max(fwidth(p.y), LEAST_AA));
	}
	slats *= 1.0 - smoothstep(AC_SLAT_REACH - la, AC_SLAT_REACH + la, lx);
	return mix(tone, AC_SLAT_TONE, clamp(slats, 0.0, 1.0));
}

void fragment() {
	vec3 albedo = srgb_to_linear(v_custom) * COLOR.rgb;
	if (COLOR.a < 0.5 * (BAY_PROP_FRONT_MARK + 1.0) && v_model_n.z > 0.5) albedo *= ac_front(v_model);
	ALBEDO = surface_out(albedo * wall_gain);
	ROUGHNESS = roughness;
}
