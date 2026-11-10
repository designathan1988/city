// Value noise on an integer hash, shared by terrain.gdshader, water.gdshader and road.gdshader.

// Integer hash (pcg, as building.gdshader): exact on every GPU, unlike fract(sin(...)).
uint pcg(uint v) {
	uint s = v * 747796405u + 2891336453u;
	uint w = ((s >> ((s >> 28u) + 4u)) ^ s) * 277803737u;
	return (w >> 22u) ^ w;
}

float hash2(vec2 p) {
	ivec2 i = ivec2(floor(p));
	uint h = uint(i.x + 32768) * 73856093u ^ uint(i.y + 32768) * 19349663u;
	return float(pcg(h)) / 4294967295.0;
}

float vnoise(vec2 p) {
	vec2 i = floor(p);
	vec2 f = fract(p);
	vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(mix(hash2(i), hash2(i + vec2(1, 0)), u.x), mix(hash2(i + vec2(0, 1)), hash2(i + vec2(1, 1)), u.x), u.y);
}
