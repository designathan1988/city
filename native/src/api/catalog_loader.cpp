// Strict Dictionary -> Catalog conversion. The tuning X-macro drives the tuning section, so a field added to
// sim/catalog/tuning_fields.h is automatically required here.
#include "api/catalog_loader.h"

#include "core/color.h"
#include "sim/roads/profiles.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city {

namespace {

struct Reader {
	PackedStringArray &errors;
	String where;

	bool has(const Dictionary &d, const char *key) {
		if (d.has(key)) return true;
		errors.push_back(where + ": falta '" + String(key) + "'");
		return false;
	}
	float num(const Dictionary &d, const char *key) {
		if (!has(d, key)) return 0.f;
		Variant v = d[key];
		if (v.get_type() != Variant::FLOAT && v.get_type() != Variant::INT) {
			errors.push_back(where + "." + String(key) + ": esperado numero");
			return 0.f;
		}
		return float(double(v));
	}
	int integer(const Dictionary &d, const char *key) { return int(num(d, key)); }
	bool flag(const Dictionary &d, const char *key) { return has(d, key) && bool(d[key]); }
	String str(const Dictionary &d, const char *key) { return has(d, key) ? String(d[key]) : String(); }
	Array arr(const Dictionary &d, const char *key) { return has(d, key) ? Array(d[key]) : Array(); }
	Dictionary dict(const Dictionary &d, const char *key) { return has(d, key) ? Dictionary(d[key]) : Dictionary(); }
	IntRange range(const Dictionary &d, const char *key) {
		Array a = arr(d, key);
		if (a.size() != 2) { errors.push_back(where + "." + String(key) + ": esperado [min, max)"); return {}; }
		return { int(a[0]), int(a[1]) };
	}
};

std::string s8(const String &s) { return std::string(s.utf8().get_data()); }
// Optional strip flags (absent = false / 0): the JSON writes only the flags a strip has.
bool opt_flag(const Dictionary &d, const char *key) { return d.has(key) && bool(d[key]); }
int opt_int(const Dictionary &d, const char *key) { return d.has(key) ? int(d[key]) : 0; }

void load_tuning(const Dictionary &t, Tuning &tn, PackedStringArray &errors) {
#define CITY_TUNING_LOAD(group, name) \
	{ \
		Reader r{ errors, "tuning." #group }; \
		Dictionary g = t.has(#group) ? Dictionary(t[#group]) : Dictionary(); \
		tn.group##_##name = r.num(g, #name); \
	}
	CITY_TUNING_FIELDS(CITY_TUNING_LOAD)
#undef CITY_TUNING_LOAD
}

StripKind strip_kind(const String &k, Reader &r) {
	static const char *NAMES[] = { "sidewalk", "curb", "parking", "bike", "lane", "median", "shoulder", "barrier" };
	for (int i = 0; i < int(StripKind::COUNT); i++)
		if (k == NAMES[i]) return StripKind(i);
	r.errors.push_back(r.where + ": faixa desconhecida '" + k + "'");
	return StripKind::LANE;
}

void load_profiles(const Dictionary &t, Catalog &c, PackedStringArray &errors) {
	Reader top{ errors, "road_profiles" };
	Dictionary pr = top.dict(t, "prices"), st = top.dict(t, "structure");
	c.prices = { top.num(pr, "carriageway"), top.num(pr, "median"), top.num(pr, "footway") };
	c.structure = { top.num(st, "ground"), top.num(st, "embankment"), top.num(st, "trench"), top.num(st, "bridge"), top.num(st, "tunnel") };
	Array list = top.arr(t, "profiles");
	for (int i = 0; i < list.size(); i++) {
		Dictionary d = list[i];
		Reader r{ errors, "road_profiles[" + String::num_int64(i) + "]" };
		Profile p;
		p.id = s8(r.str(d, "id"));
		p.name_key = s8(r.str(d, "name_key"));
		p.category = s8(r.str(d, "category"));
		p.centre = s8(r.str(d, "centre"));
		p.speed = r.num(d, "speed");
		p.one_way = r.flag(d, "one_way");
		p.ring = r.flag(d, "ring");
		p.priority = r.integer(d, "priority");
		p.surface = Surface(r.integer(d, "surface"));
		for (const Variant &sv : r.arr(d, "strips")) {
			Dictionary sd = sv;
			Strip s;
			s.kind = strip_kind(r.str(sd, "kind"), r);
			s.width = r.num(sd, "width");
			s.dir = opt_int(sd, "dir");
			s.bus = opt_flag(sd, "bus");
			s.flush = opt_flag(sd, "flush");
			s.rail = opt_flag(sd, "rail");
			s.solid = opt_flag(sd, "solid");
			s.pave = Paving(opt_int(sd, "pave"));
			s.turns = uint8_t(opt_int(sd, "turns"));
			p.strips.push_back(s);
		}
		if (p.strips.empty()) errors.push_back(r.where + ": perfil sem faixas");
		else derive_profile(p, c.prices);
		c.profiles.push_back(p);
	}
}

void load_zones(const Dictionary &t, Catalog &c, PackedStringArray &errors) {
	Reader top{ errors, "zones" };
	Array list = top.arr(t, "zones");
	if (list.size() != ZONE_CODES) errors.push_back("zones: esperado " + String::num_int64(ZONE_CODES) + " zonas");
	for (int i = 0; i < list.size() && i < ZONE_CODES; i++) {
		Dictionary d = list[i];
		Reader r{ errors, "zones[" + String::num_int64(i) + "]" };
		int code = r.integer(d, "code");
		if (code < 0 || code >= ZONE_CODES) { errors.push_back(r.where + ": codigo fora da faixa"); continue; }
		ZoneDef &z = c.zones[code];
		z.id = s8(r.str(d, "id"));
		z.name_key = s8(r.str(d, "name_key"));
		z.base = r.integer(d, "base");
		z.density = r.integer(d, "density");
		z.color = CatalogLoader::parse_color(r.str(d, "color"));
	}
	Dictionary o = top.dict(t, "residents");
	c.occupancy = { top.num(o, "floor_share"), top.num(o, "m2_per_resident"), top.num(o, "m2_per_job_commercial"),
		top.num(o, "m2_per_job_industrial"), top.num(o, "m2_per_person_mixed") };
}

void load_growth(const Dictionary &t, GrowthDef &g, PackedStringArray &errors) {
	Reader r{ errors, "growth" };
	Dictionary p = r.dict(t, "plan");
	CityPlanDef &d = g.plan;
	d.medium_radius_m = r.num(p, "medium_radius_m");
	d.centres = r.integer(p, "centres");
	d.main_offset = r.num(p, "main_offset");
	d.centre_turn = r.num(p, "centre_turn");
	d.second_from = r.num(p, "second_from");
	d.second_to = r.num(p, "second_to");
	d.second_pull = r.num(p, "second_pull");
	d.density = r.num(p, "density");
	d.density_pull = r.num(p, "density_pull");
	d.noise_cell_m = r.num(p, "noise_cell_m");
	d.noise_heat = r.num(p, "noise_heat");
	d.high_heat = r.num(p, "high_heat");
	d.medium_heat = r.num(p, "medium_heat");
	d.commerce_heat = r.num(p, "commerce_heat");
	d.main_commerce_heat = r.num(p, "main_commerce_heat");
	d.industry_from = r.num(p, "industry_from");
	d.industry_half_angle = r.num(p, "industry_half_angle");
	Dictionary cv = r.dict(t, "civic");
	g.civic_min_area = r.num(cv, "min_area");
	g.civic_chance = r.num(cv, "chance");
	Array by_zone = r.arr(cv, "by_zone");
	for (int z = 0; z < 5 && z < by_zone.size(); z++)
		for (const Variant &e : Array(by_zone[z])) { Array p = e; g.civic_by_zone[z].push_back({ int(p[0]), int(p[1]) }); }
	g.square_min_area = r.num(t, "square_min_area");
	g.organic_reach_m = r.num(t, "organic_reach_m");
	g.organic_extra_floors = r.integer(t, "organic_extra_floors");
	g.decay_grace_min = r.num(t, "decay_grace_min");
	g.decay_span_min = r.num(t, "decay_span_min");
}

void load_maps(const Dictionary &t, Catalog &c, PackedStringArray &errors) {
	Reader top{ errors, "maps" };
	Array list = top.arr(t, "maps");
	for (int i = 0; i < list.size(); i++) {
		Dictionary d = list[i];
		Reader r{ errors, "maps[" + String::num_int64(i) + "]" };
		MapDef m;
		m.id = s8(r.str(d, "id"));
		m.name_key = s8(r.str(d, "name_key"));
		m.tiles = r.integer(d, "tiles");
		m.medium = r.flag(d, "medium");
		c.maps.push_back(m);
	}
}

} // namespace

uint32_t CatalogLoader::parse_color(const String &hex) { return parse_hex_color(s8(hex)); }

bool CatalogLoader::load(const Dictionary &tables, Catalog &out, PackedStringArray &errors) {
	Catalog c;
	Reader r{ errors, "catalog" };
	load_tuning(r.dict(tables, "tuning"), c.tune, errors);
	load_maps(r.dict(tables, "maps"), c, errors);
	load_profiles(r.dict(tables, "road_profiles"), c, errors);
	load_zones(r.dict(tables, "zones"), c, errors);
	load_growth(r.dict(tables, "growth"), c.growth, errors);
	load_traffic_params(r.dict(tables, "traffic"), c.traffic, errors);
	load_street_furniture(r.dict(tables, "street_furniture"), c.furniture, errors);
	if (c.maps.empty()) errors.push_back("maps: nenhum mapa");
	if (!errors.is_empty()) return false;
	out = std::move(c);
	return true;
}

} // namespace city
