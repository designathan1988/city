// Player commands and their previews (sim/commands), as the script API. Commands hold map_mutex exclusively
// (run_edit), the read-only previews shared; the results
// are Dictionaries with `ok`, `error` (strings.csv key) and what changed, so the UI shows the core's own numbers.
#include "api/city_core.h"

#include "api/edit_result.h"
#include "sim/roads/road_variants.h"

#include <godot_cpp/variant/packed_vector3_array.hpp>

#include <algorithm>

using namespace godot;

namespace city {

namespace {
constexpr int PREVIEW_STEPS = 32; // points of each piece of the preview polyline (on the ground; the ribbon lifts itself)
constexpr double RAD_TO_DEG = 180 / 3.14159265358979323846;
constexpr int PIECE_POINTS = 3; // a piece crosses the API as its start, control and end

Dictionary anchor_dict(const CityState &s, const Anchor &a) {
	Dictionary d;
	d["position"] = Vector3(float(a.x), s.terrain.height_at(float(a.x), float(a.z)), float(a.z));
	d["node"] = a.node;
	d["seg"] = a.seg;
	return d;
}

const char *guide_name(SnapGuide g) {
	switch (g) {
		case SnapGuide::NETWORK: return "network";
		case SnapGuide::CONTINUE: return "continue";
		case SnapGuide::PERPENDICULAR: return "perpendicular";
		case SnapGuide::ORTHOGONAL: return "orthogonal";
		case SnapGuide::ANGLE: return "angle";
		default: return "";
	}
}

std::vector<Quad> quads_of(const PackedVector3Array &p) {
	std::vector<Quad> out;
	for (int i = 0; i + PIECE_POINTS <= p.size(); i += PIECE_POINTS)
		out.push_back({ p[i].x, p[i].z, p[i + 1].x, p[i + 1].z, p[i + 2].x, p[i + 2].z });
	return out;
}

Dictionary plan_dict(const CityState &s, const std::vector<Quad> &pieces, const RoadPlan &p, const Anchor &a, const Anchor &b, int profile) {
	Dictionary d;
	PackedVector3Array pts;
	for (const Quad &q : pieces)
		for (int i = pts.is_empty() ? 0 : 1; i <= PREVIEW_STEPS; i++) {
			double x, z;
			q.at(double(i) / PREVIEW_STEPS, x, z);
			pts.push_back(Vector3(float(x), s.terrain.height_at(float(x), float(z)), float(z)));
		}
	d["ok"] = p.ok;
	d["error"] = String(p.reason.c_str());
	d["price"] = p.price;
	d["length"] = p.len;
	d["crossings"] = int(p.crossings.size());
	const Profile &pf = s.catalog.profile(profile);
	d["half_width"] = pf.half();
	d["asphalt_half"] = std::max(pf.asph_left, pf.asph_right);
	d["affordable"] = s.unlimited_money || p.price <= s.money;
	d["points"] = pts;
	d["start"] = anchor_dict(s, a);
	d["end"] = anchor_dict(s, b);
	return d;
}
} // namespace

Dictionary CityCore::road_start(const Vector3 &p, double px, bool on, bool grid) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	if (!s.ready) return Dictionary();
	SnapOptions o;
	o.on = on;
	o.grid = grid;
	o.px = px;
	return anchor_dict(s, snap_road_start(s, find_anchor(s, p.x, p.z, px), o));
}

Dictionary CityCore::road_end(const Vector3 &start, const Vector3 &p, double px, bool on, bool angles, bool grid) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	if (!s.ready) return Dictionary();
	SnapOptions o{ on, angles, grid, px };
	SnappedEnd e = snap_road_end(s, find_anchor(s, start.x, start.z, px), p.x, p.z, o);
	Dictionary d;
	d["position"] = Vector3(float(e.x), s.terrain.height_at(float(e.x), float(e.z)), float(e.z));
	d["guide"] = String(guide_name(e.guide));
	d["angle_deg"] = e.angle * RAD_TO_DEG;
	d["length"] = e.length;
	return d;
}

PackedVector3Array CityCore::road_pieces(const Vector3 &a, const Vector3 &b, int alignment, bool has_control, const Vector3 &c,
		const PackedVector3Array &samples, int profile, double px) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	PackedVector3Array out;
	if (!s.ready) return out;
	RoadStroke k;
	k.alignment = RoadAlignment(alignment);
	k.start = { a.x, a.z };
	k.end = { b.x, b.z };
	k.control = { c.x, c.z };
	k.has_control = has_control;
	k.profile = profile;
	k.flat_m = s.catalog.tune.road_click_px * px;
	for (int i = 0; i < samples.size(); i++) k.samples.push_back({ samples[i].x, samples[i].z });
	for (const Quad &q : city::road_pieces(s, k)) {
		out.push_back(Vector3(float(q.x0), 0, float(q.z0)));
		out.push_back(Vector3(float(q.cx), 0, float(q.cz)));
		out.push_back(Vector3(float(q.x1), 0, float(q.z1)));
	}
	return out;
}

Dictionary CityCore::preview_road_path(const PackedVector3Array &pieces, int profile, double px) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	std::vector<Quad> q = quads_of(pieces);
	if (!s.ready || q.empty()) return Dictionary();
	Anchor a = find_anchor(s, q.front().x0, q.front().z0, px), b = find_anchor(s, q.back().x1, q.back().z1, px);
	return plan_dict(s, q, plan_path(s, q, a, b, profile), a, b, profile);
}

Dictionary CityCore::build_road_path(const PackedVector3Array &pieces, int profile, double px) {
	return run_edit(*state_, "cmd.build.road", [&](CityState &s) {
		std::vector<Quad> q = quads_of(pieces);
		if (q.empty()) {
			EditResult none;
			none.error = "refuse.road.tooShort";
			return none;
		}
		Anchor a = find_anchor(s, q.front().x0, q.front().z0, px), b = find_anchor(s, q.back().x1, q.back().z1, px);
		return build_path(s, q, a, b, profile);
	});
}

Dictionary CityCore::preview_road(const Vector3 &a, const Vector3 &b, bool has_control, const Vector3 &c, int profile, double px) {
	PackedVector3Array one;
	one.push_back(a);
	one.push_back(has_control ? c : (a + b) / 2);
	one.push_back(b);
	return preview_road_path(one, profile, px);
}

Dictionary CityCore::build_road(const Vector3 &a, const Vector3 &b, bool has_control, const Vector3 &c, int profile, double px) {
	PackedVector3Array one;
	one.push_back(a);
	one.push_back(has_control ? c : (a + b) / 2);
	one.push_back(b);
	return build_road_path(one, profile, px);
}

Dictionary CityCore::demolish(const Vector3 &p) {
	return run_edit(*state_, "cmd.demolish.road", [&](CityState &s) { return demolish_at(s, p.x, p.z); });
}

Dictionary CityCore::paint_zone(const Vector3 &p, float radius, int zone) {
	return run_edit(*state_, "cmd.place.zone", [&](CityState &s) { return city::paint_zone(s, p.x, p.z, radius, zone); });
}

int CityCore::road_variant(int base, int lanes, float width, int parking) {
	std::unique_lock<std::shared_mutex> lk(state_->map_mutex);
	RoadVariant v;
	v.base = base;
	v.lanes = lanes;
	v.width = width;
	v.parking = parking;
	return city::road_variant(state_->catalog, v);
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
		d["rank"] = p.rank;
		d["class_card"] = p.class_card;
		d["road_class"] = int(p.road_class);
		d["painted"] = !p.lines.empty();
		d["yellow"] = p.centre != "none" && p.road_class == RoadClass::URBAN;
		d["centre"] = String(p.centre.c_str());
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
