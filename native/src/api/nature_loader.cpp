// data/catalog/nature.json -> NatureDef (sim/catalog/nature_types.h) through the strict reader (api/data_reader.h): a
// missing or mistyped key, a list of the wrong length and an unknown name fail the load.
#include "api/catalog_loader.h"
#include "render/data_reader.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city {

namespace {

LimitDef limit(DataReader r, const Dictionary &l) { return { float(r.num(l, "min")), float(r.num(l, "max")), float(r.num(l, "default")) }; }
LimitDef limit(DataReader &r, const Dictionary &d, const char *key) { return limit(r.in(key), r.table(d, key)); }

void load_weather(DataReader r, const Dictionary &t, WeatherDef &w) {
#define CITY_WEATHER_READ(name) w.name = limit(r, t, #name);
	CITY_WEATHER_FIELDS(CITY_WEATHER_READ)
#undef CITY_WEATHER_READ
}

void load_clouds(DataReader r, const Dictionary &t, CloudDef &c) {
	c.max_clouds = r.integer(t, "max_clouds");
	c.scatter_tries = r.integer(t, "scatter_tries");
	c.span_m = float(r.num(t, "span_m"));
	c.crowd_share = float(r.num(t, "crowd_share"));
	c.scatter_reach = float(r.num(t, "scatter_reach"));
	c.show_from = float(r.num(t, "show_from"));
	c.show_span = float(r.num(t, "show_span"));
	c.pick_centre = float(r.num(t, "pick_centre"));
	c.pick_radius = float(r.num(t, "pick_radius"));
	c.grip_height = float(r.num(t, "grip_height"));
	c.height_m = limit(r, t, "height_m");
	c.size_m = limit(r, t, "size_m");
	c.density = limit(r, t, "density");
}

void load_fog(DataReader r, const Dictionary &t, FogDef &f) {
	f.max_dabs = r.integer(t, "max_dabs");
	f.texel_m = float(r.num(t, "fog_texel_m"));
	f.min_density = float(r.num(t, "min_density"));
	f.height_m = limit(r, t, "height_m");
	f.speed_ms = limit(r, t, "speed_ms");
}

PaintClass paint_class(const String &s, DataReader &r) {
	if (s == "ground") return PaintClass::GROUND;
	if (s == "cover") return PaintClass::COVER;
	if (s == "biome") return PaintClass::BIOME;
	if (s == "eraser") return PaintClass::ERASER;
	r.fail(String("unknown class '") + s + String("'"));
	return PaintClass::GROUND;
}

void load_paint(DataReader r, const Dictionary &t, PaintDef &p) {
	p.max_tiles = r.integer(t, "max_tiles");
	p.strength_scale = float(r.num(t, "strength_scale"));
	p.min_strength = float(r.num(t, "min_strength"));
	p.land_rim = float(r.num(t, "land_rim"));
	p.rock_reach = float(r.num(t, "rock_reach"));
	p.rock_rim = float(r.num(t, "rock_rim"));
	const Array kinds = r.list(t, "kinds");
	for (int i = 0; i < kinds.size(); i++) {
		DataReader rk = r.in("kinds").in(i);
		const Dictionary d = r.in("kinds").table_at(kinds, i);
		PaintKindDef k;
		k.id = rk.utf8(d, "id");
		k.cls = paint_class(rk.text(d, "class"), rk);
		k.slot = rk.integer(d, "slot");
		p.kinds.push_back(k);
	}
	const Array rocks = r.list(t, "landform_rock");
	for (int i = 0; i < rocks.size(); i++) {
		DataReader rr = r.in("landform_rock");
		const std::string id = rr.utf8_at(rocks, i);
		int found = -1;
		for (int k = 0; k < int(p.kinds.size()); k++)
			if (p.kinds[size_t(k)].id == id) found = k;
		if (found < 0) rr.fail(String("unknown kind '") + id.c_str() + String("'"));
		p.landform_rock.push_back(found);
	}
}

void load_trees(DataReader r, const Dictionary &t, TreeBrushDef &b) {
	b.max_planted = r.integer(t, "max_planted");
	b.max_clearings = r.integer(t, "max_clearings");
	b.max_per_dab = r.integer(t, "max_per_dab");
	b.tries_per_tree = r.integer(t, "tries_per_tree");
	b.model_slots = r.integer(t, "model_slots");
	b.min_height_share = float(r.num(t, "min_height_share"));
	b.m2_per_hectare = float(r.num(t, "m2_per_hectare"));
	b.seed_top = float(r.num(t, "seed_top"));
	const Array first = r.list(t, "kind_first"), count = r.list(t, "kind_count");
	for (int i = 0; i < first.size(); i++) b.kind_first.push_back(r.in("kind_first").integer_at(first, i));
	for (int i = 0; i < count.size(); i++) b.kind_count.push_back(r.in("kind_count").integer_at(count, i));
}

Community community(DataReader r, const Dictionary &d) {
	Community c;
	c.canopy = float(r.num(d, "closed_canopy"));
	c.trees = float(r.num(d, "trees"));
	c.emergent = float(r.num(d, "emergent"));
	c.shrub = float(r.num(d, "shrub"));
	c.palm = float(r.num(d, "palm"));
	c.cactus = float(r.num(d, "cactus"));
	c.grass = float(r.num(d, "grass"));
	c.dry = float(r.num(d, "dry"));
	c.wet = float(r.num(d, "wet"));
	c.rocky = float(r.num(d, "rocky"));
	return c;
}

// The regional style the landscape reads until the style lives in the CityState.
const char *const STYLE = "roadcraft";

void load_ecology(DataReader r, const Dictionary &t, DataReader rs, const Dictionary &style, const PaintDef &paint, EcologyDef &c) {
	static const char *const NAMES[FORMATIONS] = { "gallery", "vereda", "cerradao", "cerrado", "rupestre", "ombrophilous", "high_grassland",
		"terra_firme", "varzea", "thorn_scrub", "caatinga_river", "campos", "capao", "flooded_field", "cordilheira", "outcrop" };
	const Dictionary f = r.table(t, "formations");
	for (int i = 0; i < FORMATIONS; i++) c.formations[size_t(i)] = community(r.in("formations").in(NAMES[i]), r.in("formations").table(f, NAMES[i]));
	const Dictionary mixes = r.table(t, "mixes");
#define CITY_MIX_READ(name) c.mix.name = float(r.in("mixes").num(mixes, #name));
	CITY_ECOLOGY_MIX_FIELDS(CITY_MIX_READ)
#undef CITY_MIX_READ
	c.grid_m = float(r.num(t, "site_grid_m"));
	c.patch_m = float(r.num(t, "patch_m"));
	c.fray_m = float(r.num(t, "fray_m"));
	c.mean_cells = r.integer(t, "mean_cells");
	c.low_m = float(r.num(t, "site_low_m"));
	c.high_m = float(r.num(t, "site_high_m"));
	c.flat_from_deg = float(r.num(t, "flat_from_deg"));
	c.flat_to_deg = float(r.num(t, "flat_to_deg"));
	c.steep_from_deg = float(r.num(t, "steep_from_deg"));
	c.steep_to_deg = float(r.num(t, "steep_to_deg"));
	c.riparian_near_m = float(r.num(t, "riparian_near_m"));
	c.riparian_near_patch_m = float(r.num(t, "riparian_near_patch_m"));
	c.riparian_far_m = float(r.num(t, "riparian_far_m"));
	c.riparian_far_patch_m = float(r.num(t, "riparian_far_patch_m"));
	c.forest_line_from_m = float(r.num(t, "forest_line_from_m"));
	c.forest_line_to_m = float(r.num(t, "forest_line_to_m"));
	c.canopy_wood = float(r.num(t, "canopy_wood"));
	c.trees_wood = float(r.num(t, "trees_wood"));
	c.emergent_wood = float(r.num(t, "emergent_wood"));
	const std::string biome = rs.utf8(style, "default_biome");
	c.default_biome = -1;
	for (const PaintKindDef &k : paint.kinds)
		if (k.cls == PaintClass::BIOME && k.id == biome) c.default_biome = k.slot;
	if (c.default_biome < 0) rs.fail(String("default_biome: no biome '") + biome.c_str() + String("' among paint.kinds"));
	const Dictionary geology = r.table(t, "geology");
	DataReader rg = r.in("geology");
	auto surface = [&](const char *rock) {
		const std::string id = rg.utf8(geology, rock);
		for (const PaintKindDef &k : paint.kinds)
			if (k.cls == PaintClass::GROUND && k.id == id) return k.slot + 1; // PaintCell::surface: the ground kind's slot + 1
		rg.in(rock).fail(String("no ground kind '") + id.c_str() + String("' among paint.kinds"));
		return 0;
	};
	c.sandstone_surface = surface("sandstone");
	c.basalt_surface = surface("basalt");
}
} // namespace

bool load_nature(const Dictionary &t, NatureDef &n, PackedStringArray &errors) {
	const int64_t before = errors.size();
	DataReader r{ errors, "nature.json" };
	load_weather(r.in("weather"), r.table(t, "weather"), n.weather);
	load_clouds(r.in("clouds"), r.table(t, "clouds"), n.clouds);
	load_fog(r.in("fog"), r.table(t, "fog"), n.fog);
	load_paint(r.in("paint"), r.table(t, "paint"), n.paint);
	load_trees(r.in("trees"), r.table(t, "trees"), n.trees);
	DataReader rs = r.in("styles").in(STYLE);
	load_ecology(r.in("ecology"), r.table(t, "ecology"), rs, r.in("styles").table(r.table(t, "styles"), STYLE), n.paint, n.ecology);
	const Dictionary el = r.table(t, "elements");
	DataReader re = r.in("elements");
	n.elements.max_items = re.integer(el, "max_items");
	n.elements.max_per_dab = re.integer(el, "max_per_dab");
	const Array kinds = re.list(el, "kinds");
	n.elements.kinds = int(kinds.size());
	const String first_effect = re.text(el, "first_effect");
	n.elements.first_effect = int(kinds.find(first_effect));
	if (n.elements.first_effect < 0) re.in("first_effect").fail(String("not a kind: '") + first_effect + String("'"));
	n.elements.area_m2 = float(re.num(el, "area_m2"));
	n.elements.least_size_m = float(re.num(el, "least_size_m"));
	return errors.size() == before;
}

} // namespace city
