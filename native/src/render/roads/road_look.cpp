// Reads data/render/streets.json into StreetLook through the strict reader (render/data_reader.h): every key, type and list
// length is checked; a marking style takes the base's widths for the keys it leaves out.
#include "render/roads/road_look.h"

#include "core/color.h"
#include "render/data_reader.h"
#include "render/mesh_builder.h"
#include "render/shape_reader.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

namespace city {

namespace {
constexpr int PALETTE = 4;

Color colour(DataReader &r, const Dictionary &d, const char *key) { return rgba8_to_color(r.colour(d, key)); }
float num(DataReader &r, const Dictionary &d, const char *key) { return float(r.num(d, key)); }

void palette(DataReader r, const Array &list, Color (&out)[PALETTE]) {
	for (int i = 0; i < PALETTE && i < list.size(); i++) out[i] = rgba8_to_color(r.colour_at(list, i));
}

void load_colors(StreetLook &L, DataReader &r, const Dictionary &t) {
	palette(r.in("surface_colors"), r.list(t, "surface_colors", PALETTE), L.surface);
	palette(r.in("paving_colors"), r.list(t, "paving_colors", PALETTE), L.paving);
	palette(r.in("median_colors"), r.list(t, "median_colors", PALETTE), L.median);
	static const char *CLASSES[] = { "local", "urban", "avenue", "boulevard", "highway", "ramp", "rail" };
	const Dictionary ac = r.table(t, "asphalt_by_class");
	DataReader ra = r.in("asphalt_by_class");
	for (int i = 0; i < int(RoadClass::COUNT); i++) L.asphalt[i] = colour(ra, ac, CLASSES[i]);
	const Dictionary c = r.table(t, "strip_colors");
	DataReader rc = r.in("strip_colors");
	L.bike = colour(rc, c, "bike");
	L.shoulder = colour(rc, c, "shoulder");
	L.curb = colour(rc, c, "curb");
	L.barrier = colour(rc, c, "barrier");
}

void load_shape(StreetLook &L, DataReader &r, const Dictionary &t) {
	const Dictionary h = r.table(t, "heights");
	DataReader rh = r.in("heights");
	L.h_curb = num(rh, h, "curb");
	L.h_curb_median = num(rh, h, "curb_by_median");
	L.h_median = num(rh, h, "median");
	L.h_barrier = num(rh, h, "barrier");
	L.median_nose = num(r, t, "median_nose_m");
	// (no "verge" table: StreetLook has no band of ground beside the road, the terrain meets the paving itself, ground_seam.h)
	L.surface_error = num(r, t, "surface_error_m");
	L.far_surface_error = num(r, t, "far_surface_error_m");
	if (!(L.surface_error > 0) || !(L.far_surface_error > 0)) r.fail("surface_error_m and far_surface_error_m must be above 0");
	L.far_pull = num(r, t, "far_pull_m");
	L.face_eps = num(r, t, "face_eps_m");
	const Dictionary k = r.table(t, "dropped_kerb");
	DataReader rk = r.in("dropped_kerb");
	L.ramp_flare = num(rk, k, "flare_m");
	L.ramp_low = num(rk, k, "low_m");
	L.ramp_run = num(rk, k, "run_m");
	L.ramp_min_walk = num(rk, k, "min_walk_m");
	const Dictionary j = r.table(t, "junction");
	DataReader rj = r.in("junction");
	L.arc_step = num(rj, j, "arc_step_rad");
	L.bevel_steps = rj.integer(j, "bevel_steps");
	L.bulb_steps = rj.integer(j, "bulb_steps");
}

std::vector<float> floats(DataReader r, const Array &a) {
	std::vector<float> out;
	for (int i = 0; i < a.size(); i++) out.push_back(float(r.num_at(a, i)));
	return out;
}

StreetLook::PaintStyle style_of(DataReader r, const Dictionary &d, DataReader rb, const Dictionary &base) {
	StreetLook::PaintStyle st;
	// a style's own width, else the base's
	auto width = [&](const char *key) { return d.has(key) ? num(r, d, key) : num(rb, base, key); };
	st.centre_w = width("centre_m");
	st.lane_w = width("divider_m");
	st.fast_kmh = width("fast_kmh");
	st.fast_centre_w = width("fast_centre_m");
	st.fast_lane_w = width("fast_divider_m");
	st.exclusive_w = width("exclusive_m");
	st.double_w = width("double_line_m");
	st.double_gap = width("double_gap_m");
	st.zebra_period = width("zebra_period_m");
	st.zebra_bar = width("zebra_bar_m");
	st.zebra_edge = width("zebra_edge_m");
	st.multi_lane_double = d.has("multi_lane_double") ? r.flag(d, "multi_lane_double") : rb.flag(base, "multi_lane_double");
	st.all_white = r.flag(d, "all_white");
	st.dash_kmh = floats(r.in("dash_kmh"), r.list(d, "dash_kmh"));
	st.dash = floats(r.in("dash_m"), r.list(d, "dash_m"));
	st.gap = floats(r.in("gap_m"), r.list(d, "gap_m"));
	if (st.dash.empty() || st.dash.size() != st.gap.size() || st.dash.size() != st.dash_kmh.size() + 1)
		r.fail("dash_m and gap_m need one band more than dash_kmh");
	return st;
}

void load_marks(StreetLook &L, DataReader &r, const Dictionary &t) {
	const Dictionary m = r.table(t, "markings");
	DataReader rm = r.in("markings");
	L.white = colour(rm, m, "white");
	L.yellow = colour(rm, m, "yellow");
	L.paint_lift = num(rm, m, "lift_m");
	const Dictionary base = rm.table(m, "base");
	const Array styles = rm.list(m, "styles");
	L.styles.clear();
	for (int k = 0; k < styles.size(); k++)
		L.styles.push_back(style_of(rm.in("styles").in(k), rm.in("styles").table_at(styles, k), rm.in("base"), base));
	L.parking_line_w = num(rm, m, "mer_line_m");
	L.parking_dash = num(rm, m, "mer_dash_m");
	L.parking_gap = num(rm, m, "mer_gap_m");
	L.no_parking_w = num(rm, m, "lpp_line_m");
	L.no_parking_min = num(rm, m, "lpp_run_min_m");
}
} // namespace

// The emitters' sizes: every block of STREET_SHAPE_BLOCKS, each field under its own name (render/shape_reader.h).
static void street_shapes_of(DataReader r, const Dictionary &t, StreetShapes &out) { STREET_SHAPE_BLOCKS(SHAPE_READ_BLOCK) }

bool StreetLook::load(const Dictionary &t, PackedStringArray &errors) {
	const int64_t before = errors.size();
	DataReader r{ errors, "streets.json" };
	load_colors(*this, r, t);
	load_shape(*this, r, t);
	load_marks(*this, r, t);
	street_shapes_of(r.in("shapes"), r.table(t, "shapes"), shapes);
	return errors.size() == before;
}

} // namespace city
