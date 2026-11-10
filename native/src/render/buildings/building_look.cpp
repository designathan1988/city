// data/render/buildings.json -> BuildingLook (the render's part of the buildings table).
#include "render/buildings/building_look.h"
#include "render/region_table.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace city::b40 {

namespace {
Variant at(const Dictionary &d, const Variant &k) { return d.get(k, Variant()); }
} // namespace

bool BuildingLook::load(const Dictionary &t, PackedStringArray &errors) {
	Dictionary lod = at(t, "lod");
	parts_share = float(at(lod, "parts_share"));
	near_share = float(at(lod, "near_share"));
	far_fov_deg = float(at(lod, "fov_deg"));
	far_hysteresis = float(at(lod, "far_hysteresis"));
	far_per_eye = float(at(lod, "far_per_eye"));
	cast_props_m = float(at(lod, "cast_props_m"));
	hull_error_m = float(at(lod, "hull_error_m"));
	level_fade_m = float(at(lod, "level_fade_m"));
	level_back_m = float(at(lod, "level_back_m"));
	far_rebuild_delay_s = float(at(lod, "far_rebuild_delay_s"));
	far_rebuild_interval_s = float(at(lod, "far_rebuild_interval_s"));
	const Dictionary region = region_table(t);
	Dictionary parts = at(region, "parts");
	part_frame_bar_m = float(at(parts, "frame_bar_m"));
	part_handle_h_m = float(at(parts, "handle_h_m"));
	part_brise_fin_m = float(at(parts, "brise_fin_m"));
	guard_h_m = float(at(parts, "guard_h_m"));
	fence_bar_m = float(at(at(region, "lot"), "fence_bar_m"));
	air_handler_per_area = float(at(at(region, "roof"), "air_handler_per_area"));
	Array bands = at(lod, "part_bands");
	for (int i = 0; i < bands.size(); i++) {
		Dictionary b = bands[i];
		part_bands.push_back(PartBand{ float(at(b, "thick_max")), float(at(b, "px_min")) });
	}
	if (part_bands.empty()) errors.push_back("buildings.json: falta lod.part_bands");
	Array tones = at(at(t, "finish_far"), "tones");
	for (int i = 0; i < tones.size(); i++) {
		Array c = tones[i];
		finish_far.push_back(Lin{ float(c[0]), float(c[1]), float(c[2]) });
	}
	if (int(finish_far.size()) != FIN_COUNT) errors.push_back("buildings.json: finish_far.tones precisa de um tom por acabamento");
	return errors.is_empty();
}

} // namespace city::b40
