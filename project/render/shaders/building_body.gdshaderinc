// Buildings: one material for every building surface. COLOR.a is the
// surface kind written by native/src/render/buildings/shell.h: 0 shell face (UV2.x = cooked finish layer + 1, 0 = no
// finish), 3 far wall with painted windows (UV in bays/storeys, CUSTOM0 = window shares and flags), 4 lamp, 5 lawn. The panes,
// 1 and 2, are building_glass.gdshader's. Finishes: world-planar UVs in metres, map, normal map, roughness and metal mask. Far
// windows: building_far_window.gdshaderinc (CUSTOM0 and 3 written by shell.cpp). The mid level's bays are instances of one mesh
// that the vertex stage places and turns into these same faces (building_bay.gdshaderinc).
render_mode cull_back;
#include "res://render/shaders/contracts/buildings.gdshaderinc"
#include "res://render/shaders/bytes.gdshaderinc"


global uniform float night;
global uniform float building_recess; // 0 turns the worked-out recess off: an A/B of what it costs
global uniform vec3 sun_direction; // towards the sun, world space (daylight.gd)
global uniform float room_daylight; // how bright a room seen through its glass is by day, as light it sends out
global uniform float outside_on; // the share of outside lights lit at this hour
uniform sampler2DArray finish_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2DArray finish_normal : filter_linear_mipmap, repeat_enable;
uniform sampler2DArray finish_rough : filter_linear_mipmap, repeat_enable;
uniform float finish_m[FIN_COUNT];
uniform float finish_normal_k[FIN_COUNT];
uniform float finish_metal[FIN_COUNT];
uniform float wall_gain = 1.0;
uniform float lamp_glow;
uniform vec3 lamp_light : source_color; // what a lit lamp gives, whatever its body is by day
uniform vec4 lawn_tone; // a lawn's tone: its floor, its wide noise's share, its fine noise's share; its roughness
uniform vec2 lawn_noise; // the wide and the fine noise a metre
uniform float far_glass_rough; // the far wall's painted glass, its roughness
// The outside lamps switched on by 10 m cells, each its own pick: its column and row times two primes, offset to stay positive.
const float LAMP_CELL_PER_M = 0.1, LAMP_CELL_X = 31.0, LAMP_CELL_Z = 977.0, LAMP_CELL_SHIFT = 65536.0;
const float FINISH_ROUND = 0.001; // a finish's layer + 1, carried in a float, read up past its rounding
#include "res://render/shaders/building_glass_set.gdshaderinc"

varying vec3 v_world;
varying vec3 v_wn;
varying flat vec4 v_room0;
varying flat vec4 v_trim;
varying flat float v_recess; // how deep a far wall's panes stand, worked out in the fragment: 0 on a bay, whose recess is geometry
#ifdef RUIN
uniform sampler2D pieces : filter_nearest, repeat_disable; // three rows of a matrix a piece
varying flat vec3 v_turn0;
varying flat vec3 v_turn1;
varying flat vec3 v_turn2;
#endif

#include "res://render/shaders/building_bay.gdshaderinc"
#include "res://render/shaders/building_level.gdshaderinc"

void vertex() {
	v_room0 = CUSTOM0;
	v_trim = CUSTOM3;
	v_recess = RECESS_M * building_recess;
	if (UV2.y > BAY_MESH) {
		v_recess = 0.0;
		vec3 p = VERTEX;
		vec2 uv = UV, uv2 = UV2;
		vec4 col = COLOR, room, trim;
		bay_place(p, uv, uv2, col, room, trim, INSTANCE_CUSTOM, length(MODEL_MATRIX[0].xyz), length(MODEL_MATRIX[1].xyz));
		v_room0 = room;
		v_trim = trim;
		VERTEX = p;
		UV = uv;
		UV2 = uv2;
		COLOR = col;
	}
	v_world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	v_wn = normalize((MODEL_MATRIX * vec4(NORMAL, 0.0)).xyz);
#ifdef RUIN
	// The finishes keep to the wall as it stood, in v_world and v_wn; only where it is drawn and how it faces the light move.
	ivec4 id = ivec4(CUSTOM0 * BYTE_TOP + 0.5);
	int piece = id.r + id.g * BYTE_SPAN + id.b * TWO_BYTES_SPAN;
	ivec2 at = ivec2((piece % RUIN_PIECES_A_ROW) * 3, piece / RUIN_PIECES_A_ROW);
	vec4 r0 = texelFetch(pieces, at, 0), r1 = texelFetch(pieces, at + ivec2(1, 0), 0), r2 = texelFetch(pieces, at + ivec2(2, 0), 0);
	VERTEX = vec3(dot(r0, vec4(VERTEX, 1.0)), dot(r1, vec4(VERTEX, 1.0)), dot(r2, vec4(VERTEX, 1.0)));
	NORMAL = vec3(dot(r0.xyz, NORMAL), dot(r1.xyz, NORMAL), dot(r2.xyz, NORMAL));
	v_turn0 = r0.xyz;
	v_turn1 = r1.xyz;
	v_turn2 = r2.xyz;
#endif
	// What lies on the lot stays there: stepped back it sinks under the lawn, which is drawn by another material and stands still.
	if (level_back > 0.0 && !IN_SHADOW_PASS && int(COLOR.a * BYTE_TOP + 0.5) != SK_GROUND) VERTEX += level_local(MODEL_MATRIX, normalize(v_world - CAMERA_POSITION_WORLD) * level_back);
}

#include "res://render/shaders/building_colour.gdshaderinc"
#include "res://render/shaders/building_common.gdshaderinc"
#include "res://render/shaders/building_lamps.gdshaderinc"
#include "res://render/shaders/building_room_tone.gdshaderinc"
#include "res://render/shaders/building_sky_glass.gdshaderinc"
#include "res://render/shaders/building_far_window.gdshaderinc"

void fragment() {
#ifdef LEVEL_FADE
	if (!IN_SHADOW_PASS && level_noise(FRAGCOORD.xy) >= level_keep) discard;
#endif
	int kind = int(COLOR.a * BYTE_TOP + 0.5);
	vec3 n = normalize(v_wn);
	vec3 V = normalize(v_world - CAMERA_POSITION_WORLD);
	vec3 albedo = to_linear(COLOR.rgb), emit = vec3(0.0), nn = n;
	float rough = BUILDING_WALL_ROUGHNESS, metal = 0.0;
	if (kind == SK_LAMP) {
		emit = uniform_linear(lamp_light) * night * lamp_glow * step(h01(uint(floor(v_world.x * LAMP_CELL_PER_M) * LAMP_CELL_X + floor(v_world.z * LAMP_CELL_PER_M) * LAMP_CELL_Z + LAMP_CELL_SHIFT)), outside_on);
	} else if (kind == SK_LAWN) {
		albedo *= lawn_tone.x + lawn_tone.y * vnoise(v_world.xz * lawn_noise.x) + lawn_tone.z * vnoise(v_world.xz * lawn_noise.y);
		rough = lawn_tone.w;
	} else {
		finish_surface(int(floor(UV2.x + FINISH_ROUND)) - 1, n, albedo, nn, rough, metal);
		albedo *= wall_gain;
		if (kind == SK_FAR_WALL) {
			float glass;
			far_window(UV, v_room0, uint(fract(UV2.x) * BYTE_STEPS + 0.5), n, V, v_recess, albedo, emit, glass);
			rough = mix(rough, far_glass_rough, glass);
			nn = normalize(mix(nn, n, glass));
		}
	}
	ALBEDO = surface_out(albedo);
	EMISSION = surface_out(emit);
	ROUGHNESS = rough;
	METALLIC = metal;
#ifdef RUIN
	nn = vec3(dot(v_turn0, nn), dot(v_turn1, nn), dot(v_turn2, nn));
#endif
	NORMAL = normalize((VIEW_MATRIX * vec4(nn, 0.0)).xyz);
}
