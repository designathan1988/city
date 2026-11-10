// The land's maps for terrain.gdshader (native/src/render/terrain_bridge.h sets them) and how a grid point reads them. The fine
// ground: a layer a region of `land_region` samples a side, for the regions round the view; a region's layer is its place modulo
// `fine_side`, and fine_held says which region each layer holds (its index + 1, 0 none). The far ground: the whole map every
// `far_stride` samples, in `far_tiles` x `far_tiles` tiles of `far_tile` samples a side, read wherever no fine region stands.
// Needs: _vertex_spacing (Terrain3D's private uniform).
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
uniform int far_stride = 8;
uniform int far_tiles = 1;
uniform int far_tile = 128;
group_uniforms;

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
	t = clamp(t, ivec2(0), ivec2(max(land_side / far_stride - 1, 0)));
	ivec2 tile = t / far_tile;
	return ivec3(t - tile * far_tile, tile.y * far_tiles + tile.x);
}

// The land's height at a grid point: its fine region's sample, or between the far ground's four round it (the clipmap's far
// levels stand on the far samples themselves: the same heights either way).
float ground_height(vec2 grid) {
	ivec2 s = land_sample_in(grid);
	ivec3 at = fine_texel(s);
	if (at.z >= 0) return texelFetch(fine_heights, at, 0).r;
	ivec2 a = s / far_stride;
	vec2 f = vec2(s - a * far_stride) / float(far_stride);
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
	return floatBitsToUint(texelFetch(far_controls, far_texel((s + far_stride / 2) / far_stride), 0).r);
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
