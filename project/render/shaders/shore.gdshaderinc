// The shore: a band set by height over the water beside it (render/water/shore_field.h), sand on a gentle bank and damp
// earth on a steep one, darker where it is wet. Needs land() and dirt_layer, dirt_m.

uniform highp sampler2DArray shore_map : repeat_disable, filter_nearest;
uniform highp sampler2D shore_pages : repeat_disable, filter_nearest;
uniform vec3 shore_grid = vec3(0.0, 1.0, 2.0); // half the map, metres between samples, samples a side; no half: no levels yet
uniform vec3 sand : source_color;
uniform vec3 bank_tint : source_color;
uniform float beach_from_m;
uniform float beach_to_m;
uniform float beach_wander_m;
uniform float bank_from_m;
uniform float bank_to_m;
uniform float bank_wander_m;
uniform float wet_from_m;
uniform float wet_to_m;
uniform float wet_keep;
uniform float shore_gentle_from; // 1 - normal.y where a beach starts to give way to a bank
uniform float shore_gentle_to;
uniform float shore_wander_len_m; // metres one wander of the band's edge runs along the shore
uniform float sand_grain_m;
uniform float sand_speck_m;

const float SHORE_DRY = -500.0; // under this a sample has no water by it (TerrainGen::NO_WATER)
const float SHORE_FADE_FROM = 0.15, SHORE_FADE_TO = 0.85, SHORE_FADE_WANDER = 0.3;
const float SHORE_WET_FROM = 0.03, SHORE_WET_TO = 0.35;

// Four-corner sampling crosses pages explicitly. The bank table switches only after every changed page is ready.
float shore_texel(ivec2 p) {
	int side = textureSize(shore_map, 0).x;
	ivec2 page = p / side;
	int across = textureSize(shore_pages, 0).x;
	int bank = int(texelFetch(shore_pages, page, 0).r + 0.5);
	int layer = (page.y * across + page.x) * 2 + bank;
	return texelFetch(shore_map, ivec3(p - page * side, layer), 0).r;
}

// The level of the water beside `xz` from the samples round it that have one; `cover` is how much of the point they cover.
float shore_level(vec2 xz, out float cover) {
	vec2 g = clamp((xz + shore_grid.x) / shore_grid.y, vec2(0.0), vec2(shore_grid.z - 1.001));
	vec2 i = floor(g), f = g - i;
	ivec2 p = ivec2(i);
	vec4 level = vec4(shore_texel(p), shore_texel(p + ivec2(1, 0)),
		shore_texel(p + ivec2(0, 1)), shore_texel(p + ivec2(1, 1)));
	vec4 w = vec4((1.0 - f.x) * (1.0 - f.y), f.x * (1.0 - f.y), (1.0 - f.x) * f.y, f.x * f.y) * step(vec4(SHORE_DRY), level);
	cover = w.x + w.y + w.z + w.w;
	return cover > 0.00001 ? dot(level, w) / cover : SHORE_DRY;
}

vec3 shore_over(vec3 c, vec3 world, float slope, vec3 wdx, vec3 wdy) {
	if (shore_grid.x <= 0.0) return c;
	float cover;
	float level = shore_level(world.xz, cover);
	if (level <= SHORE_DRY) return c;
	float above = world.y - level;
	if (above > bank_to_m + bank_wander_m) return c;
	float wander = land(dirt_layer, fract(world.xz / shore_wander_len_m), wdx.xz / shore_wander_len_m, wdy.xz / shore_wander_len_m).b;
	float fade = smoothstep(SHORE_FADE_FROM, SHORE_FADE_TO, cover + (wander - 0.5) * SHORE_FADE_WANDER);
	float gentle = 1.0 - smoothstep(shore_gentle_from, shore_gentle_to, slope);
	float grain = dot(land(dirt_layer, fract(world.xz / sand_grain_m), wdx.xz / sand_grain_m, wdy.xz / sand_grain_m), vec3(0.33));
	float speck = dot(land(dirt_layer, fract(world.xz / sand_speck_m), wdx.xz / sand_speck_m, wdy.xz / sand_speck_m), vec3(0.33));
	float beach = (1.0 - smoothstep(beach_from_m + wander * beach_wander_m, beach_to_m + wander * beach_wander_m, above)) * gentle * fade;
	c = mix(c, sand * mix(0.86, 1.1, grain) * mix(0.94, 1.04, speck), beach);
	float bank = (1.0 - smoothstep(bank_from_m + wander * bank_wander_m, bank_to_m + wander * bank_wander_m, above)) * (1.0 - gentle) * fade;
	vec3 earth = land(dirt_layer, fract(world.xz / dirt_m), wdx.xz / dirt_m, wdy.xz / dirt_m);
	c = mix(c, earth * bank_tint, bank);
	float wet = (1.0 - smoothstep(wet_from_m, wet_to_m, above)) * smoothstep(SHORE_WET_FROM, SHORE_WET_TO, cover);
	return c * mix(1.0, wet_keep, wet);
}
