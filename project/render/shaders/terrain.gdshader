// Toon terrain for Terrain3D (shader override). Vertex stage adapted from Terrain3D's minimum.gdshader
// (MIT, (c) Cory Petkovsek, Roope Palmroos and contributors); fragment paints flat stylised colours.
shader_type spatial;
render_mode blend_mix,depth_draw_opaque,cull_back,skip_vertex_transform;

// Defined Constants
#define MAX_REGIONS 1024 // Match Terrain3DMaterial.max_regions. Set to 64-256 for mobile

// Inline Functions
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
    #define fma(a, b, c) ((a) * (b) + (c))
    #define dFdxCoarse(a) dFdx(a)
    #define dFdyCoarse(a) dFdy(a)
#endif

// Private uniforms
// Commented uniforms aren't needed for this shader, but are available for your own needs.
group_uniforms private;
uniform vec3 _target_pos = vec3(0.f);
uniform float _mesh_size = 48.f;
uniform float _subdiv = 1.f;
uniform uint _background_mode = 1u; // NONE = 0, FLAT = 1, NOISE = 2
uniform uint _mouse_layer = 0x80000000u; // Layer 32
uniform float _vertex_spacing = 1.0;
uniform float _vertex_density = 1.0; // = 1./_vertex_spacing
uniform float _region_size = 1024.0;
uniform float _region_texel_size = 0.0009765625; // = 1./region_size
uniform int _region_map_size = 32;
uniform int _region_map[1024];
uniform highp sampler2DArray _height_maps : repeat_disable;
uniform highp sampler2DArray _control_maps : repeat_disable;
group_uniforms;

// Public uniforms
group_uniforms ground;
uniform sampler2D grass_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform float grass_m = 14.0; // metres per repeat of the grass map
uniform vec3 rock : source_color;
uniform float rock_slope; // 1 - normal.y; data/render/palette.json terrain (angle of repose)
uniform float patch_scale;
uniform sampler2DArray land_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform float dirt_layer = 8.0;
uniform float rock_layer = 2.0;
uniform float dirt_m = 13.6;
uniform float rock_m = 23.2;
uniform float dirt_from; // 1 - normal.y where soil starts to show; data/render/palette.json terrain (angle of repose)
uniform float dirt_to; // ... and where it has replaced the grass
uniform float map_half = 5120.0;
uniform float past_map_y_m; // past the map the land lies under the sea
uniform sampler2D grass_detail : filter_linear_mipmap_anisotropic, repeat_enable; // data, not colour: mean 0.5, doubled in the shader
uniform float detail_m = 1.6; // metres one tile of the grain covers
uniform float detail_near_px; // metres a pixel under which the grain is whole, and over which it is gone
uniform float detail_far_px;
uniform float detail_wide_from_px; // metres a pixel where the blades' colour starts to thin, gone at the next
uniform float detail_wide_to_px;
uniform float detail_wide_share;
uniform float detail_albedo = 0.9;
global uniform float night;
group_uniforms;

// Varyings & Types
// Some are required for editor functions
varying vec3 v_vertex;
varying float v_vertex_xz_dist;
varying vec3 v_camera_pos;

////////////////////////
// Vertex
////////////////////////

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

// Takes in world space XZ (UV) and returns if the coordinate should be part of the NONE background.
bool is_none_bg(const vec2 uv) {
	ivec4 regions = ivec4(
		get_index_coord(uv - vec2(0.5, 0.0)).z,
		get_index_coord(uv - vec2(0.0, 0.5)).z,
		get_index_coord(uv + vec2(1.0, 0.0)).z,
		get_index_coord(uv + vec2(0.0, 1.0)).z);
	return any(equal(regions, ivec4(-1)));
}

float interpolated_height(vec2 pos) {
	const vec2 offsets = vec2(0, 1);
	vec2 index_id = floor(pos);
	ivec3 index[4];
	index[0] = get_index_coord(index_id + offsets.xy);
	index[1] = get_index_coord(index_id + offsets.yy);
	index[2] = get_index_coord(index_id + offsets.yx);
	index[3] = get_index_coord(index_id + offsets.xx);
	float h0 = texelFetch(_height_maps, index[0], 0).r;
	float h1 = texelFetch(_height_maps, index[1], 0).r;
	float h2 = texelFetch(_height_maps, index[2], 0).r;
	float h3 = texelFetch(_height_maps, index[3], 0).r;
	vec2 f = fract(pos);
	vec2 i = 1.0 - f;
	vec4 w = vec4(i.x * f.y, f.x * f.y, f.x * i.y, i.x * i.y);
	float h = h0 * w[0] + h1 * w[1] + h2 * w[2] + h3 * w[3];
	return h;
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

	// UV coordinates in region space. 0-1 covers 1 region, 1-2 is the next region, etc.
	UV = v_vertex.xz * _vertex_density;

	// UV coordinates in region space + texel offset. Values are 0 to 1 within regions
	UV2 = fma(UV, vec2(_region_texel_size), vec2(0.5 * _region_texel_size));

	// Discard vertices for Holes. 1 lookup
	ivec3 v_region = get_index_coord(start_pos);
	uint control = floatBitsToUint(texelFetch(_control_maps, v_region, 0)).r;
	bool hole = bool(control >>2u & 0x1u);

	// Show holes to all cameras except mouse camera (on exactly 1 layer)
	if ( !(CAMERA_VISIBLE_LAYERS == _mouse_layer) &&
			(hole || (_background_mode == 0u && is_none_bg(UV)))) {
		v_vertex.x = 0. / 0.;
	} else {
		// Set final vertex height.
		float h;
		// This branch is static for each of the clipmap segments
		// Interpolated reads only occur where sub-texel values are required.
		if (scale < _vertex_spacing) {
			h = interpolated_height(UV);
		} else {
			ivec3 coord_a = get_index_coord(start_pos);
			ivec3 coord_b = get_index_coord(end_pos);
			h = mix(texelFetch(_height_maps, coord_a, 0).r,texelFetch(_height_maps, coord_b, 0).r,vertex_lerp);
		}
		v_vertex.y = v_region.z < 0 ? past_map_y_m : h;
	}

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
	vec2 index_id = floor(uv);
	ivec3 i0 = get_index_coord(index_id + vec2(0, 1));
	ivec3 i2 = get_index_coord(index_id + vec2(1, 0));
	ivec3 i3 = get_index_coord(index_id);
	float h3 = texelFetch(_height_maps, i3, 0).r;
	float h2 = texelFetch(_height_maps, i2, 0).r;
	float h0 = texelFetch(_height_maps, i0, 0).r;
	vec3 w_normal = normalize(vec3(h3 - h2, _vertex_spacing, h3 - h0));
	ivec3 i1 = get_index_coord(index_id + vec2(1, 1));
	vec2 fc = fract(uv);
	PaintCorner p00 = read_paint(i3), p10 = read_paint(i2), p01 = read_paint(i0), p11 = read_paint(i1);
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
	vec2 tilt = grass_tilt(v_vertex.xz, per_pixel) * (1.0 - max(dirt_w, rock_w));
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
		vec3 sum = vec3(0.0);
		float weights = 0.0;
		if (p00.surface > 0 && cw.x * p00.amount > 0.0) { sum += surface_colour(p00.surface, g, dirt_c, v_vertex, wdx, wdy, w_normal, grain, speck) * cw.x * p00.amount; weights += cw.x * p00.amount; }
		if (p10.surface > 0 && cw.y * p10.amount > 0.0) { sum += surface_colour(p10.surface, g, dirt_c, v_vertex, wdx, wdy, w_normal, grain, speck) * cw.y * p10.amount; weights += cw.y * p10.amount; }
		if (p01.surface > 0 && cw.z * p01.amount > 0.0) { sum += surface_colour(p01.surface, g, dirt_c, v_vertex, wdx, wdy, w_normal, grain, speck) * cw.z * p01.amount; weights += cw.z * p01.amount; }
		if (p11.surface > 0 && cw.w * p11.amount > 0.0) { sum += surface_colour(p11.surface, g, dirt_c, v_vertex, wdx, wdy, w_normal, grain, speck) * cw.w * p11.amount; weights += cw.w * p11.amount; }
		if (weights > 0.0) c = mix(c, sum / weights, clamp(weights, 0.0, 1.0));
	}
	c *= gully_shade;
	c *= 1.0 - 0.4 * step(map_half - 0.5, max(abs(v_vertex.x), abs(v_vertex.z)));
	ALBEDO = c;
	ROUGHNESS = mix(0.95, 0.6, rain_wet);
}
