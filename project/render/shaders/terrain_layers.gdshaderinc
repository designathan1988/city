// Terrain sampling for terrain.gdshader: value noise, the stacked land map read flat or along the three world axes
// (triplanar) and the grass map without visible tiling. Needs the uniforms land_map, land_layers and grass_map; the rest of its
// look is data/render/palette.json terrain.

#include "res://render/shaders/lattice_noise.gdshaderinc"
#include "res://render/shaders/contracts/land.gdshaderinc"


// One layer of the stacked land map at `p`, with the derivatives of the unwrapped coordinates taken by the caller in
// uniform control flow (no seams at the wrap, no undefined derivatives inside a branch).
vec3 land(float layer, vec2 p, vec2 dx, vec2 dy) {
	return textureGrad(land_map, vec3(p, layer), dx, dy).rgb;
}

// TRIPLANAR: the layer projected along each world axis and blended by how much the surface faces it, so a wall takes
// the texture square on instead of stretched down its height (Geiss, "Generating Complex Procedural Terrains Using the
// GPU", GPU Gems 3 ch. 1: weights |n| - 0.2, scaled by 7, clamped at 0 and normalised). `w` and its derivatives are the
// world position over the layer's scale.
const float TRIPLANAR_FLOOR = 0.2, TRIPLANAR_GAIN = 7.0; // Geiss's weights
const float LEAST_WEIGHT = 1e-4; // the weights' sum divided by never under this
vec3 land_triplanar(float layer, vec3 w, vec3 dx, vec3 dy, vec3 n) {
	vec3 b = max((abs(n) - TRIPLANAR_FLOOR) * TRIPLANAR_GAIN, vec3(0.0));
	b /= max(b.x + b.y + b.z, LEAST_WEIGHT);
	vec3 c = vec3(0.0);
	if (b.x > 0.0) c += b.x * land(layer, w.zy, dx.zy, dy.zy);
	if (b.y > 0.0) c += b.y * land(layer, w.xz, dx.xz, dy.xz);
	if (b.z > 0.0) c += b.z * land(layer, w.xy, dx.xy, dy.xy);
	return c;
}

const vec3 CHANNEL_MEAN = vec3(0.33); // a texel's three channels averaged, near enough
const float TURN_30_COS = 0.866, TURN_30_SIN = 0.5; // a turn of 30 degrees
const float TURN_20_COS = 0.9397, TURN_20_SIN = 0.342; // a turn of 20 degrees
const float TILE_SHIFT = 0.37, WIDE_SHIFT = 0.21; // a second read off the first's lattice; the wide read's
const float LEAST_LUMA = 0.02; // a luma divided by never under this
// The lattice offsets of the straw's and the land's noises, each off the others (any number off the integers).
const float STRAW_SHIFT_WIDE = 7.0, STRAW_SHIFT_FINE = 3.0, MACRO_SHIFT_SMALL = 3.1, MACRO_SHIFT_MID = 7.3, MACRO_SHIFT_DRY = 11.0;
uniform vec2 detail_fade_m; // the grass's detail fades out between these distances (its second read and micro noise stop paying)
uniform vec2 ground_rough; // the ground's roughness dry and wet
uniform vec2 grass_blotch; // the two reads of the grass blend between these edges of the blotches' noise
uniform float grass_wide_scale; // the wide read of the grass map, of its scale (its macro tone and its tufts)

// Grass without visible tiling: two reads, the second rotated 30 degrees, blended by large blotches.
vec3 grass(vec2 p, float detail) {
	vec3 g = texture(grass_map, p).rgb;
	if (detail <= 0.0) return g;
	vec2 r = vec2(TURN_30_COS * p.x - TURN_30_SIN * p.y, TURN_30_SIN * p.x + TURN_30_COS * p.y) + TILE_SHIFT;
	float k = smoothstep(grass_blotch.x, grass_blotch.y, vnoise(p * 0.25));
	return mix(g, texture(grass_map, r).rgb, k * detail);
}

uniform float grain_per_m;
uniform float grain_repeat;
uniform float grain_low;
uniform float grain_high;
uniform float grain_share;
uniform float grass_mean; // the map's mean luma, linear (render/city_world.gd)
uniform vec2 grain_lod; // the speckle comes in between these levels of the map's mip chain
uniform float tuft_wide_share; // the wide read's share of the tufts
uniform vec4 straw_patch; // the straw's patches: their wide and fine noise (m), the wide's share and the fine's
uniform vec2 straw_gold; // how gold a patch turns: from and over its noise, times the biome's dry share
uniform vec4 straw_mask; // the straw's edges, the map's luma it reads from and over
uniform float straw_luma_share; // the luma's share in the edge
uniform vec3 straw_tone; // the straw's tone over the grass's luma, and its gain
uniform float straw_gain;
uniform vec4 macro_m; // the land's tones: their wide, mid, small and dry noise (m)
uniform vec3 macro_mix; // the wide's, the mid's and the small's shares of the lush noise
uniform vec4 macro_dry; // the dry stretches' edges; the dry noise's share and the small's
uniform vec2 macro_lush; // the lush noise's edges
uniform vec3 macro_dark; // a hollow's, a meadow's and a dry stretch's tone over the grass
uniform vec3 macro_green;
uniform vec3 macro_straw;
uniform float macro_dry_share; // how much of a dry stretch turns to straw
uniform vec3 patch_mix; // the patches' noise: its share, the fine one's scale over it, the fine one's share
uniform vec2 macro_clamp; // the grass's macro tone, between these
uniform float macro_share;
uniform vec2 patch_shade; // a patch's shade, from and to

// The close-up grain of the grass (Roadcraft's detail layer): a tile of blades under the map, twice its value around 1,
// read a second time turned and larger so it does not repeat (detail_pair.gdshaderinc), gone once a metre takes under a few pixels.
#include "res://render/shaders/detail_pair.gdshaderinc"

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
	return mix(vec3(1.0), detail_tone(grass_detail, uv, dx, dy), w * detail_albedo);
}

// The grass's own map again, at the scale of a pixel wherever the camera stands: tufts half a pixel to one across, two
// octaves of the map a level apart, so a field keeps its grain from any height; the mip chain would have averaged it away.
float grass_speckle(vec2 xz, float per_pixel) {
	float lod = max(0.0, log2(per_pixel * grain_per_m));
	float level = floor(lod);
	vec2 uv = xz / grass_m * grain_repeat;
	vec2 u0 = uv / exp2(level), u1 = uv / exp2(level + 1.0) + TILE_SHIFT;
	vec2 d0x = dFdx(u0), d0y = dFdy(u0), d1x = dFdx(u1), d1y = dFdy(u1);
	float g0 = dot(textureGrad(grass_map, u0, d0x, d0y).rgb, LUMA);
	float g1 = dot(textureGrad(grass_map, u1, d1x, d1y).rgb, LUMA);
	float grain = mix(g0, g1, lod - level) / max(grass_mean, LEAST_LUMA);
	return mix(1.0, clamp(grain, grain_low, grain_high), smoothstep(grain_lod.x, grain_lod.y, lod) * grain_share);
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
	vec2 wide = vec2(TURN_20_COS * p.x - TURN_20_SIN * p.y, TURN_20_SIN * p.x + TURN_20_COS * p.y) * grass_wide_scale + WIDE_SHIFT;
	vec2 blade = xz / detail_m;
	vec2 p_dx = dFdx(p), p_dy = dFdy(p);
	vec2 wide_dx = dFdx(wide), wide_dy = dFdy(wide);
	vec2 blade_dx = dFdx(blade), blade_dy = dFdy(blade);
	if (share == 0.0) return vec2(0.0);
	vec2 near_n = textureGrad(grass_normal, p, p_dx, p_dy).xy * 2.0 - 1.0;
	vec2 wide_n = textureGrad(grass_normal, wide, wide_dx, wide_dy).xy * 2.0 - 1.0;
	vec2 tufts = mix(near_n, wide_n, tuft_wide_share) * grass_bump;
	float blades = blade_weight(per_pixel);
	if (blades > 0.0) {
		vec2 blade_n = textureGrad(grass_blades_normal, blade, blade_dx, blade_dy).xy * 2.0 - 1.0;
		tufts = tufts + blade_n * blade_bump * blades;
	}
	return tufts * share;
}

uniform float biome_dry[1 << PAINT_ID_BITS]; // by the overlay id, as biome_tint: none, then the biomes
uniform float straw_share;

// The dry season over the grass: the map's own texture with its hue turned to straw, patch by patch, each patch green or
// gold with its edge broken by the tufts (a share of each everywhere is only an olive wash).
vec3 straw_over(vec3 c, vec3 map_colour, vec2 xz, int biome) {
	float luma = dot(map_colour, LUMA);
	float patchy = vnoise(xz / straw_patch.x + STRAW_SHIFT_WIDE) * straw_patch.z + vnoise(xz / straw_patch.y + STRAW_SHIFT_FINE) * straw_patch.w;
	float golden = clamp(biome_dry[biome] * (straw_gold.x + straw_gold.y * patchy), 0.0, 1.0);
	float mask = smoothstep(straw_mask.x, straw_mask.y, golden + (clamp((luma - straw_mask.z) / straw_mask.w, 0.0, 1.0) - 0.5) * straw_luma_share) * straw_share;
	return mix(c, dot(c, LUMA) * straw_tone * straw_gain, mask);
}

// The ground's grass at a world point, before soil and rock: g its map's colour, n its patches, for whoever carries on.
// Land seen from afar is never one green: meadows, dry stretches and darker hollows, wider than any tile of the grass, so the map stops
// reading as one colour however far the camera stands (macro variation over the detail texture).
vec3 macro_tone(vec2 xz) {
	float small = vnoise(xz / macro_m.z + MACRO_SHIFT_SMALL);
	float lush = vnoise(xz / macro_m.x) * macro_mix.x + vnoise(xz / macro_m.y + MACRO_SHIFT_MID) * macro_mix.y + small * macro_mix.z;
	float dry = smoothstep(macro_dry.x, macro_dry.y, vnoise(xz / macro_m.w + MACRO_SHIFT_DRY) * macro_dry.z + small * macro_dry.w);
	vec3 tone = mix(macro_dark, macro_green, smoothstep(macro_lush.x, macro_lush.y, lush));
	return mix(tone, tone * macro_straw, dry * macro_dry_share);
}

vec3 grass_ground(vec2 xz, float detail, vec3 tint, int biome, float per_pixel, out vec3 g, out float n) {
	n = mix(0.5, vnoise(xz * patch_scale) * patch_mix.x + vnoise(xz * patch_scale * patch_mix.y) * patch_mix.z, detail);
	vec2 gp = xz / grass_m;
	g = grass(gp, detail);
	float macro = clamp(dot(grass(gp * grass_wide_scale + WIDE_SHIFT, detail), LUMA) / max(dot(g, LUMA), LEAST_LUMA), macro_clamp.x, macro_clamp.y);
	vec3 c = straw_over(g * mix(1.0, macro, macro_share) * tint * mix(patch_shade.x, patch_shade.y, n), g, xz, biome);
	return c * macro_tone(xz) * grass_grain(xz, per_pixel) * grass_speckle(xz, per_pixel);
}
