// What the terrain (terrain.gdshader, Terrain3D's override) and the ground seams beside the roads (terrain_seam.gdshader) share: the
// private uniforms Terrain3D sets on its material (copied onto the seams' by render/city_world.gd), the look's uniforms, the varyings
// the ground's fragment reads, a world point's texel in the region maps, and the land's height and painted ground at a point of the
// grid (ground_height, ground_control): a region's where one stands, the far maps' anywhere else (the fine ground follows the view,
// native/src/render/terrain_bridge.h). Vertex stage adapted from Terrain3D's minimum.gdshader (MIT, (c) Cory Petkovsek, Roope
// Palmroos and contributors).

// Defined Constants
#define MAX_REGIONS 1024 // Match Terrain3DMaterial.max_regions. Set to 64-256 for mobile
// Terrain3D's defaults for the private uniforms it sets itself (its minimum.gdshader): the clipmap's mesh size, the mouse
// camera's layer (32), a region's side in vertices and its inverse, the region map's side; LOD0 is a clipmap scale under the
// vertex spacing by less than its slack.
#define T3D_MESH_SIZE 48.f
#define T3D_MOUSE_LAYER 0x80000000u
#define T3D_REGION_SIZE 1024.0
#define T3D_REGION_TEXEL 0.0009765625
#define T3D_REGION_MAP_SIZE 32
#define T3D_LOD0_SLACK 1e-6
// The nearest level stands unmorphed while a vertex is within (mesh size + this many cells) of the target (Terrain3D's
// minimum.gdshader vertex_lerp).
#define T3D_MORPH_FROM_CELLS 4.0

// Inline Functions
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
    #define fma(a, b, c) ((a) * (b) + (c))
    #define dFdxCoarse(a) dFdx(a)
    #define dFdyCoarse(a) dFdy(a)
#endif

// Private uniforms
group_uniforms private;
uniform vec3 _target_pos = vec3(0.f);
uniform float _mesh_size = T3D_MESH_SIZE;
uniform float _subdiv = 1.f;
uniform uint _background_mode = 1u; // NONE = 0, FLAT = 1, NOISE = 2
uniform uint _mouse_layer = T3D_MOUSE_LAYER;
uniform float _vertex_spacing = 1.0;
uniform float _vertex_density = 1.0; // = 1./_vertex_spacing
uniform float _region_size = T3D_REGION_SIZE;
uniform float _region_texel_size = T3D_REGION_TEXEL; // = 1./region_size
uniform int _region_map_size = T3D_REGION_MAP_SIZE;
uniform int _region_map[T3D_REGION_MAP_SIZE * T3D_REGION_MAP_SIZE];
uniform highp sampler2DArray _height_maps : repeat_disable;
uniform highp sampler2DArray _control_maps : repeat_disable;
group_uniforms;

#include "res://render/shaders/terrain_uniforms.gdshaderinc"

// Varyings & Types
global uniform float map_grid;
varying vec3 v_vertex;
varying vec3 v_ground_n; // the ground's normal at the vertices, interpolated: what slope and light read, with no step at each cell
varying float v_vertex_xz_dist;
varying vec3 v_camera_pos;

// Takes in world space XZ (UV) coordinates
// Returns ivec3 with:
// XY: (0 to _region_size - 1) coordinates within a region
// Z: layer index used for texturearrays, -1 if not in a region
ivec3 get_index_coord(const vec2 uv) {
	vec2 r_uv = round(uv);
	ivec2 pos = ivec2(floor(r_uv * _region_texel_size)) + (_region_map_size / 2);
	int bounds = int(uint(pos.x | pos.y) < uint(_region_map_size));
	int map_val = _region_map[pos.y * _region_map_size + pos.x];
	int raw_index = map_val - 1;
	// Real regions limited by max_regions
	int is_region = bounds * int(raw_index >= 0) * int(raw_index < MAX_REGIONS);
	// Editor dummies are negative; keep them != -1 so the shader still sees a region
	int is_dummy = bounds * int(map_val < 0);
	int layer_index = is_region * raw_index
	               + is_dummy * (map_val - 1)
	               - (1 - is_region - is_dummy);
	return ivec3(ivec2(mod(r_uv, _region_size)), layer_index);
}

// The whole map every `far_cells` vertices, one texel each: its heights and its painted ground (control words, as a region's). What
// the land is read from where no region stands (native/src/render/terrain_bridge.cpp make_far).
uniform highp sampler2D far_heights : repeat_disable, filter_nearest;
uniform highp sampler2D far_controls : repeat_disable, filter_nearest;
uniform float far_cells; // set with the maps (tuning render.far_land_cells)

// A grid point (vertices from the map's middle) in the far maps' texels: texel i holds the vertex i x far_cells from the map's corner.
vec2 far_texel(vec2 grid) {
	return (grid + map_half * _vertex_density) / far_cells;
}

// Whether a world point lies at the map's end or past it: the land stops there (the wall and the sea's floor).
bool past_map(vec2 xz) {
	return max(abs(xz.x), abs(xz.y)) >= map_half;
}

// The land's height at a grid point: its region's sample, or between the far map's four round it.
float ground_height(vec2 grid) {
	ivec3 at = get_index_coord(grid);
	if (at.z >= 0) return texelFetch(_height_maps, at, 0).r;
	vec2 t = far_texel(round(grid));
	ivec2 last = textureSize(far_heights, 0) - 1;
	ivec2 a = clamp(ivec2(floor(t)), ivec2(0), last), b = min(a + 1, last);
	vec2 f = clamp(t - vec2(a), 0.0, 1.0);
	float low = mix(texelFetch(far_heights, a, 0).r, texelFetch(far_heights, ivec2(b.x, a.y), 0).r, f.x);
	float high = mix(texelFetch(far_heights, ivec2(a.x, b.y), 0).r, texelFetch(far_heights, b, 0).r, f.x);
	return mix(low, high, f.y);
}

// The land's painted ground at a grid point (Terrain3D's control word): its region's, or the far map's nearest.
uint ground_control(vec2 grid) {
	ivec3 at = get_index_coord(grid);
	if (at.z >= 0) return floatBitsToUint(texelFetch(_control_maps, at, 0)).r;
	ivec2 last = textureSize(far_controls, 0) - 1;
	return floatBitsToUint(texelFetch(far_controls, clamp(ivec2(floor(far_texel(round(grid)) + 0.5)), ivec2(0), last), 0)).r;
}

// The ground's normal at grid corner `at` (vertices), from its four neighbours `cells` apart (the terrain's own, per vertex); level
// at the map's end.
vec3 ground_normal_at(vec2 at, float cells) {
	if (max(abs(at.x), abs(at.y)) + cells >= map_half * _vertex_density) return vec3(0.0, 1.0, 0.0);
	return vec3(ground_height(at - vec2(cells, 0.0)) - ground_height(at + vec2(cells, 0.0)), 2.0 * cells * _vertex_spacing,
			ground_height(at - vec2(0.0, cells)) - ground_height(at + vec2(0.0, cells)));
}

#include "res://render/shaders/terrain_layers.gdshaderinc"
#include "res://render/shaders/ground_paint.gdshaderinc"
#include "res://render/shaders/shore.gdshaderinc"
#include "res://render/shaders/rock_strata.gdshaderinc"
#include "res://render/shaders/ecology.gdshaderinc"

global uniform float rain_wet;
