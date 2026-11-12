// Strict Dictionary -> StreetFurnitureDef conversion for project/data/catalog/street_furniture.json. The X-macros of
// sim/catalog/furniture_types.h drive the scalar blocks, so a field added there is automatically required here.
#include "api/catalog_loader.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <algorithm>

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

constexpr int AXES = 3;
constexpr float HALF = 0.5f;
// The list of a kind's parts in "parts" (data/render/streets.json props, handed over by DataTables.catalog), in FurnitureKind
// order; null: a kind with no list of parts (a shrub, a plate's post).
const char *const PART_KEYS[FURNITURE_KINDS] = { "signal", "pole", "hydrant", "bin", "bench", "tree_pit", "pole", "postbox", "phone", "drain", nullptr,
	"pole", nullptr };

bool triple(const Variant &v, float out[AXES]) {
	const Array a = v;
	if (a.size() != AXES) return false;
	for (int i = 0; i < AXES; i++) out[i] = float(double(a[i]));
	return true;
}

float scalar(const Dictionary &p, const char *key) { return float(double(p.get(key, 0.0))); }

// The box that holds one part, by its shape (the shapes render/props/prop_kit.cpp draws); false for a shape it does not know.
bool part_box(const Dictionary &p, PartBox &b) {
	const String shape = p.get("shape", "");
	float a[AXES], c[AXES];
	if (shape == "box") return triple(p.get("min", Variant()), b.lo) && triple(p.get("max", Variant()), b.hi);
	if (shape == "cylinder" || shape == "frustum") {
		if (!triple(p.get("base", Variant()), a)) return false;
		const float r = std::max(scalar(p, "radius"), scalar(p, "top_radius"));
		for (int i = 0; i < AXES; i++) {
			b.lo[i] = a[i] - (i == 1 ? 0.f : r);
			b.hi[i] = a[i] + (i == 1 ? scalar(p, "height") : r);
		}
		return true;
	}
	if (shape == "blob") {
		if (!triple(p.get("center", Variant()), a) || !triple(p.get("radius", Variant()), c)) return false;
		for (int i = 0; i < AXES; i++) {
			b.lo[i] = a[i] - c[i];
			b.hi[i] = a[i] + c[i];
		}
		return true;
	}
	if (shape == "beam") {
		if (!triple(p.get("from", Variant()), a) || !triple(p.get("to", Variant()), c)) return false;
		const float half = std::max(scalar(p, "width"), scalar(p, "thick")) * HALF;
		for (int i = 0; i < AXES; i++) {
			b.lo[i] = std::min(a[i], c[i]) - half;
			b.hi[i] = std::max(a[i], c[i]) + half;
		}
		return true;
	}
	return false;
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

// The plates standing, from the one list of the plates (data/render/streets.json signs, handed over by DataTables.catalog): the
// post, where the face stands before it, a blade's half thickness, and each type's plate.
void load_plates(const Dictionary &plates, SignDef &signs, PackedStringArray &errors) {
	const Dictionary sizes = plates.get("plates", Dictionary());
	number(plates, "plates", "post_radius_m", signs.post_radius_m, errors);
	number(plates, "plates", "face_out_m", signs.face_out_m, errors);
	number(plates, "plates", "blade_half_m", signs.blade_half_m, errors);
	for (int k = 0; k < SIGN_TYPES; k++) {
		const Dictionary p = sizes.get(SIGN_TYPE_KEYS[k], Dictionary());
		number(p, SIGN_TYPE_KEYS[k], "w_m", signs.plate[k].w_m, errors);
		number(p, SIGN_TYPE_KEYS[k], "h_m", signs.plate[k].h_m, errors);
		number(p, SIGN_TYPE_KEYS[k], "z_m", signs.plate[k].z_m, errors);
	}
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
	const Dictionary names = block(t, "street_names", errors);
	number(names, "street_names", "reach_m", d.street_names.reach_m, errors);
	d.street_names.prefix = String(names.get("prefix", "")).utf8().get_data();
	const Dictionary prefixes = names.get("prefixes", Dictionary());
	const Array keys = prefixes.keys();
	for (int k = 0; k < keys.size(); k++) d.street_names.prefixes[String(keys[k]).utf8().get_data()] = String(prefixes[keys[k]]).utf8().get_data();
	const Array list = names.get("names", Array());
	for (int k = 0; k < list.size(); k++) d.street_names.names.push_back(String(list[k]).utf8().get_data());
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
	load_plates(block(t, "plates", errors), d.signs, errors);
	const Dictionary parts = block(t, "parts", errors);
	for (int k = 0; k < FURNITURE_KINDS; k++) {
		d.parts[k].clear();
		if (!PART_KEYS[k]) continue;
		const Array list = parts.get(PART_KEYS[k], Array());
		if (list.is_empty()) errors.push_back(String("street_furniture.parts: no parts of '") + PART_KEYS[k] + "'");
		for (int i = 0; i < list.size(); i++) {
			const Dictionary p = list[i];
			PartBox b;
			if (!part_box(p, b)) {
				errors.push_back(String("street_furniture.parts.") + PART_KEYS[k] + ": a part of a shape or sizes not known");
				continue;
			}
			b.part = int(p.get("part", 0));
			d.parts[k].push_back(b);
		}
	}
	return errors.is_empty();
}

} // namespace city
