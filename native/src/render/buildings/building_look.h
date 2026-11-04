// How the buildings are drawn, from data/render/buildings.json ("lod", "parts", "lot", "roof"): the distance shares of the
// detail levels, the part visibility bands, the far switch and rebuild pacing, and the sizes of the thin drawn pieces.
// The generator's own tables (styles, palettes, openings) are the simulation's (sim/buildings/b40/data.h).
#pragma once

#include "render/buildings/bay_props.h"
#include "sim/buildings/b40/paint.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <vector>

namespace city::b40 {

// One visibility band of instanced facade parts (lod.part_bands): boxes whose bar width (second largest side) is at
// most `thick_max` stay drawn until they cover `px_min` pixels on a 1080p screen.
struct PartBand {
	float thick_max = 0;
	float px_min = 0;
};

struct BuildingLook {
	float parts_share = 0, near_share = 0; // of render.building_detail_end_m
	float part_frame_bar_m = 0, part_handle_h_m = 0, part_brise_fin_m = 0; // part_models.cpp
	float guard_h_m = 0; // top of every guard above its floor (facade_parts.cpp, roof_emit.cpp)
	float fence_bar_m = 0; // lot_emit.cpp
	float air_handler_per_area = 0; // roof_plant.cpp
	std::vector<PartBand> part_bands; // near-to-far, last one unbounded (thick_max large)
	std::vector<Lin> finish_far; // each finish from afar, by layer: the far level draws no map
	float far_fov_deg = 0; // the camera's far lens (mirrors data/game/camera.json far_fov_deg)
	float far_hysteresis = 0; // superchunk far switch band around building_mid_end_m (a share of it)
	float far_rebuild_delay_s = 0, far_rebuild_interval_s = 0; // deferred far rebuilds
	BayPropLook bay; // bay_props.json

	bool load(const godot::Dictionary &t, godot::PackedStringArray &errors);
};

} // namespace city::b40
