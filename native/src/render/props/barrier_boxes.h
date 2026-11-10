// The boxes of the barriers of a chunk, see barrier_boxes.cpp.
#pragma once

#include "render/props/street_props.h"

#include <vector>

namespace city {

// What a barrier is made of: its colour, metalness and roughness (props.gdshader PropTag::FINISHED).
struct BarrierFinish {
	godot::Color color;
	float metal = 0, rough = 1;
};

// How the barriers look (data/render/streets.json "barriers").
struct BarrierLook {
	float piece_m = 0, sink = 0, piece_min_m = 0, coping_m = 0, coping_over = 0, hedge_lap = 0, rail_m = 0, rail_thick_m = 0, fence_step_m = 0, fence_post_m = 0;
	float guard_step_m = 0, guard_post_m = 0, guard_beam_m = 0, beam_thick_m = 0, beam_foot_m = 0, lap_m = 0;
	float rail_step_m = 0, rail_post_m = 0, bar_m = 0, bar_low_m = 0, bar_thin_m = 0, bar_step_m = 0, bar_top_m = 0, bar_foot_m = 0;
	std::vector<float> rail_levels;
	BarrierFinish wall, coping, hedge, fence, beam, guard_post, railing;
	bool load(const godot::Dictionary &t, godot::PackedStringArray &errors);
};

void place_barriers(const CityView &s, const StreetLook &L, const BarrierLook &B, int64_t key, godot::PackedFloat32Array &buf,
		godot::PackedFloat32Array &beams);

} // namespace city
