// The painted ground for terrain.gdshader, read from Terrain3D's control map (native/src/render/terrain_bridge.cpp packs
// it; https://terrain3d.readthedocs.io controlmap_format): base id the surface, blend how much of it; overlay id the
// biome and the uv bits how much of it; the reserved bits the gullies. Each corner of the height grid holds one of each,
// read at the four corners and mixed by distance (ids cannot be filtered).
// Needs: _control_maps, get_index_coord, land_triplanar, the uniforms below.
#include "res://render/shaders/contracts/land.gdshaderinc"

uniform vec3 biome_tint[1 << PAINT_ID_BITS] : source_color; // by the overlay id: none, then the biomes of data/catalog/nature.json
uniform int map_biome = 0; // 0 none, else the biome's index + 1
uniform float gully_auto; // the steep land's own gullies (the weather's; data/catalog/nature.json's default before it is read)
uniform float gully_m; // a runnel's spacing across the fall line
uniform float gully_depth;
uniform float gully_slope_from;
uniform float gully_slope_to;
uniform float gully_stretch; // the runnels' noise stretched along the fall line, this many times
uniform vec3 gully_shade_k; // the darkening in the runnels: how much; the noise's edges it falls between
uniform float paint_least; // a painted amount under this leaves the ground as it is
uniform float paint_grain_per_m; // the painted surfaces' grain and speck, read off the soil layer at these scales
uniform float paint_speck_per_m;

const uint ID_MASK = (1u << uint(PAINT_ID_BITS)) - 1u, BLEND_MASK = (1u << uint(PAINT_BLEND_BITS)) - 1u, BIOME_MASK = (1u << uint(PAINT_BIOME_BITS)) - 1u;
const uint GULLY_MASK = (1u << uint(PAINT_GULLY_BITS)) - 1u, BIT_WIPE = 1u << uint(PAINT_GULLY_BITS); // the wipe: the reserved bit over the gully's
const float BLEND_FULL = float(BLEND_MASK), BIOME_FULL = float(BIOME_MASK), GULLY_FULL = float(GULLY_MASK);
const float ROCK_GRANITE = 2.0, ROCK_SANDSTONE = 4.0, ROCK_BASALT = 6.0; // the rock faces among the layers of the land map
const vec3 ONLY_GRANITE = vec3(1.0, 0.0, 0.0), ONLY_SANDSTONE = vec3(0.0, 1.0, 0.0), ONLY_BASALT = vec3(0.0, 0.0, 1.0); // a geology of one rock
// The painted surfaces: data/catalog/nature.json paint.kinds, their ground slots + 1 (render/terrain_bridge.cpp packs them).
const int PAINT_SAND = 1, PAINT_SOIL = 2, PAINT_MEADOW = 3, PAINT_SNOW = 4, PAINT_GRAVEL = 5, PAINT_ASPHALT = 6, PAINT_CONCRETE = 7;
const int PAINT_GRANITE = 8, PAINT_SANDSTONE = 9, PAINT_BASALT = 10;
const int PAINT_TONES = PAINT_CONCRETE + 1; // the surfaces with a tone of their own
uniform vec3 paint_tone[PAINT_TONES]; // by surface: its colour, or the tint over the soil's or the grass's (linear)
uniform vec4 paint_mix[PAINT_TONES]; // by surface: its shade by the grain, from and to; by the speck, from and to (0 where unread)
#include "res://render/shaders/rock_strata.gdshaderinc" // the painted rocks wear their strata, as the rock breaking out does
const float LEAST_FALL = 1e-4; // keeps normalize off a flat ground's fall line
const float RIDGE_STEP = 0.1, RIDGE_GAIN = 10.0; // the ridged noise's slope across: a second read this far on, 1 / RIDGE_STEP

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
		case PAINT_SAND: return paint_tone[PAINT_SAND] * mix(paint_mix[PAINT_SAND].x, paint_mix[PAINT_SAND].y, grain) * mix(paint_mix[PAINT_SAND].z, paint_mix[PAINT_SAND].w, speck);
		case PAINT_SOIL: return dirt_c * paint_tone[PAINT_SOIL] * mix(paint_mix[PAINT_SOIL].x, paint_mix[PAINT_SOIL].y, grain);
		case PAINT_MEADOW: return grass_c * paint_tone[PAINT_MEADOW] * mix(paint_mix[PAINT_MEADOW].z, paint_mix[PAINT_MEADOW].w, speck);
		case PAINT_SNOW: return paint_tone[PAINT_SNOW] * mix(paint_mix[PAINT_SNOW].x, paint_mix[PAINT_SNOW].y, grain);
		case PAINT_GRAVEL: return paint_tone[PAINT_GRAVEL] * mix(paint_mix[PAINT_GRAVEL].z, paint_mix[PAINT_GRAVEL].w, speck) * mix(paint_mix[PAINT_GRAVEL].x, paint_mix[PAINT_GRAVEL].y, grain);
		case PAINT_ASPHALT: return paint_tone[PAINT_ASPHALT] * mix(paint_mix[PAINT_ASPHALT].z, paint_mix[PAINT_ASPHALT].w, speck);
		case PAINT_CONCRETE: return paint_tone[PAINT_CONCRETE] * mix(paint_mix[PAINT_CONCRETE].x, paint_mix[PAINT_CONCRETE].y, grain) * mix(paint_mix[PAINT_CONCRETE].z, paint_mix[PAINT_CONCRETE].w, speck);
		case PAINT_GRANITE: return rock_strata(world, dx, dy, n, ONLY_GRANITE);
		case PAINT_SANDSTONE: return rock_strata(world, dx, dy, n, ONLY_SANDSTONE);
		default: return rock_strata(world, dx, dy, n, ONLY_BASALT); // basalt
	}
}

// How much the gullies cut here: what the player cut and the steep land's own, less what they wiped.
float gully_weight(float cut, float wipe, float slope) {
	return clamp(cut + gully_auto * smoothstep(gully_slope_from, gully_slope_to, slope) * (1.0 - wipe), 0.0, 1.0);
}

// The normal of a gullied slope: runnels down the fall line, ridged noise across it, stretched along it.
vec3 gully_normal(vec3 n, vec2 xz, float w, out float shade) {
	vec2 fall = normalize(n.xz + vec2(LEAST_FALL));
	vec2 across = vec2(-fall.y, fall.x);
	vec2 q = vec2(dot(xz, across) / gully_m, dot(xz, fall) / (gully_m * gully_stretch));
	float a = vnoise(q), b = vnoise(q + vec2(RIDGE_STEP, 0.0));
	shade = 1.0 - gully_shade_k.x * w * (1.0 - smoothstep(gully_shade_k.y, gully_shade_k.z, a));
	return normalize(n + vec3(across.x, 0.0, across.y) * (b - a) * RIDGE_GAIN * w * gully_depth);
}

// The painted surfaces over `c` (`g` the grass's map colour, `world` the point, `w_normal` its normal): each corner's surface mixed
// in by its amount and its distance. Each surface's colour is worked out once, whatever number of corners carry it (it depends
// on the surface alone: the derivatives are passed in); the corners then add it in their order, as each did with its own.
vec3 paint_over(vec3 c, vec3 g, PaintCorner p00, PaintCorner p10, PaintCorner p01, PaintCorner p11, vec4 cw, vec3 world, vec3 wdx, vec3 wdy, vec3 w_normal) {
	float painted = dot(cw, vec4(p00.amount, p10.amount, p01.amount, p11.amount));
	if (painted > paint_least) {
		vec3 dirt_c = land(dirt_layer, fract(world.xz / dirt_m), wdx.xz / dirt_m, wdy.xz / dirt_m);
		float grain = dot(land(dirt_layer, fract(world.xz * paint_grain_per_m), wdx.xz * paint_grain_per_m, wdy.xz * paint_grain_per_m), CHANNEL_MEAN);
		float speck = dot(land(dirt_layer, fract(world.xz * paint_speck_per_m), wdx.xz * paint_speck_per_m, wdy.xz * paint_speck_per_m), CHANNEL_MEAN);
		bool on00 = p00.surface > 0 && cw.x * p00.amount > 0.0;
		bool on10 = p10.surface > 0 && cw.y * p10.amount > 0.0;
		bool on01 = p01.surface > 0 && cw.z * p01.amount > 0.0;
		bool on11 = p11.surface > 0 && cw.w * p11.amount > 0.0;
		vec3 c00 = vec3(0.0), c10 = vec3(0.0), c01 = vec3(0.0), c11 = vec3(0.0);
		if (on00) c00 = surface_colour(p00.surface, g, dirt_c, world, wdx, wdy, w_normal, grain, speck);
		if (on10) {
			if (on00 && p10.surface == p00.surface) c10 = c00;
			else c10 = surface_colour(p10.surface, g, dirt_c, world, wdx, wdy, w_normal, grain, speck);
		}
		if (on01) {
			if (on00 && p01.surface == p00.surface) c01 = c00;
			else if (on10 && p01.surface == p10.surface) c01 = c10;
			else c01 = surface_colour(p01.surface, g, dirt_c, world, wdx, wdy, w_normal, grain, speck);
		}
		if (on11) {
			if (on00 && p11.surface == p00.surface) c11 = c00;
			else if (on10 && p11.surface == p10.surface) c11 = c10;
			else if (on01 && p11.surface == p01.surface) c11 = c01;
			else c11 = surface_colour(p11.surface, g, dirt_c, world, wdx, wdy, w_normal, grain, speck);
		}
		vec3 sum = vec3(0.0);
		float weights = 0.0;
		if (on00) { sum += c00 * cw.x * p00.amount; weights += cw.x * p00.amount; }
		if (on10) { sum += c10 * cw.y * p10.amount; weights += cw.y * p10.amount; }
		if (on01) { sum += c01 * cw.z * p01.amount; weights += cw.z * p01.amount; }
		if (on11) { sum += c11 * cw.w * p11.amount; weights += cw.w * p11.amount; }
		if (weights > 0.0) c = mix(c, sum / weights, clamp(weights, 0.0, 1.0));
	}
	return c;
}
