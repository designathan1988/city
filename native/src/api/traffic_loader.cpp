// Strict Dictionary -> TrafficParams conversion for project/data/catalog/traffic.json. The CITY_TRAFFIC_FIELDS X-macro
// drives the scalar section, so a field added to sim/catalog/traffic_params.h is automatically required here.
#include "api/catalog_loader.h"

#include "core/color.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <cstring>

using namespace godot;

namespace city {

namespace {

constexpr int CLASS_FIELDS = 8, LINE_ALPHA_BITS = 8; // RGBA8 to RGB
static_assert(sizeof(VehicleClass) == sizeof(float) * CLASS_FIELDS, "traffic.json classes rows map onto VehicleClass");

bool numbers(const Dictionary &t, const char *key, float *out, int count, PackedStringArray &errors) {
	if (!t.has(key)) {
		errors.push_back(String("traffic: falta '") + key + "'");
		return false;
	}
	Array a = t[key];
	if (a.size() != count) {
		errors.push_back(String("traffic.") + key + ": esperado " + String::num_int64(count) + " valores");
		return false;
	}
	for (int i = 0; i < count; i++) out[i] = float(double(a[i]));
	return true;
}

} // namespace

bool load_traffic_params(const Dictionary &t, TrafficParams &p, PackedStringArray &errors) {
	Dictionary params = t.has("params") ? Dictionary(t["params"]) : Dictionary();
#define CITY_TRAFFIC_LOAD(name) \
	if (!params.has(#name)) errors.push_back("traffic.params: falta '" #name "'"); \
	else p.name = float(double(params[#name]));
	CITY_TRAFFIC_FIELDS(CITY_TRAFFIC_LOAD)
#undef CITY_TRAFFIC_LOAD
	for (const Variant &c : Array(t.get("line_colours", Array()))) p.line_colours.push_back(parse_hex_color(String(c).utf8().get_data()) >> LINE_ALPHA_BITS);
	const Dictionary styles = t.has("styles") ? Dictionary(t["styles"]) : Dictionary();
	const Dictionary style = styles.has(REGIONAL_STYLE) ? Dictionary(styles[REGIONAL_STYLE]) : Dictionary();
	// the lanes are laid out for right-hand traffic (sim/roads/profiles.h): a left-hand style would come out half mirrored
	if (String(style.get("drive_side", "")) != "right") errors.push_back("traffic.styles: drive_side deve ser 'right' por enquanto");
	p.drive_side = 0;
	Array classes = t.has("classes") ? Array(t["classes"]) : Array();
	if (classes.size() != VEHICLE_TYPES) errors.push_back("traffic.classes: esperado uma linha por tipo de veiculo");
	for (int i = 0; i < classes.size() && i < VEHICLE_TYPES; i++) {
		float v[CLASS_FIELDS] = {};
		Dictionary row;
		row["r"] = classes[i];
		if (!numbers(row, "r", v, CLASS_FIELDS, errors)) continue;
		std::memcpy(&p.classes[i], v, sizeof(v));
	}
	numbers(t, "signal_extension_s", p.signal_extension_s, EXTENSION_LANES, errors);
	numbers(t, "style_share", p.style_share, CAR_STYLES, errors);
	numbers(t, "zone_home", p.zone_home, BASE_ZONES, errors);
	numbers(t, "zone_work", p.zone_work, BASE_ZONES, errors);
	numbers(t, "zone_mid", p.zone_mid, BASE_ZONES, errors);
	numbers(t, "zone_truck", p.zone_truck, BASE_ZONES, errors);
	numbers(t, "zone_walk", p.zone_walk, BASE_ZONES, errors);
	numbers(t, "hours_working", p.hours_working, DAY_HOURS, errors);
	numbers(t, "hours_purchasing", p.hours_purchasing, DAY_HOURS, errors);
	return errors.is_empty();
}

} // namespace city
