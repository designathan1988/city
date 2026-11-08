// The panes of the near level, a surface of their own on the building's mesh: COLOR.r, g its room's seed, b its own, a its kind
// (low 3 bits: 1 clear, 2 dark, 6 clear with no room behind it) and the opening behind it in cm (the bits above, shell.cpp). The walls keep building.gdshader.
// light() because an opening's sun is not its sky: the sky comes by AO (ambient times the share the opening lets through), and
// a point the wall hides from a light must take none of it with a sky above zero, which AO_LIGHT_AFFECT cannot say.
render_mode cull_back;
#include "res://render/shaders/contracts/buildings.gdshaderinc"

global uniform float night;
global uniform float room_daylight; // how bright a room seen through its glass is by day, as light it sends out
global uniform float building_glass; // the Effects panel: 0 leaves the panes out
uniform float window_glow = 1.5;
uniform float glass_reflect = 0.08;
uniform vec3 glass_sky : source_color = vec3(0.45, 0.55, 0.66);
uniform float glass_sky_floor = 0.15;
uniform float furniture_tone[120]; // linear tone of each furniture kind, three floats a kind (materials.json)

const int KIND_MASK = (1 << SURFACE_KIND_BITS) - 1;

varying vec3 v_world;
varying vec3 v_wn;
varying flat vec4 v_room0;
varying flat vec4 v_room1;
varying flat vec4 v_room2;
varying flat vec4 v_trim;
// Fragment to light: the point lit in the opening and the pane's axes in view space. v_hole.x 0: no opening.
varying vec3 v_at;
varying vec2 v_hole;
varying vec3 v_axis_x;
varying vec3 v_axis_y;
varying vec3 v_axis_z;

#include "res://render/shaders/building_level.gdshaderinc"

void vertex() {
	if (building_glass < 0.5) VERTEX = vec3(0.0); // triangles of no area: nothing to shade, and no discard to cost the depth pass
	v_world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	v_wn = normalize((MODEL_MATRIX * vec4(NORMAL, 0.0)).xyz);
	v_room0 = CUSTOM0;
	v_room1 = CUSTOM1;
	v_room2 = CUSTOM2;
	v_trim = CUSTOM3;
}

#include "res://render/shaders/building_colour.gdshaderinc"
#include "res://render/shaders/hash.gdshaderinc"
#include "res://render/shaders/building_lamps.gdshaderinc"
#include "res://render/shaders/building_room_tone.gdshaderinc"
#include "res://render/shaders/building_sky_glass.gdshaderinc"
#include "res://render/shaders/building_pane.gdshaderinc"

void fragment() {
#ifdef LEVEL_FADE
	if (!IN_SHADOW_PASS && level_noise(FRAGCOORD.xy) >= level_keep) discard;
#endif
	int coded = int(COLOR.a * 255.0 + 0.5);
	uint code = uint(COLOR.r * 255.0 + 0.5) | (uint(COLOR.g * 255.0 + 0.5) << 8u);
	float deep = float(coded >> SURFACE_KIND_BITS) * PANE_RECESS_STEP_M;
	vec2 size = max(UV2, vec2(0.3));
	vec3 n = normalize(v_wn);
	vec3 V = normalize(v_world - CAMERA_POSITION_WORLD);
	vec3 albedo, emit, nn, at;
	float rough, metal, open;
	if ((coded & KIND_MASK) == SK_PANE_CLEAR) {
		pane_clear(n, V, albedo, emit, nn, rough, metal);
		at = vec3(0.0);
		open = 1.0;
	} else {
		pane(UV, size, code, uint(COLOR.b * 255.0 + 0.5), (coded & KIND_MASK) == SK_PANE_DARK, deep, n, V, albedo, emit, nn, rough, metal, at, open);
	}
	v_at = at;
	v_hole = deep > 0.0 ? 0.5 * size : vec2(0.0);
	v_axis_x = (VIEW_MATRIX * vec4(normalize(vec3(n.z, 0.0, -n.x)), 0.0)).xyz;
	v_axis_y = (VIEW_MATRIX * vec4(0.0, 1.0, 0.0, 0.0)).xyz;
	v_axis_z = (VIEW_MATRIX * vec4(n, 0.0)).xyz;
	ALBEDO = surface_out(albedo);
	EMISSION = surface_out(emit);
	ROUGHNESS = rough;
	METALLIC = metal;
	AO = open;
	NORMAL = normalize((VIEW_MATRIX * vec4(nn, 0.0)).xyz);
}

// Lambert and GGX, as the renderer's own (LIGHT_COLOR carries PI; Karis, "Real Shading in Unreal Engine 4", 2013), times whether
// the ray back to the light leaves by the opening: the recess's own shadow, exact, where the shadow map's texels are a hand wide.
void light() {
	float through = 1.0;
	if (v_hole.x > 0.0) {
		vec3 s = vec3(dot(LIGHT, v_axis_x), dot(LIGHT, v_axis_y), -dot(LIGHT, v_axis_z));
		vec2 leaves = v_at.xy + s.xy * (v_at.z / max(-s.z, 1e-5));
		through = s.z < -1e-4 ? step(abs(leaves.x), v_hole.x) * step(abs(leaves.y), v_hole.y) : 0.0;
	}
	vec3 lit = LIGHT_COLOR * ATTENUATION * through;
	float nl = clamp(dot(NORMAL, LIGHT), 0.0, 1.0);
	DIFFUSE_LIGHT += lit * nl / PI;
	vec3 half_way = normalize(VIEW + LIGHT);
	float nv = max(dot(NORMAL, VIEW), 1e-3), nh = clamp(dot(NORMAL, half_way), 0.0, 1.0), lh = clamp(dot(LIGHT, half_way), 0.0, 1.0);
	float a2 = ROUGHNESS * ROUGHNESS * ROUGHNESS * ROUGHNESS;
	float lobe = nh * nh * (a2 - 1.0) + 1.0;
	float visible = 0.5 / max(nl * sqrt(nv * nv * (1.0 - a2) + a2) + nv * sqrt(nl * nl * (1.0 - a2) + a2), 1e-5);
	vec3 f0 = mix(vec3(0.04), ALBEDO, METALLIC);
	SPECULAR_LIGHT += lit * nl * a2 / (PI * lobe * lobe) * visible * (f0 + (1.0 - f0) * pow(1.0 - lh, 5.0)) * SPECULAR_AMOUNT;
}
