// Far/HLOD building masses with their lots' paved covers and walls (native/src/render/buildings/building_renderer_build.cpp
// build_super_cpu, superchunk level): the cheap sibling of
// building.gdshader. No interior mapping, no finish textures and no vertex UV2: the colour comes from the vertex, the windows
// from the bay grid in UV (its x carries an offset per building, so two buildings light different rooms) and each side's own
// opening in CUSTOM0, as the other levels code it (shell.h), filtered so a far block keeps its glass and its share of lit rooms
// at any distance.
shader_type spatial;
render_mode cull_back;

global uniform float night;
global uniform float rooms_awake;
global uniform float room_daylight;
global uniform float building_night_windows; // the Effects panel: 0 puts the lit rooms out

uniform vec3 glass_sky : source_color = vec3(0.45, 0.55, 0.66);
uniform float glass_floor = 0.18;
uniform float wall_gain = 1.0;
uniform float window_glow = 1.5;

// The superchunk's chunks drawn in their own detail (building_renderer.h): their buildings fold away here, so the far
// masses never draw over them. COLOR.a carries the surface kind (low 3 bits) and the building's chunk in the
// superchunk (0..15, the bits above), written by BuildingRenderer::build_super_cpu.
instance uniform int chunk_mask = 0;
const int KIND_BITS = 3;
const int KIND_MASK = 7;
const int OFFICE_BIT = 128; // COLOR.a's top bit: its rooms take the office lamps

varying vec3 v_world;
varying vec3 v_wn;
varying flat vec4 v_opening; // width, sill and head as shares of the bay and the storey; flags

const int WINDOW_BLANK = 4; // shell.h WINDOW_FLAG_BLANK: a side with no openings
const float FOOT_SHADE = 0.62; // the foot of a wall in the street's shade
const float FOOT_STOREYS = 0.47; // its height, storeys (1.4 m of a 3 m storey)
#include "res://render/shaders/building_room_tone.gdshaderinc"
const vec3 PANE_ALBEDO = vec3(0.0075, 0.0096, 0.0115); // glass scatters next to nothing (building_pane.gdshaderinc)

#include "res://render/shaders/building_colour.gdshaderinc"
#include "res://render/shaders/building_hash.gdshaderinc"
#include "res://render/shaders/building_lamps.gdshaderinc"
#define glass_sky_floor 0.0
#include "res://render/shaders/building_sky_glass.gdshaderinc"

void vertex() {
	int chunk = (int(COLOR.a * 255.0 + 0.5) & ~OFFICE_BIT) >> KIND_BITS;
	if (((chunk_mask >> chunk) & 1) != 0) {
		VERTEX = vec3(0.0); // every vertex of its triangles at one point: degenerate, never rasterized
	}
	v_world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	v_wn = normalize((MODEL_MATRIX * vec4(NORMAL, 0.0)).xyz);
	v_opening = CUSTOM0;
}

void fragment() {
	int kind = int(COLOR.a * 255.0 + 0.5) & KIND_MASK;
	vec3 n = normalize(v_wn);
	vec3 albedo = to_linear(COLOR.rgb);
	vec3 glass = mix(vec3(0.05, 0.06, 0.07), uniform_linear(glass_sky), glass_floor) * (1.0 - 0.92 * night);
	vec3 emit = vec3(0.0);
	float rough = 0.9, metal = 0.0;
	if (kind == 1 || kind == 2) {
		albedo = glass;
		rough = 0.2;
	}
	if (kind == 3) {
		vec2 foot = grid_footprint(UV);
		float open = (int(v_opening.a * 255.0 + 0.5) & WINDOW_BLANK) != 0 ? 0.0 : 1.0;
		float m = open * clamp(pulse(UV.x, foot.x, 0.5 - 0.5 * v_opening.r, 0.5 + 0.5 * v_opening.r) * pulse(UV.y, foot.y, v_opening.g, v_opening.b), 0.0, 1.0);
		vec2 room = floor(UV);
		float lot = h01(uint(room.x + 512.0) * 131u + uint(room.y + 512.0) * 7177u);
		float sub = smoothstep(0.5, 1.5, max(foot.x, foot.y));
		float office = (int(COLOR.a * 255.0 + 0.5) & OFFICE_BIT) != 0 ? 1.0 : 0.0;
		// the glass of the levels before it (building_far_window.gdshaderinc): next to nothing scattered, the room through it and the sky on
		// it; its lamp by the rule of every level (building_lamps.gdshaderinc), a cell by its own lot, the mean under a pixel
		vec3 V = normalize(v_world - CAMERA_POSITION_WORLD);
		float cosv = clamp(dot(-V, n), 0.0, 1.0);
		float fresnel = 0.04 + 0.96 * pow(1.0 - cosv, 5.0);
		vec3 seen = pane_mean(room_tone(V, n, office > 0.5 ? FLOOR_STONE : FLOOR_HOME)) * (1.0 - fresnel);
		vec3 lamp = vec3(0.0);
		if (night > 0.0) lamp = lamp_on_room(mix(cell_light(lot, office), pane_light_mean(office), sub), seen) * building_night_windows;
		albedo = mix(albedo, PANE_ALBEDO, m) * mix(FOOT_SHADE, 1.0, smoothstep(0.0, FOOT_STOREYS, UV.y));
		rough = mix(rough, 0.35, m);
		emit = (lamp * night * window_glow + seen * room_daylight + glass_reflection(V, n, fresnel, GLASS_ENV)) * m;
	}
	albedo *= wall_gain;
	if (kind == 4) emit += albedo * night;
	ALBEDO = surface_out(albedo);
	EMISSION = surface_out(emit);
	ROUGHNESS = rough;
	METALLIC = metal;
	NORMAL = normalize((VIEW_MATRIX * vec4(n, 0.0)).xyz);
}
