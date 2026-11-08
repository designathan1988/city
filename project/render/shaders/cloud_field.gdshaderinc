// The shape of a placed cloud, for atmosphere.gdshader: a cumulus of a few puffs joined by a smooth minimum, flat at its
// base (its condensation level), its outline worn by low and fine noise as Schneider builds the clouds of Horizon Zero
// Dawn ("The Real-Time Volumetric Cloudscapes of Horizon Zero Dawn", SIGGRAPH 2015): a base shape less an erosion that
// spares its heart. Needs the uniforms cloud, puff, cloud_look and noise_volume; its own look is data/game/sky.json atmosphere.

const float FAR_HEAP = 1e5; // farther from every puff than any point the pass asks about, in metres

uniform float puff_flat; // each puff a little flattened: its height counted this much more
uniform float puff_blend; // the smooth minimum joining the puffs, of the cloud's size
uniform vec2 cloud_base_edge; // the flat base fades in from this share of the size under it to this share over it
uniform vec3 cloud_boil; // the noise's drift a second, across, up and along (of a noise cell)
uniform vec3 cloud_noise; // the low noise's cell and the fine one's, of the size; the low noise's share of the drift
uniform vec4 cloud_shape_k; // the heap's gain; what the low noise wears, from and over the life spent; what the life spent takes
uniform vec2 cloud_erode; // what the fine noise wears off the outline, from and over the life spent

// Where a ray enters and leaves a sphere (both negative: it misses).
vec2 sphere_span(vec3 ro, vec3 rd, vec4 s) {
	vec3 oc = ro - s.xyz;
	float b = dot(oc, rd);
	float h = b * b - (dot(oc, oc) - s.w * s.w);
	if (h < 0.0) return vec2(-1.0);
	h = sqrt(h);
	return vec2(-b - h, -b + h);
}

float smooth_min(float a, float b, float k) {
	float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
	return mix(b, a, h) - k * h * (1.0 - h);
}

// How far inside the heap a point is, in the cloud's own size (> 0 inside).
float heap(int c, vec3 p) {
	float size = cloud[c].w;
	float d = FAR_HEAP;
	for (int k = 0; k < PUFFS; k++) {
		vec4 s = puff[c * PUFFS + k];
		vec3 o = p - s.xyz;
		o.y *= puff_flat;
		d = smooth_min(d, length(o) - s.w, size * puff_blend);
	}
	float base = smoothstep(cloud_look[c].x - size * cloud_base_edge.x, cloud_look[c].x + size * cloud_base_edge.y, p.y);
	return -d / size * base;
}

// The base shape at a point of heap `h`: the heap less what the low noise wears (`worn`, 1 - the noise), thinner as the cloud's
// `life` runs out. The body and its shadow take the same shape (the shadow with the noise at its mean).
float cloud_shape(float h, float worn, float life) {
	return clamp(h * cloud_shape_k.x - worn * (cloud_shape_k.y + cloud_shape_k.z * (1.0 - life)) - (1.0 - life) * cloud_shape_k.w, 0.0, 1.0);
}

// The density left of `shape` once the fine noise's `detail` wears its outline.
float cloud_worn(float shape, float detail, float life) {
	float erode = detail * (cloud_erode.x + cloud_erode.y * (1.0 - life));
	return clamp((shape - erode) / (1.0 - erode), 0.0, 1.0);
}

// Its density at a point: the outline broken by the low noise, the thin edge worn into wisps by the fine one.
float cloud_density(int c, vec3 p, float time) {
	float size = cloud[c].w, life = cloud_look[c].y;
	vec3 boil = time * cloud_boil;
	float low = texture(noise_volume, p / (size * cloud_noise.x) + boil * cloud_noise.z).r;
	float shape = cloud_shape(heap(c, p), 1.0 - low, life);
	if (shape <= 0.0) return 0.0;
	float fine = texture(noise_volume, p / (size * cloud_noise.y) + boil).r;
	float up = clamp((p.y - cloud_look[c].x) / size, 0.0, 1.0);
	float detail = mix(1.0 - fine, fine, clamp(up * 3.0, 0.0, 1.0));
	return cloud_worn(shape, detail, life) * cloud_look[c].z;
}
