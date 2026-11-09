// What the ecosystem leaves on the ground at a world point (native/src/render/cover/ecology_raster.h, handed over by
// render/ecology_view.gd): the dry herbs, the waterlogged ground and the bare soil, each 0..1, read bilinear between the corners
// of the ecology's grid by the sampler (Roadcraft render/terrain.ts terrainCorner). No raster yet: none of them. Needs LUMA and
// straw_mask (terrain_layers.gdshaderinc).
#ifndef ECOLOGY_INC
#define ECOLOGY_INC

uniform sampler2D ecology_map : filter_linear, repeat_disable;
uniform vec3 ecology_grid = vec3(0.0, 1.0, 2.0); // half the map, metres between corners, corners a side; no half: no raster yet

// x the dry herbs (grass times its dry share), y the waterlogged ground, z the soil the herbs and trees leave bare.
vec3 ecology_at(vec2 xz) {
	if (ecology_grid.x <= 0.0) return vec3(0.0);
	vec2 g = (xz + ecology_grid.x) / ecology_grid.y;
	return texture(ecology_map, (g + 0.5) / ecology_grid.z).rgb;
}

uniform float bare_share; // how much of the bare share turns the ground to soil
uniform vec2 bare_lift; // the soil's shade: from, and over the grass map's lift (its luma between straw_mask's)
uniform float sodden_share; // how much of the waterlogged share darkens the ground
uniform vec3 sodden_tone; // the waterlogged ground's tone over it: darker, a deeper green, a little blue

// The ecosystem over the grass `c` (`g` the grass map's colour, `soil` the soil's): the red soil where the herbs are sparse and the
// darker, deeper green of the waterlogged ground (Roadcraft render/terrain.ts, the ecology's classes after the straw).
vec3 ecology_over(vec3 c, vec3 g, vec3 eco, vec3 soil) {
	float lift = clamp((dot(g, LUMA) - straw_mask.z) / straw_mask.w, 0.0, 1.0);
	c = mix(c, soil * (bare_lift.x + bare_lift.y * lift), eco.z * bare_share);
	return mix(c, c * sodden_tone, eco.y * sodden_share);
}

#endif
