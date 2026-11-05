// Far/HLOD building masses (native/src/render/buildings/building_renderer.cpp, superchunk level): the cheap sibling of
// building.gdshader. No interior mapping, no finish textures and no vertex UV2: the colour comes from the vertex, the windows
// from the bay grid in UV (its x carries an offset per building, so two buildings light different rooms), filtered so a far
// block keeps its glass and its share of lit rooms at any distance.
shader_type spatial;
render_mode cull_back;

global uniform float night;
global uniform float rooms_awake;
global uniform float room_daylight;

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

// Window mask of a far wall cell: the painted-window row is gone with UV2, so a fixed share stands in for it.
const float WIN_W = 0.45;
const float WIN_SILL = 0.35;
const float WIN_HEAD = 0.80;
const float MEAN_BOOST = 1.45; // a window under a pixel keeps the glass the detail shows, not washed into the wall
const float FOOT_SHADE = 0.62; // the foot of a wall in the street's shade
const float FOOT_STOREYS = 0.47; // its height, storeys (1.4 m of a 3 m storey)
const vec3 MEAN_SEEN = vec3(0.42, 0.45, 0.47); // a room under a pixel by day (building.gdshader PANE_MEAN_SEEN)

#include "res://render/shaders/building_colour.gdshaderinc"
#include "res://render/shaders/building_hash.gdshaderinc"
#include "res://render/shaders/building_lamps.gdshaderinc"

void vertex() {
	int chunk = (int(COLOR.a * 255.0 + 0.5) & ~OFFICE_BIT) >> KIND_BITS;
	if (((chunk_mask >> chunk) & 1) != 0) {
		VERTEX = vec3(0.0); // every vertex of its triangles at one point: degenerate, never rasterized
	}
	v_world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	v_wn = normalize((MODEL_MATRIX * vec4(NORMAL, 0.0)).xyz);
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
		float m = clamp(pulse(UV.x, foot.x, 0.5 - 0.5 * WIN_W, 0.5 + 0.5 * WIN_W) * pulse(UV.y, foot.y, WIN_SILL, WIN_HEAD) * MEAN_BOOST, 0.0, 1.0);
		vec2 room = floor(UV);
		float lot = h01(uint(room.x + 512.0) * 131u + uint(room.y + 512.0) * 7177u);
		float sub = smoothstep(0.5, 1.5, max(foot.x, foot.y));
		float office = (int(COLOR.a * 255.0 + 0.5) & OFFICE_BIT) != 0 ? 1.0 : 0.0;
		vec3 lamp = mix(room_lamp(fract(lot * 17.31), office), room_lamp_mean(office), sub);
		float lit = mix(step(1.0 - rooms_awake, lot), rooms_awake, sub);
		albedo = mix(albedo, glass, m) * mix(FOOT_SHADE, 1.0, smoothstep(0.0, FOOT_STOREYS, UV.y));
		rough = mix(rough, 0.35, m);
		emit = (lamp * lit * night * window_glow + MEAN_SEEN * room_daylight) * m;
	}
	albedo *= wall_gain;
	if (kind == 4) emit += albedo * night;
	ALBEDO = surface_out(albedo);
	EMISSION = surface_out(emit);
	ROUGHNESS = rough;
	METALLIC = metal;
	NORMAL = normalize((VIEW_MATRIX * vec4(n, 0.0)).xyz);
}
