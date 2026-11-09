// The building generator's tables, filled once from data/catalog/buildings.json by api/buildings_loader.cpp: styles,
// style weights per kind and era, palettes, named colours and the opening table. Plain data (no Godot types): the
// simulation reads it to lay the lots out, the render to draw them. How far each level is drawn is the render's
// (render/buildings/building_look.h).
#pragma once

#include "sim/buildings/b40/types.h"
#include "sim/buildings/b40/body_cache.h"
#include "sim/buildings/b40/body_table.h"
#include "sim/buildings/b40/model.h"
#include "sim/buildings/b40/shapes.h"
#include "sim/buildings/interior/tables.h"

#include <atomic>
#include <cstdint>
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

// One row of the opening table: w = min(W - w_in, w_max), sill, head = min(H - head_in, head_max).
struct OpeningDef {
	bool has = false;
	float w_in = 0, w_max = 0, sill = 0, head_in = 0, head_max = 0, depth = 0;
};

// One way of closing a lot's front: a wall, fence, hedge or railing this high, standing on a base wall (fences).
struct BoundaryStyle {
	Element kind = EL_WALL;
	float h = 0, base = 0;
};

// The lot layout's rules (buildings.json "lots"; the sources are in its _doc). Metres.
struct LotRules {
	float stall_w = 0, stall_d = 0, aisle = 0, stall_line = 0; // a car bay, the lane in front of it, its painted line
	float drive_lane = 0, car_clear_h = 0; // least clear width of a car's way, least headroom over a car
	float person_gate[2] = {}, party_wall[2] = {}; // house, other lot
	float front_low = 0; // the side walls beside the front garden
	float pool_w = 0, pool_d = 0, pool_depth = 0, pool_share = 0, terrace = 0;
	float garage_back_share = 0, garage_closed_share = 0, yard_life_depth = 0;
	float lorry_w = 0, lorry_l = 0, dock_margin = 0; // the widest and the single-unit lorry; room round it in a dock
	int max_elements = 0;
	std::vector<BoundaryStyle> boundary[KIND_COUNT];
	float row_depth(int rows) const { return rows <= 0 ? 0 : float(rows) * stall_d + aisle; }
};

// A design of its own that grows on a lot of `kind` (buildings.json "grown_designs"): its body read off its lot document and brought
// to metres (gen.h scale_body), drawn `weight` times as often as a design of weight 1 among those that fit the lot.
struct GrownDesign {
	Kind kind = KIND_OFFICE;
	float weight = 1;
	std::shared_ptr<const Body> body;
};

// A number no filling of the tables had before (Data::generation).
inline uint64_t new_generation() {
	static std::atomic<uint64_t> next{ 1 };
	return next.fetch_add(1, std::memory_order_relaxed);
}

struct Data {
	// Which filling of the tables this is: a new Data (the catalogue loaded again) has another, a copy the same. What caches
	// bodies made from the tables keys them by it.
	uint64_t generation = new_generation();
	ModelTable models; // the Build > Models catalogue (data/catalog/models.json)
	std::shared_ptr<BodyTable> edited = std::make_shared<BodyTable>(); // the bodies the player made; the rest of Data is read-only
	std::shared_ptr<BodyCache> made = std::make_shared<BodyCache>(); // the records' bodies made from these tables (record.h), shared by a copy
	// By zone code, what the generator builds there and whether it is a low shop street (zones.json, set at boot: buildings_loader.h
	// set_zone_kinds).
	std::vector<Kind> zone_kind;
	std::vector<bool> zone_low_shop;
	Kind kind_of_zone(int code) const { return code >= 0 && code < int(zone_kind.size()) ? zone_kind[size_t(code)] : KIND_HOUSE; }
	bool low_shop(int code) const { return code >= 0 && code < int(zone_low_shop.size()) && zone_low_shop[size_t(code)]; }
	LotRules lots;
	Shapes shapes; // every generator file's values (buildings.json "shapes")
	InteriorTables interior;
	std::vector<StyleDef> styles;
	std::vector<std::pair<int, float>> weights[KIND_COUNT];
	float era[STYLE_COUNT][ERAS] = {};
	std::vector<Paint> bricks, hard, roof_tiles, flat_roofs, works_roofs, lot_walls;
	std::vector<uint32_t> awnings, shop_awnings, tanks, warehouse_walls;
	std::unordered_map<std::string, Paint> colors;
	OpeningDef openings[BAY_COUNT];
	float lit_share[2] = {};
	// The designs of their own a lot may grow, and the share of each kind's lots that stand one when one fits (the owner's order: a
	// downtown of many glass towers).
	std::vector<GrownDesign> grown_designs;
	float grown_share[KIND_COUNT] = {};
	float grown_least_storey_m = 0; // a design stands smaller on a smaller lot only while its storeys keep this floor-to-floor


	// A named colour of "colors" (magenta when missing: the loader rejects a table without the names asked for).
	const Paint &color(const char *name) const {
		static const Paint MISSING{ FIN_PLASTER, 0xff00ff }; // magenta: a colour the table lacks shows
		auto it = colors.find(name);
		return it == colors.end() ? MISSING : it->second;
	}
	bool is_clay(const Paint &p) const { return p.finish == FIN_TILE; }
	// The last palette a generator may give a body: a body's palette picks its lot walls (lot_boundary.cpp).
	int last_palette() const { return int(lot_walls.size()) - 1; }
};

} // namespace city::b40
