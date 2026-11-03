// What the road meshes look like, read once from project/data/render/streets.json: strip colours and heights, the
// junction corner and dead-end shapes, painted markings, gutters, dropped kerbs and the street-lamp layout the night
// light pools follow. Also the vertex conventions shared with render/shaders/road.gdshader (material id in COLOR.a).
#pragma once

#include "sim/roads/profiles.h"

#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

namespace city {

// Road shader materials (render/shaders/road.gdshader decodes COLOR.a * MAT_SCALE).
enum class RoadMat : int { ASPHALT = 0, PLATE = 1, FOOTWAY = 2, KERB = 3, GRASS = 4, CONCRETE = 5, GUTTER = 6, PAINT = 7,
	PAVERS = 8, STONE = 9, COBBLE = 10, BALLAST = 11, SKIRT = 12 };
constexpr float MAT_SCALE = 16.f;
constexpr float MAT_HALF = 0.5f;
// UV2.x of a vertex that is not lit by a lamp pool, and of a junction plate (lit evenly).
constexpr float POOL_NONE = 1000.f, POOL_PLATE = -1000.f;

inline godot::Color road_color(const godot::Color &srgb, RoadMat m) {
	return godot::Color(srgb.r, srgb.g, srgb.b, (float(int(m)) + MAT_HALF) / MAT_SCALE);
}

struct LampLook {
	float first = 0, step = 1, end_clear = 0, inset = 0, reach = 0, median_min = 0;
};

struct StreetLook {
	godot::Color surface[4], paving[4], median[4], parking, bike, shoulder, curb, barrier, gutter, plate_concrete;
	float h_curb = 0, h_curb_median = 0, h_sidewalk = 0, h_median = 0, h_barrier = 0, step = 1, skirt = 0;
	float gutter_w = 0;
	// dropped kerbs at crossings (metres from the mouth) and the ramp band of the footway
	float ramp_full = 0, ramp_flat = 0, ramp_flare = 0, ramp_low = 0, ramp_run = 0, ramp_min_walk = 0;
	float arc_step = 0, dead_end_depth = 0;
	int bevel_steps = 1, bulb_steps = 1;
	godot::Color white, yellow;
	float paint_lift = 0, line_clear = 0, dash_period = 1, dash_duty = 0, zebra_from = 0, zebra_to = 0, zebra_period = 1, zebra_bar = 0;
	float stop_from = 0, stop_to = 0, double_gap = 0;
	LampLook lamp;

	bool load(const godot::Dictionary &t, godot::PackedStringArray &errors);
};

// Where a profile's lamps stand (Roadcraft jobs/props.ts lamps, render/roads.ts pools): pole u on the right
// sidewalk (else the left), the head `reach` metres toward the carriageway; a second row on the left sidewalk when
// the profile has a median. Pools: du = |u - centre| - half_span.
struct LampRow {
	bool any = false, twin = false;
	float pole_u = 0, head_u = 0, pole_u2 = 0, head_u2 = 0;
	float centre = 0, half_span = 0;
};
LampRow lamp_row(const Profile &p, const LampLook &l);

} // namespace city
