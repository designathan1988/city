// Undo / redo, strokes and the drawer edits (split, upgrade, roundabout, relief brush, drawn buildings) as the script
// API. Each edit is one undo entry (a stroke between begin_stroke and end_stroke is one entry).
#include "api/city_core.h"

#include "api/edit_result.h"
#include "sim/roads/road_network.h"

#include <godot_cpp/variant/packed_vector3_array.hpp>

#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr int SEG_PREVIEW_STEPS = 24;
} // namespace

bool CityCore::undo() {
	std::unique_lock<std::shared_mutex> lk(state_->map_mutex);
	return state_->ready && state_->history.undo(*state_);
}

bool CityCore::redo() {
	std::unique_lock<std::shared_mutex> lk(state_->map_mutex);
	return state_->ready && state_->history.redo(*state_);
}

void CityCore::begin_stroke(const String &label) {
	std::unique_lock<std::shared_mutex> lk(state_->map_mutex);
	state_->history.begin_group(label.utf8().get_data());
}

void CityCore::end_stroke() {
	std::unique_lock<std::shared_mutex> lk(state_->map_mutex);
	state_->history.end_group(*state_);
}

Dictionary CityCore::split_road(const Vector3 &p) {
	return run_edit(*state_, "cmd.place.splitRoad", [&](CityState &s) { return split_road_at(s, p.x, p.z); });
}

Dictionary CityCore::set_road_end(int node, bool bulb) {
	return run_edit(*state_, "cmd.build.roadEnd", [&](CityState &s) { return city::set_road_end(s, node, bulb); });
}

Dictionary CityCore::upgrade_road(const Vector3 &p, int profile, bool whole_street) {
	const char *label = whole_street ? "cmd.build.changeStreetProfile" : "cmd.build.changeProfile";
	return run_edit(*state_, label, [&](CityState &s) { return change_profile_at(s, p.x, p.z, profile, whole_street); });
}

Dictionary CityCore::roundabout(const Vector3 &p, float radius, bool cut) {
	return run_edit(*state_, "cmd.build.roundabout", [&](CityState &s) {
		double r = s.catalog.tune.road_snap_m;
		Anchor a = snap_point(s, p.x, p.z, r);
		return build_roundabout(s, a.x, a.z, radius, cut || a.node >= 0);
	});
}

Dictionary CityCore::preview_roundabout(const Vector3 &p, float radius, bool cut) {
	std::unique_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	Dictionary d;
	if (!s.ready) return d;
	Anchor a = snap_point(s, p.x, p.z, s.catalog.tune.road_snap_m);
	std::string why = roundabout_refusal(s, a.x, a.z, radius, cut || a.node >= 0);
	d["ok"] = why.empty();
	d["error"] = String(why.c_str());
	d["centre"] = Vector3(float(a.x), s.terrain.height_at(float(a.x), float(a.z)), float(a.z));
	return d;
}

Dictionary CityCore::segment_at(const Vector3 &p) {
	std::unique_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	Dictionary d;
	if (!s.ready) return d;
	double t, dist;
	int seg = s.roads.find_segment_near(p.x, p.z, s.catalog.tune.road_pick_m, t, dist);
	d["seg"] = seg;
	if (seg < 0) return d;
	const RoadSegment &g = s.roads.segs[seg];
	PackedVector3Array pts;
	for (int i = 0; i <= SEG_PREVIEW_STEPS; i++) {
		double x, z;
		g.q.at(double(i) / SEG_PREVIEW_STEPS, x, z);
		pts.push_back(Vector3(float(x), float(s.roads.seg_y(seg, g.len * i / SEG_PREVIEW_STEPS)), float(z)));
	}
	double px, pz;
	g.q.at(t, px, pz);
	d["points"] = pts;
	d["t"] = t;
	d["at"] = Vector3(float(px), s.terrain.height_at(float(px), float(pz)), float(pz));
	d["profile"] = g.profile;
	d["half_width"] = s.roads.profile_of(seg).half();
	d["length"] = g.len;
	d["ring"] = s.roads.profile_of(seg).ring;
	return d;
}

Dictionary CityCore::terrain_brush(const Vector3 &p, float radius, float strength, float dt, int mode) {
	return run_edit(*state_, "cmd.place.terrainBrush", [&](CityState &s) { return terrain_dab(s, p.x, p.z, radius, strength, dt, mode); });
}

Dictionary CityCore::draw_building(const Vector3 &a, const Vector3 &b, int fn, int zone) {
	return run_edit(*state_, "cmd.build.lotDrawn", [&](CityState &s) { return place_building(s, a.x, a.z, b.x, b.z, fn, zone); });
}

Dictionary CityCore::preview_lot(const Vector3 &a, const Vector3 &b) {
	std::unique_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	Dictionary d;
	if (!s.ready) return d;
	Building lot;
	std::string why = lot_frame(s, a.x, a.z, b.x, b.z, lot);
	float c = std::cos(lot.rot), sn = std::sin(lot.rot), h = lot.d / 2;
	PackedVector3Array pts;
	for (float side : { -h, h }) {
		float x = lot.x + side * sn, z = lot.z + side * c;
		pts.push_back(Vector3(x, s.terrain.height_at(x, z), z));
	}
	d["ok"] = why.empty();
	d["error"] = String(why.c_str());
	d["points"] = pts;
	d["half_width"] = lot.w / 2;
	return d;
}

Dictionary CityCore::set_building(int id, int fn, int floors) {
	return run_edit(*state_, "cmd.place.editBuilding", [&](CityState &s) { return edit_building(s, id, fn, floors); });
}

} // namespace city
