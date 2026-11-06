// What the Build tab's script commands share: a point of the map in the lot's frame, and an edit of a body as one step of undo.
#pragma once

#include "api/city_builder.h" // the Variant types, which edit_result.h takes as read
#include "api/edit_result.h"
#include "sim/buildings/b40/record.h"
#include "sim/commands/body_ops.h"
#include "sim/state/city_state.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace city {

constexpr int SIZE_AXES = 3;

// A card of the strip as what it sets.
inline PartSpec part_spec(const godot::Dictionary &d) {
	PartSpec s;
	godot::Array size = d.get("size", godot::Array());
	s.kind = int(d.get("value", 0));
	s.fit = int(d.get("fit_kind", 0));
	if (size.size() == SIZE_AXES) {
		s.w = float(size[0]);
		s.d = float(size[1]);
		s.h = float(size[2]);
	}
	s.out = float(d.get("standoff_m", 0.0));
	s.rise = float(d.get("rise_m", 0.0));
	s.drop = float(d.get("drop_m", 0.0));
	return s;
}

inline PrimitiveSpec primitive_of(const godot::Dictionary &spec) { return { int(spec.get("value", 0)), int(spec.get("top_kind", 0)), float(spec.get("top_deg", 0.0)) }; }

// The body of a building that stands; false when there is none by that id.
inline bool body_of_record(const CityState &s, int id, b40::Body &body) {
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[id].alive) return false;
	b40::record_body(s.buildings.designs, s.buildings.items[id], body);
	return true;
}

inline V2 lot_point(const CityState &s, int id, const godot::Vector3 &at) {
	if (id < 0 || id >= int(s.buildings.items.size())) return V2(0, 0);
	b40::LotFrame f = b40::lot_frame(s.buildings.items[id]);
	V2 rel = V2(at.x, at.z) - f.o;
	return V2(rel.x * f.e0.x + rel.y * f.e0.y, rel.x * f.e1.x + rel.y * f.e1.y);
}

template <typename F>
godot::Dictionary edit_of_body(CityState &state, int id, F &&op) {
	return run_edit(state, "cmd.place.editBuilding", [&](CityState &s) {
		return edit_body(s, id, [&](b40::Body &b) { return op(b, s.catalog.tune); });
	});
}

} // namespace city
