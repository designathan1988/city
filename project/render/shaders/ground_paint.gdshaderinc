// The painted ground for terrain.gdshader, read from Terrain3D's control map (native/src/render/terrain_bridge.cpp packs
// it; https://terrain3d.readthedocs.io controlmap_format): base id the surface, blend how much of it; overlay id the
// biome and the uv bits how much of it; the reserved bits the gullies. Each corner of the height grid holds one of each,
// read at the four corners and mixed by distance (ids cannot be filtered).
// Needs: _control_maps, get_index_coord, land_triplanar, the uniforms below.
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
		case 8: return land_triplanar(ROCK_GRANITE, world / rock_m, dx / rock_m, dy / rock_m, n); // granite
		case 9: return land_triplanar(ROCK_SANDSTONE, world / rock_m, dx / rock_m, dy / rock_m, n); // sandstone
		default: return land_triplanar(ROCK_BASALT, world / rock_m, dx / rock_m, dy / rock_m, n); // basalt
	}
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
