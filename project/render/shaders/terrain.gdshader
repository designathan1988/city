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
uniform vec3 grass_tint : source_color = vec3(1.0);
uniform vec3 rock : source_color;
uniform vec3 sand : source_color;
uniform float water_level = 0.0;
uniform float sand_band = 2.0;
uniform float rock_slope = 0.45;
uniform float patch_scale;
uniform sampler2D land_map : source_color, filter_linear_mipmap_anisotropic, repeat_disable;
uniform float land_layers = 9.0; // terrain-land.map: square layers stacked vertically
uniform float dirt_layer = 8.0;
uniform float rock_layer = 2.0;
uniform float dirt_m = 13.6;
uniform float rock_m = 23.2;
uniform vec2 dirt_slope = vec2(0.11, 0.26); // 1 - normal.y: Roadcraft dirt 27..42 degrees
uniform float map_half = 5120.0;
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
		v_vertex.y = h;
	}

	// Convert model space to view space w/ skip_vertex_transform render mode
	VERTEX = (VIEW_MATRIX * vec4(v_vertex, 1.0)).xyz;
	NORMAL = normalize((MODELVIEW_MATRIX * vec4(NORMAL, 0.0)).xyz);
	BINORMAL = normalize((MODELVIEW_MATRIX * vec4(BINORMAL, 0.0)).xyz);
	TANGENT = normalize((MODELVIEW_MATRIX * vec4(TANGENT, 0.0)).xyz);
}
////////////////////////
// Fragment: Roadcraft grass map in world space, sand at the shore, rock on steep slopes
////////////////////////

// Integer hash (pcg, as building.gdshader): exact on every GPU, unlike fract(sin(...)).
uint pcg(uint v) {
	uint s = v * 747796405u + 2891336453u;
	uint w = ((s >> ((s >> 28u) + 4u)) ^ s) * 277803737u;
	return (w >> 22u) ^ w;
}

float hash2(vec2 p) {
	ivec2 i = ivec2(floor(p));
	uint h = uint(i.x + 32768) * 73856093u ^ uint(i.y + 32768) * 19349663u;
	return float(pcg(h)) / 4294967295.0;
}

float vnoise(vec2 p) {
	vec2 i = floor(p);
	vec2 f = fract(p);
	vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(mix(hash2(i), hash2(i + vec2(1, 0)), u.x), mix(hash2(i + vec2(0, 1)), hash2(i + vec2(1, 1)), u.x), u.y);
}

// One layer of the stacked land map with the derivatives of the unwrapped coordinates (no seams at the wrap).
vec3 land(float layer, vec2 p) {
	vec2 q = vec2(fract(p.x), (layer + fract(p.y)) / land_layers);
	return textureGrad(land_map, q, dFdx(p) * vec2(1.0, 1.0 / land_layers), dFdy(p) * vec2(1.0, 1.0 / land_layers)).rgb;
}

// Grass detail fades out beyond ~300 m (the second rotated read and the micro noise stop paying for themselves).
const float DETAIL_NEAR_M = 240.0;
const float DETAIL_FAR_M = 300.0;

// Grass without visible tiling: two reads, the second rotated 30 degrees, blended by large blotches.
vec3 grass(vec2 p, float detail) {
	vec3 g = texture(grass_map, p).rgb;
	if (detail <= 0.0) return g;
	vec2 r = vec2(0.866 * p.x - 0.5 * p.y, 0.5 * p.x + 0.866 * p.y) + 0.37;
	float k = smoothstep(0.35, 0.65, vnoise(p * 0.25));
	return mix(g, texture(grass_map, r).rgb, k * detail);
}

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
	NORMAL = mat3(VIEW_MATRIX) * w_normal;
	float slope = 1.0 - w_normal.y;
	float detail = 1.0 - smoothstep(DETAIL_NEAR_M, DETAIL_FAR_M, length(v_vertex.xz - v_camera_pos.xz));
	float n = vnoise(v_vertex.xz * patch_scale) * 0.7 + vnoise(v_vertex.xz * patch_scale * 6.0) * 0.3;
	n = mix(0.5, n, detail);
	vec2 gp = v_vertex.xz / grass_m;
	vec3 g = grass(gp, detail);
	float macro = clamp(dot(grass(gp * 0.137 + 0.21, detail), vec3(0.3, 0.6, 0.1)) / max(dot(g, vec3(0.3, 0.6, 0.1)), 0.02), 0.7, 1.3);
	vec3 c = g * mix(1.0, macro, 0.35) * grass_tint * mix(0.9, 1.08, n);
	c = mix(vec3(dot(c, vec3(0.3, 0.6, 0.1))), c, 0.85);
	float wander = (n - 0.5) * 0.08;
	vec3 dirt = land(dirt_layer, v_vertex.xz / dirt_m);
	c = mix(c, dirt, smoothstep(dirt_slope.x, dirt_slope.y, slope + wander) * (1.0 - smoothstep(rock_slope, rock_slope + 0.1, slope)));
	c = mix(c, sand, 1.0 - smoothstep(water_level + sand_band * 0.5, water_level + sand_band, v_vertex.y));
	vec3 stone = mix(land(rock_layer, v_vertex.xz / rock_m), rock, 0.25);
	c = mix(c, stone, smoothstep(rock_slope, rock_slope + 0.1, slope + wander));
	c *= 1.0 - 0.4 * step(map_half - 0.5, max(abs(v_vertex.x), abs(v_vertex.z)));
	ALBEDO = c;
	ROUGHNESS = 0.95;
}
