// What the glass of both building levels shows of the sky. Needs glass_sky, glass_sky_floor and night.
const float GLASS_ENV = 1.6, GLASS_ENV_DARK = 1.3; // how much of the sky the clear and the darker glass reflect

// The sky's reflection on glass, as light leaving it, for a glass reflecting `env` times the sky: Forward+ already reflects it
// once by itself, the compatibility renderer little of it.
vec3 glass_reflection(vec3 V, vec3 n, float F, float env) {
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
	float own = env;
#else
	float own = env - 1.0;
#endif
	return uniform_linear(glass_sky) * own * mix(0.75, 1.15, clamp(-V.y + 0.2, 0.0, 1.0)) * (F + glass_sky_floor) * (1.0 - night);
}
