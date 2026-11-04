// Strict Dictionary -> StreetFurnitureDef conversion for project/data/catalog/street_furniture.json. The X-macros of
// sim/catalog/furniture_types.h drive the scalar blocks, so a field added there is automatically required here.
#include "api/catalog_loader.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city {

namespace {

constexpr int FOOTPRINT_VALUES = 2; // [x_m, z_m]
// street_furniture.json "footprints", in FurnitureKind order.
const char *const FOOTPRINT_KEYS[FURNITURE_KINDS] = { "signal_post", "lamp_post", "fire_hydrant", "litter_bin", "street_bench", "street_tree",
	"lamp_column", "post_box", "phone", "kerb_drain", "shrub", "utility_pole", "sign_post" };
// street_furniture.json "barriers" "kinds", in BarrierKind order.
const char *const BARRIER_KEYS[BARRIER_KINDS] = { "fence", "wall", "hedge", "guardrail", "railing" };

Dictionary block(const Dictionary &t, const char *key, PackedStringArray &errors) {
	if (!t.has(key)) errors.push_back(String("street_furniture: falta '") + key + "'");
	return t.get(key, Dictionary());
}

bool number(const Dictionary &d, const char *where, const char *key, float &out, PackedStringArray &errors) {
	if (!d.has(key)) {
		errors.push_back(String("street_furniture.") + where + ": falta '" + key + "'");
		return false;
	}
	const Variant v = d[key];
	if (v.get_type() != Variant::FLOAT && v.get_type() != Variant::INT) {
		errors.push_back(String("street_furniture.") + where + "." + key + ": esperado numero");
		return false;
	}
	out = float(double(v));
	return true;
}

} // namespace

bool load_street_furniture(const Dictionary &t, StreetFurnitureDef &d, PackedStringArray &errors) {
	const Dictionary footprints = block(t, "footprints", errors);
	const Dictionary sidewalk = block(t, "sidewalk", errors), lamps = block(t, "lamps", errors), signals = block(t, "signals", errors);
	const Dictionary hydrants = block(t, "hydrants", errors), furniture = block(t, "furniture", errors), trees = block(t, "trees", errors);
	const Dictionary crossing = block(t, "crossing", errors);
#define CITY_SIDEWALK_LOAD(name) number(sidewalk, "sidewalk", #name, d.sidewalk.name, errors);
	CITY_SIDEWALK_FIELDS(CITY_SIDEWALK_LOAD)
#undef CITY_SIDEWALK_LOAD
#define CITY_LAMP_LOAD(name) number(lamps, "lamps", #name, d.lamps.name, errors);
	CITY_LAMP_FIELDS(CITY_LAMP_LOAD)
#undef CITY_LAMP_LOAD
#define CITY_SIGNAL_LOAD(name) number(signals, "signals", #name, d.signals.name, errors);
	CITY_SIGNAL_FIELDS(CITY_SIGNAL_LOAD)
#undef CITY_SIGNAL_LOAD
#define CITY_HYDRANT_LOAD(name) number(hydrants, "hydrants", #name, d.hydrants.name, errors);
	CITY_HYDRANT_FIELDS(CITY_HYDRANT_LOAD)
#undef CITY_HYDRANT_LOAD
#define CITY_FURNITURE_LOAD(name) number(furniture, "furniture", #name, d.furniture.name, errors);
	CITY_FURNITURE_FIELDS(CITY_FURNITURE_LOAD)
#undef CITY_FURNITURE_LOAD
#define CITY_TREE_LOAD(name) number(trees, "trees", #name, d.trees.name, errors);
	CITY_TREE_FIELDS(CITY_TREE_LOAD)
#undef CITY_TREE_LOAD
#define CITY_CROSSING_LOAD(name) number(crossing, "crossing", #name, d.crossing.name, errors);
	CITY_CROSSING_FIELDS(CITY_CROSSING_LOAD)
#undef CITY_CROSSING_LOAD
	const Dictionary poles = block(t, "poles", errors), barriers = block(t, "barriers", errors), signs = block(t, "signs", errors);
	const Dictionary items = block(t, "items", errors);
#define CITY_POLE_LOAD(name) number(poles, "poles", #name, d.poles.name, errors);
	CITY_POLE_FIELDS(CITY_POLE_LOAD)
#undef CITY_POLE_LOAD
#define CITY_BARRIER_LOAD(name) number(barriers, "barriers", #name, d.barriers.name, errors);
	CITY_BARRIER_FIELDS(CITY_BARRIER_LOAD)
#undef CITY_BARRIER_LOAD
#define CITY_SIGN_LOAD(name) number(signs, "signs", #name, d.signs.name, errors);
	CITY_SIGN_FIELDS(CITY_SIGN_LOAD)
#undef CITY_SIGN_LOAD
#define CITY_ITEM_LOAD(name) number(items, "items", #name, d.items.name, errors);
	CITY_ITEM_FIELDS(CITY_ITEM_LOAD)
#undef CITY_ITEM_LOAD
	const Dictionary kinds = barriers.get("kinds", Dictionary());
	for (int k = 0; k < BARRIER_KINDS; k++) {
		const Dictionary b = kinds.get(BARRIER_KEYS[k], Dictionary());
		number(b, BARRIER_KEYS[k], "height_m", d.barriers.height_m[k], errors);
		number(b, BARRIER_KEYS[k], "thickness_m", d.barriers.thickness_m[k], errors);
	}
	for (int k = 0; k < FURNITURE_KINDS; k++) {
		const Array a = footprints.get(FOOTPRINT_KEYS[k], Array());
		if (a.size() != FOOTPRINT_VALUES) {
			errors.push_back(String("street_furniture.footprints.") + FOOTPRINT_KEYS[k] + ": esperado [x_m, z_m]");
			continue;
		}
		d.footprint[k] = { float(double(a[0])), float(double(a[1])) };
	}
	return errors.is_empty();
}

} // namespace city
