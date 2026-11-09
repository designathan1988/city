// Instanced facade parts (native/src/render/buildings/shell.h BoxInstance): frames, sills, railings, shutters, brise
// fins, roof rails and fence panels drawn as unit-cube MultiMesh instances. INSTANCE_CUSTOM = (sRGB tone rgb, finish
// layer); the geometry carries no finish vertex attribute, so this is building.gdshader with the finish and tone read
// from the instance instead of COLOR/UV2.
// The same material draws the flags of the roof masts (flag_cloth.h), one cloth instance each, UV2.x = 1: the hoist's top is
// the instance origin, the basis lengths its width and height, and INSTANCE_CUSTOM its design (three colours packed as
// 0xRRGGBB and the pattern). The cloth lies downwind of the map's wind (wind_x, wind_z, as the trees) and waves as the
// Roadcraft's (flagLayer.ts: a wave travelling from the hoist to the fly, the fly drooping where the wave is slack); its
// designs are the Roadcraft's fourteen (flags.ts FLAG_PATTERNS, native types.h FlagPattern). One shader for both, as
// props.gdshader does: each spatial shader's variants cost ~3 MiB.
render_mode cull_back, skip_vertex_transform;
#include "res://render/shaders/contracts/buildings.gdshaderinc"

global uniform float night;
global uniform float wind_x; // the way the wind blows, x and z of a unit vector (render/sky_view.gd)
global uniform float wind_z;

uniform sampler2DArray finish_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2DArray finish_normal : filter_linear_mipmap, repeat_enable;
uniform sampler2DArray finish_rough : filter_linear_mipmap, repeat_enable;
uniform float finish_m[FIN_COUNT];
uniform float finish_normal_k[FIN_COUNT];
uniform float finish_metal[FIN_COUNT];
uniform float wall_gain = 1.0;
uniform float cloth_roughness; // the Roadcraft's flag material (flagLayer.ts MeshStandardMaterial; materials.json building_parts)
// The fourteen designs (native types.h FlagPattern, flags.ts FLAG_PATTERNS): their codes, and their geometry as shares of the
// cloth: thirds; a cross's bar halves; the Nordic cross's upright; a saltire's half width; a canton's width and height; the
// stripes' count; a border's widths; a disc's stretch and radius.
const int FLAG_HALVES_H = 1, FLAG_HALVES_V = 2, FLAG_THIRDS_H = 3, FLAG_THIRDS_V = 4, FLAG_CROSS = 5, FLAG_NORDIC = 6, FLAG_DIAGONAL = 7;
const int FLAG_SALTIRE = 8, FLAG_CANTON = 9, FLAG_STRIPES = 10, FLAG_BORDER = 11, FLAG_DISC = 12, FLAG_TRIANGLE = 13;
const float THIRD = 0.333, TWO_THIRDS = 0.667, CROSS_X = 0.09, CROSS_Y = 0.12, NORDIC_AT = 0.36, NORDIC_X = 0.07, NORDIC_Y = 0.1;
const float SALTIRE_HALF = 0.11, CANTON_W = 0.42, STARS_H = 0.54, STRIPES = 13.0, BORDER_X = 0.1, BORDER_Y = 0.15, BORDER_X_FAR = 0.9, BORDER_Y_FAR = 0.85;
const vec2 DISC_STRETCH = vec2(1.5, 1.0);
const float DISC_R = 0.3, LEAST_WIND = 1e-4; // a wind under this has no way: the flag hangs along x

// The Roadcraft's wave (flagLayer.ts): its clock runs WIND_PACE times real time (wind.ts), the two sines travel from the
// hoist, their swing grows with the width, the fly droops by the square of its reach.
const float WIND_PACE = 1.6;
const float CLOTH_CLOCK = 4.2, CLOTH_RIPPLE_K = 7.5, CLOTH_RIPPLE_A = 0.6, CLOTH_FLUTTER_K = 13.0, CLOTH_FLUTTER_CLOCK = 1.7, CLOTH_FLUTTER_ACROSS = 2.0, CLOTH_FLUTTER_A = 0.25;
const float SWING_PER_WIDTH = 0.11, DROOP = 0.05, DROOP_SLACK = 1.2;
const vec2 SEED_PER_M = vec2(0.13, 0.07); // each flag its own phase, by where it stands
const float PACKED_ROUND = 0.5, BYTE_MAX = 255.0;
const int BYTE_BITS = 8, BYTE_MASK = 255;

varying vec3 v_world;
varying vec3 v_wn;
varying flat vec4 v_custom;
varying flat float v_cloth;
varying vec2 v_uv;

#include "res://render/shaders/building_colour.gdshaderinc"
#include "res://render/shaders/building_common.gdshaderinc"

// A colour packed as 0xRRGGBB in a float (exact under 2^24), as sRGB.
vec3 unpack_rgb(float packed) {
	int c = int(packed + PACKED_ROUND);
	return vec3(float((c >> (2 * BYTE_BITS)) & BYTE_MASK), float((c >> BYTE_BITS) & BYTE_MASK), float(c & BYTE_MASK)) / BYTE_MAX;
}

// The Roadcraft's designs (flagLayer.ts flagColour): x from the hoist, y from the top; k a FlagPattern.
vec3 flag_colour(vec2 uv, int k, vec3 c0, vec3 c1, vec3 c2) {
	float x = uv.x, y = 1.0 - uv.y;
	if (k == FLAG_HALVES_H) return y < 0.5 ? c0 : c1;
	if (k == FLAG_HALVES_V) return x < 0.5 ? c0 : c1;
	if (k == FLAG_THIRDS_H) return y < THIRD ? c0 : (y < TWO_THIRDS ? c1 : c2);
	if (k == FLAG_THIRDS_V) return x < THIRD ? c0 : (x < TWO_THIRDS ? c1 : c2);
	if (k == FLAG_CROSS) return (abs(x - 0.5) < CROSS_X || abs(y - 0.5) < CROSS_Y) ? c1 : c0;
	if (k == FLAG_NORDIC) return (abs(x - NORDIC_AT) < NORDIC_X || abs(y - 0.5) < NORDIC_Y) ? c1 : c0;
	if (k == FLAG_DIAGONAL) return y < x ? c0 : c1;
	if (k == FLAG_SALTIRE) return (abs(y - x) < SALTIRE_HALF || abs(y - (1.0 - x)) < SALTIRE_HALF) ? c1 : c0;
	if (k == FLAG_CANTON) return (x < CANTON_W && y < 0.5) ? c1 : c0;
	if (k == FLAG_STRIPES) {
		if (x < CANTON_W && y < STARS_H) return c2;
		return mod(floor(y * STRIPES), 2.0) < 1.0 ? c0 : c1;
	}
	if (k == FLAG_BORDER) return (x < BORDER_X || x > BORDER_X_FAR || y < BORDER_Y || y > BORDER_Y_FAR) ? c1 : c0;
	if (k == FLAG_DISC) return length((uv - 0.5) * DISC_STRETCH) < DISC_R ? c1 : c0;
	if (k == FLAG_TRIANGLE) return x < 0.5 * (1.0 - abs(y - 0.5) * 2.0) ? c1 : c0;
	return c0;
}

// The cloth's vertex in the world: x downwind of the hoist, y down it, the wave across.
vec3 cloth_vertex(vec3 p, vec2 uv, vec3 origin, float width, float height, vec3 along, vec3 across) {
	float fly = uv.x;
	float seed = dot(origin.xz, SEED_PER_M);
	float t = TIME * WIND_PACE * CLOTH_CLOCK + seed;
	float wave = sin(fly * CLOTH_RIPPLE_K - t) * CLOTH_RIPPLE_A + sin(fly * CLOTH_FLUTTER_K - t * CLOTH_FLUTTER_CLOCK + uv.y * CLOTH_FLUTTER_ACROSS) * CLOTH_FLUTTER_A;
	float out_m = p.z + wave * fly * SWING_PER_WIDTH * width;
	float y = p.y - fly * fly * DROOP * (DROOP_SLACK - abs(wave));
	return origin + along * (p.x * width) + vec3(0.0, y * height, 0.0) + across * out_m;
}

#include "res://render/shaders/building_level.gdshaderinc"

void vertex() {
	v_custom = INSTANCE_CUSTOM;
	v_cloth = UV2.x;
	v_uv = UV;
	if (UV2.x > 0.5) {
		vec2 w = vec2(wind_x, wind_z);
		w = length(w) > LEAST_WIND ? normalize(w) : vec2(1.0, 0.0);
		vec3 along = vec3(w.x, 0.0, w.y), across = vec3(-w.y, 0.0, w.x);
		v_world = cloth_vertex(VERTEX, UV, MODEL_MATRIX[3].xyz, length(MODEL_MATRIX[0].xyz), length(MODEL_MATRIX[1].xyz), along, across);
		v_wn = across * NORMAL.z; // the mesh's two faces, each its own way
		VERTEX = (VIEW_MATRIX * vec4(v_world, 1.0)).xyz;
		NORMAL = normalize((VIEW_MATRIX * vec4(v_wn, 0.0)).xyz);
	} else {
		v_world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
		v_wn = normalize((MODEL_MATRIX * vec4(NORMAL, 0.0)).xyz);
		VERTEX = (MODELVIEW_MATRIX * vec4(VERTEX, 1.0)).xyz;
		NORMAL = normalize((MODELVIEW_MATRIX * vec4(NORMAL, 0.0)).xyz);
		if (level_back > 0.0 && !IN_SHADOW_PASS) VERTEX += normalize(VERTEX) * level_back;
	}
}

void fragment() {
	if (level_keep < 1.0 && !IN_SHADOW_PASS && level_noise(FRAGCOORD.xy) >= level_keep) discard; // the hand-over (building_level.gdshaderinc)
	vec3 n = normalize(v_wn);
	if (v_cloth > 0.5) {
		vec3 c = flag_colour(v_uv, int(v_custom.a + PACKED_ROUND), unpack_rgb(v_custom.r), unpack_rgb(v_custom.g), unpack_rgb(v_custom.b));
		ALBEDO = surface_out(srgb_to_linear(c));
		ROUGHNESS = cloth_roughness;
		METALLIC = 0.0;
		NORMAL = normalize((VIEW_MATRIX * vec4(n, 0.0)).xyz);
	} else {
		vec3 albedo = srgb_to_linear(v_custom.rgb), nn = n;
		float rough = BUILDING_WALL_ROUGHNESS, metal = 0.0;
		finish_surface(int(floor(v_custom.a + 0.5)), n, 0.0, albedo, nn, rough, metal); // an instance's colour carries its wear (shell.cpp) // -1: no finish (int() would truncate -0.5 to layer 0)
		ALBEDO = surface_out(albedo * wall_gain);
		ROUGHNESS = rough;
		METALLIC = metal;
		NORMAL = normalize((VIEW_MATRIX * vec4(nn, 0.0)).xyz);
	}
}
