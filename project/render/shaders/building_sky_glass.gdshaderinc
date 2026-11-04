// What the glass of both building levels shows of the sky. Needs glass_sky, glass_sky_floor and night.
// The sky's reflection on glass, as light leaving it (the compatibility renderer reflects little of the sky on its own).
vec3 glass_reflection(vec3 V, vec3 n, float F) {
	return uniform_linear(glass_sky) * mix(0.75, 1.15, clamp(-V.y + 0.2, 0.0, 1.0)) * (F + glass_sky_floor) * (1.0 - night);
}
