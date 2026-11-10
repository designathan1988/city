// How far past the lamps with a light of their own a point of the road is, 0 .. 1 (`lit`: from where they start giving way to
// where they end, render/props/lamp_lights.h): beyond them the road draws the pools itself (road.gdshader).
float pool_handed_over(float eye_m, vec2 lit) {
	return clamp((eye_m - lit.x) / max(lit.y - lit.x, 0.001), 0.0, 1.0);
}

// The fall of a street lamp's pool from its centre to its rim (Roadcraft lightLevels.ts): a bright core and a long soft
// edge that ends at nothing, so the ground between two lamps goes dark.
float pool_fall(float r) {
	if (r < 0.35) {
		return mix(1.0, 0.55, r / 0.35);
	}
	if (r < 0.7) {
		return mix(0.55, 0.15, (r - 0.35) / 0.35);
	}
	return mix(0.15, 0.0, clamp((r - 0.7) / 0.3, 0.0, 1.0));
}
