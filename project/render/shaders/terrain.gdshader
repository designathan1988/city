// Toon terrain for Terrain3D (shader override). Vertex stage adapted from Terrain3D's minimum.gdshader
// (MIT, (c) Cory Petkovsek, Roope Palmroos and contributors); fragment paints flat stylised colours.
shader_type spatial;
render_mode blend_mix,depth_draw_opaque,cull_back,skip_vertex_transform;

// Inline Functions
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
    #define fma(a, b, c) ((a) * (b) + (c))
    #define dFdxCoarse(a) dFdx(a)
    #define dFdyCoarse(a) dFdy(a)
#endif

// Private uniforms: what Terrain3D sets on its material for the clipmap's meshes (it keeps no region: the land's maps are below).
group_uniforms private;
uniform vec3 _target_pos = vec3(0.f);
uniform float _mesh_size = 48.f;
uniform float _subdiv = 1.f;
uniform float _vertex_spacing = 1.0;
uniform float _vertex_density = 1.0; // = 1./_vertex_spacing
group_uniforms;

// The land's maps (native/src/render/terrain_bridge.h sets them). The fine ground: a layer a region of `land_region` samples a
// side, for the regions round the view; a region's layer is its place modulo `fine_side`, and fine_held says which region each
// layer holds (its index + 1, 0 none). The far ground: the whole map every `far_cells` samples, in `far_tiles` x `far_tiles`
// tiles of `far_tile` samples a side, read wherever no fine region stands.
#define FINE_LAYERS 64 // terrain_bridge.cpp FINE_SIDE_MOST, squared
group_uniforms land;
uniform highp sampler2DArray fine_heights : repeat_disable, filter_nearest;
uniform highp sampler2DArray fine_controls : repeat_disable, filter_nearest;
uniform highp sampler2DArray far_heights : repeat_disable, filter_nearest;
uniform highp sampler2DArray far_controls : repeat_disable, filter_nearest;
uniform int fine_held[FINE_LAYERS];
uniform int fine_side = 1;
uniform int land_side = 0; // the land's samples a side; 0 while no land stands: every point is past it
uniform int land_region = 256;
uniform int far_cells = 8;
uniform int far_tiles = 1;
uniform int far_tile = 128;
group_uniforms;

#include "res://render/shaders/terrain_uniforms.gdshaderinc"

// Varyings & Types
// Some are required for editor functions
global uniform float map_grid;
varying vec3 v_vertex;
varying vec3 v_ground_n; // the ground's normal at the vertices, interpolated: what slope and light read, with no step at each cell
varying float v_vertex_xz_dist;
varying vec3 v_camera_pos;

////////////////////////
// Vertex
////////////////////////

// A grid point (vertices from the map's middle, as UV) as the land's sample nearest it, counted from the map's corner.
ivec2 land_sample(vec2 grid) {
	return ivec2(round(grid)) + land_side / 2;
}

// Whether a sample lies past the land: it ends there (the wall, and the sea's floor beyond).
bool past_map(ivec2 s) {
	return any(lessThan(s, ivec2(0))) || any(greaterThanEqual(s, ivec2(land_side)));
}

// The same sample held inside the land.
ivec2 land_sample_in(vec2 grid) {
	return clamp(land_sample(grid), ivec2(0), ivec2(max(land_side - 1, 0)));
}

// A sample of the land in the fine maps: its texel and its layer, the layer -1 where no fine region stands.
ivec3 fine_texel(ivec2 s) {
	ivec2 r = s / land_region;
	ivec2 slot = r - (r / fine_side) * fine_side;
	int layer = slot.y * fine_side + slot.x;
	return ivec3(s - r * land_region, fine_held[layer] == r.y * (land_side / land_region) + r.x + 1 ? layer : -1);
}

// A far sample (the whole map's, counted from its corner) in the far maps: its tile's texel and layer.
ivec3 far_texel(ivec2 t) {
	t = clamp(t, ivec2(0), ivec2(max(land_side / far_cells - 1, 0)));
	ivec2 tile = t / far_tile;
	return ivec3(t - tile * far_tile, tile.y * far_tiles + tile.x);
}

// The land's height at a grid point: its fine region's sample, or between the far ground's four round it (the clipmap's far
// levels stand on the far samples themselves: the same heights either way).
float ground_height(vec2 grid) {
	ivec2 s = land_sample_in(grid);
	ivec3 at = fine_texel(s);
	if (at.z >= 0) return texelFetch(fine_heights, at, 0).r;
	ivec2 a = s / far_cells;
	vec2 f = vec2(s - a * far_cells) / float(far_cells);
	float low = mix(texelFetch(far_heights, far_texel(a), 0).r, texelFetch(far_heights, far_texel(a + ivec2(1, 0)), 0).r, f.x);
	float high = mix(texelFetch(far_heights, far_texel(a + ivec2(0, 1)), 0).r, texelFetch(far_heights, far_texel(a + ivec2(1, 1)), 0).r, f.x);
	return mix(low, high, f.y);
}

// The land's painted ground at a grid point, a control word (ground_paint.gdshaderinc): its fine region's, or the far ground's
// nearest.
uint ground_control(vec2 grid) {
	ivec2 s = land_sample_in(grid);
	ivec3 at = fine_texel(s);
	if (at.z >= 0) return floatBitsToUint(texelFetch(fine_controls, at, 0).r);
	return floatBitsToUint(texelFetch(far_controls, far_texel((s + far_cells / 2) / far_cells), 0).r);
}

// The land's height between the grid's points (the nearest level's sub-texel vertices).
float interpolated_height(vec2 pos) {
	vec2 at = floor(pos), f = fract(pos);
	float low = mix(ground_height(at), ground_height(at + vec2(1.0, 0.0)), f.x);
	float high = mix(ground_height(at + vec2(0.0, 1.0)), ground_height(at + vec2(1.0, 1.0)), f.x);
	return mix(low, high, f.y);
}

// The ground's normal at grid corner `at`, from its four neighbours `cells` apart; level where one of them lies past the land.
vec3 ground_normal_at(vec2 at, float cells) {
	ivec2 s = land_sample(at);
	if (past_map(s - int(cells)) || past_map(s + int(cells))) return vec3(0.0, 1.0, 0.0);
	return vec3(ground_height(at - vec2(cells, 0.0)) - ground_height(at + vec2(cells, 0.0)), 2.0 * cells * _vertex_spacing,
			ground_height(at - vec2(0.0, cells)) - ground_height(at + vec2(0.0, cells)));
}

void vertex() {
	// Save Camera Position to varying for access in later functions
	v_camera_pos = MAIN_CAM_INV_VIEW_MATRIX[3].xyz;

	// Get vertex of flat plane in world coordinates and set world UV
	v_vertex = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;

	// Distance from target node to vertex on a flat plane
	v_vertex_xz_dist = length(v_vertex.xz - _target_pos.xz);

	// Geomorph vertex across clipmap LODs, set end and start for linear height interpolate
	float scale = MODEL_MATRIX[0][0];
	float inv_scale = 1.0 / scale;
	float max_xz = max(abs(v_vertex.x - _target_pos.x), abs(v_vertex.z - _target_pos.z));
	float vertex_lerp = smoothstep(0.0, 1.0, (max_xz * inv_scale - _mesh_size - 4.0) / (_mesh_size - 4.0));
	vec2 vertex_fract = fract(VERTEX.xz * 0.5) * 2.0;
	// For LOD0 morph from a regular grid to an alternating grid to align with LOD1+
	vec2 shift = (scale < _vertex_spacing / _subdiv + 1e-6) ? // LOD0 or not
		// Shift from regular to symmetric
		mix(vertex_fract, vec2(vertex_fract.x, -vertex_fract.y),
			round(fract(round(mod(v_vertex.z * inv_scale, 4.0)) *
			round(mod(v_vertex.x * inv_scale, 4.0)) * 0.25))) :
		// Symmetric shift
		vertex_fract * round((fract(v_vertex.xz * 0.25 * inv_scale) - 0.5) * 4.0);
	vec2 start_pos = v_vertex.xz * _vertex_density;
	vec2 end_pos = (v_vertex.xz - shift * scale) * _vertex_density;
	v_vertex.xz -= shift * scale * vertex_lerp;

	// The grid's coordinates: a unit a vertex of the land, from the map's middle.
	UV = v_vertex.xz * _vertex_density;

	// The vertex's height. The game holes nothing in the ground (the roads' seams lie over it), so every vertex is drawn.
	bool past = past_map(land_sample(start_pos));
	float h;
	// This branch is static for each of the clipmap segments
	// Interpolated reads only occur where sub-texel values are required.
	if (scale < _vertex_spacing) {
		h = interpolated_height(UV);
	} else {
		h = mix(ground_height(start_pos), ground_height(end_pos), vertex_lerp);
	}
	v_vertex.y = past ? past_map_y_m : h;
	// past the map a vertex stands at its end: the clipmap follows the camera, and the wall it makes must not
	if (past) v_vertex.xz = clamp(v_vertex.xz, vec2(-map_half), vec2(map_half));
	v_ground_n = ground_normal_at(round(UV), max(1.0, scale * _vertex_density));

	// Convert model space to view space w/ skip_vertex_transform render mode
	VERTEX = (VIEW_MATRIX * vec4(v_vertex, 1.0)).xyz;
	NORMAL = normalize((MODELVIEW_MATRIX * vec4(NORMAL, 0.0)).xyz);
	BINORMAL = normalize((MODELVIEW_MATRIX * vec4(BINORMAL, 0.0)).xyz);
	TANGENT = normalize((MODELVIEW_MATRIX * vec4(TANGENT, 0.0)).xyz);
}
////////////////////////
// Fragment: grass map in world space, sand at the shore, soil and rock on slopes steeper than loose debris rests
////////////////////////

#include "res://render/shaders/terrain_layers.gdshaderinc"
#include "res://render/shaders/ground_paint.gdshaderinc"
#include "res://render/shaders/shore.gdshaderinc"

global uniform float rain_wet;

void fragment() {
	vec2 uv = UV; // Terrain3D injects editor overlays that read `uv`
	vec3 w_normal = normalize(v_ground_n);
	vec2 past = abs(v_vertex.xz);
	if (max(past.x, past.y) >= map_half - 0.5) { // the wall at the end of the map: rock on its own plane
		w_normal = past.x > past.y ? vec3(sign(v_vertex.x), 0.0, 0.0) : vec3(0.0, 0.0, sign(v_vertex.z));
	}
	// The painted ground's cell here, its four corners mixed by distance: the grid's where a fine region stands, the far ground's
	// (far_cells of the grid a side) anywhere else.
	float cell = fine_texel(land_sample_in(floor(uv))).z >= 0 ? 1.0 : float(far_cells);
	vec2 corner = floor(uv / cell) * cell;
	vec2 fc = (uv - corner) / cell;
	PaintCorner p00 = read_paint(corner), p10 = read_paint(corner + vec2(cell, 0.0)), p01 = read_paint(corner + vec2(0.0, cell)), p11 = read_paint(corner + vec2(cell));
	vec4 cw = vec4((1.0 - fc.x) * (1.0 - fc.y), fc.x * (1.0 - fc.y), (1.0 - fc.x) * fc.y, fc.x * fc.y);
	float slope = 1.0 - w_normal.y;
	float gully = gully_weight(dot(cw, vec4(p00.gully, p10.gully, p01.gully, p11.gully)), dot(cw, vec4(p00.wipe, p10.wipe, p01.wipe, p11.wipe)), slope);
	float gully_shade = 1.0;
	if (gully > 0.0) w_normal = gully_normal(w_normal, v_vertex.xz, gully, gully_shade);
	float detail = 1.0 - smoothstep(DETAIL_NEAR_M, DETAIL_FAR_M, length(v_vertex.xz - v_camera_pos.xz));
	vec3 tint = biome_tint[map_biome];
	float biome_cover = dot(cw, vec4(p00.biome_amount, p10.biome_amount, p01.biome_amount, p11.biome_amount));
	if (biome_cover > 0.0) tint = mix(tint, biome_tint[(cw.x + cw.y > cw.z + cw.w ? (cw.x > cw.y ? p00.biome : p10.biome) : (cw.z > cw.w ? p01.biome : p11.biome))], biome_cover);
	float per_pixel = max(fwidth(v_vertex.x), fwidth(v_vertex.z));
	vec3 g;
	float n;
	vec3 c = grass_ground(v_vertex.xz, detail, tint, map_biome, per_pixel, g, n);
	float wander = (n - 0.5) * 0.08;
	float dirt_w = smoothstep(dirt_from, dirt_to, slope + wander) * (1.0 - smoothstep(rock_slope, rock_slope + 0.1, slope));
	float rock_w = smoothstep(rock_slope, rock_slope + 0.1, slope + wander);
	vec3 wdx = dFdx(v_vertex), wdy = dFdy(v_vertex);
	vec2 tilt = grass_tilt(v_vertex.xz, per_pixel, 1.0 - max(dirt_w, rock_w));
	NORMAL = mat3(VIEW_MATRIX) * normalize(w_normal + vec3(tilt.x, 0.0, tilt.y));
	if (dirt_w > 0.0) {
		c = mix(c, land_triplanar(dirt_layer, v_vertex / dirt_m, wdx / dirt_m, wdy / dirt_m, w_normal), dirt_w);
	}
	c = shore_over(c, v_vertex, slope, wdx, wdy);
	if (rock_w > 0.0) {
		c = mix(c, mix(land_triplanar(rock_layer, v_vertex / rock_m, wdx / rock_m, wdy / rock_m, w_normal), rock, 0.25), rock_w);
	}
	float painted = dot(cw, vec4(p00.amount, p10.amount, p01.amount, p11.amount));
	if (painted > 0.004) {
		vec3 dirt_c = land(dirt_layer, fract(v_vertex.xz / dirt_m), wdx.xz / dirt_m, wdy.xz / dirt_m);
		float grain = dot(land(dirt_layer, fract(v_vertex.xz * 0.27), wdx.xz * 0.27, wdy.xz * 0.27), vec3(0.33));
		float speck = dot(land(dirt_layer, fract(v_vertex.xz * 1.325), wdx.xz * 1.325, wdy.xz * 1.325), vec3(0.33));
		// Each surface's colour is worked out once, whatever number of corners carry it (it depends on the surface alone: the
		// derivatives are passed in); the corners then add it in their order, as each did with its own.
		bool on00 = p00.surface > 0 && cw.x * p00.amount > 0.0;
		bool on10 = p10.surface > 0 && cw.y * p10.amount > 0.0;
		bool on01 = p01.surface > 0 && cw.z * p01.amount > 0.0;
		bool on11 = p11.surface > 0 && cw.w * p11.amount > 0.0;
		vec3 c00 = vec3(0.0), c10 = vec3(0.0), c01 = vec3(0.0), c11 = vec3(0.0);
		if (on00) c00 = surface_colour(p00.surface, g, dirt_c, v_vertex, wdx, wdy, w_normal, grain, speck);
		if (on10) {
			if (on00 && p10.surface == p00.surface) c10 = c00;
			else c10 = surface_colour(p10.surface, g, dirt_c, v_vertex, wdx, wdy, w_normal, grain, speck);
		}
		if (on01) {
			if (on00 && p01.surface == p00.surface) c01 = c00;
			else if (on10 && p01.surface == p10.surface) c01 = c10;
			else c01 = surface_colour(p01.surface, g, dirt_c, v_vertex, wdx, wdy, w_normal, grain, speck);
		}
		if (on11) {
			if (on00 && p11.surface == p00.surface) c11 = c00;
			else if (on10 && p11.surface == p10.surface) c11 = c10;
			else if (on01 && p11.surface == p01.surface) c11 = c01;
			else c11 = surface_colour(p11.surface, g, dirt_c, v_vertex, wdx, wdy, w_normal, grain, speck);
		}
		vec3 sum = vec3(0.0);
		float weights = 0.0;
		if (on00) { sum += c00 * cw.x * p00.amount; weights += cw.x * p00.amount; }
		if (on10) { sum += c10 * cw.y * p10.amount; weights += cw.y * p10.amount; }
		if (on01) { sum += c01 * cw.z * p01.amount; weights += cw.z * p01.amount; }
		if (on11) { sum += c11 * cw.w * p11.amount; weights += cw.w * p11.amount; }
		if (weights > 0.0) c = mix(c, sum / weights, clamp(weights, 0.0, 1.0));
	}
	c *= gully_shade;
	c *= 1.0 - 0.4 * step(map_half - 0.5, max(abs(v_vertex.x), abs(v_vertex.z)));
	if (map_grid > 0.5) { // the Layers popover: the zoning grid over the ground, one line a cell, as wide as a pixel of it
		vec2 cell = abs(fract(v_vertex.xz / grid_m - 0.5) - 0.5) / fwidth(v_vertex.xz / grid_m);
		c = mix(c, vec3(1.0), (1.0 - clamp(min(cell.x, cell.y), 0.0, 1.0)) * grid_alpha);
	}
	ALBEDO = c;
	ROUGHNESS = mix(0.95, 0.6, rain_wet);
}
