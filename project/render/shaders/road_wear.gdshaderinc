// Where a street is dirty and where it is worn. COLOR holds how far the vertex is from the surface's two edges
// (road_look.h wear_color), so a band follows the kerb round every curve without a texture of its own.
#include "res://render/shaders/contracts/streets.gdshaderinc"
const float TONE_SLOTS = float(ROAD_TONE_SLOTS);
// Their look: data/render/materials.json road.params, by eye as the 4.0's streets; lengths in metres from an edge, shares of
// the surface's light; each vector's parts are said here.
uniform vec2 wear_grain; // what the grain adds to a stain: from and over it
uniform vec2 gutter_band; // the gutter: darker between these distances from the kerb
uniform vec2 gutter_look; // how much darker, and the share of its brown tint
uniform vec3 gutter_tint;
uniform vec3 lane_tracks; // the wheel tracks' distance from the lane's middle and their width; the oil's width down it
uniform vec4 track_look; // how much the tracks shine, how much the oil darkens, its grain from and over
uniform vec2 kerb_band; // the footway's grime: between these distances from the kerb
uniform float lot_band; // its damp: within this distance of the lot
uniform vec2 walk_band; // where people walk: the middle, this share of the width wide, at least the second
uniform vec4 footway_look; // how dark the grime and the damp, how much lighter the walked middle, the damp's green share
uniform vec3 lot_tint;

// The carriageway: darker and browner towards the kerb, each lane's wheel tracks polished and oil down its middle.
vec3 lane_wear(float edge, float lane, float grain) {
	float gutter = 1.0 - smoothstep(gutter_band.x, gutter_band.y, edge);
	vec3 k = vec3(1.0 - gutter_look.x * gutter * (wear_grain.x + wear_grain.y * grain)) * mix(vec3(1.0), gutter_tint, gutter * gutter_look.y);
	if (lane > 0.5 && edge > 0.0) {
		float centre = abs(mod(edge, lane) - 0.5 * lane);
		float tracks = exp(-pow((centre - lane_tracks.x) / lane_tracks.y, 2.0));
		float oil = exp(-pow(centre / lane_tracks.z, 2.0));
		k *= 1.0 + track_look.x * tracks - track_look.y * oil * (track_look.z + track_look.w * grain);
	}
	return k;
}

// The footway: grime by the kerb, damp and green by the lot, lighter down the middle where people walk.
vec3 footway_wear(float from_kerb, float width, float grain) {
	float near_kerb = 1.0 - smoothstep(kerb_band.x, kerb_band.y, from_kerb);
	float near_lot = 1.0 - smoothstep(0.0, lot_band, width - from_kerb);
	float walked = exp(-pow((from_kerb - 0.5 * width) / max(walk_band.x * width, walk_band.y), 2.0));
	float g = wear_grain.x + wear_grain.y * grain;
	return vec3((1.0 - footway_look.x * near_kerb * g) * (1.0 - footway_look.y * near_lot * g) * (1.0 + footway_look.z * walked))
			* mix(vec3(1.0), lot_tint, near_lot * footway_look.w);
}
