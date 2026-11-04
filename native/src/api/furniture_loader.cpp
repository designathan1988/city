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
const char *const FOOTPRINT_KEYS[FURNITURE_KINDS] = { "signal_post", "lamp_post", "fire_hydrant", "litter_bin", "street_bench", "street_tree" };

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
	const Dictionary lamps = block(t, "lamps", errors), furniture = block(t, "furniture", errors);
	const Dictionary crossing = block(t, "crossing", errors), footprints = block(t, "footprints", errors);
#define CITY_LAMP_LOAD(name) number(lamps, "lamps", #name, d.lamps.name, errors);
	CITY_LAMP_FIELDS(CITY_LAMP_LOAD)
#undef CITY_LAMP_LOAD
#define CITY_FURNITURE_LOAD(name) number(furniture, "furniture", #name, d.furniture.name, errors);
	CITY_FURNITURE_FIELDS(CITY_FURNITURE_LOAD)
#undef CITY_FURNITURE_LOAD
#define CITY_CROSSING_LOAD(name) number(crossing, "crossing", #name, d.crossing.name, errors);
	CITY_CROSSING_FIELDS(CITY_CROSSING_LOAD)
#undef CITY_CROSSING_LOAD
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
