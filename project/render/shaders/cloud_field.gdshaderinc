// The shape of a placed cloud, for atmosphere.gdshader: a cumulus of a few puffs joined by a smooth minimum, flat at its
// base (its condensation level), its outline worn by low and fine noise as Schneider builds the clouds of Horizon Zero
// Dawn ("The Real-Time Volumetric Cloudscapes of Horizon Zero Dawn", SIGGRAPH 2015): a base shape less an erosion that
// spares its heart. Needs the uniforms cloud, puff, cloud_look and noise_volume.

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
	float d = 1e5;
	for (int k = 0; k < PUFFS; k++) {
		vec4 s = puff[c * PUFFS + k];
		s.xz += cloud[c].xz; // shape offsets are immutable; only the 24 cloud centres move with wind/view wrapping
		vec3 o = p - s.xyz;
		o.y *= 1.3; // each puff a little flattened
		d = smooth_min(d, length(o) - s.w, size * 0.2);
	}
	float base = smoothstep(cloud_look[c].x - size * 0.12, cloud_look[c].x + size * 0.08, p.y);
	return -d / size * base;
}

// Its density at a point: the outline broken by the low noise, the thin edge worn into wisps by the fine one.
float cloud_density(int c, vec3 p, float time) {
	float size = cloud[c].w, life = cloud_look[c].y;
	vec3 boil = vec3(time * 0.05, -time * 0.04, time * 0.02);
	float low = texture(noise_volume, p / (size * 1.2) + boil * 0.25).r;
	float shape = clamp(heap(c, p) * 3.2 - (1.0 - low) * (0.55 + 0.4 * (1.0 - life)) - (1.0 - life) * 0.8, 0.0, 1.0);
	if (shape <= 0.0) return 0.0;
	float fine = texture(noise_volume, p / (size * 0.22) + boil).r;
	float up = clamp((p.y - cloud_look[c].x) / size, 0.0, 1.0);
	float detail = mix(1.0 - fine, fine, clamp(up * 3.0, 0.0, 1.0));
	float erode = detail * (0.55 + 0.3 * (1.0 - life));
	return clamp((shape - erode) / (1.0 - erode), 0.0, 1.0) * cloud_look[c].z;
}
