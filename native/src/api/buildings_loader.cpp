// Parses data/render/buildings.json into the building generator's tables (sim/buildings/b40/data.h). Every name is
// checked; an unknown bay, finish or style name is an error at startup, never a silent default.
#include "api/buildings_loader.h"
#include "render/region_table.h"

#include "core/color.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city {

using namespace b40;

namespace {
constexpr uint32_t RGB_MASK = 0xffffff;
constexpr int RGBA_SHIFT = 8;
const char *const BAY_NAMES[BAY_COUNT] = { "wall", "window", "sash", "french", "bayWindow", "ribbon", "wide", "balcony", "door",
	"doubleDoor", "garageDoor", "shopfront", "loadingDoor", "pillar", "loggia", "brise", "shuttered", "gourmet" };
const char *const FINISH_NAMES[FIN_COUNT] = { "plaster", "stucco", "ceramic", "brick", "stone", "concrete", "wood", "metal",
	"glass", "tile", "slate", "panel", "roofing" };
const char *const STYLE_NAMES[STYLE_COUNT] = { "colonial", "artDeco", "modernist", "contemporary", "brick", "tropical", "glass",
	"industrial", "colonialHouse", "bungalow", "modernHouse", "sobrado" };
const char *const KIND_NAMES[KIND_COUNT] = { "house", "flats", "tower", "shop", "office", "works" };
const char *const LINE_NAMES[] = { "every", "base", "slab", "none" };
const char *const CROWN_NAMES[] = { "cornice", "deco", "slab", "attic", "none" };
// Finish names outside the cooked layers: lawn/hedge ground has no finish, plastic reads as plaster.
const char *const NO_FINISH = "grass";
const char *const PLASTIC = "plastic";
// The note a table carries for its readers (skipped where a table's keys are its rows).
const char *const DOC_KEY = "_doc";

Variant at(const Dictionary &d, const Variant &k) { return d.get(k, Variant()); }
std::string str(const Variant &v) { return std::string(String(v).utf8().get_data()); }
uint32_t hex(const Variant &v) { return (parse_hex_color(str(v)) >> RGBA_SHIFT) & RGB_MASK; }

template <size_t N>
int index_of(const char *const (&names)[N], const std::string &s, const char *what, PackedStringArray &errors) {
	for (size_t i = 0; i < N; i++)
		if (s == names[i]) return int(i);
	errors.push_back(String("buildings.json: ") + what + " desconhecido: " + s.c_str());
	return 0;
}

int8_t finish_of(const std::string &s, PackedStringArray &errors) {
	if (s == NO_FINISH) return FIN_NONE;
	if (s == PLASTIC) return FIN_PLASTER;
	return int8_t(index_of(FINISH_NAMES, s, "acabamento", errors));
}

Paint paint_of(const Variant &pair, PackedStringArray &errors) {
	Array a = pair;
	return Paint{ finish_of(str(a[0]), errors), hex(a[1]) };
}

std::vector<Paint> paints(const Array &a, PackedStringArray &errors) {
	std::vector<Paint> out;
	for (int i = 0; i < a.size(); i++) out.push_back(paint_of(a[i], errors));
	return out;
}

std::vector<uint32_t> hexes(const Array &a) {
	std::vector<uint32_t> out;
	for (int i = 0; i < a.size(); i++) out.push_back(hex(a[i]));
	return out;
}

uint8_t bay_of(const Variant &v, PackedStringArray &errors) {
	std::string s = str(v);
	return s.empty() ? uint8_t(BAY_NONE) : uint8_t(index_of(BAY_NAMES, s, "componente", errors));
}

StyleDef style_of(const Dictionary &d, PackedStringArray &errors) {
	StyleDef s;
	Array skins = at(d, "skins");
	for (int i = 0; i < skins.size(); i++) {
		Array group = skins[i];
		Array cols = group[1];
		int8_t fin = finish_of(str(group[0]), errors);
		for (int k = 0; k < cols.size(); k++) s.skins.push_back(Paint{ fin, hex(cols[k]) });
	}
	s.trims = paints(at(d, "trims"), errors);
	Array rh = at(d, "rhythms");
	for (int i = 0; i < rh.size(); i++) {
		Array r = rh[i];
		std::vector<uint8_t> row;
		for (int k = 0; k < r.size(); k++) row.push_back(bay_of(r[k], errors));
		s.rhythms.push_back(row);
	}
	s.side_fill = bay_of(at(d, "side_fill"), errors);
	s.ground_fill = bay_of(at(d, "ground_fill"), errors);
	s.top = bay_of(at(d, "top"), errors);
	s.lines = Lines(index_of(LINE_NAMES, str(at(d, "lines")), "linha", errors));
	s.crown = Crown(index_of(CROWN_NAMES, str(at(d, "crown")), "coroa", errors));
	Array o = at(d, "opening");
	s.win_w0 = float(o[0]); s.win_w1 = float(o[1]); s.win_h0 = float(o[2]); s.win_h1 = float(o[3]); s.sill = float(o[4]);
	Array p = at(d, "piers");
	if (p.size() >= 3) {
		s.piers = true;
		s.pier_w = float(p[0]); s.pier_d = float(p[1]); s.pier_every = int(p[2]);
	}
	s.stone_base = bool(at(d, "stone_base"));
	s.module = float(at(d, "module"));
	return s;
}

const char *const BOUNDARY_NAMES[] = { "wall", "fence", "railing", "hedge" };
const Element BOUNDARY_KINDS[] = { EL_WALL, EL_FENCE, EL_RAILING, EL_HEDGE };

void pair_of(const Variant &v, float (&out)[2]) {
	Array a = v;
	out[0] = float(a[0]);
	out[1] = float(a[1]);
}

bool lot_rules(const Dictionary &d, LotRules &l, PackedStringArray &errors) {
	l.stall_w = float(at(d, "stall_w")); l.stall_d = float(at(d, "stall_d")); l.aisle = float(at(d, "aisle"));
	l.stall_line = float(at(d, "stall_line")); l.drive_lane = float(at(d, "drive_lane")); l.car_clear_h = float(at(d, "car_clear_h"));
	pair_of(at(d, "person_gate"), l.person_gate);
	pair_of(at(d, "party_wall"), l.party_wall);
	l.front_low = float(at(d, "front_low"));
	Array pool = at(d, "pool");
	enum { POOL_W, POOL_D, POOL_DEPTH };
	l.pool_w = float(pool[POOL_W]); l.pool_d = float(pool[POOL_D]); l.pool_depth = float(pool[POOL_DEPTH]);
	l.pool_share = float(at(d, "pool_share")); l.terrace = float(at(d, "terrace"));
	l.garage_back_share = float(at(d, "garage_back_share")); l.garage_closed_share = float(at(d, "garage_closed_share"));
	l.yard_life_depth = float(at(d, "yard_life_depth")); l.max_elements = int(at(d, "max_elements"));
	l.lorry_w = float(at(d, "lorry_w")); l.lorry_l = float(at(d, "lorry_l")); l.dock_margin = float(at(d, "dock_margin"));
	Dictionary b = at(d, "boundary");
	for (int k = 0; k < KIND_COUNT; k++) {
		Array styles = at(b, KIND_NAMES[k]);
		for (int i = 0; i < styles.size(); i++) {
			Array row = styles[i];
			enum { KIND, HEIGHT, BASE };
			int kind = index_of(BOUNDARY_NAMES, str(row[KIND]), "divisa", errors);
			l.boundary[k].push_back(BoundaryStyle{ BOUNDARY_KINDS[kind], float(row[HEIGHT]), float(row[BASE]) });
		}
		if (l.boundary[k].empty()) errors.push_back(String("buildings.json: lots.boundary sem estilo para ") + KIND_NAMES[k]);
	}
	return errors.is_empty();
}
} // namespace

uint8_t bay_named(const std::string &s, PackedStringArray &errors) { return uint8_t(index_of(BAY_NAMES, s, "componente", errors)); }
Kind kind_named(const std::string &s, PackedStringArray &errors) { return Kind(index_of(KIND_NAMES, s, "tipo", errors)); }
Paint paint_named(const Variant &pair, PackedStringArray &errors) { return paint_of(pair, errors); }

bool load_buildings(const Dictionary &table, Data &d, PackedStringArray &errors) {
	const Dictionary t = region_table(table);
	Array st = at(t, "architectures");
	for (int i = 0; i < st.size(); i++) d.styles.push_back(style_of(st[i], errors));
	if (int(d.styles.size()) != STYLE_COUNT) errors.push_back("buildings.json: são 12 estilos");
	Dictionary kw = at(t, "kind_weights");
	for (int k = 0; k < KIND_COUNT; k++) {
		Array list = at(kw, KIND_NAMES[k]);
		for (int i = 0; i < list.size(); i++) {
			Array pair = list[i];
			d.weights[k].push_back({ index_of(STYLE_NAMES, str(pair[0]), "estilo", errors), float(pair[1]) });
		}
	}
	Array ea = at(t, "era_affinity");
	for (int s = 0; s < STYLE_COUNT && s < ea.size(); s++) {
		Array e = ea[s];
		for (int k = 0; k < 3; k++) d.era[s][k] = float(e[k]);
	}
	Dictionary pal = at(t, "palettes");
	d.bricks = paints(at(pal, "bricks"), errors); d.hard = paints(at(pal, "hard"), errors);
	d.roof_tiles = paints(at(pal, "roof_tiles"), errors); d.flat_roofs = paints(at(pal, "flat_roofs"), errors);
	d.works_roofs = paints(at(pal, "works_roofs"), errors); d.lot_walls = paints(at(pal, "lot_walls"), errors);
	d.awnings = hexes(at(pal, "awnings")); d.shop_awnings = hexes(at(pal, "shop_awnings")); d.ac_cases = hexes(at(pal, "ac_cases"));
	d.tanks = hexes(at(pal, "tanks")); d.warehouse_walls = hexes(at(pal, "warehouse_walls"));
	Dictionary cols = at(t, "colors");
	Array keys = cols.keys();
	for (int i = 0; i < keys.size(); i++) d.colors[str(keys[i])] = paint_of(at(cols, keys[i]), errors);
	Dictionary op = at(t, "openings");
	Array ok = op.keys();
	for (int i = 0; i < ok.size(); i++) {
		if (str(ok[i]) == DOC_KEY) continue;
		Array r = at(op, ok[i]);
		OpeningDef &o = d.openings[index_of(BAY_NAMES, str(ok[i]), "abertura", errors)];
		enum { W_IN, W_MAX, SILL, HEAD_IN, HEAD_MAX, DEPTH };
		o = OpeningDef{ true, float(r[W_IN]), float(r[W_MAX]), float(r[SILL]), float(r[HEAD_IN]), float(r[HEAD_MAX]), float(r[DEPTH]) };
	}
	if (!lot_rules(at(t, "lots"), d.lots, errors)) return false;
	Dictionary props = at(t, "props");
	Array ac = at(props, "ac_chance"), lit = at(props, "lit_share");
	for (int i = 0; i < 3; i++) d.ac_chance[i] = float(ac[i]);
	d.dish_chance = float(at(props, "dish_chance"));
	d.lit_share[0] = float(lit[0]); d.lit_share[1] = float(lit[1]);
	return errors.is_empty();
}

} // namespace city
