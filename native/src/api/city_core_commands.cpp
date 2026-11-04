// Player commands and their previews (sim/commands), as the script API. Commands hold map_mutex exclusively
// (run_edit), the read-only previews shared; the results
// are Dictionaries with `ok`, `error` (strings.csv key) and what changed, so the UI shows the core's own numbers.
#include "api/city_core.h"

#include "api/edit_result.h"
#include "sim/economy/road_economy.h"

#include <godot_cpp/variant/packed_vector3_array.hpp>

#include <algorithm>

using namespace godot;

namespace city {

namespace {
constexpr int PREVIEW_STEPS = 32; // points of the preview polyline (on the ground; the ribbon lifts itself)
constexpr double SNAP_PAD_M = 2; // roadTool.ts: anchor radius max(snap, half width + 2)

double snap_radius(const CityState &s, int profile) {
	return std::max(double(s.catalog.tune.road_snap_m), double(s.catalog.profile(profile).half()) + SNAP_PAD_M);
}

Dictionary anchor_dict(const CityState &s, const Anchor &a) {
	Dictionary d;
	d["position"] = Vector3(float(a.x), s.terrain.height_at(float(a.x), float(a.z)), float(a.z));
	d["node"] = a.node;
	d["seg"] = a.seg;
	return d;
}
} // namespace

Dictionary CityCore::snap_road(const Vector3 &p, int profile) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	if (!s.ready) return Dictionary();
	double r = snap_radius(s, profile);
	return anchor_dict(s, snap_point(s, p.x, p.z, r));
}

Dictionary CityCore::preview_road(const Vector3 &a, const Vector3 &b, bool has_control, const Vector3 &c, int profile) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	Dictionary d;
	if (!s.ready) return d;
	double r = snap_radius(s, profile);
	Anchor sa = snap_point(s, a.x, a.z, r), sb = snap_point(s, b.x, b.z, r);
	RoadPlan p = plan_road(s, sa, has_control, c.x, c.z, sb, profile);
	PackedVector3Array pts;
	for (int i = 0; i <= PREVIEW_STEPS; i++) {
		double x, z;
		p.q.at(double(i) / PREVIEW_STEPS, x, z);
		pts.push_back(Vector3(float(x), s.terrain.height_at(float(x), float(z)), float(z)));
	}
	d["ok"] = p.ok;
	d["error"] = String(p.reason.c_str());
	d["price"] = p.price;
	d["length"] = p.len;
	d["crossings"] = int(p.crossings.size());
	d["half_width"] = s.catalog.profile(profile).half();
	d["affordable"] = charge_of(s, p.price).affordable;
	d["points"] = pts;
	d["start"] = anchor_dict(s, sa);
	d["end"] = anchor_dict(s, sb);
	return d;
}

Dictionary CityCore::build_road(const Vector3 &a, const Vector3 &b, bool has_control, const Vector3 &c, int profile) {
	return run_edit(*state_, "cmd.build.road", [&](CityState &s) {
		double r = snap_radius(s, profile);
		Anchor sa = snap_point(s, a.x, a.z, r), sb = snap_point(s, b.x, b.z, r);
		return city::build_road(s, sa, has_control, c.x, c.z, sb, profile);
	});
}

Dictionary CityCore::demolish(const Vector3 &p) {
	return run_edit(*state_, "cmd.demolish.road", [&](CityState &s) { return demolish_at(s, p.x, p.z); });
}

Dictionary CityCore::paint_zone(const Vector3 &p, float radius, int zone) {
	return run_edit(*state_, "cmd.place.zone", [&](CityState &s) { return city::paint_zone(s, p.x, p.z, radius, zone); });
}

Array CityCore::road_profiles() const {
	Array out;
	const Catalog &c = state_->catalog;
	for (int i = 0; i < int(c.profiles.size()); i++) {
		const Profile &p = c.profiles[i];
		Dictionary d;
		d["index"] = i;
		d["id"] = String(p.id.c_str());
		d["name_key"] = String(p.name_key.c_str());
		d["category"] = String(p.category.c_str());
		d["width"] = p.left + p.right;
		d["cost_per_m"] = p.cost_per_m;
		d["lanes"] = p.lanes_fwd() + p.lanes_back();
		d["one_way"] = p.one_way;
		d["speed"] = p.speed;
		d["ring"] = p.ring;
		d["rail"] = p.rail;
		out.push_back(d);
	}
	return out;
}

} // namespace city
