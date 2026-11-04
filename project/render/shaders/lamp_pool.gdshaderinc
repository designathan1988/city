// The fall of a street lamp's pool from its centre to its rim (Roadcraft lightLevels.ts): a bright core and a long soft
// edge that ends at nothing, so the ground between two lamps goes dark. Read by the road and by a placed lamp's pool.
float pool_fall(float r) {
	if (r < 0.35) {
		return mix(1.0, 0.55, r / 0.35);
	}
	if (r < 0.7) {
		return mix(0.55, 0.15, (r - 0.35) / 0.35);
	}
	return mix(0.15, 0.0, clamp((r - 0.7) / 0.3, 0.0, 1.0));
}
