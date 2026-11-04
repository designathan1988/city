// Reads data/render/streets.json into StreetLook.
#include "render/roads/road_look.h"

#include "core/color.h"
#include "render/mesh_builder.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

namespace city {

namespace {
constexpr int PALETTE = 4;

Color hexc(const Variant &v) { return rgba8_to_color(parse_hex_color(String(v).utf8().get_data())); }

// A required table: reports the key when it is missing so the caller fails loudly.
Dictionary sub(const Dictionary &t, const char *key, PackedStringArray &errors) {
	if (!t.has(key)) errors.push_back(String("streets.json: falta ") + key);
	return t.get(key, Dictionary());
}

void load_colors(StreetLook &L, const Dictionary &t, PackedStringArray &errors) {
	Array sc = t.get("surface_colors", Array()), pc = t.get("paving_colors", Array()), mc = t.get("median_colors", Array());
	if (sc.size() < PALETTE || pc.size() < PALETTE || mc.size() < PALETTE) {
		errors.push_back("streets.json: paletas de superfície incompletas");
		return;
	}
	for (int i = 0; i < PALETTE; i++) {
		L.surface[i] = hexc(sc[i]);
		L.paving[i] = hexc(pc[i]);
		L.median[i] = hexc(mc[i]);
	}
	static const char *CLASSES[] = { "local", "urban", "avenue", "boulevard", "highway", "ramp", "rail" };
	Dictionary ac = sub(t, "asphalt_by_class", errors);
	for (int i = 0; i < int(RoadClass::COUNT); i++) L.asphalt[i] = hexc(ac[CLASSES[i]]);
	Dictionary c = sub(t, "strip_colors", errors);
	L.bike = hexc(c["bike"]);
	L.shoulder = hexc(c["shoulder"]);
	L.curb = hexc(c["curb"]);
	L.barrier = hexc(c["barrier"]);
}

void load_shape(StreetLook &L, const Dictionary &t, PackedStringArray &errors) {
	Dictionary h = sub(t, "heights", errors);
	L.h_curb = float(h["curb"]);
	L.h_curb_median = float(h["curb_by_median"]);
	L.h_sidewalk = float(h["sidewalk"]);
	L.h_median = float(h["median"]);
	L.h_barrier = float(h["barrier"]);
	L.skirt = float(h["skirt"]);
	L.median_nose = float(t["median_nose_m"]);
	Dictionary v = sub(t, "verge", errors);
	L.verge_reach = float(v["reach_m"]);
	L.verge_under = float(v["under_m"]);
	L.verge_strips = int(v["strips"]);
	L.step = float(t["sample_step_m"]);
	L.face_eps = float(t["face_eps_m"]);
	Dictionary r = sub(t, "dropped_kerb", errors);
	L.ramp_flare = float(r["flare_m"]);
	L.ramp_low = float(r["low_m"]);
	L.ramp_run = float(r["run_m"]);
	L.ramp_min_walk = float(r["min_walk_m"]);
	Dictionary j = sub(t, "junction", errors);
	L.arc_step = float(j["arc_step_rad"]);
	L.bevel_steps = int(j["bevel_steps"]);
	L.bulb_steps = int(j["bulb_steps"]);
}

void load_marks(StreetLook &L, const Dictionary &t, PackedStringArray &errors) {
	Dictionary m = sub(t, "markings", errors);
	L.white = hexc(m["white"]);
	L.yellow = hexc(m["yellow"]);
	L.paint_lift = float(m["lift_m"]);
	Dictionary base = sub(m, "base", errors);
	Array styles = m["styles"];
	L.styles.clear();
	for (int k = 0; k < styles.size(); k++) {
		Dictionary d = styles[k];
		StreetLook::PaintStyle st;
		auto num = [&](const char *key) { return float(d.has(key) ? d[key] : base[key]); }; // a style's own width, else the base's
		st.centre_w = num("centre_m");
		st.lane_w = num("divider_m");
		st.fast_kmh = num("fast_kmh");
		st.fast_centre_w = num("fast_centre_m");
		st.fast_lane_w = num("fast_divider_m");
		st.exclusive_w = num("exclusive_m");
		st.double_w = num("double_line_m");
		st.double_gap = num("double_gap_m");
		st.zebra_period = num("zebra_period_m");
		st.zebra_bar = num("zebra_bar_m");
		st.zebra_edge = num("zebra_edge_m");
		st.multi_lane_double = bool(d.has("multi_lane_double") ? d["multi_lane_double"] : base["multi_lane_double"]);
		st.all_white = bool(d["all_white"]);
		auto floats = [&](const char *key, std::vector<float> &out) {
			Array a = d[key];
			for (int i = 0; i < a.size(); i++) out.push_back(float(a[i]));
		};
		floats("dash_kmh", st.dash_kmh);
		floats("dash_m", st.dash);
		floats("gap_m", st.gap);
		if (st.dash.empty() || st.dash.size() != st.gap.size() || st.dash.size() != st.dash_kmh.size() + 1)
			errors.push_back("streets.json markings.styles: dash_m e gap_m com uma faixa a mais que dash_kmh");
		L.styles.push_back(st);
	}
	L.parking_line_w = float(m["mer_line_m"]);
	L.parking_dash = float(m["mer_dash_m"]);
	L.parking_gap = float(m["mer_gap_m"]);
	L.no_parking_w = float(m["lpp_line_m"]);
	L.no_parking_min = float(m["lpp_run_min_m"]);
}
} // namespace

bool StreetLook::load(const Dictionary &t, PackedStringArray &errors) {
	load_colors(*this, t, errors);
	load_shape(*this, t, errors);
	load_marks(*this, t, errors);
	return errors.is_empty();
}

} // namespace city
