// The painted ground for terrain.gdshader, read from the land's control words (native/src/render/terrain_bridge.cpp packs
// them, laid out as Terrain3D's control map: https://terrain3d.readthedocs.io controlmap_format): base id the surface, blend
// how much of it; overlay id the biome and the uv bits how much of it; the reserved bits the gullies. Each corner of the
// painted ground's cell holds one of each, read at the four corners and mixed by distance (ids cannot be filtered).
// Needs: ground_control (terrain_land.gdshaderinc), land_triplanar, the uniforms below.

uniform vec3 biome_tint[7] : source_color; // none, then the biomes of data/catalog/nature.json
uniform int map_biome = 0; // 0 none, else the biome's index + 1
uniform float gully_auto = 0.25;
uniform float gully_m = 9.0;
uniform float gully_depth = 1.4;
uniform float gully_slope_from = 0.1;
uniform float gully_slope_to = 0.3;

const float BLEND_FULL = 255.0;
const float BIOME_FULL = 127.0;
const float GULLY_FULL = 7.0;
const uint BIT_WIPE = 8u;
const float ROCK_GRANITE = 2.0, ROCK_SANDSTONE = 4.0, ROCK_BASALT = 6.0; // the rock faces among the layers of the land map

struct PaintCorner {
	int surface;
	float amount;
	int biome;
	float biome_amount;
	float gully;
	float wipe;
};

PaintCorner read_paint(vec2 grid) {
	uint c = ground_control(grid);
	PaintCorner p;
	p.surface = int(c >> 27u & 0x1Fu);
	p.biome = int(c >> 22u & 0x1Fu);
	p.amount = float(c >> 14u & 0xFFu) / BLEND_FULL;
	p.biome_amount = float(c >> 7u & 0x7Fu) / BIOME_FULL;
	uint g = c >> 3u & 0xFu;
	p.gully = float(g & 7u) / GULLY_FULL;
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
