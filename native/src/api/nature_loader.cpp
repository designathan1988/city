// data/catalog/nature.json -> NatureDef (sim/catalog/nature_types.h), strict: a missing or mistyped key fails the load.
#include "api/catalog_loader.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city {

namespace {
struct Read {
	PackedStringArray &errors;
	String where;
	bool has(const Dictionary &d, const char *k) {
		if (d.has(k)) return true;
		errors.push_back("nature." + where + ": falta '" + String(k) + "'");
		return false;
	}
	float num(const Dictionary &d, const char *k) { return has(d, k) ? float(double(d[k])) : 0.f; }
	int integer(const Dictionary &d, const char *k) { return int(num(d, k)); }
	Dictionary dict(const Dictionary &d, const char *k) { return has(d, k) ? Dictionary(d[k]) : Dictionary(); }
	Array arr(const Dictionary &d, const char *k) { return has(d, k) ? Array(d[k]) : Array(); }
	LimitDef limit(const Dictionary &d, const char *k) {
		Dictionary l = dict(d, k);
		return { num(l, "min"), num(l, "max"), num(l, "default") };
	}
};

std::string text(const Variant &v) { return std::string(String(v).utf8().get_data()); }

void load_weather(const Dictionary &t, WeatherDef &w, PackedStringArray &e) {
	Read r{ e, "weather" };
#define CITY_WEATHER_READ(name) w.name = r.limit(t, #name);
	CITY_WEATHER_FIELDS(CITY_WEATHER_READ)
#undef CITY_WEATHER_READ
}

void load_clouds(const Dictionary &t, CloudDef &c, PackedStringArray &e) {
	Read r{ e, "clouds" };
	c.max_clouds = r.integer(t, "max_clouds");
	c.scatter_tries = r.integer(t, "scatter_tries");
	c.span_m = r.num(t, "span_m");
	c.crowd_share = r.num(t, "crowd_share");
	c.scatter_reach = r.num(t, "scatter_reach");
	c.show_from = r.num(t, "show_from");
	c.show_span = r.num(t, "show_span");
	c.pick_centre = r.num(t, "pick_centre");
	c.pick_radius = r.num(t, "pick_radius");
	c.grip_height = r.num(t, "grip_height");
	c.height_m = r.limit(t, "height_m");
	c.size_m = r.limit(t, "size_m");
	c.density = r.limit(t, "density");
}

void load_fog(const Dictionary &t, FogDef &f, PackedStringArray &e) {
	Read r{ e, "fog" };
	f.max_dabs = r.integer(t, "max_dabs");
	f.texel_m = r.num(t, "fog_texel_m");
	f.min_density = r.num(t, "min_density");
	f.height_m = r.limit(t, "height_m");
	f.speed_ms = r.limit(t, "speed_ms");
}

PaintClass paint_class(const String &s, Read &r) {
	if (s == "ground") return PaintClass::GROUND;
	if (s == "cover") return PaintClass::COVER;
	if (s == "biome") return PaintClass::BIOME;
	if (s == "eraser") return PaintClass::ERASER;
	r.errors.push_back("nature.paint: classe desconhecida '" + s + "'");
	return PaintClass::GROUND;
}

void load_paint(const Dictionary &t, PaintDef &p, PackedStringArray &e) {
	Read r{ e, "paint" };
	p.max_tiles = r.integer(t, "max_tiles");
	p.strength_scale = r.num(t, "strength_scale");
	p.min_strength = r.num(t, "min_strength");
	p.land_rim = r.num(t, "land_rim");
	p.rock_reach = r.num(t, "rock_reach");
	Array kinds = r.arr(t, "kinds");
	for (int i = 0; i < kinds.size(); i++) {
		Dictionary d = kinds[i];
		PaintKindDef k;
		k.id = text(d.get("id", ""));
		k.cls = paint_class(String(d.get("class", "")), r);
		k.slot = r.integer(d, "slot");
		p.kinds.push_back(k);
	}
	Array rocks = r.arr(t, "landform_rock");
	for (int i = 0; i < rocks.size(); i++) {
		int found = -1;
		for (int k = 0; k < int(p.kinds.size()); k++)
			if (p.kinds[size_t(k)].id == text(rocks[i])) found = k;
		if (found < 0) e.push_back("nature.paint.landform_rock: tipo desconhecido");
		p.landform_rock.push_back(found);
	}
	std::string sand = text(t.get("river_sand", ""));
	for (const PaintKindDef &k : p.kinds)
		if (k.id == sand) p.river_sand = int(&k - p.kinds.data());
	if (p.river_sand < 0) e.push_back("nature.paint.river_sand: tipo desconhecido");
	p.river_sand_reach = r.num(t, "river_sand_reach");
}

void load_trees(const Dictionary &t, TreeBrushDef &b, PackedStringArray &e) {
	Read r{ e, "trees" };
	b.max_planted = r.integer(t, "max_planted");
	b.max_clearings = r.integer(t, "max_clearings");
	b.max_per_dab = r.integer(t, "max_per_dab");
	b.tries_per_tree = r.integer(t, "tries_per_tree");
	b.model_slots = r.integer(t, "model_slots");
	b.min_height_share = r.num(t, "min_height_share");
	b.m2_per_hectare = r.num(t, "m2_per_hectare");
	b.seed_top = r.num(t, "seed_top");
	for (const Variant &v : r.arr(t, "kind_first")) b.kind_first.push_back(int(v));
	for (const Variant &v : r.arr(t, "kind_count")) b.kind_count.push_back(int(v));
}

Community community(const Dictionary &d, Read &r) {
	Community c;
	c.canopy = r.num(d, "closed_canopy"); c.trees = r.num(d, "trees"); c.emergent = r.num(d, "emergent"); c.shrub = r.num(d, "shrub");
	c.palm = r.num(d, "palm"); c.cactus = r.num(d, "cactus"); c.grass = r.num(d, "grass"); c.dry = r.num(d, "dry");
	c.wet = r.num(d, "wet"); c.rocky = r.num(d, "rocky");
	return c;
}

// The regional style the landscape reads until the style lives in the CityState.
const char *const STYLE = "roadcraft";

void load_ecology(const Dictionary &t, const Dictionary &style, const PaintDef &paint, EcologyDef &c, PackedStringArray &e) {
	static const char *const NAMES[FORMATIONS] = { "gallery", "vereda", "cerradao", "cerrado", "rupestre", "ombrophilous", "high_grassland",
		"terra_firme", "varzea", "thorn_scrub", "caatinga_river", "campos", "capao", "flooded_field", "cordilheira", "outcrop" };
	Read r{ e, "ecology" };
	Dictionary f = r.dict(t, "formations");
	for (int i = 0; i < FORMATIONS; i++) c.formations[size_t(i)] = community(r.dict(f, NAMES[i]), r);
	Dictionary mixes = r.dict(t, "mixes");
#define CITY_MIX_READ(name) c.mix.name = r.num(mixes, #name);
	CITY_ECOLOGY_MIX_FIELDS(CITY_MIX_READ)
#undef CITY_MIX_READ
	c.grid_m = r.num(t, "site_grid_m"); c.patch_m = r.num(t, "patch_m"); c.fray_m = r.num(t, "fray_m"); c.mean_cells = r.integer(t, "mean_cells");
	c.low_m = r.num(t, "site_low_m"); c.high_m = r.num(t, "site_high_m");
	c.flat_from_deg = r.num(t, "flat_from_deg"); c.flat_to_deg = r.num(t, "flat_to_deg");
	c.steep_from_deg = r.num(t, "steep_from_deg"); c.steep_to_deg = r.num(t, "steep_to_deg");
	c.riparian_near_m = r.num(t, "riparian_near_m"); c.riparian_near_patch_m = r.num(t, "riparian_near_patch_m");
	c.riparian_far_m = r.num(t, "riparian_far_m"); c.riparian_far_patch_m = r.num(t, "riparian_far_patch_m");
	c.forest_line_from_m = r.num(t, "forest_line_from_m"); c.forest_line_to_m = r.num(t, "forest_line_to_m");
	c.canopy_wood = r.num(t, "canopy_wood"); c.trees_wood = r.num(t, "trees_wood"); c.emergent_wood = r.num(t, "emergent_wood");
	std::string def = text(style.get("default_biome", ""));
	c.default_biome = -1;
	for (const PaintKindDef &k : paint.kinds)
		if (k.cls == PaintClass::BIOME && k.id == def) c.default_biome = k.slot;
	if (c.default_biome < 0) e.push_back("nature.ecology.default_biome: bioma desconhecido");
}
} // namespace

bool load_nature(const Dictionary &t, NatureDef &n, PackedStringArray &errors) {
	Read r{ errors, "" };
	load_weather(r.dict(t, "weather"), n.weather, errors);
	load_clouds(r.dict(t, "clouds"), n.clouds, errors);
	load_fog(r.dict(t, "fog"), n.fog, errors);
	load_paint(r.dict(t, "paint"), n.paint, errors);
	load_trees(r.dict(t, "trees"), n.trees, errors);
	load_ecology(r.dict(t, "ecology"), r.dict(r.dict(t, "styles"), STYLE), n.paint, n.ecology, errors);
	Dictionary el = r.dict(t, "elements");
	n.elements.max_items = r.integer(el, "max_items");
	n.elements.max_per_dab = r.integer(el, "max_per_dab");
	n.elements.kinds = int(r.arr(el, "kinds").size());
	n.elements.area_m2 = r.num(el, "area_m2");
	n.elements.least_size_m = r.num(el, "least_size_m");
	return errors.is_empty();
}

} // namespace city
