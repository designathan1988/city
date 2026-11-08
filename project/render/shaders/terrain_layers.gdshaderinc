// Terrain sampling for terrain.gdshader: value noise, the stacked land map read flat or along the three world axes
// (triplanar) and the grass map without visible tiling. Needs the uniforms land_map, land_layers and grass_map.

#include "res://render/shaders/lattice_noise.gdshaderinc"
#include "res://render/shaders/shader_contracts.gdshaderinc"

const vec3 LUMA = vec3(LUMA_R, LUMA_G, LUMA_B); // the weights a colour's luma is read with

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

// The blades' colour reaches further than their light: under the fine grain, a share of it stays out to where a pixel holds a tuft.
float blade_colour_weight(float per_pixel) {
	return max(blade_weight(per_pixel), detail_wide_share * (1.0 - smoothstep(detail_wide_from_px, detail_wide_to_px, per_pixel)));
}

vec3 grass_grain(vec2 xz, float per_pixel) {
	float w = blade_colour_weight(per_pixel);
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
	float g0 = dot(textureGrad(grass_map, u0, d0x, d0y).rgb, LUMA);
	float g1 = dot(textureGrad(grass_map, u1, d1x, d1y).rgb, LUMA);
	float grain = mix(g0, g1, lod - level) / max(grass_mean, 0.02);
	return mix(1.0, clamp(grain, grain_low, grain_high), smoothstep(0.15, 0.6, lod) * grain_share);
}

uniform sampler2D grass_normal : hint_normal, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D grass_blades_normal : hint_normal, filter_linear_mipmap_anisotropic, repeat_enable;
uniform float grass_bump;
uniform float blade_bump;

// The tilt the grass gives the ground, at `share` of it: the tufts of its normal map, at its scale and turned at a wider one so
// the two never line up, and the blades under them while they show. Most of what the eye reads as a lawn is this light, not
// colour. A read whose weight is zero is not made: the derivatives are taken first, in uniform flow, and the reads under the
// guards pass them explicitly.
vec2 grass_tilt(vec2 xz, float per_pixel, float share) {
	vec2 p = xz / grass_m;
	vec2 wide = vec2(0.9397 * p.x - 0.342 * p.y, 0.342 * p.x + 0.9397 * p.y) * 0.137 + 0.21;
	vec2 blade = xz / detail_m;
	vec2 p_dx = dFdx(p), p_dy = dFdy(p);
	vec2 wide_dx = dFdx(wide), wide_dy = dFdy(wide);
	vec2 blade_dx = dFdx(blade), blade_dy = dFdy(blade);
	if (share == 0.0) return vec2(0.0);
	vec2 near_n = textureGrad(grass_normal, p, p_dx, p_dy).xy * 2.0 - 1.0;
	vec2 wide_n = textureGrad(grass_normal, wide, wide_dx, wide_dy).xy * 2.0 - 1.0;
	vec2 tufts = mix(near_n, wide_n, 0.42) * grass_bump;
	float blades = blade_weight(per_pixel);
	if (blades > 0.0) {
		vec2 blade_n = textureGrad(grass_blades_normal, blade, blade_dx, blade_dy).xy * 2.0 - 1.0;
		tufts = tufts + blade_n * blade_bump * blades;
	}
	return tufts * share;
}

uniform float biome_dry[7];
uniform float straw_share;

// The dry season over the grass: the map's own texture with its hue turned to straw, patch by patch, each patch green or
// gold with its edge broken by the tufts (a share of each everywhere is only an olive wash).
vec3 straw_over(vec3 c, vec3 map_colour, vec2 xz, int biome) {
	float luma = dot(map_colour, LUMA);
	float patchy = vnoise(xz / 37.0 + 7.0) * 0.6 + vnoise(xz / 13.0 + 3.0) * 0.4;
	float golden = clamp(biome_dry[biome] * (0.35 + 0.9 * patchy), 0.0, 1.0);
	float mask = smoothstep(0.35, 0.8, golden + (clamp((luma - 0.17) / 0.2, 0.0, 1.0) - 0.5) * 0.3) * straw_share;
	return mix(c, dot(c, LUMA) * vec3(1.32, 1.04, 0.46) * 1.05, mask);
}

// The ground's grass at a world point, before soil and rock: g its map's colour, n its patches, for whoever carries on.
// Land seen from afar is never one green: meadows, dry stretches and darker hollows, wider than any tile of the grass, so the map stops
// reading as one colour however far the camera stands (macro variation over the detail texture).
const float MACRO_WIDE_M = 1500.0, MACRO_MID_M = 430.0, MACRO_SMALL_M = 125.0, MACRO_DRY_M = 680.0;
const vec3 MACRO_DARK = vec3(0.88, 0.92, 0.84), MACRO_LUSH = vec3(1.07, 1.05, 0.97), MACRO_STRAW = vec3(1.16, 1.04, 0.84);
const float MACRO_DRY_SHARE = 0.4;

vec3 macro_tone(vec2 xz) {
	float small = vnoise(xz / MACRO_SMALL_M + 3.1);
	float lush = vnoise(xz / MACRO_WIDE_M) * 0.5 + vnoise(xz / MACRO_MID_M + 7.3) * 0.32 + small * 0.18;
	float dry = smoothstep(0.42, 0.68, vnoise(xz / MACRO_DRY_M + 11.0) * 0.6 + small * 0.4);
	vec3 tone = mix(MACRO_DARK, MACRO_LUSH, smoothstep(0.25, 0.75, lush));
	return mix(tone, tone * MACRO_STRAW, dry * MACRO_DRY_SHARE);
}

vec3 grass_ground(vec2 xz, float detail, vec3 tint, int biome, float per_pixel, out vec3 g, out float n) {
	n = mix(0.5, vnoise(xz * patch_scale) * 0.7 + vnoise(xz * patch_scale * 6.0) * 0.3, detail);
	vec2 gp = xz / grass_m;
	g = grass(gp, detail);
	float macro = clamp(dot(grass(gp * 0.137 + 0.21, detail), LUMA) / max(dot(g, LUMA), 0.02), 0.7, 1.3);
	vec3 c = straw_over(g * mix(1.0, macro, 0.35) * tint * mix(0.9, 1.08, n), g, xz, biome);
	return c * macro_tone(xz) * grass_grain(xz, per_pixel) * grass_speckle(xz, per_pixel);
}
