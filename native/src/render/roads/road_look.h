// What the road meshes look like, read once from project/data/render/streets.json: strip colours and heights, the
// junction corner and dead-end shapes, painted markings, gutters and dropped kerbs. Where the lamps the night light
// pools follow stand and where the crosswalks lie is the simulation's (sim/roads/street_furniture.h). Also the vertex
// conventions shared with render/shaders/road.gdshader (material id in COLOR.a).
#pragma once

#include "sim/roads/profiles.h"

#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <vector>

namespace city {

// Road shader materials (render/shaders/road.gdshader decodes COLOR.a * MAT_SCALE).
enum class RoadMat : int { ASPHALT = 0, PLATE, FOOTWAY, KERB, GRASS, CONCRETE, LANE, PAINT, PAVERS, STONE, COBBLE,
	BALLAST, SKIRT };
constexpr float MAT_SCALE = 16.f;
constexpr float MAT_HALF = 0.5f;
// UV2.x of a vertex that is not lit by a lamp pool, and of a junction plate (lit evenly).
constexpr float POOL_NONE = 1000.f, POOL_PLATE = -1000.f;

inline godot::Color road_color(const godot::Color &srgb, RoadMat m) {
	return godot::Color(srgb.r, srgb.g, srgb.b, (float(int(m)) + MAT_HALF) / MAT_SCALE);
}

// The wear (road_wear.gdshaderinc) reads how far a vertex is from its surface's two edges in place of a colour: R and G in
// metres over WEAR_SPAN_M, interpolated exactly across a strip because they are linear in it. On a lane, B is the lane's
// width over WEAR_LANE_M, and the class's tone travels in the fraction of the material id.
constexpr float WEAR_SPAN_M = 25.5f, WEAR_LANE_M = 12.75f;
constexpr float TONE_SLOTS = 8.f;
inline godot::Color wear_color(float from_a, float from_b, float lane, RoadMat m, int tone) {
	return godot::Color(from_a / WEAR_SPAN_M, from_b / WEAR_SPAN_M, lane / WEAR_LANE_M, (float(int(m)) + (float(tone) + MAT_HALF) / TONE_SLOTS) / MAT_SCALE);
}

struct StreetLook {
	godot::Color surface[4], paving[4], median[4], asphalt[int(RoadClass::COUNT)], bike, shoulder, curb, barrier, plate_concrete;
	float h_curb = 0, h_curb_median = 0, h_sidewalk = 0, h_median = 0, h_barrier = 0, step = 1, skirt = 0, median_nose = 0;
	float face_eps = 0; // coplanar faces closer than this are one
	float verge_reach = 0, verge_under = 0;
	int verge_strips = 1;
	// dropped kerbs at crossings (metres from the mouth) and the ramp band of the footway
	float ramp_full = 0, ramp_flat = 0, ramp_flare = 0, ramp_low = 0, ramp_run = 0, ramp_min_walk = 0;
	float arc_step = 0;
	int bevel_steps = 1, bulb_steps = 1;
	godot::Color white, yellow;
	float paint_lift = 0, zebra_period = 1, zebra_bar = 0, zebra_edge = 0, double_gap = 0;
	// Longitudinal lines of the chosen paint style (streets.json markings.styles): the centre and lane widths, wider from
	// fast_kmh on where the style says so, a bus lane's `exclusive_w`, a double line's `double_w`; dashes `dash[k]` long and
	// `gap[k]` apart below dash_kmh[k]. multi_lane_double: the norm's double line where a direction has more than one lane.
	float centre_w = 0, lane_w = 0, fast_kmh = 0, fast_centre_w = 0, fast_lane_w = 0, exclusive_w = 0, double_w = 0;
	bool multi_lane_double = false;
	std::vector<float> dash_kmh, dash, gap;

	bool load(const godot::Dictionary &t, godot::PackedStringArray &errors);
	// The carriageway's colour: asphalt keeps its class's tone, the other pavings their own.
	const godot::Color &carriage(const Profile &p) const {
		return p.surface == Surface::ASPHALT ? asphalt[int(p.road_class)] : surface[int(p.surface)];
	}
};

} // namespace city
