// Where a street is dirty and where it is worn. COLOR holds how far the vertex is from the surface's two edges
// (road_look.h wear_color), so a band follows the kerb round every curve without a texture of its own.
#include "res://render/shaders/contracts/streets.gdshaderinc"
const float TONE_SLOTS = float(ROAD_TONE_SLOTS);

// The carriageway: darker and browner towards the kerb, each lane's wheel tracks polished and oil down its middle.
vec3 lane_wear(float edge, float lane, float grain) {
	float gutter = 1.0 - smoothstep(0.05, 0.65, edge);
	vec3 k = vec3(1.0 - 0.3 * gutter * (0.6 + 0.8 * grain)) * mix(vec3(1.0), vec3(1.06, 0.99, 0.9), gutter * 0.6);
	if (lane > 0.5 && edge > 0.0) {
		float centre = abs(mod(edge, lane) - 0.5 * lane);
		float tracks = exp(-pow((centre - 0.85) / 0.3, 2.0));
		float oil = exp(-pow(centre / 0.32, 2.0));
		k *= 1.0 + 0.11 * tracks - 0.14 * oil * (0.7 + 0.6 * grain);
	}
	return k;
}

// The footway: grime by the kerb, damp and green by the lot, lighter down the middle where people walk.
vec3 footway_wear(float from_kerb, float width, float grain) {
	float near_kerb = 1.0 - smoothstep(0.1, 0.6, from_kerb);
	float near_lot = 1.0 - smoothstep(0.0, 0.55, width - from_kerb);
	float walked = exp(-pow((from_kerb - 0.5 * width) / max(0.22 * width, 0.3), 2.0));
	float g = 0.6 + 0.8 * grain;
	return vec3((1.0 - 0.2 * near_kerb * g) * (1.0 - 0.24 * near_lot * g) * (1.0 + 0.07 * walked))
			* mix(vec3(1.0), vec3(0.9, 1.0, 0.86), near_lot * 0.4);
}
