// Strict Dictionary -> StreetFurnitureDef conversion for project/data/catalog/street_furniture.json, through the strict reader
// (api/data_reader.h). The X-macros of sim/catalog/furniture_types.h drive the scalar blocks, so a field added there is
// automatically required here.
#include "api/catalog_loader.h"
#include "render/data_reader.h"

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
const char *const DOC_KEY = "_doc";

void street_names(DataReader r, const Dictionary &names, StreetFurnitureDef &d) {
	d.street_names.reach_m = float(r.num(names, "reach_m"));
	d.street_names.prefix = r.utf8(names, "prefix");
	const Dictionary prefixes = r.table(names, "prefixes"); // profile id or road category -> prefix
	const Array keys = prefixes.keys();
	DataReader rp = r.in("prefixes");
	for (int k = 0; k < keys.size(); k++) {
		const String key = keys[k];
		if (key == DOC_KEY) continue;
		d.street_names.prefixes[key.utf8().get_data()] = rp.utf8(prefixes, key.utf8().get_data());
	}
	const Array list = r.list(names, "names");
	for (int k = 0; k < list.size(); k++) d.street_names.names.push_back(r.in("names").utf8_at(list, k));
}

} // namespace

bool load_street_furniture(const Dictionary &t, StreetFurnitureDef &d, PackedStringArray &errors) {
	const int64_t before = errors.size();
	DataReader r{ errors, "street_furniture.json" };
	const Dictionary sidewalk = r.table(t, "sidewalk"), lamps = r.table(t, "lamps"), signals = r.table(t, "signals");
	const Dictionary hydrants = r.table(t, "hydrants"), furniture = r.table(t, "furniture"), trees = r.table(t, "trees");
	const Dictionary crossing = r.table(t, "crossing"), poles = r.table(t, "poles"), barriers = r.table(t, "barriers");
	const Dictionary signs = r.table(t, "signs"), items = r.table(t, "items");
// Each block's fields by the block's own name: d.<block>.<field> = t.<block>.<field>.
#define CITY_FIELD(block, name) d.block.name = float(r.in(#block).num(block, #name));
#define CITY_SIDEWALK_LOAD(name) CITY_FIELD(sidewalk, name)
	CITY_SIDEWALK_FIELDS(CITY_SIDEWALK_LOAD)
#undef CITY_SIDEWALK_LOAD
#define CITY_LAMP_LOAD(name) CITY_FIELD(lamps, name)
	CITY_LAMP_FIELDS(CITY_LAMP_LOAD)
#undef CITY_LAMP_LOAD
#define CITY_SIGNAL_LOAD(name) CITY_FIELD(signals, name)
	CITY_SIGNAL_FIELDS(CITY_SIGNAL_LOAD)
#undef CITY_SIGNAL_LOAD
#define CITY_HYDRANT_LOAD(name) CITY_FIELD(hydrants, name)
	CITY_HYDRANT_FIELDS(CITY_HYDRANT_LOAD)
#undef CITY_HYDRANT_LOAD
#define CITY_FURNITURE_LOAD(name) CITY_FIELD(furniture, name)
	CITY_FURNITURE_FIELDS(CITY_FURNITURE_LOAD)
#undef CITY_FURNITURE_LOAD
#define CITY_TREE_LOAD(name) CITY_FIELD(trees, name)
	CITY_TREE_FIELDS(CITY_TREE_LOAD)
#undef CITY_TREE_LOAD
#define CITY_CROSSING_LOAD(name) CITY_FIELD(crossing, name)
	CITY_CROSSING_FIELDS(CITY_CROSSING_LOAD)
#undef CITY_CROSSING_LOAD
#define CITY_POLE_LOAD(name) CITY_FIELD(poles, name)
	CITY_POLE_FIELDS(CITY_POLE_LOAD)
#undef CITY_POLE_LOAD
#define CITY_BARRIER_LOAD(name) CITY_FIELD(barriers, name)
	CITY_BARRIER_FIELDS(CITY_BARRIER_LOAD)
#undef CITY_BARRIER_LOAD
#define CITY_SIGN_LOAD(name) CITY_FIELD(signs, name)
	CITY_SIGN_FIELDS(CITY_SIGN_LOAD)
#undef CITY_SIGN_LOAD
#define CITY_ITEM_LOAD(name) CITY_FIELD(items, name)
	CITY_ITEM_FIELDS(CITY_ITEM_LOAD)
#undef CITY_ITEM_LOAD
#undef CITY_FIELD
	street_names(r.in("street_names"), r.table(t, "street_names"), d);
	const Dictionary kinds = r.in("barriers").table(barriers, "kinds");
	DataReader rk = r.in("barriers").in("kinds");
	for (int k = 0; k < BARRIER_KINDS; k++) {
		const Dictionary b = rk.table(kinds, BARRIER_KEYS[k]);
		d.barriers.height_m[k] = float(rk.in(BARRIER_KEYS[k]).num(b, "height_m"));
		d.barriers.thickness_m[k] = float(rk.in(BARRIER_KEYS[k]).num(b, "thickness_m"));
	}
	const Dictionary footprints = r.table(t, "footprints");
	DataReader rf = r.in("footprints");
	for (int k = 0; k < FURNITURE_KINDS; k++) {
		const Array a = rf.list(footprints, FOOTPRINT_KEYS[k], FOOTPRINT_VALUES);
		d.footprint[k] = { float(rf.in(FOOTPRINT_KEYS[k]).num_at(a, 0)), float(rf.in(FOOTPRINT_KEYS[k]).num_at(a, 1)) };
	}
	return errors.size() == before;
}

} // namespace city
