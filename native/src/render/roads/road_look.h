// What the road meshes look like, read once from project/data/render/streets.json: strip colours, painted markings, gutters and
// dropped kerbs. The raised strips' heights are the catalogue's (tuning.json street, which the simulation reads too: a lot meets the
// footway's top), and so are a junction corner's and a turning bulb's outlines (sim/roads/node_outline.h: the lots are cut by them);
// where the crosswalks lie is the simulation's (sim/roads/street_furniture.h). The light the lamps lay at night is no part of the road:
// each lamp's own pool (render/props/lamp_pools.h). Also the vertex conventions shared with render/shaders/road.gdshader (material id
// in COLOR.a; UV2 the asphalt's wear in metres: x how far from the carriageway's edge on a node's paving, a bay, a bike strip or a
// shoulder, and on a lane how far across it from its own edge; y, on a lane, how far from the nearest mouth on a node's paving).
#pragma once

#include "sim/catalog/catalog.h"
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
	float far_step = 0, far_pull = 0; // the far level: its stretches on a curve; how far towards the eye it is drawn, over the coarser ground out there
	float h_curb = 0, h_curb_median = 0, h_sidewalk = 0, h_median = 0, h_barrier = 0, step = 1, skirt = 0, median_nose = 0;
	float face_eps = 0; // coplanar faces closer than this are one
	float plate_tone_power = 0; // how fast an arm's asphalt gives way on a node's paving, off its mouth (road_plate.cpp)
	float verge_reach = 0, verge_under = 0;
	int verge_strips = 1;
	// dropped kerbs at crossings (metres from the mouth) and the ramp band of the footway
	float ramp_full = 0, ramp_flat = 0, ramp_flare = 0, ramp_low = 0, ramp_run = 0, ramp_min_walk = 0;
	// a lot's car gate (a vehicle access): the kerb's lip there, the band beside the kerb its ramp takes, the flare along the kerb
	float drive_low = 0, drive_band = 0, drive_flare = 0;
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

	// `t` streets.json; `tn` the catalogue's tuning, whose street group gives the raised strips' heights.
	bool load(const godot::Dictionary &t, const Tuning &tn, godot::PackedStringArray &errors);
	// The carriageway's colour: asphalt keeps its class's tone, the other pavings their own.
	const godot::Color &carriage(const Profile &p) const {
		return p.surface == Surface::ASPHALT ? asphalt[int(p.road_class)] : surface[int(p.surface)];
	}
};

} // namespace city
