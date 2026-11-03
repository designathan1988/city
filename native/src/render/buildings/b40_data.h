// The 4.0 building data read once from data/render/buildings.json: styles, style weights per kind and era,
// palettes, named colours and the opening table. Owns the name -> code tables of bays, finishes, dress.
#pragma once

#include "render/buildings/b40_types.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace city::b40 {

struct StyleDef {
	std::vector<Paint> skins, trims;
	std::vector<std::vector<uint8_t>> rhythms;
	uint8_t side_fill = BAY_WINDOW, ground_fill = BAY_WINDOW, top = BAY_NONE;
	Lines lines = LINES_NONE;
	Crown crown = CROWN_NONE;
	float win_w0 = 0, win_w1 = 0, win_h0 = 0, win_h1 = 0, sill = 0;
	bool piers = false;
	float pier_w = 0, pier_d = 0;
	int pier_every = 1;
	bool stone_base = false;
	float module = 3;
};

// One row of the opening table (4.0 openingOf): w = min(W - w_in, w_max), sill, head = min(H - head_in, head_max).
struct OpeningDef {
	bool has = false;
	float w_in = 0, w_max = 0, sill = 0, head_in = 0, head_max = 0, depth = 0;
};

// One visibility band of instanced facade parts (data/render/buildings.json lod.part_bands): boxes whose bar width
// (second largest side) is at most `thick_max` stay drawn until they cover `px_min` pixels on a 1080p screen.
struct PartBand {
	float thick_max = 0;
	float px_min = 0;
};

struct Data {
	std::vector<StyleDef> styles;
	std::vector<std::pair<int, float>> weights[KIND_COUNT];
	float era[STYLE_COUNT][3] = {};
	std::vector<Paint> bricks, hard, roof_tiles, flat_roofs, works_roofs, lot_walls;
	std::vector<uint32_t> awnings, shop_awnings, ac_cases, tanks, warehouse_walls;
	std::unordered_map<std::string, Paint> colors;
	OpeningDef openings[BAY_COUNT];
	float ac_chance[3] = {}, dish_chance = 0, lit_share[2] = {};
	float parts_share = 0, near_share = 0; // of render.building_detail_end_m
	float part_frame_bar_m = 0, part_handle_h_m = 0, part_brise_fin_m = 0; // part_models.cpp
	float fence_bar_m = 0; // lot_emit.cpp
	float air_handler_per_area = 0; // roof_plant.cpp
	std::vector<PartBand> part_bands; // near-to-far, last one unbounded (thick_max large)
	float far_fov_deg = 0; // the camera's far lens (mirrors data/game/camera.json far_fov_deg)
	float far_hysteresis = 0; // superchunk far switch band around building_mid_end_m (Roadcraft plate rule 0.9 / 1.1)
	float far_rebuild_delay_s = 0, far_rebuild_interval_s = 0; // deferred far rebuilds (Roadcraft deferredRebuild.ts)

	bool load(const godot::Dictionary &t, godot::PackedStringArray &errors);
	// A named colour of "colors" (magenta when missing; load() reports missing names it was asked for at startup).
	const Paint &color(const char *name) const;
	bool is_clay(const Paint &p) const { return p.finish == FIN_TILE; }
};

} // namespace city::b40
