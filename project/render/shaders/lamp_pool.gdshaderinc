// How far past the reach of the lamps' own pools a point of the road is, 0 .. 1: near, a lamp lays its pool as a disc
// (lamp_glow.gdshader); from `far_m` on, where the chunks of street furniture end, the road draws them itself (road.gdshader).
float pool_handed_over(float eye_m, float far_m, float handover_m) {
	return clamp((eye_m - far_m + handover_m) / handover_m, 0.0, 1.0);
}

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
