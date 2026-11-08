// data/render/buildings.json -> BuildingLook (the render's part of the buildings table), through the strict reader
// (render/data_reader.h). The lists are built whole and assigned, so a second boot reads the table again instead of adding to it.
#include "render/buildings/building_look.h"

#include "render/data_reader.h"
#include "render/region_table.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city::b40 {

namespace {
enum { TONE_R, TONE_G, TONE_B, TONE_SIZE }; // finish_far.tones rows: a linear colour

float num(DataReader &r, const Dictionary &d, const char *key) { return float(r.num(d, key)); }
} // namespace

bool BuildingLook::load(const Dictionary &t, PackedStringArray &errors) {
	const int64_t before = errors.size();
	DataReader r{ errors, "buildings.json" };
	const Dictionary lod = r.table(t, "lod");
	DataReader rl = r.in("lod");
	parts_share = num(rl, lod, "parts_share");
	near_share = num(rl, lod, "near_share");
	far_fov_deg = num(rl, lod, "fov_deg");
	far_hysteresis = num(rl, lod, "far_hysteresis");
	far_per_eye = num(rl, lod, "far_per_eye");
	cast_props_m = num(rl, lod, "cast_props_m");
	hull_error_m = num(rl, lod, "hull_error_m");
	level_fade_m = num(rl, lod, "level_fade_m");
	level_back_m = num(rl, lod, "level_back_m");
	far_rebuild_delay_s = num(rl, lod, "far_rebuild_delay_s");
	far_rebuild_interval_s = num(rl, lod, "far_rebuild_interval_s");
	const Dictionary region = region_table(t);
	DataReader rr = r.in("styles").in(DEFAULT_REGION);
	const Dictionary parts = rr.table(region, "parts");
	DataReader rp = rr.in("parts");
	part_frame_bar_m = num(rp, parts, "frame_bar_m");
	part_handle_h_m = num(rp, parts, "handle_h_m");
	part_brise_fin_m = num(rp, parts, "brise_fin_m");
	guard_h_m = num(rp, parts, "guard_h_m");
	DataReader rlot = rr.in("lot"), rroof = rr.in("roof");
	fence_bar_m = num(rlot, rr.table(region, "lot"), "fence_bar_m");
	air_handler_per_area = num(rroof, rr.table(region, "roof"), "air_handler_per_area");
	const Array bands = rl.list(lod, "part_bands");
	std::vector<PartBand> read_bands;
	for (int i = 0; i < bands.size(); i++) {
		DataReader rb = rl.in("part_bands").in(i);
		const Dictionary b = rl.in("part_bands").table_at(bands, i);
		read_bands.push_back(PartBand{ num(rb, b, "thick_max"), num(rb, b, "px_min") });
	}
	if (read_bands.empty()) rl.in("part_bands").fail("no band");
	part_bands = std::move(read_bands);
	DataReader rt = r.in("finish_far").in("tones");
	const Array tones = r.in("finish_far").list(r.table(t, "finish_far"), "tones", FIN_COUNT); // one tone per finish
	std::vector<Lin> read_tones;
	for (int i = 0; i < tones.size(); i++) {
		const Array c = rt.list_at(tones, i, TONE_SIZE);
		DataReader rc = rt.in(i);
		read_tones.push_back(Lin{ float(rc.num_at(c, TONE_R)), float(rc.num_at(c, TONE_G)), float(rc.num_at(c, TONE_B)) });
	}
	finish_far = std::move(read_tones);
	return errors.size() == before;
}

} // namespace city::b40
