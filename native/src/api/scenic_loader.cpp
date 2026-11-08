// data/catalog/scenic.json -> Catalog::scenic (sim/catalog/scenic.h), strict: a missing or mistyped key, an unknown profile
// or biome, a road on a river the city does not have, a road's name key with no text, fails the load. Read after the
// profiles and the nature tables, and after the translations (a road's name is the player's text of its key).
#include "api/catalog_loader.h"

#include <godot_cpp/classes/translation_server.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city {

namespace {
constexpr int POINT_PARTS = 2, LINE_LEAST = 2, RING_LEAST = 3;

struct Scene {
	PackedStringArray &errors;
	const Catalog &c;
	String where;

	void fail(const String &what) { errors.push_back("scenic." + where + ": " + what); }
	bool has(const Dictionary &d, const char *k) {
		if (d.has(k)) return true;
		fail("missing '" + String(k) + "'");
		return false;
	}
	float num(const Dictionary &d, const char *k) {
		if (!has(d, k)) return 0.f;
		const Variant v = d[k];
		if (v.get_type() == Variant::FLOAT || v.get_type() == Variant::INT) return float(double(v));
		fail(String(k) + ": expected a number");
		return 0.f;
	}
	std::string text(const Dictionary &d, const char *k) { return has(d, k) ? std::string(String(d[k]).utf8().get_data()) : std::string(); }
	Array arr(const Dictionary &d, const char *k) { return has(d, k) ? Array(d[k]) : Array(); }
	Dictionary dict(const Dictionary &d, const char *k) { return has(d, k) ? Dictionary(d[k]) : Dictionary(); }
	V2 point(const Variant &v) {
		const Array a = v;
		if (a.size() == POINT_PARTS) return V2(float(double(a[0])), float(double(a[1])));
		fail("expected [x, z]");
		return V2();
	}
	Polyline line(const Dictionary &d, const char *k, int least) {
		Polyline out;
		for (const Variant &p : arr(d, k)) out.push_back(point(p));
		if (int(out.size()) < least) fail(String(k) + ": too few points");
		return out;
	}
	// The player's text of an i18n key (data/i18n, loaded before the catalogue): a key with no text fails.
	std::string translated(const std::string &key) {
		const String text = TranslationServer::get_singleton()->translate(StringName(key.c_str()));
		if (text == String(key.c_str())) fail("no text for '" + text + "'");
		return std::string(text.utf8().get_data());
	}
	int profile(const Dictionary &d, const char *k) {
		const std::string id = text(d, k);
		const int p = c.profile_id(id);
		if (p < 0) fail("unknown profile '" + String(id.c_str()) + "'");
		return p;
	}
};

void load_land(Scene &r, const Dictionary &t, ScenicLand &land) {
	land.base_m = r.num(t, "base_m");
	land.relief_m = r.num(t, "relief_m");
	land.relief_cell_m = r.num(t, "relief_cell_m");
	for (const Variant &v : r.arr(t, "ridges")) {
		const Dictionary d = v;
		land.ridges.push_back({ r.line(d, "line", LINE_LEAST), r.num(d, "height_m"), r.num(d, "half_width_m") });
	}
	for (const Variant &v : r.arr(t, "rivers")) {
		const Dictionary d = v;
		ScenicRiver w;
		w.id = r.text(d, "id");
		w.line = r.line(d, "line", LINE_LEAST);
		w.half_width_m = r.num(d, "half_width_m");
		w.depth_m = r.num(d, "depth_m");
		w.wall_m = r.num(d, "wall_m");
		w.freeboard_m = r.num(d, "freeboard_m");
		w.valley_m = r.num(d, "valley_m");
		w.level_m = r.num(d, "level_m");
		w.level_end_m = r.num(d, "level_end_m");
		land.rivers.push_back(std::move(w));
	}
	for (const Variant &v : r.arr(t, "lakes")) {
		const Dictionary d = v;
		land.lakes.push_back({ r.line(d, "shore", RING_LEAST), r.num(d, "drop_m"), r.num(d, "depth_m"), r.num(d, "shore_m") });
	}
}

void load_districts(Scene &r, const Array &list, std::vector<ScenicDistrict> &out) {
	for (const Variant &v : list) {
		const Dictionary d = v;
		ScenicDistrict k;
		k.id = r.text(d, "id");
		k.seed = r.has(d, "seed") ? r.point(d["seed"]) : V2();
		const V2 block = r.has(d, "block_m") ? r.point(d["block_m"]) : V2();
		k.block_along_m = block.x;
		k.block_across_m = block.y;
		k.turn_deg = r.num(d, "turn_deg");
		k.heat = r.num(d, "heat");
		k.trade = r.num(d, "trade");
		k.local = r.profile(d, "local");
		k.collector = r.profile(d, "collector");
		k.collector_every = int(r.num(d, "collector_every"));
		if (k.block_along_m <= 0 || k.block_across_m <= 0) r.fail(String(k.id.c_str()) + ": blocks must be wider than 0");
		out.push_back(std::move(k));
	}
}

void load_roads(Scene &r, const Array &list, const ScenicLand &land, std::vector<ScenicRoad> &out) {
	for (const Variant &v : list) {
		const Dictionary d = v;
		ScenicRoad road;
		if (d.has("name_key")) road.name = r.translated(r.text(d, "name_key"));
		road.profile = r.profile(d, "profile");
		if (d.has("river")) {
			const std::string id = std::string(String(d["river"]).utf8().get_data());
			for (size_t i = 0; i < land.rivers.size(); i++)
				if (land.rivers[i].id == id) road.river = int(i);
			if (road.river < 0) r.fail("a road on no river '" + String(id.c_str()) + "'");
			road.offset_m = r.num(d, "offset_m");
		} else road.line = r.line(d, "line", LINE_LEAST);
		out.push_back(std::move(road));
	}
}

void load_rings(Scene &r, const Array &list, std::vector<Polyline> &out) {
	for (const Variant &v : list) out.push_back(r.line(Dictionary(v), "ring", RING_LEAST));
}

void load_rules(Scene &r, const Dictionary &t, ScenicCity &city) {
	const Dictionary st = r.dict(t, "streets");
	city.streets = { r.profile(st, "seam"), r.num(st, "seam_overshoot_m"), r.num(st, "highway_clear_m"), r.num(st, "bridge_max_m"), r.num(st, "march_step_m") };
	const Dictionary z = r.dict(t, "zoning");
	city.zoning = { r.num(z, "high_heat"), r.num(z, "medium_heat"), r.num(z, "commerce_trade"), r.num(z, "main_trade"), r.num(z, "noise_cell_m"), r.num(z, "noise_heat") };
	const Dictionary tr = r.dict(t, "trees");
	city.trees = { r.num(tr, "spacing_m"), r.num(tr, "jitter"), r.num(tr, "grove_cell_m"), r.num(tr, "grove_cut"), r.num(tr, "low_m"), r.num(tr, "high_m"), r.num(tr, "road_clear_m") };
	for (const Variant &v : r.arr(t, "heat")) {
		const Dictionary d = v;
		city.heat.push_back({ r.line(d, "line", 1), r.num(d, "heat"), r.num(d, "trade"), r.num(d, "reach_m") });
	}
}

int biome_slot(Scene &r, const std::string &id) {
	for (const PaintKindDef &k : r.c.nature.paint.kinds)
		if (k.cls == PaintClass::BIOME && k.id == id) return k.slot;
	r.fail("unknown biome '" + String(id.c_str()) + "'");
	return 0;
}
} // namespace

bool load_scenic(const Dictionary &t, Catalog &c, PackedStringArray &errors) {
	const int before = int(errors.size());
	Scene top{ errors, c, "cities" };
	for (const Variant &v : top.arr(t, "cities")) {
		const Dictionary d = v;
		ScenicCity city;
		Scene r{ errors, c, String(d.get("id", "?")) };
		city.id = r.text(d, "id");
		city.half_m = r.num(d, "half_m");
		city.biome = biome_slot(r, r.text(d, "biome"));
		load_land(r, r.dict(d, "land"), city.land);
		load_districts(r, r.arr(d, "districts"), city.districts);
		load_roads(r, r.arr(d, "roads"), city.land, city.roads);
		load_rings(r, r.arr(d, "parks"), city.parks);
		load_rings(r, r.arr(d, "works"), city.works);
		load_rules(r, d, city);
		if (city.districts.empty()) r.fail("a city needs a district");
		c.scenic.push_back(std::move(city));
	}
	return int(errors.size()) == before;
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
