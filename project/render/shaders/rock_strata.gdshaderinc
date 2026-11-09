// The rock where it breaks out of the land (terrain_ground.gdshaderinc), as Roadcraft's render/terrain.ts draws it: the rock the
// land is made of (the painted geology, granite where none was), each with its own layering by HEIGHT, level on every cliff and
// never repeating: granite's exfoliation sheets, sandstone's beds of tan, cream and rust with a harder ledge now and then, basalt's
// stacked lava flows with a redder, weathered top. Where two rocks meet, the boundary breaks along the stone (height blending,
// Mishkinis, "Advanced Terrain Texture Splatting") instead of one rock fading into the other. The soil on a slope is the rock it
// weathered from. Needs land_triplanar, hash2, vnoise, LUMA, rock_m, rock_layer (granite's face) and the land map's other rock faces
// (ground_paint.gdshaderinc ROCK_*); its
// look is data/render/palette.json terrain (strata_*, granite_*, sandstone_*, basalt_*, rock_blend, soil_by_rock).
#ifndef ROCK_STRATA_INC
#define ROCK_STRATA_INC

const int SAND_TINTS = 6; // the sandstone's bed tints
uniform vec2 strata_warp_m; // the two noises that warp the layers off level: a wide one and a hill-sized one
uniform vec3 granite_sheet; // a sheet's thickness, how far the wide and the hill-sized noise move it (m)
uniform vec3 granite_tones[2]; // a sheet's tone over the face texture, between these
uniform vec2 granite_edge; // where a sheet gives way to the next, over its thickness
uniform vec3 sandstone_bed; // a principal bed's thickness, how far the wide and the hill-sized noise move it (m)
uniform vec3 sandstone_tints[SAND_TINTS]; // a bed's tint over the face texture: tan and orange mostly, now and then cream, rust or brown-grey
uniform float sandstone_odds[SAND_TINTS]; // the odds, summed in order, of each tint
uniform vec4 sandstone_edge; // where a principal bed and a fine one give way to the next, over their thickness
uniform vec3 sandstone_fine; // fine beds per principal one, their shift, their share of the tint
uniform vec2 sandstone_hard; // the odds a bed is not hard; how much darker its ledge is
uniform vec4 sandstone_hard_edge; // where the hard ledge comes in and goes again, over the bed's thickness
uniform vec2 basalt_flow; // a lava flow's thickness, how far the wide noise moves it (m)
uniform vec3 basalt_tones[2]; // a flow's grey over the face texture, between these
uniform vec3 basalt_top; // the weathered, bubbly top of each flow
uniform vec4 basalt_top_edge; // where the top comes in and goes again, over the flow's thickness
uniform vec2 rock_blend; // the rock's luma's share in the height blend between rocks, and the blend's softness
uniform float rock_least; // a rock under this share is not read
uniform vec3 soil_by_rock[3]; // the soil's tint over granite, sandstone and basalt

// Lattice rows of the hash, one per use, apart so no two read the same values (any numbers off each other: Roadcraft's).
const float ROW_SAND_TINT = 17.0, ROW_SAND_FINE = 40.0, ROW_SAND_HARD = 5.0, ROW_GRANITE = 47.0, ROW_BASALT = 31.0;
const float WARP_SHIFT = 0.37; // the hill-sized warp read off the wide one's lattice

float strata_hash(float i, float row) { return hash2(vec2(i, row)); }

// The two warps at a point, 0..1 each.
vec2 strata_warp(vec2 xz) {
	return vec2(vnoise(xz / strata_warp_m.x), vnoise(xz / strata_warp_m.y + WARP_SHIFT));
}

vec3 sandstone_tint(float i) {
	float r = strata_hash(i, ROW_SAND_TINT);
	for (int k = 0; k < SAND_TINTS - 1; k++)
		if (r < sandstone_odds[k]) return sandstone_tints[k];
	return sandstone_tints[SAND_TINTS - 1];
}

vec3 sandstone_beds(float y, vec2 warp) {
	float t = (y + (warp.x - 0.5) * sandstone_bed.y + (warp.y - 0.5) * sandstone_bed.z) / sandstone_bed.x;
	float i = floor(t), f = t - i;
	vec3 tint = mix(sandstone_tint(i), sandstone_tint(i + 1.0), smoothstep(sandstone_edge.x, sandstone_edge.y, f));
	float t2 = t * sandstone_fine.x + sandstone_fine.y;
	float i2 = floor(t2);
	vec3 fine = mix(sandstone_tint(i2 + ROW_SAND_FINE), sandstone_tint(i2 + ROW_SAND_FINE + 1.0), smoothstep(sandstone_edge.z, sandstone_edge.w, t2 - i2));
	tint *= mix(vec3(1.0), fine, sandstone_fine.z);
	float hard = step(sandstone_hard.x, strata_hash(i + 1.0, ROW_SAND_HARD));
	return tint * (1.0 - hard * sandstone_hard.y * smoothstep(sandstone_hard_edge.x, sandstone_hard_edge.y, f) * (1.0 - smoothstep(sandstone_hard_edge.z, sandstone_hard_edge.w, f)));
}

vec3 granite_sheets(float y, vec2 warp) {
	float t = (y + (warp.x - 0.5) * granite_sheet.y + (warp.y - 0.5) * granite_sheet.z) / granite_sheet.x;
	float i = floor(t), f = t - i;
	vec3 a = mix(granite_tones[0], granite_tones[1], strata_hash(i, ROW_GRANITE));
	vec3 b = mix(granite_tones[0], granite_tones[1], strata_hash(i + 1.0, ROW_GRANITE));
	return mix(a, b, smoothstep(granite_edge.x, granite_edge.y, f));
}

vec3 basalt_flows(float y, vec2 warp) {
	float t = (y + (warp.x - 0.5) * basalt_flow.y) / basalt_flow.x;
	float i = floor(t), f = t - i;
	vec3 tint = mix(basalt_tones[0], basalt_tones[1], strata_hash(i, ROW_BASALT));
	return tint * mix(vec3(1.0), basalt_top, smoothstep(basalt_top_edge.x, basalt_top_edge.y, f) * (1.0 - smoothstep(basalt_top_edge.z, basalt_top_edge.w, f)));
}

// The rock at `world` (`dx`, `dy` its derivatives, `n` its normal) of the land made of `geology` (granite, sandstone, basalt).
vec3 rock_strata(vec3 world, vec3 dx, vec3 dy, vec3 n, vec3 geology) {
	vec2 warp = strata_warp(world.xz);
	vec3 w = world / rock_m, wx = dx / rock_m, wy = dy / rock_m;
	vec3 has = step(vec3(rock_least), geology);
	vec3 cg = vec3(0.0), cs = vec3(0.0), cb = vec3(0.0);
	if (has.x > 0.0) cg = land_triplanar(rock_layer, w, wx, wy, n) * granite_sheets(world.y, warp);
	if (has.y > 0.0) cs = land_triplanar(ROCK_SANDSTONE, w, wx, wy, n) * sandstone_beds(world.y, warp);
	if (has.z > 0.0) cb = land_triplanar(ROCK_BASALT, w, wx, wy, n) * basalt_flows(world.y, warp);
	vec3 hk = (geology + vec3(dot(cg, LUMA), dot(cs, LUMA), dot(cb, LUMA)) * rock_blend.x) * has;
	vec3 bk = max(hk - (max(hk.x, max(hk.y, hk.z)) - rock_blend.y), vec3(0.0)) * has;
	bk /= max(bk.x + bk.y + bk.z, LEAST_WEIGHT);
	return cg * bk.x + cs * bk.y + cb * bk.z;
}

// The soil's tint where the land is made of `geology`.
vec3 soil_tint(vec3 geology) {
	return soil_by_rock[0] * geology.x + soil_by_rock[1] * geology.y + soil_by_rock[2] * geology.z;
}

#endif
