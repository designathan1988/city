// Strict Dictionary -> TrafficParams conversion for project/data/catalog/traffic.json, through the strict reader
// (api/data_reader.h). The CITY_TRAFFIC_FIELDS X-macro drives the scalar section, so a field added to
// sim/catalog/traffic_params.h is automatically required here.
#include "api/catalog_loader.h"
#include "render/data_reader.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <cstring>

using namespace godot;

namespace city {

namespace {

constexpr int CLASS_FIELDS = 8, LINE_ALPHA_BITS = 8; // RGBA8 to RGB
static_assert(sizeof(VehicleClass) == sizeof(float) * CLASS_FIELDS, "traffic.json classes rows map onto VehicleClass");
const char *const RIGHT_HAND = "right";

// `count` numbers from a list of exactly that many.
void numbers(DataReader r, const Array &a, float *out, int count) {
	r.expect(a, count);
	for (int i = 0; i < count && i < a.size(); i++) out[i] = float(r.num_at(a, i));
}

void numbers(DataReader &r, const Dictionary &t, const char *key, float *out, int count) { numbers(r.in(key), r.list(t, key), out, count); }

} // namespace

bool load_traffic_params(const Dictionary &t, TrafficParams &p, PackedStringArray &errors) {
	const int64_t before = errors.size();
	DataReader r{ errors, "traffic.json" };
	const Dictionary params = r.table(t, "params");
#define CITY_TRAFFIC_LOAD(name) p.name = float(r.in("params").num(params, #name));
	CITY_TRAFFIC_FIELDS(CITY_TRAFFIC_LOAD)
#undef CITY_TRAFFIC_LOAD
	const Array colours = r.list(t, "line_colours");
	for (int i = 0; i < colours.size(); i++) p.line_colours.push_back(r.in("line_colours").colour_at(colours, i) >> LINE_ALPHA_BITS);
	const Dictionary style = r.in("styles").table(r.table(t, "styles"), REGIONAL_STYLE);
	// the lanes are laid out for right-hand traffic (sim/roads/profiles.h): a left-hand style would come out half mirrored
	const String side = r.in("styles").in(REGIONAL_STYLE).text(style, "drive_side");
	if (side != RIGHT_HAND) r.in("styles").in(REGIONAL_STYLE).fail(String("drive_side: only 'right' is laid out, found '") + side + String("'"));
	p.drive_side = 0;
	const Array classes = r.list(t, "classes", VEHICLE_TYPES);
	for (int i = 0; i < classes.size() && i < VEHICLE_TYPES; i++) {
		float v[CLASS_FIELDS] = {};
		numbers(r.in("classes").in(i), r.in("classes").list_at(classes, i), v, CLASS_FIELDS);
		std::memcpy(&p.classes[i], v, sizeof(v));
	}
	numbers(r, t, "signal_extension_s", p.signal_extension_s, EXTENSION_LANES);
	numbers(r, t, "style_share", p.style_share, CAR_STYLES);
	numbers(r, t, "zone_home", p.zone_home, BASE_ZONES);
	numbers(r, t, "zone_work", p.zone_work, BASE_ZONES);
	numbers(r, t, "zone_mid", p.zone_mid, BASE_ZONES);
	numbers(r, t, "zone_truck", p.zone_truck, BASE_ZONES);
	numbers(r, t, "people_kind_m", p.people_kind_m, PLACE_KINDS);
	numbers(r, t, "people_busy", p.people_busy, PLACE_KINDS * HOURS);
	numbers(r, t, "people_working", p.people_working, HOURS);
	numbers(r, t, "people_purchasing", p.people_purchasing, HOURS);
	numbers(r, t, "zone_kinds", p.zone_kinds, BASE_ZONES * PLACE_KINDS);
	return errors.size() == before;
}

} // namespace city
