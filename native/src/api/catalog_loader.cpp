// Strict Dictionary -> Catalog conversion through the strict reader (render/data_reader.h). The tuning X-macro drives the tuning
// section, so a field added to sim/catalog/tuning_fields.h is automatically required here. The plan's doubles are read through
// float, as they always were (log/review.md §6.8: a B3 change, it moves every generated city).
#include "api/catalog_loader.h"

#include "render/data_reader.h"
#include "sim/roads/profiles.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city {

namespace {

constexpr int CIVIC_ROW = 2; // [function, weight]

float num(DataReader &r, const Dictionary &d, const char *key) { return float(r.num(d, key)); }

void load_tuning(DataReader r, const Dictionary &t, Tuning &tn) {
#define CITY_TUNING_LOAD(group, name) tn.group##_##name = float(r.in(#group).num(r.table(t, #group), #name));
	CITY_TUNING_FIELDS(CITY_TUNING_LOAD)
#undef CITY_TUNING_LOAD
}

StripKind strip_kind(const String &k, DataReader &r) {
	for (int i = 0; i < int(StripKind::COUNT); i++)
		if (k == STRIP_KIND_NAMES[i]) return StripKind(i);
	r.fail(String("unknown strip kind '") + k + String("'"));
	return StripKind::LANE;
}

RoadClass road_class(const String &k, DataReader &r) {
	static const char *NAMES[] = { "local", "urban", "avenue", "boulevard", "highway", "ramp", "rail" };
	for (int i = 0; i < int(RoadClass::COUNT); i++)
		if (k == NAMES[i]) return RoadClass(i);
	r.fail(String("unknown road class '") + k + String("'"));
	return RoadClass::LOCAL;
}

// The regional style the prices come from, until the style lives in the CityState.
const char *const STYLE = "roadcraft";

// A strip writes only the flags it has (absent: false or 0).
Strip strip_of(DataReader r, const Dictionary &sd) {
	Strip s;
	s.kind = strip_kind(r.text(sd, "kind"), r);
	s.width = num(r, sd, "width");
	s.dir = r.integer_or(sd, "dir", 0);
	s.bus = r.flag_or(sd, "bus", false);
	s.flush = r.flag_or(sd, "flush", false);
	s.rail = r.flag_or(sd, "rail", false);
	s.solid = r.flag_or(sd, "solid", false);
	s.pave = Paving(r.integer_or(sd, "pave", 0));
	s.turns = uint8_t(r.integer_or(sd, "turns", 0));
	return s;
}

void load_profiles(DataReader top, const Dictionary &t, Catalog &c) {
	DataReader rs = top.in("styles").in(STYLE);
	const Dictionary style = top.in("styles").table(top.table(t, "styles"), STYLE);
	const Dictionary pr = rs.table(style, "prices"), st = rs.table(style, "structure");
	DataReader rp = rs.in("prices"), rt = rs.in("structure");
	c.prices = { num(rp, pr, "carriageway"), num(rp, pr, "median"), num(rp, pr, "footway") };
	c.structure = { num(rt, st, "ground"), num(rt, st, "embankment"), num(rt, st, "trench"), num(rt, st, "bridge"), num(rt, st, "tunnel") };
	const Array list = top.list(t, "profiles");
	c.profiles.reserve(size_t(list.size()) + size_t(c.tune.road_max_variants));
	for (int i = 0; i < list.size(); i++) {
		DataReader r = top.in("profiles").in(i);
		const Dictionary d = top.in("profiles").table_at(list, i);
		Profile p;
		p.id = r.utf8(d, "id");
		p.name_key = r.utf8(d, "name_key");
		p.category = r.utf8(d, "category");
		p.centre = r.utf8(d, "centre");
		p.speed = num(r, d, "speed");
		p.one_way = r.flag(d, "one_way");
		p.ring = r.flag(d, "ring");
		p.priority = r.integer(d, "priority");
		p.surface = Surface(r.integer(d, "surface"));
		p.road_class = road_class(r.text(d, "road_class"), r);
		p.rank = r.integer(d, "rank");
		p.class_card = r.flag(d, "class_card");
		const Array strips = r.list(d, "strips");
		for (int k = 0; k < strips.size(); k++) p.strips.push_back(strip_of(r.in("strips").in(k), r.in("strips").table_at(strips, k)));
		if (p.strips.empty()) r.fail("a profile with no strips");
		else derive_profile(p, c.prices);
		c.add_profile(std::move(p));
	}
}

void load_zones(DataReader top, const Dictionary &t, Catalog &c) {
	const Array list = top.list(t, "zones", ZONE_CODES);
	for (int i = 0; i < list.size() && i < ZONE_CODES; i++) {
		DataReader r = top.in("zones").in(i);
		const Dictionary d = top.in("zones").table_at(list, i);
		const int code = r.integer(d, "code");
		if (code < 0 || code >= ZONE_CODES) {
			r.fail(String("code ") + String::num_int64(code) + String(" outside 0..") + String::num_int64(ZONE_CODES - 1));
			continue;
		}
		ZoneDef &z = c.zones[code];
		z.id = r.utf8(d, "id");
		z.name_key = r.utf8(d, "name_key");
		z.base = r.integer(d, "base");
		if (z.base < 0 || z.base >= BASE_COUNT) r.fail("a base past the base zones (BaseZone)");
		z.density = r.integer(d, "density");
		z.color = r.colour(d, "color");
	}
	const Dictionary o = top.table(t, "residents");
	DataReader ro = top.in("residents");
	c.occupancy = { num(ro, o, "floor_share"), num(ro, o, "m2_per_resident"), num(ro, o, "m2_per_job_commercial"), num(ro, o, "m2_per_job_industrial"),
		num(ro, o, "m2_per_person_mixed") };
}

void load_plan(DataReader r, const Dictionary &p, CityPlanDef &d) {
	d.centres = r.integer(p, "centres");
	d.main_offset = num(r, p, "main_offset");
	d.centre_turn = num(r, p, "centre_turn");
	d.second_from = num(r, p, "second_from");
	d.second_to = num(r, p, "second_to");
	d.second_pull = num(r, p, "second_pull");
	d.density = num(r, p, "density");
	d.density_pull = num(r, p, "density_pull");
	d.noise_cell_m = num(r, p, "noise_cell_m");
	d.noise_heat = num(r, p, "noise_heat");
	d.high_heat = num(r, p, "high_heat");
	d.medium_heat = num(r, p, "medium_heat");
	d.commerce_heat = num(r, p, "commerce_heat");
	d.main_commerce_heat = num(r, p, "main_commerce_heat");
	d.industry_from = num(r, p, "industry_from");
	d.industry_half_angle = num(r, p, "industry_half_angle");
	const Dictionary sq = r.table(p, "squares");
	DataReader rq = r.in("squares");
	d.square_cell_m = num(rq, sq, "green_cell_m");
	d.square_touch_m = num(rq, sq, "green_touch_m");
	d.square_front_m = num(rq, sq, "green_front_m");
	d.square_area_low = num(rq, sq, "green_area_low_m2");
	d.square_area_high = num(rq, sq, "green_area_high_m2");
	d.square_walk_half_m = num(rq, sq, "green_walk_half_m");
	d.square_plaza_m = num(rq, sq, "green_plaza_m");
	d.square_tree_grid_m = num(rq, sq, "green_tree_grid_m");
	d.square_tree_edge_m = num(rq, sq, "green_tree_edge_m");
	d.square_tree_low_m = num(rq, sq, "green_tree_low_m");
	d.square_tree_high_m = num(rq, sq, "green_tree_high_m");
	const Array roads = rq.list(sq, "green_main_roads");
	for (int i = 0; i < roads.size(); i++) d.square_main_roads.push_back(rq.in("green_main_roads").utf8_at(roads, i));
}

void load_growth(DataReader r, const Dictionary &t, GrowthDef &g) {
	load_plan(r.in("plan"), r.table(t, "plan"), g.plan);
	const Dictionary cv = r.table(t, "civic");
	DataReader rc = r.in("civic");
	g.civic_min_area = num(rc, cv, "min_area");
	g.civic_chance = num(rc, cv, "chance");
	constexpr int zones = BASE_ZONE_SLOTS; // civic_by_zone's rows
	const Array by_zone = rc.list(cv, "by_zone", zones);
	for (int z = 0; z < zones && z < by_zone.size(); z++) {
		DataReader rz = rc.in("by_zone").in(z);
		const Array rows = rc.in("by_zone").list_at(by_zone, z);
		for (int i = 0; i < rows.size(); i++) {
			const Array row = rz.list_at(rows, i, CIVIC_ROW);
			g.civic_by_zone[z].push_back({ rz.in(i).integer_at(row, 0), rz.in(i).integer_at(row, 1) });
		}
	}
	const Dictionary fit = r.table(t, "fit");
	DataReader rf = r.in("fit");
	g.fit_inset_m = num(rf, fit, "inset_m");
	const Array tries = rf.list(fit, "tries");
	g.fit_tries.clear();
	for (int i = 0; i < tries.size(); i++) {
		DataReader rt = rf.in("tries").in(i);
		const Dictionary row = rf.in("tries").table_at(tries, i);
		g.fit_tries.push_back({ num(rt, row, "setback_share"), num(rt, row, "width_share"), rt.integer(row, "density_drop") });
	}
	if (g.fit_tries.empty()) rf.fail("tries: at least one");
	g.pick_reach_m = num(r, t, "pick_reach_m");
	g.organic_reach_m = num(r, t, "organic_reach_m");
	g.organic_extra_floors = r.integer(t, "organic_extra_floors");
	g.decay_span_min = num(r, t, "decay_span_min");
}

void load_maps(DataReader top, const Dictionary &t, Catalog &c, std::vector<std::string> &scenic) {
	const Array list = top.list(t, "maps");
	for (int i = 0; i < list.size(); i++) {
		DataReader r = top.in("maps").in(i);
		const Dictionary d = top.in("maps").table_at(list, i);
		MapDef m;
		m.id = r.utf8(d, "id");
		m.name_key = r.utf8(d, "name_key");
		m.tiles = r.integer(d, "tiles");
		m.medium = r.flag(d, "medium");
		m.city_radius_m = r.num_or(d, "city_radius_m", 0.0);
		scenic.push_back(d.has("scenic") ? r.utf8(d, "scenic") : std::string()); // linked once the scenic cities are read
		c.maps.push_back(m);
	}
	if (c.maps.empty()) top.fail("no map");
}

} // namespace

bool CatalogLoader::load(const Dictionary &tables, Catalog &out, PackedStringArray &errors) {
	const int64_t before = errors.size();
	Catalog c;
	DataReader r{ errors, "catalog" };
	load_tuning(DataReader{ errors, "tuning.json" }, r.table(tables, "tuning"), c.tune);
	std::vector<std::string> scenic_names;
	load_maps(DataReader{ errors, "maps.json" }, r.table(tables, "maps"), c, scenic_names);
	load_profiles(DataReader{ errors, "road_profiles.json" }, r.table(tables, "road_profiles"), c);
	load_zones(DataReader{ errors, "zones.json" }, r.table(tables, "zones"), c);
	load_growth(DataReader{ errors, "growth.json" }, r.table(tables, "growth"), c.growth);
	load_traffic_params(r.table(tables, "traffic"), c.traffic, errors);
	load_street_furniture(r.table(tables, "street_furniture"), c.furniture, errors);
	load_blast_params(r.table(tables, "blast"), c.blast, errors);
	load_nature(r.table(tables, "nature"), c.nature, errors);
	if (load_scenic(r.table(tables, "scenic"), c, errors)) link_scenic_maps(scenic_names, c, errors);
	if (errors.size() != before) return false;
	out = std::move(c);
	return true;
}

} // namespace city
