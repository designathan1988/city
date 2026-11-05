// Terrain sampling for terrain.gdshader: value noise, the stacked land map read flat or along the three world axes
// (triplanar) and the grass map without visible tiling. Needs the uniforms land_map, land_layers and grass_map.

#include "res://render/shaders/lattice_noise.gdshaderinc"

// One layer of the stacked land map at `p`, with the derivatives of the unwrapped coordinates taken by the caller in
// uniform control flow (no seams at the wrap, no undefined derivatives inside a branch).
vec3 land(float layer, vec2 p, vec2 dx, vec2 dy) {
	return textureGrad(land_map, vec3(p, layer), dx, dy).rgb;
}

// TRIPLANAR: the layer projected along each world axis and blended by how much the surface faces it, so a wall takes
// the texture square on instead of stretched down its height (Geiss, "Generating Complex Procedural Terrains Using the
// GPU", GPU Gems 3 ch. 1: weights |n| - 0.2, scaled by 7, clamped at 0 and normalised). `w` and its derivatives are the
// world position over the layer's scale.
vec3 land_triplanar(float layer, vec3 w, vec3 dx, vec3 dy, vec3 n) {
	vec3 b = max((abs(n) - 0.2) * 7.0, vec3(0.0));
	b /= max(b.x + b.y + b.z, 1e-4);
	vec3 c = vec3(0.0);
	if (b.x > 0.0) c += b.x * land(layer, w.zy, dx.zy, dy.zy);
	if (b.y > 0.0) c += b.y * land(layer, w.xz, dx.xz, dy.xz);
	if (b.z > 0.0) c += b.z * land(layer, w.xy, dx.xy, dy.xy);
	return c;
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

uniform float grain_per_m;
uniform float grain_repeat;
uniform float grain_low;
uniform float grain_high;
uniform float grain_share;
uniform float grass_mean = 0.1; // the map's mean luma, linear (render/city_world.gd)

// The close-up grain of the grass (Roadcraft's detail layer): a tile of blades under the map, twice its value around 1,
// read a second time turned and larger so it does not repeat, gone once a metre takes under a few pixels.
const float DETAIL_TURN_C = 0.5403;
const float DETAIL_TURN_S = 0.8415;
const float DETAIL_SECOND = 0.4137;

float blade_weight(float per_pixel) {
	return 1.0 - smoothstep(detail_near_px, detail_far_px, per_pixel);
}

vec3 grass_grain(vec2 xz, float per_pixel) {
	float w = blade_weight(per_pixel);
	if (w <= 0.0) return vec3(1.0);
	vec2 uv = xz / detail_m;
	vec2 dx = dFdx(uv), dy = dFdy(uv);
	mat2 turn = mat2(vec2(DETAIL_TURN_C, DETAIL_TURN_S), vec2(-DETAIL_TURN_S, DETAIL_TURN_C));
	vec3 a = textureGrad(grass_detail, uv, dx, dy).rgb * 2.0;
	vec3 b = textureGrad(grass_detail, turn * uv * DETAIL_SECOND + 0.31, turn * dx * DETAIL_SECOND, turn * dy * DETAIL_SECOND).rgb * 2.0;
	return mix(vec3(1.0), a * mix(vec3(1.0), b, 0.55), w * detail_albedo);
}

// The grass's own map again, at the scale of a pixel wherever the camera stands: tufts half a pixel to one across, two
// octaves of the map a level apart, so a field keeps its grain from any height; the mip chain would have averaged it away.
float grass_speckle(vec2 xz, float per_pixel) {
	float lod = max(0.0, log2(per_pixel * grain_per_m));
	float level = floor(lod);
	vec2 uv = xz / grass_m * grain_repeat;
	vec2 u0 = uv / exp2(level), u1 = uv / exp2(level + 1.0) + 0.37;
	vec2 d0x = dFdx(u0), d0y = dFdy(u0), d1x = dFdx(u1), d1y = dFdy(u1);
	float g0 = dot(textureGrad(grass_map, u0, d0x, d0y).rgb, vec3(0.3, 0.6, 0.1));
	float g1 = dot(textureGrad(grass_map, u1, d1x, d1y).rgb, vec3(0.3, 0.6, 0.1));
	float grain = mix(g0, g1, lod - level) / max(grass_mean, 0.02);
	return mix(1.0, clamp(grain, grain_low, grain_high), smoothstep(0.15, 0.6, lod) * grain_share);
}

uniform sampler2D grass_normal : hint_normal, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D grass_blades_normal : hint_normal, filter_linear_mipmap_anisotropic, repeat_enable;
uniform float grass_bump;
uniform float blade_bump;

// The tilt the grass gives the ground: the tufts of its normal map, at its scale and turned at a wider one so the two
// never line up, and the blades under them while they show. Most of what the eye reads as a lawn is this light, not colour.
vec2 grass_tilt(vec2 xz, float per_pixel) {
	vec2 p = xz / grass_m;
	vec2 near_n = texture(grass_normal, p).xy * 2.0 - 1.0;
	vec2 wide_n = texture(grass_normal, vec2(0.9397 * p.x - 0.342 * p.y, 0.342 * p.x + 0.9397 * p.y) * 0.137 + 0.21).xy * 2.0 - 1.0;
	vec2 blade_n = texture(grass_blades_normal, xz / detail_m).xy * 2.0 - 1.0;
	return mix(near_n, wide_n, 0.42) * grass_bump + blade_n * blade_bump * blade_weight(per_pixel);
}

uniform float biome_dry[7];
uniform float straw_share;

// The dry season over the grass: the map's own texture with its hue turned to straw, patch by patch, each patch green or
// gold with its edge broken by the tufts (a share of each everywhere is only an olive wash).
vec3 straw_over(vec3 c, vec3 map_colour, vec2 xz, int biome) {
	float luma = dot(map_colour, vec3(0.3, 0.6, 0.1));
	float patchy = vnoise(xz / 37.0 + 7.0) * 0.6 + vnoise(xz / 13.0 + 3.0) * 0.4;
	float golden = clamp(biome_dry[biome] * (0.35 + 0.9 * patchy), 0.0, 1.0);
	float mask = smoothstep(0.35, 0.8, golden + (clamp((luma - 0.17) / 0.2, 0.0, 1.0) - 0.5) * 0.3) * straw_share;
	return mix(c, dot(c, vec3(0.3, 0.6, 0.1)) * vec3(1.32, 1.04, 0.46) * 1.05, mask);
}

// The ground's grass at a world point, before soil and rock: g its map's colour, n its patches, for whoever carries on.
vec3 grass_ground(vec2 xz, float detail, vec3 tint, int biome, float per_pixel, out vec3 g, out float n) {
	n = mix(0.5, vnoise(xz * patch_scale) * 0.7 + vnoise(xz * patch_scale * 6.0) * 0.3, detail);
	vec2 gp = xz / grass_m;
	g = grass(gp, detail);
	float macro = clamp(dot(grass(gp * 0.137 + 0.21, detail), vec3(0.3, 0.6, 0.1)) / max(dot(g, vec3(0.3, 0.6, 0.1)), 0.02), 0.7, 1.3);
	vec3 c = straw_over(g * mix(1.0, macro, 0.35) * tint * mix(0.9, 1.08, n), g, xz, biome);
	return c * grass_grain(xz, per_pixel) * grass_speckle(xz, per_pixel);
}
