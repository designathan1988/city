// Value noise on the integer hash, shared by terrain.gdshader, water.gdshader and road.gdshader.
#include "res://render/shaders/hash.gdshaderinc"

float hash2(vec2 p) {
	ivec2 i = ivec2(floor(p));
	return h01(spatial_hash(uint(i.x + 32768), uint(i.y + 32768)));
}

float vnoise(vec2 p) {
	vec2 i = floor(p);
	vec2 f = fract(p);
	vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(mix(hash2(i), hash2(i + vec2(1, 0)), u.x), mix(hash2(i + vec2(0, 1)), hash2(i + vec2(1, 1)), u.x), u.y);
}
