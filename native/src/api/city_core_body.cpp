// A building's body for the Build tools: what it holds, and its edits (sim/commands/body_ops.h) as script commands.
#include "api/city_core.h"
#include "api/edit_result.h"
#include "sim/buildings/b40/record.h"
#include "sim/commands/body_ops.h"
#include "sim/persistence/body_codec.h"

#include <godot_cpp/variant/array.hpp>

using namespace godot;

namespace city {

namespace {
constexpr const char *EDIT_LABEL = "cmd.place.editBuilding";

template <typename F>
Dictionary edit(CityState &state, int id, F &&op) {
	return run_edit(state, EDIT_LABEL, [&](CityState &s) {
		return edit_body(s, id, [&](b40::Body &b) { return op(b, s.catalog.tune); });
	});
}
} // namespace

Dictionary CityCore::body_info(int id) const {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Dictionary d;
	const CityState &s = *state_;
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[id].alive) return d;
	const Building &rec = s.buildings.items[id];
	b40::Body body;
	b40::record_body(s.buildings.designs, rec, body);
	d["edited"] = rec.body != 0;
	d["digest"] = int64_t(body_digest(body));
	d["problem"] = String(body_problem(s.catalog.tune, body).c_str());
	d["ground_h"] = body.ground_h;
	d["storey_h"] = body.storey_h;
	Array volumes;
	for (const b40::Volume &v : body.volumes) {
		Dictionary e;
		e["x"] = v.x;
		e["y"] = v.y;
		e["w"] = v.w;
		e["d"] = v.d;
		e["base"] = v.base;
		e["storeys"] = v.storeys;
		e["roof"] = int(v.roof);
		volumes.push_back(e);
	}
	d["volumes"] = volumes;
	return d;
}

Dictionary CityCore::body_storeys(int id, int vol, int count) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_set_storeys(b, t, vol, count); });
}

Dictionary CityCore::body_insert_storey(int id, int vol, int at, bool above) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_insert_storey(b, t, vol, at, above); });
}

Dictionary CityCore::body_resize(int id, int vol, int side, double delta, bool snap) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_resize(b, t, vol, side, float(delta), snap); });
}

Dictionary CityCore::body_add_wing(int id, int vol, int side, double depth, double length) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_add_wing(b, t, vol, side, float(depth), float(length)); });
}

Dictionary CityCore::body_add_setback(int id, int vol, double inset, int storeys) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_add_setback(b, t, vol, float(inset), storeys); });
}

Dictionary CityCore::body_split(int id, int vol, int level) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_split_at_floor(b, t, vol, level); });
}

Dictionary CityCore::body_remove_volume(int id, int vol) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_remove_volume(b, t, vol); });
}

Dictionary CityCore::body_heights(int id, double ground_h, double storey_h) {
	return edit(*state_, id, [&](b40::Body &b, const Tuning &t) { return op_set_heights(b, t, float(ground_h), float(storey_h)); });
}

} // namespace city
