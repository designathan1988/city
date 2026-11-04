// What the road meshes look like, read once from project/data/render/streets.json: strip colours and heights, the
// junction corner and dead-end shapes, painted markings, gutters and dropped kerbs. Where the lamps the night light
// pools follow stand and where the crosswalks lie is the simulation's (sim/roads/street_furniture.h). Also the vertex
// conventions shared with render/shaders/road.gdshader (material id in COLOR.a).
#pragma once

#include "sim/roads/profiles.h"

#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

namespace city {

// Road shader materials (render/shaders/road.gdshader decodes COLOR.a * MAT_SCALE).
enum class RoadMat : int { ASPHALT = 0, PLATE, FOOTWAY, KERB, GRASS, CONCRETE, GUTTER, PAINT, PAVERS, STONE, COBBLE,
	BALLAST, SKIRT };
constexpr float MAT_SCALE = 16.f;
constexpr float MAT_HALF = 0.5f;
// UV2.x of a vertex that is not lit by a lamp pool, and of a junction plate (lit evenly).
constexpr float POOL_NONE = 1000.f, POOL_PLATE = -1000.f;

inline godot::Color road_color(const godot::Color &srgb, RoadMat m) {
	return godot::Color(srgb.r, srgb.g, srgb.b, (float(int(m)) + MAT_HALF) / MAT_SCALE);
}

struct StreetLook {
	godot::Color surface[4], paving[4], median[4], parking, bike, shoulder, curb, barrier, gutter, plate_concrete;
	float h_curb = 0, h_curb_median = 0, h_sidewalk = 0, h_median = 0, h_barrier = 0, step = 1, skirt = 0;
	float gutter_w = 0, face_eps = 0; // coplanar faces closer than this are one
	// dropped kerbs at crossings (metres from the mouth) and the ramp band of the footway
	float ramp_full = 0, ramp_flat = 0, ramp_flare = 0, ramp_low = 0, ramp_run = 0, ramp_min_walk = 0;
	float arc_step = 0, dead_end_depth = 0;
	int bevel_steps = 1, bulb_steps = 1;
	godot::Color white, yellow;
	float paint_lift = 0, line_clear = 0, dash_period = 1, dash_duty = 0, zebra_period = 1, zebra_bar = 0, double_gap = 0;

	bool load(const godot::Dictionary &t, godot::PackedStringArray &errors);
};

} // namespace city
