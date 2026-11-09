// Far/HLOD building masses (native/src/render/buildings/building_renderer.cpp, superchunk level): the cheap sibling of
// building.gdshader. No interior mapping, no finish textures and no vertex UV2: the colour comes from the vertex, the windows
// from the bay grid in UV (its x carries an offset per building, so two buildings light different rooms) and each side's own
// opening in CUSTOM0, as the other levels code it (shell.h), filtered so a far block keeps its glass and its share of lit rooms
// at any distance.
shader_type spatial;
render_mode cull_back;
#include "res://render/shaders/contracts/buildings.gdshaderinc"
#include "res://render/shaders/bytes.gdshaderinc"
#include "res://render/shaders/fresnel.gdshaderinc"

global uniform float night;
global uniform float room_daylight;

uniform vec3 glass_sky : source_color; // materials.json sky_in_glass
uniform float glass_floor; // materials.json building_far, as the next
uniform vec3 far_glass_dark; // the painted glass's own tone, under the sky it shows by its floor
uniform float far_glass_night; // what night takes of the glass
uniform vec2 far_rough; // the far mass's roughness: its walls, its glass
uniform float wall_gain = 1.0;
uniform float window_glow = 1.5;

// The superchunk's chunks drawn in their own detail (building_renderer.h): their buildings fold away here, so the far
// masses never draw over them. COLOR.a carries the surface kind (low 3 bits) and the building's chunk in the
// superchunk (0..15, the bits above), written by BuildingRenderer::build_super_cpu.
instance uniform int chunk_mask = 0;
const int KIND_MASK = (1 << SURFACE_KIND_BITS) - 1;

varying vec3 v_world;
varying vec3 v_wn;
varying flat vec4 v_opening; // width, sill and head as shares of the bay and the storey; flags

const float FOOT_SHADE = 0.62; // the foot of a wall in the street's shade
const float FOOT_STOREYS = 0.47; // its height, storeys (1.4 m of a 3 m storey)
#include "res://render/shaders/building_room_tone.gdshaderinc"

#include "res://render/shaders/building_colour.gdshaderinc"
#include "res://render/shaders/hash.gdshaderinc"
#include "res://render/shaders/building_lamps.gdshaderinc"
#define glass_sky_floor 0.0
#include "res://render/shaders/building_sky_glass.gdshaderinc"

void vertex() {
	int chunk = (int(COLOR.a * BYTE_TOP + 0.5) & ~FAR_OFFICE_BIT) >> SURFACE_KIND_BITS;
	if (((chunk_mask >> chunk) & 1) != 0) {
		VERTEX = vec3(0.0); // every vertex of its triangles at one point: degenerate, never rasterized
	}
	v_world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	v_wn = normalize((MODEL_MATRIX * vec4(NORMAL, 0.0)).xyz);
	v_opening = CUSTOM0;
}

void fragment() {
	int kind = int(COLOR.a * BYTE_TOP + 0.5) & KIND_MASK;
	vec3 n = normalize(v_wn);
	vec3 albedo = srgb_to_linear(COLOR.rgb);
	vec3 glass = mix(far_glass_dark, uniform_linear(glass_sky), glass_floor) * (1.0 - far_glass_night * night);
	vec3 emit = vec3(0.0);
	float rough = BUILDING_FAR_ROUGHNESS, metal = 0.0;
	if (kind == SK_PANE || kind == SK_PANE_DARK) {
		albedo = glass;
		rough = far_rough.x;
	}
	if (kind == SK_FAR_WALL) {
		vec2 foot = grid_footprint(UV);
		float open = (int(v_opening.a * BYTE_TOP + 0.5) & WINDOW_FLAG_BLANK) != 0 ? 0.0 : 1.0;
		float m = open * clamp(pulse(UV.x, foot.x, 0.5 - 0.5 * v_opening.r, 0.5 + 0.5 * v_opening.r) * pulse(UV.y, foot.y, v_opening.g, v_opening.b), 0.0, 1.0);
		vec2 room = floor(UV);
		float lot = h01(room_cell(room));
		float sub = far_pick_sub(foot);
		float office = (int(COLOR.a * BYTE_TOP + 0.5) & FAR_OFFICE_BIT) != 0 ? 1.0 : 0.0;
		vec3 lamp = far_room_lamp(lot, office, sub);
		float lit = far_room_lit(lot, room, 0u, office, sub); // the grid's x runs on per building: no salt of its own
		// the glass of the levels before it (building_far_window.gdshaderinc): next to nothing scattered, the room through it and the sky on it
		vec3 V = normalize(v_world - CAMERA_POSITION_WORLD);
		float cosv = clamp(dot(-V, n), 0.0, 1.0);
		float fresnel = schlick(cosv);
		albedo = mix(albedo, PANE_ALBEDO, m) * mix(FOOT_SHADE, 1.0, smoothstep(0.0, FOOT_STOREYS, UV.y));
		rough = mix(rough, far_rough.y, m);
		emit = ((lamp * lit * night * window_glow + pane_mean(room_tone(V, n, FLOOR_HOME)) * room_daylight) * (1.0 - fresnel) + glass_reflection(V, n, fresnel, GLASS_ENV)) * m;
	}
	albedo *= wall_gain;
	if (kind == SK_LAMP) emit += albedo * night;
	ALBEDO = surface_out(albedo);
	EMISSION = surface_out(emit);
	ROUGHNESS = rough;
	METALLIC = metal;
	NORMAL = normalize((VIEW_MATRIX * vec4(n, 0.0)).xyz);
}
