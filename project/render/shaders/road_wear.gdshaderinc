// Where a street is dirty and where it is worn. COLOR holds how far the vertex is from the surface's two edges
// (road_look.h wear_color), so a band follows the kerb round every curve without a texture of its own; UV2 the asphalt's own wear
// (road_look.h: off the carriageway's edge; a lane across itself and off a mouth).
const float WEAR_SPAN_M = 25.5;
const float WEAR_LANE_M = 12.75;
const float TONE_SLOTS = 8.0;

const float MOUTH_FADE_M = 6.0; // a lane's tracks and oil come in over this far from a mouth on a node's paving, which has none

// The carriageway by its edge, `edge` metres from it: darker and browner towards the kerb. Every asphalt of a carriageway wears it:
// the lanes, the bays, a node's paving round its corners.
vec3 gutter_wear(float edge, float grain) {
	float gutter = 1.0 - smoothstep(0.05, 0.65, edge);
	return vec3(1.0 - 0.3 * gutter * (0.6 + 0.8 * grain)) * mix(vec3(1.0), vec3(1.06, 0.99, 0.9), gutter * 0.6);
}

// A lane, `across` metres from its own edge: its wheel tracks polished and oil down its middle, `off_mouth` metres along from the
// nearest mouth that opens on a node's paving.
float track_wear(float across, float lane, float off_mouth, float grain) {
	if (lane <= 0.5) return 1.0;
	float centre = abs(mod(across, lane) - 0.5 * lane);
	float tracks = exp(-pow((centre - 0.85) / 0.3, 2.0));
	float oil = exp(-pow(centre / 0.32, 2.0));
	return 1.0 + smoothstep(0.0, MOUTH_FADE_M, off_mouth) * (0.11 * tracks - 0.14 * oil * (0.7 + 0.6 * grain));
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
