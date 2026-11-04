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
	Dictionary c = sub(t, "strip_colors", errors);
	L.parking = hexc(c["parking"]);
	L.bike = hexc(c["bike"]);
	L.shoulder = hexc(c["shoulder"]);
	L.curb = hexc(c["curb"]);
	L.barrier = hexc(c["barrier"]);
	L.gutter = hexc(c["gutter"]);
}

void load_shape(StreetLook &L, const Dictionary &t, PackedStringArray &errors) {
	Dictionary h = sub(t, "heights", errors);
	L.h_curb = float(h["curb"]);
	L.h_curb_median = float(h["curb_by_median"]);
	L.h_sidewalk = float(h["sidewalk"]);
	L.h_median = float(h["median"]);
	L.h_barrier = float(h["barrier"]);
	L.skirt = float(h["skirt"]);
	L.step = float(t["sample_step_m"]);
	L.face_eps = float(t["face_eps_m"]);
	L.gutter_w = float(t["gutter_width_m"]);
	Dictionary r = sub(t, "dropped_kerb", errors);
	L.ramp_full = float(r["full_m"]);
	L.ramp_flat = float(r["flat_m"]);
	L.ramp_flare = float(r["flare_m"]);
	L.ramp_low = float(r["low_m"]);
	L.ramp_run = float(r["run_m"]);
	L.ramp_min_walk = float(r["min_walk_m"]);
	Dictionary j = sub(t, "junction", errors);
	L.arc_step = float(j["arc_step_rad"]);
	L.bevel_steps = int(j["bevel_steps"]);
	L.bulb_steps = int(j["bulb_steps"]);
	L.dead_end_depth = float(j["dead_end_depth_m"]);
}

void load_marks(StreetLook &L, const Dictionary &t, PackedStringArray &errors) {
	Dictionary m = sub(t, "markings", errors);
	L.white = hexc(m["white"]);
	L.yellow = hexc(m["yellow"]);
	L.paint_lift = float(m["lift_m"]);
	L.line_clear = float(m["line_clear_m"]);
	L.dash_period = float(m["dash_period_m"]);
	L.dash_duty = float(m["dash_duty"]);
	L.zebra_period = float(m["zebra_period_m"]);
	L.zebra_bar = float(m["zebra_bar_m"]);
	L.double_gap = float(m["double_gap_m"]);
}
} // namespace

bool StreetLook::load(const Dictionary &t, PackedStringArray &errors) {
	load_colors(*this, t, errors);
	load_shape(*this, t, errors);
	load_marks(*this, t, errors);
	return errors.is_empty();
}

} // namespace city
