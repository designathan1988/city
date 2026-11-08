// What the road meshes look like, read once from project/data/render/streets.json: strip colours and heights, the
// junction corner and dead-end shapes, painted markings, gutters and dropped kerbs. Where the lamps the night light
// pools follow stand and where the crosswalks lie is the simulation's (sim/roads/street_furniture.h). Also the vertex
// conventions shared with render/shaders/road.gdshader (material id in COLOR.a).
#pragma once

#include "render/roads/street_shapes.h"
#include "sim/roads/profiles.h"
#include "core/shader_contracts/streets.h"

#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <vector>

namespace city {

// A road vertex's material (RoadMat) in COLOR.a over ROAD_MAT_SCALE, at its middle; UV2.x of a vertex no lamp pool lights and of a
// junction plate, lit evenly (ROAD_POOL_NONE, ROAD_POOL_PLATE): core/shader_contracts/streets.h's, which road_body.gdshaderinc reads.
constexpr float MAT_HALF = 0.5f;

inline godot::Color road_color(const godot::Color &srgb, RoadMat m) {
	return godot::Color(srgb.r, srgb.g, srgb.b, (float(int(m)) + MAT_HALF) / ROAD_MAT_SCALE);
}

// The wear (road_wear.gdshaderinc) reads how far a vertex is from its surface's two edges in place of a colour: R and G in
// metres over ROAD_WEAR_SPAN_M, interpolated exactly across a strip because they are linear in it. On a lane, B is the lane's
// width over ROAD_WEAR_LANE_M, and the class's tone travels in the fraction of the material id.
constexpr float TONE_SLOTS = float(ROAD_TONE_SLOTS);
inline godot::Color wear_color(float from_a, float from_b, float lane, RoadMat m, int tone) {
	return godot::Color(from_a / ROAD_WEAR_SPAN_M, from_b / ROAD_WEAR_SPAN_M, lane / ROAD_WEAR_LANE_M, (float(int(m)) + (float(tone) + MAT_HALF) / TONE_SLOTS) / ROAD_MAT_SCALE);
}

struct StreetLook {
	StreetShapes shapes; // every size the emitters draw with beyond the section (streets.json "shapes")
	godot::Color surface[4], paving[4], median[4], asphalt[int(RoadClass::COUNT)], bike, shoulder, curb, barrier;
	float far_step = 0, far_pull = 0; // the far level: its stretches on a curve; how far towards the eye it is drawn, over the coarser ground out there
	float h_curb = 0, h_curb_median = 0, h_sidewalk = 0, h_median = 0, h_barrier = 0, step = 1, skirt = 0, median_nose = 0;
	float face_eps = 0; // coplanar faces closer than this are one
	// dropped kerbs at crossings (metres from the mouth) and the ramp band of the footway
	float ramp_full = 0, ramp_flat = 0, ramp_flare = 0, ramp_low = 0, ramp_run = 0, ramp_min_walk = 0;
	float arc_step = 0;
	int bevel_steps = 1, bulb_steps = 1;
	godot::Color white, yellow;
	float paint_lift = 0;
	// The regional paint standards (Menu > Paint), by the map's marking_style: the widths of the lines and of the zebra, the dashes
	// dash[k] long and gap[k] apart below dash_kmh[k] (the last band beyond), whether every line is white, and the norm's double line
	// where a direction has more than one lane (multi_lane_double); the lines are wider from fast_kmh on.
	struct PaintStyle {
		float centre_w = 0, lane_w = 0, fast_kmh = 0, fast_centre_w = 0, fast_lane_w = 0, exclusive_w = 0, double_w = 0, double_gap = 0;
		float zebra_period = 1, zebra_bar = 0, zebra_edge = 0;
		bool multi_lane_double = false, all_white = false;
		std::vector<float> dash_kmh, dash, gap;
	};
	std::vector<PaintStyle> styles;
	float parking_line_w = 0, parking_dash = 1, parking_gap = 1, no_parking_w = 0, no_parking_min = 0; // MER and LPP

	bool load(const godot::Dictionary &t, godot::PackedStringArray &errors);
	// The carriageway's colour: asphalt keeps its class's tone, the other pavings their own.
	const godot::Color &carriage(const Profile &p) const {
		return p.surface == Surface::ASPHALT ? asphalt[int(p.road_class)] : surface[int(p.surface)];
	}
};

} // namespace city
