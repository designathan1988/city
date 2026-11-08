// The painted ground for terrain.gdshader, read from Terrain3D's control map (native/src/render/terrain_bridge.cpp packs
// it; https://terrain3d.readthedocs.io controlmap_format): base id the surface, blend how much of it; overlay id the
// biome and the uv bits how much of it; the reserved bits the gullies. Each corner of the height grid holds one of each,
// read at the four corners and mixed by distance (ids cannot be filtered).
// Needs: _control_maps, get_index_coord, land, land_triplanar, strata_over, dirt_layer, dirt_m, the uniforms below.
#include "res://render/shaders/contracts/land.gdshaderinc"

uniform vec3 biome_tint[1 << PAINT_ID_BITS] : source_color; // by the overlay id: none, then the biomes of data/catalog/nature.json
uniform int map_biome = 0; // 0 none, else the biome's index + 1
uniform float gully_auto = 0.25;
uniform float gully_m = 9.0;
uniform float gully_depth = 1.4;
uniform float gully_slope_from = 0.1;
uniform float gully_slope_to = 0.3;

const uint ID_MASK = (1u << uint(PAINT_ID_BITS)) - 1u, BLEND_MASK = (1u << uint(PAINT_BLEND_BITS)) - 1u, BIOME_MASK = (1u << uint(PAINT_BIOME_BITS)) - 1u;
const uint GULLY_MASK = (1u << uint(PAINT_GULLY_BITS)) - 1u, BIT_WIPE = 1u << uint(PAINT_GULLY_BITS); // the wipe: the reserved bit over the gully's
const float BLEND_FULL = float(BLEND_MASK), BIOME_FULL = float(BIOME_MASK), GULLY_FULL = float(GULLY_MASK);
const float ROCK_GRANITE = 2.0, ROCK_SANDSTONE = 4.0, ROCK_BASALT = 6.0; // the rock faces among the layers of the land map

struct PaintCorner {
	int surface;
	float amount;
	int biome;
	float biome_amount;
	float gully;
	float wipe;
};

PaintCorner read_paint(ivec3 at) {
	uint c = floatBitsToUint(texelFetch(_control_maps, at, 0)).r;
	PaintCorner p;
	p.surface = int(c >> uint(PAINT_BASE_SHIFT) & ID_MASK);
	p.biome = int(c >> uint(PAINT_OVERLAY_SHIFT) & ID_MASK);
	p.amount = float(c >> uint(PAINT_BLEND_SHIFT) & BLEND_MASK) / BLEND_FULL;
	p.biome_amount = float(c >> uint(PAINT_BIOME_SHIFT) & BIOME_MASK) / BIOME_FULL;
	uint g = c >> uint(PAINT_GULLY_SHIFT) & (BIT_WIPE | GULLY_MASK);
	p.gully = float(g & GULLY_MASK) / GULLY_FULL;
	p.wipe = float((g & BIT_WIPE) != 0u);
	return p;
}

// A painted rock (a landform lays one over its cliff): its layer of the land map, bedded where the face is steep as the
// land's own rock is.
vec3 rock_face(float layer, vec3 world, vec3 dx, vec3 dy, vec3 n) {
	return strata_over(land_triplanar(layer, world / rock_m, dx / rock_m, dy / rock_m, n), world, 1.0 - n.y, abs(dx.y) + abs(dy.y));
}

// The colour of one surface (table: data/catalog/nature.json paint.kinds, their ground slots + 1), `n` a fine grain.
vec3 surface_colour(int surface, vec3 grass_c, vec3 dirt_c, vec3 world, vec3 dx, vec3 dy, vec3 n, float grain, float speck) {
	switch (surface) {
		case 1: return vec3(0.80, 0.70, 0.50) * mix(0.86, 1.1, grain) * mix(0.94, 1.04, speck); // sand
		case 2: return dirt_c * vec3(0.92, 0.78, 0.62) * mix(0.8, 1.08, grain); // soil
		case 3: return grass_c * vec3(0.95, 1.08, 0.62) * mix(0.75, 1.15, speck); // meadow
		case 4: return vec3(0.92, 0.94, 0.97) * mix(0.93, 1.0, grain); // snow
		case 5: return vec3(0.55, 0.54, 0.51) * mix(0.6, 1.25, speck) * mix(0.9, 1.05, grain); // gravel
		case 6: return vec3(0.17, 0.17, 0.19) * mix(0.85, 1.15, speck); // asphalt
		case 7: return vec3(0.66, 0.65, 0.62) * mix(0.9, 1.06, grain) * mix(0.96, 1.03, speck); // concrete
		case 8: return rock_face(ROCK_GRANITE, world, dx, dy, n); // granite
		case 9: return rock_face(ROCK_SANDSTONE, world, dx, dy, n); // sandstone
		default: return rock_face(ROCK_BASALT, world, dx, dy, n); // basalt
	}
}

// The painted surfaces of the four corners over the ground's colour `c`, each corner's by its weight `cw` and its amount.
vec3 painted_over(vec3 c, PaintCorner p00, PaintCorner p10, PaintCorner p01, PaintCorner p11, vec4 cw, vec3 g, vec3 world, vec3 wdx, vec3 wdy, vec3 n) {
	float painted = dot(cw, vec4(p00.amount, p10.amount, p01.amount, p11.amount));
	if (painted <= 0.004) return c;
	vec3 dirt_c = land(dirt_layer, fract(world.xz / dirt_m), wdx.xz / dirt_m, wdy.xz / dirt_m);
	float grain = dot(land(dirt_layer, fract(world.xz * 0.27), wdx.xz * 0.27, wdy.xz * 0.27), vec3(0.33));
	float speck = dot(land(dirt_layer, fract(world.xz * 1.325), wdx.xz * 1.325, wdy.xz * 1.325), vec3(0.33));
	// Each surface's colour is worked out once, whatever number of corners carry it (it depends on the surface alone: the
	// derivatives are passed in); the corners then add it in their order, as each did with its own.
	bool on00 = p00.surface > 0 && cw.x * p00.amount > 0.0;
	bool on10 = p10.surface > 0 && cw.y * p10.amount > 0.0;
	bool on01 = p01.surface > 0 && cw.z * p01.amount > 0.0;
	bool on11 = p11.surface > 0 && cw.w * p11.amount > 0.0;
	vec3 c00 = vec3(0.0), c10 = vec3(0.0), c01 = vec3(0.0), c11 = vec3(0.0);
	if (on00) c00 = surface_colour(p00.surface, g, dirt_c, world, wdx, wdy, n, grain, speck);
	if (on10) {
		if (on00 && p10.surface == p00.surface) c10 = c00;
		else c10 = surface_colour(p10.surface, g, dirt_c, world, wdx, wdy, n, grain, speck);
	}
	if (on01) {
		if (on00 && p01.surface == p00.surface) c01 = c00;
		else if (on10 && p01.surface == p10.surface) c01 = c10;
		else c01 = surface_colour(p01.surface, g, dirt_c, world, wdx, wdy, n, grain, speck);
	}
	if (on11) {
		if (on00 && p11.surface == p00.surface) c11 = c00;
		else if (on10 && p11.surface == p10.surface) c11 = c10;
		else if (on01 && p11.surface == p01.surface) c11 = c01;
		else c11 = surface_colour(p11.surface, g, dirt_c, world, wdx, wdy, n, grain, speck);
	}
	vec3 sum = vec3(0.0);
	float weights = 0.0;
	if (on00) { sum += c00 * cw.x * p00.amount; weights += cw.x * p00.amount; }
	if (on10) { sum += c10 * cw.y * p10.amount; weights += cw.y * p10.amount; }
	if (on01) { sum += c01 * cw.z * p01.amount; weights += cw.z * p01.amount; }
	if (on11) { sum += c11 * cw.w * p11.amount; weights += cw.w * p11.amount; }
	return weights > 0.0 ? mix(c, sum / weights, clamp(weights, 0.0, 1.0)) : c;
}

// How much the gullies cut here: what the player cut and the steep land's own, less what they wiped.
float gully_weight(float cut, float wipe, float slope) {
	return clamp(cut + gully_auto * smoothstep(gully_slope_from, gully_slope_to, slope) * (1.0 - wipe), 0.0, 1.0);
}

// The normal of a gullied slope: runnels down the fall line, ridged noise across it, stretched along it.
vec3 gully_normal(vec3 n, vec2 xz, float w, out float shade) {
	vec2 fall = normalize(n.xz + vec2(1e-4));
	vec2 across = vec2(-fall.y, fall.x);
	vec2 q = vec2(dot(xz, across) / gully_m, dot(xz, fall) / (gully_m * 6.0));
	float a = vnoise(q), b = vnoise(q + vec2(0.1, 0.0));
	shade = 1.0 - 0.35 * w * (1.0 - smoothstep(0.2, 0.45, a));
	return normalize(n + vec3(across.x, 0.0, across.y) * (b - a) * 10.0 * w * gully_depth);
}
