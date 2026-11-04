// data/render/buildings.json -> BuildingLook (the render's part of the buildings table).
#include "render/buildings/building_look.h"

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
	far_rebuild_delay_s = float(at(lod, "far_rebuild_delay_s"));
	far_rebuild_interval_s = float(at(lod, "far_rebuild_interval_s"));
	Dictionary parts = at(t, "parts");
	part_frame_bar_m = float(at(parts, "frame_bar_m"));
	part_handle_h_m = float(at(parts, "handle_h_m"));
	part_brise_fin_m = float(at(parts, "brise_fin_m"));
	fence_bar_m = float(at(at(t, "lot"), "fence_bar_m"));
	air_handler_per_area = float(at(at(t, "roof"), "air_handler_per_area"));
	Array bands = at(lod, "part_bands");
	for (int i = 0; i < bands.size(); i++) {
		Dictionary b = bands[i];
		part_bands.push_back(PartBand{ float(at(b, "thick_max")), float(at(b, "px_min")) });
	}
	if (part_bands.empty()) errors.push_back("buildings.json: falta lod.part_bands");
	return errors.is_empty();
}

} // namespace city::b40
