// data/catalog/scenic.json -> Catalog::scenic (sim/catalog/scenic.h) through the strict reader (render/data_reader.h): a
// missing or mistyped key, a list too short, an unknown profile or biome, a road on a river the city does not have, a road's
// name key with no text, fails the load. Read after the profiles and the nature tables, and after the translations (a road's
// name is the player's text of its key).
#include "api/catalog_loader.h"
#include "render/data_reader.h"

#include <godot_cpp/classes/translation_server.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city {

namespace {
constexpr int POINT_PARTS = 2, LINE_LEAST = 2, RING_LEAST = 3;

V2 point(DataReader r, const Array &a) {
	r.expect(a, POINT_PARTS);
	return V2(float(r.num_at(a, 0)), float(r.num_at(a, 1)));
}
V2 point(DataReader &r, const Dictionary &d, const char *key) { return point(r.in(key), r.list(d, key)); }

Polyline line(DataReader &r, const Dictionary &d, const char *key, int least) {
	Polyline out;
	const Array points = r.list(d, key);
	for (int i = 0; i < points.size(); i++) out.push_back(point(r.in(key).in(i), r.in(key).list_at(points, i)));
	if (int(out.size()) < least) r.in(key).fail("too few points");
	return out;
}

// The player's text of an i18n key (data/i18n, loaded before the catalogue): a key with no text fails.
std::string translated(DataReader &r, const std::string &key) {
	const String text = TranslationServer::get_singleton()->translate(StringName(key.c_str()));
	if (text == String(key.c_str())) r.fail(String("no text for '") + text + String("'"));
	return std::string(text.utf8().get_data());
}

int profile(DataReader &r, const Catalog &c, const Dictionary &d, const char *key) {
	const std::string id = r.utf8(d, key);
	const int p = c.profile_id(id);
	if (p < 0) r.in(key).fail(String("unknown profile '") + id.c_str() + String("'"));
	return p;
}

void load_land(DataReader r, const Dictionary &t, ScenicLand &land) {
	land.base_m = float(r.num(t, "base_m"));
	land.relief_m = float(r.num(t, "relief_amp_m"));
	land.relief_cell_m = float(r.num(t, "relief_cell_m"));
	const Array ridges = r.list(t, "ridges"), rivers = r.list(t, "rivers"), lakes = r.list(t, "lakes");
	for (int i = 0; i < ridges.size(); i++) {
		DataReader rr = r.in("ridges").in(i);
		const Dictionary d = r.in("ridges").table_at(ridges, i);
		land.ridges.push_back({ line(rr, d, "polyline", LINE_LEAST), float(rr.num(d, "crest_m")), float(rr.num(d, "half_width_m")) });
	}
	for (int i = 0; i < rivers.size(); i++) {
		DataReader rw = r.in("rivers").in(i);
		const Dictionary d = r.in("rivers").table_at(rivers, i);
		ScenicRiver w;
		w.id = rw.utf8(d, "id");
		w.line = line(rw, d, "polyline", LINE_LEAST);
		w.half_width_m = float(rw.num(d, "half_width_m"));
		w.depth_m = float(rw.num(d, "deep_m"));
		w.wall_m = float(rw.num(d, "channel_wall_m"));
		w.freeboard_m = float(rw.num(d, "freeboard_m"));
		w.valley_m = float(rw.num(d, "valley_m"));
		w.level_m = float(rw.num(d, "level_start_m"));
		w.level_end_m = float(rw.num(d, "level_end_m"));
		land.rivers.push_back(std::move(w));
	}
	for (int i = 0; i < lakes.size(); i++) {
		DataReader rl = r.in("lakes").in(i);
		const Dictionary d = r.in("lakes").table_at(lakes, i);
		land.lakes.push_back({ line(rl, d, "shore", RING_LEAST), float(rl.num(d, "below_shore_m")), float(rl.num(d, "deep_m")), float(rl.num(d, "shore_m")) });
	}
}

void load_districts(DataReader r, const Catalog &c, const Array &list, std::vector<ScenicDistrict> &out) {
	for (int i = 0; i < list.size(); i++) {
		DataReader rd = r.in(i);
		const Dictionary d = r.table_at(list, i);
		ScenicDistrict k;
		k.id = rd.utf8(d, "id");
		k.seed = point(rd, d, "seed_point");
		const V2 block = point(rd, d, "blocks_m");
		k.block_along_m = block.x;
		k.block_across_m = block.y;
		k.turn_deg = float(rd.num(d, "bearing_deg"));
		k.heat = float(rd.num(d, "heat"));
		k.trade = float(rd.num(d, "trade"));
		k.local = profile(rd, c, d, "local");
		k.collector = profile(rd, c, d, "collector");
		k.collector_every = rd.integer(d, "collector_every");
		if (k.block_along_m <= 0 || k.block_across_m <= 0) rd.fail("blocks must be wider than 0");
		out.push_back(std::move(k));
	}
}

void load_roads(DataReader r, const Catalog &c, const Array &list, const ScenicLand &land, std::vector<ScenicRoad> &out) {
	for (int i = 0; i < list.size(); i++) {
		DataReader rr = r.in(i);
		const Dictionary d = r.table_at(list, i);
		ScenicRoad road;
		if (d.has("name_key")) road.name = translated(rr, rr.utf8(d, "name_key"));
		road.profile = profile(rr, c, d, "profile");
		if (d.has("river")) {
			const std::string id = rr.utf8(d, "river");
			for (size_t k = 0; k < land.rivers.size(); k++)
				if (land.rivers[k].id == id) road.river = int(k);
			if (road.river < 0) rr.fail(String("a road on no river '") + id.c_str() + String("'"));
			road.offset_m = float(rr.num(d, "offset_m"));
		} else road.line = line(rr, d, "polyline", LINE_LEAST);
		out.push_back(std::move(road));
	}
}

void load_rings(DataReader r, const Array &list, std::vector<Polyline> &out) {
	for (int i = 0; i < list.size(); i++) {
		DataReader ri = r.in(i);
		out.push_back(line(ri, r.table_at(list, i), "boundary", RING_LEAST));
	}
}

void load_rules(DataReader r, const Catalog &c, const Dictionary &t, ScenicCity &city) {
	const Dictionary st = r.table(t, "streets");
	DataReader rs = r.in("streets");
	city.streets = { profile(rs, c, st, "seam_profile"), float(rs.num(st, "seam_overshoot_m")), float(rs.num(st, "highway_clear_m")),
		float(rs.num(st, "bridge_span_m")), float(rs.num(st, "march_step_m")) };
	const Dictionary z = r.table(t, "zoning");
	DataReader rz = r.in("zoning");
	city.zoning = { float(rz.num(z, "high_heat")), float(rz.num(z, "medium_heat")), float(rz.num(z, "commerce_trade")), float(rz.num(z, "main_trade")),
		float(rz.num(z, "noise_cell_m")), float(rz.num(z, "noise_heat")) };
	const Dictionary tr = r.table(t, "trees");
	DataReader rt = r.in("trees");
	city.trees = { float(rt.num(tr, "spacing_m")), float(rt.num(tr, "jitter")), float(rt.num(tr, "grove_cell_m")), float(rt.num(tr, "grove_cut")),
		float(rt.num(tr, "tree_short_m")), float(rt.num(tr, "tree_tall_m")), float(rt.num(tr, "road_clear_m")) };
	const Array heat = r.list(t, "heat");
	for (int i = 0; i < heat.size(); i++) {
		DataReader rh = r.in("heat").in(i);
		const Dictionary d = r.in("heat").table_at(heat, i);
		city.heat.push_back({ line(rh, d, "polyline", 1), float(rh.num(d, "heat")), float(rh.num(d, "trade")), float(rh.num(d, "reach_m")) });
	}
}

int biome_slot(DataReader &r, const Catalog &c, const std::string &id) {
	for (const PaintKindDef &k : c.nature.paint.kinds)
		if (k.cls == PaintClass::BIOME && k.id == id) return k.slot;
	r.fail(String("unknown biome '") + id.c_str() + String("'"));
	return 0;
}
} // namespace

bool load_scenic(const Dictionary &t, Catalog &c, PackedStringArray &errors) {
	const int64_t before = errors.size();
	DataReader top{ errors, "scenic.json" };
	const Array cities = top.list(t, "cities");
	for (int i = 0; i < cities.size(); i++) {
		DataReader r = top.in("cities").in(i);
		const Dictionary d = top.in("cities").table_at(cities, i);
		ScenicCity city;
		city.id = r.utf8(d, "id");
		city.half_m = float(r.num(d, "city_half_m"));
		city.biome = biome_slot(r, c, r.utf8(d, "biome"));
		load_land(r.in("land"), r.table(d, "land"), city.land);
		load_districts(r.in("districts"), c, r.list(d, "districts"), city.districts);
		load_roads(r.in("roads"), c, r.list(d, "roads"), city.land, city.roads);
		load_rings(r.in("parks"), r.list(d, "parks"), city.parks);
		load_rings(r.in("works"), r.list(d, "works"), city.works);
		load_rules(r, c, d, city);
		if (city.districts.empty()) r.fail("a city needs a district");
		c.scenic.push_back(std::move(city));
	}
	return errors.size() == before;
}

bool link_scenic_maps(const std::vector<std::string> &names, Catalog &c, PackedStringArray &errors) {
	bool ok = true;
	for (size_t m = 0; m < names.size() && m < c.maps.size(); m++) {
		if (names[m].empty()) continue;
		for (size_t i = 0; i < c.scenic.size(); i++)
			if (c.scenic[i].id == names[m]) c.maps[m].scenic = int(i);
		if (c.maps[m].scenic >= 0) continue;
		errors.push_back("maps[" + String::num_int64(int64_t(m)) + "]: no scenic city '" + String(names[m].c_str()) + "'");
		ok = false;
	}
	return ok;
}

} // namespace city
