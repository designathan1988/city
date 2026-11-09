// Parses data/catalog/buildings.json into the building generator's tables (sim/buildings/b40/data.h) through the strict reader
// (api/data_reader.h): every key, type, list length and name is checked, and a problem is an error at startup, never a default.
#include "api/buildings_loader.h"

#include "api/body_doc.h"
#include "render/region_table.h"
#include "render/shape_reader.h"
#include "sim/buildings/b40/gen.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city {

using namespace b40;

namespace {
// A table's colour is 0xRRGGBBAA (core/color.h): the body keeps 0xRRGGBB, the alpha byte shifted off.
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
const char *const BOUNDARY_NAMES[] = { "wall", "fence", "railing", "hedge" };
const Element BOUNDARY_KINDS[] = { EL_WALL, EL_FENCE, EL_RAILING, EL_HEDGE };
// Finish names outside the cooked layers: lawn/hedge ground has no finish, plastic reads as plaster.
const char *const NO_FINISH = "grass";
const char *const PLASTIC = "plastic";
// The note a table carries for its readers (skipped where a table's keys are its rows).
const char *const DOC_KEY = "_doc";
constexpr int PAIR = 2; // [finish, colour], [style, weight], [min, max]

int8_t finish_of(const std::string &s, DataReader &r) {
	if (s == NO_FINISH) return FIN_NONE;
	if (s == PLASTIC) return FIN_PLASTER;
	return int8_t(r.index_of(FINISH_NAMES, s, "finish"));
}

uint32_t rgb(uint32_t rgba) { return (rgba >> RGBA_SHIFT) & RGB_MASK; }

// [finish, '#rrggbb'], its length checked by whoever read it.
Paint paint_of(DataReader r, const Array &pair) { return Paint{ finish_of(r.utf8_at(pair, 0), r), rgb(r.colour_at(pair, 1)) }; }

std::vector<Paint> paints(DataReader r, const Array &a) {
	std::vector<Paint> out;
	for (int i = 0; i < a.size(); i++) out.push_back(paint_of(r.in(i), r.list_at(a, i, PAIR)));
	return out;
}

std::vector<uint32_t> hexes(DataReader r, const Array &a) {
	std::vector<uint32_t> out;
	for (int i = 0; i < a.size(); i++) out.push_back(rgb(r.colour_at(a, i)));
	return out;
}

// An empty name is no bay (a rhythm's gap, a style without a ground fill).
uint8_t bay_of(const std::string &s, DataReader &r) { return s.empty() ? uint8_t(BAY_NONE) : uint8_t(r.index_of(BAY_NAMES, s, "bay")); }

void skins_of(DataReader r, const Array &skins, StyleDef &s) { // [finish, [colour...]] groups
	for (int i = 0; i < skins.size(); i++) {
		const Array group = r.list_at(skins, i, PAIR);
		DataReader g = r.in(i);
		const int8_t fin = finish_of(g.utf8_at(group, 0), g);
		const Array cols = g.list_at(group, 1);
		for (int k = 0; k < cols.size(); k++) s.skins.push_back(Paint{ fin, rgb(g.colour_at(cols, k)) });
	}
}

StyleDef style_of(DataReader r, const Dictionary &d) {
	StyleDef s;
	skins_of(r.in("skins"), r.list(d, "skins"), s);
	s.trims = paints(r.in("trims"), r.list(d, "trims"));
	const Array rh = r.list(d, "rhythms");
	for (int i = 0; i < rh.size(); i++) {
		const Array row = r.in("rhythms").list_at(rh, i);
		DataReader rr = r.in("rhythms").in(i);
		std::vector<uint8_t> bays;
		for (int k = 0; k < row.size(); k++) bays.push_back(bay_of(rr.utf8_at(row, k), rr));
		s.rhythms.push_back(bays);
	}
	s.side_fill = bay_of(r.utf8(d, "side_fill"), r);
	s.ground_fill = bay_of(r.utf8(d, "ground_fill"), r);
	s.top = bay_of(r.utf8(d, "top"), r);
	s.lines = Lines(r.index_of(LINE_NAMES, r.utf8(d, "lines"), "line"));
	s.crown = Crown(r.index_of(CROWN_NAMES, r.utf8(d, "crown"), "crown"));
	enum { WIN_W0, WIN_W1, WIN_H0, WIN_H1, SILL, OPENING_SIZE };
	const Array o = r.list(d, "opening", OPENING_SIZE);
	DataReader ro = r.in("opening");
	s.win_w0 = float(ro.num_at(o, WIN_W0));
	s.win_w1 = float(ro.num_at(o, WIN_W1));
	s.win_h0 = float(ro.num_at(o, WIN_H0));
	s.win_h1 = float(ro.num_at(o, WIN_H1));
	s.sill = float(ro.num_at(o, SILL));
	enum { PIER_W, PIER_D, PIER_EVERY, PIERS_SIZE };
	const Array p = r.list(d, "piers"); // empty: a style without piers
	if (!p.is_empty()) {
		DataReader rp = r.in("piers");
		rp.expect(p, PIERS_SIZE);
		s.piers = true;
		s.pier_w = float(rp.num_at(p, PIER_W));
		s.pier_d = float(rp.num_at(p, PIER_D));
		s.pier_every = rp.integer_at(p, PIER_EVERY);
	}
	s.stone_base = r.flag(d, "stone_base");
	s.module = float(r.num(d, "module"));
	return s;
}

void pair_of(DataReader &r, const Dictionary &d, const char *key, float (&out)[PAIR]) {
	const Array a = r.list(d, key, PAIR);
	DataReader rk = r.in(key);
	out[0] = float(rk.num_at(a, 0));
	out[1] = float(rk.num_at(a, 1));
}

void boundaries(DataReader r, const Dictionary &b, LotRules &l) { // per kind, rows of [boundary, height, base]
	for (int k = 0; k < KIND_COUNT; k++) {
		const Array styles = r.list(b, KIND_NAMES[k]);
		DataReader rk = r.in(KIND_NAMES[k]);
		for (int i = 0; i < styles.size(); i++) {
			enum { KIND, HEIGHT, BASE, ROW_SIZE };
			const Array row = rk.list_at(styles, i, ROW_SIZE);
			DataReader ri = rk.in(i);
			const int kind = ri.index_of(BOUNDARY_NAMES, ri.utf8_at(row, KIND), "boundary");
			l.boundary[k].push_back(BoundaryStyle{ BOUNDARY_KINDS[kind], float(ri.num_at(row, HEIGHT)), float(ri.num_at(row, BASE)) });
		}
		if (l.boundary[k].empty()) rk.fail("no style");
	}
}

void lot_rules(DataReader r, const Dictionary &d, LotRules &l) {
	l.stall_w = float(r.num(d, "stall_w"));
	l.stall_d = float(r.num(d, "stall_d"));
	l.aisle = float(r.num(d, "aisle"));
	l.stall_line = float(r.num(d, "stall_line"));
	l.drive_lane = float(r.num(d, "drive_lane"));
	l.car_clear_h = float(r.num(d, "car_clear_h"));
	pair_of(r, d, "person_gate", l.person_gate);
	pair_of(r, d, "party_wall", l.party_wall);
	l.front_low = float(r.num(d, "front_low"));
	enum { POOL_W, POOL_D, POOL_DEPTH, POOL_SIZE };
	const Array pool = r.list(d, "pool", POOL_SIZE);
	DataReader rp = r.in("pool");
	l.pool_w = float(rp.num_at(pool, POOL_W));
	l.pool_d = float(rp.num_at(pool, POOL_D));
	l.pool_depth = float(rp.num_at(pool, POOL_DEPTH));
	l.pool_share = float(r.num(d, "pool_share"));
	l.terrace = float(r.num(d, "terrace"));
	l.garage_back_share = float(r.num(d, "garage_back_share"));
	l.garage_closed_share = float(r.num(d, "garage_closed_share"));
	l.yard_life_depth = float(r.num(d, "yard_life_depth"));
	l.max_elements = r.integer(d, "max_elements");
	l.lorry_w = float(r.num(d, "lorry_w"));
	l.lorry_l = float(r.num(d, "lorry_l"));
	l.dock_margin = float(r.num(d, "dock_margin"));
	boundaries(r.in("boundary"), r.table(d, "boundary"), l);
}

// Every block of "shapes": each field of its list read under its own name, of its own type.
void shapes(DataReader r, const Dictionary &t, Shapes &out) {
	B40_SHAPES(SHAPE_READ_BLOCK)
}

void kind_weights(DataReader r, const Dictionary &kw, Data &d) { // per kind, [style, weight] pairs
	for (int k = 0; k < KIND_COUNT; k++) {
		const Array list = r.list(kw, KIND_NAMES[k]);
		DataReader rk = r.in(KIND_NAMES[k]);
		for (int i = 0; i < list.size(); i++) {
			const Array pair = rk.list_at(list, i, PAIR);
			DataReader ri = rk.in(i);
			d.weights[k].push_back({ ri.index_of(STYLE_NAMES, ri.utf8_at(pair, 0), "style"), float(ri.num_at(pair, 1)) });
		}
	}
}

void palettes(DataReader r, const Dictionary &pal, Data &d) {
	const auto paint_list = [&](const char *key) { return paints(r.in(key), r.list(pal, key)); };
	const auto hex_list = [&](const char *key) { return hexes(r.in(key), r.list(pal, key)); };
	d.bricks = paint_list("bricks");
	d.hard = paint_list("hard");
	d.roof_tiles = paint_list("roof_tiles");
	d.flat_roofs = paint_list("flat_roofs");
	d.works_roofs = paint_list("works_roofs");
	d.lot_walls = paint_list("lot_walls");
	d.awnings = hex_list("awnings");
	d.shop_awnings = hex_list("shop_awnings");
	d.tanks = hex_list("tanks");
	d.warehouse_walls = hex_list("warehouse_walls");
}

void colors(DataReader r, const Dictionary &cols, Data &d) { // name -> [finish, colour]
	const Array keys = cols.keys();
	for (int i = 0; i < keys.size(); i++) {
		const String key = keys[i];
		const std::string name(key.utf8().get_data());
		if (name == DOC_KEY) continue;
		d.colors[name] = paint_of(r.in(key), r.list(cols, name.c_str(), PAIR));
	}
}

void openings(DataReader r, const Dictionary &op, Data &d) { // bay -> [w_in, w_max, sill, head_in, head_max, depth]
	const Array keys = op.keys();
	for (int i = 0; i < keys.size(); i++) {
		const String key = keys[i];
		const std::string name(key.utf8().get_data());
		if (name == DOC_KEY) continue;
		enum { W_IN, W_MAX, SILL, HEAD_IN, HEAD_MAX, DEPTH, ROW_SIZE };
		const Array row = r.list(op, name.c_str(), ROW_SIZE);
		DataReader ro = r.in(key);
		OpeningDef &o = d.openings[ro.index_of(BAY_NAMES, name, "bay")];
		o = OpeningDef{ true, float(ro.num_at(row, W_IN)), float(ro.num_at(row, W_MAX)), float(ro.num_at(row, SILL)),
			float(ro.num_at(row, HEAD_IN)), float(ro.num_at(row, HEAD_MAX)), float(ro.num_at(row, DEPTH)) };
	}
}
// The designs of their own a lot grows: each [design, kind, weight], its lot document (folder/<design>.json, the shelf's) read into a
// body and brought from the documents' unit to metres; the share of each kind's lots that stand one, [kind, share]. A design that
// cannot be read is an error at startup.
void grown_designs(DataReader r, const Dictionary &g, Data &d) {
	enum { NAME, KIND, WEIGHT, ROW_SIZE };
	const String folder = r.text(g, "folder");
	const float unit = float(r.num(g, "unit"));
	d.grown_least_storey_m = float(r.num(g, "least_storey_m"));
	const Array shares = r.list(g, "shares");
	for (int i = 0; i < shares.size(); i++) {
		const Array pair = r.in("shares").list_at(shares, i, PAIR);
		DataReader rs = r.in("shares").in(i);
		d.grown_share[rs.index_of(KIND_NAMES, rs.utf8_at(pair, 0), "kind")] = float(rs.num_at(pair, 1));
	}
	const Array rows = r.list(g, "designs");
	for (int i = 0; i < rows.size(); i++) {
		const Array row = r.in("designs").list_at(rows, i, ROW_SIZE);
		DataReader ri = r.in("designs").in(i);
		const String path = folder + ri.text_at(row, NAME) + ".json";
		const Variant doc = JSON::parse_string(FileAccess::get_file_as_string(path));
		if (doc.get_type() != Variant::DICTIONARY || !Dictionary(doc).has("body")) {
			ri.fail(String("design unreadable: ") + path);
			continue;
		}
		auto body = std::make_shared<Body>();
		float front_x = 0, front_y = 0;
		int function = 0;
		if (!body_from_doc(Dictionary(doc)["body"], d, *body, front_x, front_y, function, r.errors)) {
			ri.fail(String("design refused: ") + path);
			continue;
		}
		scale_body(*body, unit);
		d.grown_designs.push_back({ Kind(ri.index_of(KIND_NAMES, ri.utf8_at(row, KIND), "kind")), float(ri.num_at(row, WEIGHT)), std::move(body) });
	}
}
} // namespace

uint8_t bay_named(const std::string &s, DataReader &r) { return uint8_t(r.index_of(BAY_NAMES, s, "bay")); }
Kind kind_named(const std::string &s, DataReader &r) { return Kind(r.index_of(KIND_NAMES, s, "kind")); }
Paint paint_named(const Variant &pair, DataReader r) {
	if (pair.get_type() != Variant::ARRAY) {
		r.fail(String("expected [finish, '#rrggbb'], found ") + Variant::get_type_name(pair.get_type()));
		return Paint{};
	}
	const Array a = pair;
	r.expect(a, PAIR);
	return paint_of(r, a);
}

bool load_buildings(const Dictionary &table, Data &d, PackedStringArray &errors) {
	const Dictionary t = region_table(table);
	const int64_t before = errors.size();
	DataReader r{ errors, "buildings.json" };
	const Array st = r.list(t, "architectures", STYLE_COUNT);
	for (int i = 0; i < st.size(); i++) d.styles.push_back(style_of(r.in("architectures").in(i), r.in("architectures").table_at(st, i)));
	kind_weights(r.in("kind_weights"), r.table(t, "kind_weights"), d);
	const Array ea = r.list(t, "era_affinity", STYLE_COUNT);
	for (int s = 0; s < STYLE_COUNT && s < ea.size(); s++) {
		const Array e = r.in("era_affinity").list_at(ea, s, ERAS);
		DataReader re = r.in("era_affinity").in(s);
		for (int k = 0; k < ERAS; k++) d.era[s][k] = float(re.num_at(e, k));
	}
	palettes(r.in("palettes"), r.table(t, "palettes"), d);
	colors(r.in("colors"), r.table(t, "colors"), d);
	openings(r.in("openings"), r.table(t, "openings"), d);
	lot_rules(r.in("lots"), r.table(t, "lots"), d.lots);
	shapes(r.in("shapes"), r.table(t, "shapes"), d.shapes);
	if (errors.size() == before) grown_designs(r.in("grown_designs"), r.table(t, "grown_designs"), d); // the documents read with the colours
	return errors.size() == before;
}

bool set_zone_kinds(const Catalog &catalog, Data &d, PackedStringArray &errors) {
	const int64_t before = errors.size();
	d.zone_kind.assign(ZONE_CODES, KIND_HOUSE);
	d.zone_low_shop.assign(ZONE_CODES, false);
	for (int code = 0; code < ZONE_CODES; code++) {
		const ZoneDef &z = catalog.zones[code];
		int kind = 0;
		while (kind < KIND_COUNT && z.building != KIND_NAMES[kind]) kind++;
		if (kind == KIND_COUNT) errors.push_back(String("zones.json: zone '") + String(z.id.c_str()) + String("' builds '") + String(z.building.c_str()) + String("', no building kind"));
		else d.zone_kind[size_t(code)] = Kind(kind);
		d.zone_low_shop[size_t(code)] = z.base == BASE_COMMERCIAL && z.density == 0;
	}
	return errors.size() == before;
}

} // namespace city
