// Terrain sampling for terrain.gdshader: value noise, the stacked land map read flat or along the three world axes
// (triplanar) and the grass map without visible tiling. Needs the uniforms land_map, land_layers and grass_map.

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

// One layer of the stacked land map at `p`, with the derivatives of the unwrapped coordinates taken by the caller in
// uniform control flow (no seams at the wrap, no undefined derivatives inside a branch).
vec3 land(float layer, vec2 p, vec2 dx, vec2 dy) {
	vec2 q = vec2(fract(p.x), (layer + fract(p.y)) / land_layers);
	vec2 k = vec2(1.0, 1.0 / land_layers);
	return textureGrad(land_map, q, dx * k, dy * k).rgb;
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
