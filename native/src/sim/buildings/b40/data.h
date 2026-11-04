// The building generator's tables, filled once from data/render/buildings.json by api/buildings_loader.cpp: styles,
// style weights per kind and era, palettes, named colours and the opening table. Plain data (no Godot types): the
// simulation reads it to lay the lots out, the render to draw them. How far each level is drawn is the render's
// (render/buildings/building_look.h).
#pragma once

#include "sim/buildings/b40/types.h"

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

struct Data {
	std::vector<StyleDef> styles;
	std::vector<std::pair<int, float>> weights[KIND_COUNT];
	float era[STYLE_COUNT][3] = {};
	std::vector<Paint> bricks, hard, roof_tiles, flat_roofs, works_roofs, lot_walls;
	std::vector<uint32_t> awnings, shop_awnings, ac_cases, tanks, warehouse_walls;
	std::unordered_map<std::string, Paint> colors;
	OpeningDef openings[BAY_COUNT];
	float ac_chance[3] = {}, dish_chance = 0, lit_share[2] = {};

	// A named colour of "colors" (magenta when missing: the loader rejects a table without the names asked for).
	const Paint &color(const char *name) const {
		static const Paint MISSING{ FIN_PLASTER, 0xff00ff };
		auto it = colors.find(name);
		return it == colors.end() ? MISSING : it->second;
	}
	bool is_clay(const Paint &p) const { return p.finish == FIN_TILE; }
};

} // namespace city::b40
