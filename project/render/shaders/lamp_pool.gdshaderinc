// How far past the lamps with a light of their own a point of the road is, 0 .. 1 (`lit`: from where they start giving way to
// where they end, render/props/lamp_lights.h): beyond them the road draws the pools itself (road.gdshader).
const float LEAST_SPAN_M = 0.001; // the hand-over's span divided by never under this

float pool_handed_over(float eye_m, vec2 lit) {
	return clamp((eye_m - lit.x) / max(lit.y - lit.x, LEAST_SPAN_M), 0.0, 1.0);
}

// The fall of a street lamp's pool from its centre to its rim (Roadcraft lightLevels.ts): a bright core and a long soft
// edge that ends at nothing, so the ground between two lamps goes dark. Its stops: data/render/materials.json _shared lamp_pool.
uniform vec4 pool_stops; // the core's radius and its level there, the edge's radius and its level there; nothing at 1
float pool_fall(float r) {
	if (r < pool_stops.x) {
		return mix(1.0, pool_stops.y, r / pool_stops.x);
	}
	if (r < pool_stops.z) {
		return mix(pool_stops.y, pool_stops.w, (r - pool_stops.x) / (pool_stops.z - pool_stops.x));
	}
	return mix(pool_stops.w, 0.0, clamp((r - pool_stops.z) / (1.0 - pool_stops.z), 0.0, 1.0));
}
