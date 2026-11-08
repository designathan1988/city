// How the buildings are drawn, from data/render/building_look.json ("lod", "parts", "lot", "roof"): the distance shares of the
// detail levels, the part visibility bands, the far switch and rebuild pacing, and the sizes of the thin drawn pieces.
// The generator's own tables (styles, palettes, openings) are the simulation's (sim/buildings/b40/data.h).
#pragma once

#include "render/buildings/bay_props.h"
#include "render/buildings/furniture_look.h"
#include "render/buildings/look_shapes.h"
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

// An office tower's roof landing deck (buildings.json roof.helipad; roof_plant.cpp): where it stands, its size and its markings.
struct HelipadLook {
	int storeys_least = 0;
	float share = 0, side_m = 0, deck_up_m = 0, h_tall_m = 0, h_wide_m = 0, h_stroke_m = 0, circle_line_m = 0, edge_line_m = 0, net_m = 0;
};

struct BuildingLook {
	float parts_share = 0, near_share = 0; // of render.building_detail_end_m
	float part_frame_bar_m = 0, part_handle_h_m = 0, part_brise_fin_m = 0; // part_models.cpp
	float guard_h_m = 0; // top of every guard above its floor (facade_parts.cpp, roof_emit.cpp)
	float fence_bar_m = 0; // lot_emit.cpp
	float air_handler_per_area = 0; // roof_plant.cpp
	HelipadLook helipad; // roof_plant.cpp
	std::vector<PartBand> part_bands; // near-to-far, last one unbounded (thick_max large)
	InteriorLook interior; // the furniture models and the paints of a building cut open (furniture.json)
	std::vector<Lin> finish_far; // each finish from afar, by layer: the far level draws no map
	float far_fov_deg = 0; // the camera's far lens (data/game/camera.json far_fov_deg): the levels' bands are measured for it
	float cast_props_m = 0; // how far the balcony pieces throw a shadow
	float hull_error_m = 0; // how far the mid level's shadow hull may stray from its walls
	float level_fade_m = 0, level_back_m = 0; // the near level's hand-over (building_level.gdshaderinc)
	float far_per_eye = 0; // metres of far reach each metre of eye height adds: the view from up there runs that much further
	float far_hysteresis = 0; // superchunk far switch band around building_mid_end_m (a share of it)
	float far_rebuild_delay_s = 0, far_rebuild_interval_s = 0; // deferred far rebuilds
	BayPropLook bay; // bay_props.json
	LookShapes shapes; // every render file's sizes, shares and shades (building_look.json "shapes")

	// buildings.json, bay_props.json and furniture.json (the look's own numbers, its bay props, the interior's furniture).
	bool load(const godot::Dictionary &t, const godot::Dictionary &bay_props, const godot::Dictionary &furniture, const godot::Dictionary &camera,
			godot::PackedStringArray &errors);
};

} // namespace city::b40
