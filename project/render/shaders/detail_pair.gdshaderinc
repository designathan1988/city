// A close-up map read twice, the second time turned by a radian, smaller and shifted so that no tile shows: the street surfaces'
// detail layer (road_detail.gdshaderinc) and the grass's grain (terrain_layers.gdshaderinc).
#ifndef DETAIL_PAIR_INC
#define DETAIL_PAIR_INC

const mat2 DETAIL_TURN = mat2(vec2(0.5403, 0.8415), vec2(-0.8415, 0.5403)); // a turn of one radian: cos 1, sin 1
const float DETAIL_SECOND = 0.4137; // scale of the second read
const float DETAIL_MIX = 0.55; // its share
const vec2 DETAIL_SHIFT = vec2(0.31); // the second read off the first's lattice

vec3 detail_tone(sampler2D map, vec2 uv, vec2 dx, vec2 dy) {
	vec3 a = textureGrad(map, uv, dx, dy).rgb * 2.0;
	vec3 b = textureGrad(map, DETAIL_TURN * uv * DETAIL_SECOND + DETAIL_SHIFT, DETAIL_TURN * dx * DETAIL_SECOND, DETAIL_TURN * dy * DETAIL_SECOND).rgb * 2.0;
	return a * mix(vec3(1.0), b, DETAIL_MIX);
}

vec2 detail_bend(sampler2D map, vec2 uv, vec2 dx, vec2 dy) {
	vec2 a = textureGrad(map, uv, dx, dy).xy * 2.0 - 1.0;
	vec2 b = textureGrad(map, DETAIL_TURN * uv * DETAIL_SECOND + DETAIL_SHIFT, DETAIL_TURN * dx * DETAIL_SECOND, DETAIL_TURN * dy * DETAIL_SECOND).xy * 2.0 - 1.0;
	return a + (b * DETAIL_TURN) * DETAIL_MIX;
}

#endif
