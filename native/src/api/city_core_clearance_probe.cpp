// What the photos cannot settle, measured: how far a car in a lot stays from what is solid along its whole way (lot_clearance), and
// where each vehicle near a point stands on its lane (vehicles_near).
#include "api/city_core.h"

#include "sim/buildings/b40/record.h"
#include "sim/buildings/lot_access.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>
#include <mutex>
#include <shared_mutex>

using namespace godot;

namespace city {

namespace {
constexpr float WAY_STEP_M = 0.25f; // poses looked at along a way
constexpr float LOOK_M = 20.f; // what is solid is read this far round a way's pose
constexpr float FOOT_NO_ROOF_M = 2.f; // what stands over a car's roof is no obstacle
constexpr float HALF = 0.5f, FAR_GAP_M = 1e6f;

using Ring = std::vector<V2>;

float point_segment(const V2 &p, const V2 &a, const V2 &b) {
	const V2 d = b - a;
	const float l2 = d.dot(d), t = l2 > 0 ? std::clamp((p - a).dot(d) / l2, 0.f, 1.f) : 0.f;
	return (p - (a + d * t)).len();
}

bool inside(const Ring &r, const V2 &p) {
	bool in = false;
	for (size_t a = 0, b = r.size() - 1; a < r.size(); b = a++)
		if ((r[a].y > p.y) != (r[b].y > p.y) && p.x < r[a].x + (p.y - r[a].y) * (r[b].x - r[a].x) / (r[b].y - r[a].y)) in = !in;
	return in;
}

// The gap between two convex outlines; negative when one reaches into the other: how far its deepest corner is in.
float gap_between(const Ring &a, const Ring &b) {
	float best = FAR_GAP_M, deepest = 0;
	for (const Ring *r : { &a, &b }) {
		const Ring &other = r == &a ? b : a;
		for (const V2 &p : *r) {
			float edge = FAR_GAP_M;
			for (size_t i = 0, j = other.size() - 1; i < other.size(); j = i++) edge = std::min(edge, point_segment(p, other[i], other[j]));
			if (inside(other, p)) deepest = std::max(deepest, edge);
			else best = std::min(best, edge);
		}
	}
	return deepest > 0 ? -deepest : best;
}

Ring car_box(const OffPose &q, float half_len, float half_w) {
	const V2 f(std::cos(q.angle), std::sin(q.angle)), l(-f.y, f.x), c(q.x, q.z);
	return { c + f * half_len + l * half_w, c + f * half_len - l * half_w, c - f * half_len - l * half_w, c - f * half_len + l * half_w };
}

// What is solid to a car near a point: every lot's volumes and walls, its own included.
void solids_near(const CityState &s, float x, float z, std::vector<Ring> &out, std::vector<int> &kinds) {
	b40::Body body;
	s.buildings.query(x - LOOK_M, z - LOOK_M, x + LOOK_M, z + LOOK_M, [&](int id) {
		const Building &rec = s.buildings.items[size_t(id)];
		b40::record_body(s.buildings.designs, rec, body);
		lot_solids(s.buildings.designs, body, b40::lot_frame(rec), FOOT_NO_ROOF_M, out, &kinds);
	});
}

// The least gap along a car's whole way.
float way_gap(const CityState &s, const LotCar &car, Vector3 &worst, int &kind) {
	const TrafficParams &p = s.catalog.traffic;
	const Vehicle &v = s.traffic.vehicles[size_t(car.id)];
	const float half_len = p.classes[int(v.type)].length * HALF, half_w = p.classes[int(v.type)].half_width;
	std::vector<Ring> solids;
	std::vector<int> kinds;
	const OffPose mid = car.path.pose_at(car.path.length() * HALF);
	solids_near(s, mid.x, mid.z, solids, kinds);
	float best = FAR_GAP_M;
	for (float at = 0; at <= car.path.length(); at += WAY_STEP_M) {
		const OffPose q = car.path.pose_at(at);
		const Ring box = car_box(q, half_len, half_w);
		for (size_t i = 0; i < solids.size(); i++) {
			const float g = gap_between(box, solids[i]);
			if (g < best) {
				best = g;
				worst = Vector3(q.x, 0, q.z);
				kind = kinds[i];
			}
		}
	}
	return best;
}
} // namespace

Dictionary CityCore::lot_clearance() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const CityState &s = *state_;
	Dictionary d;
	int cars = 0, touching = 0;
	float worst = FAR_GAP_M;
	Vector3 where;
	Dictionary by_kind, depth_of_kind;
	Array depths;
	for (const LotCar &car : s.traffic.park.cars) {
		if (car.path.length() <= 0) continue;
		Vector3 at;
		int kind = 0;
		const float g = way_gap(s, car, at, kind);
		cars++;
		touching += g <= 0 ? 1 : 0;
		if (g <= 0) {
			by_kind[kind] = int(by_kind.get(kind, 0)) + 1;
			depth_of_kind[kind] = std::min(float(depth_of_kind.get(kind, 0.f)), g);
			depths.push_back(g);
		}
		if (g < worst) {
			worst = g;
			where = at;
		}
	}
	d["cars"] = cars;
	d["touching"] = touching;
	d["worst_gap_m"] = worst;
	d["touching_by_kind"] = by_kind; // element kind (-1 a volume): how many cars
	d["deepest_by_kind_m"] = depth_of_kind;
	depths.sort();
	d["median_depth_m"] = depths.is_empty() ? Variant(0) : depths[depths.size() / 2];
	d["worst_at"] = where;
	return d;
}

Array CityCore::vehicles_near(const Vector3 &at, float radius) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const CityState &s = *state_;
	const TrafficState &t = s.traffic;
	Array out;
	for (int id : t.micro_list) {
		const Vehicle &v = t.vehicles[size_t(id)];
		if (std::hypot(v.cur.x - at.x, v.cur.z - at.z) > radius) continue;
		Dictionary d;
		d["id"] = id;
		d["type"] = int(v.type);
		d["on_connector"] = v.conn >= 0;
		d["speed"] = v.v;
		if (v.conn < 0 && v.ds >= 0) {
			const float len = s.catalog.traffic.classes[int(v.type)].length;
			Pose c;
			traffic::lane_pose(s, v.ds, v.lane, v.s - len * HALF, c);
			const DirSeg &lane = t.g.ds[size_t(v.ds)];
			const Profile &p = s.roads.profile_of(lane.seg);
			d["lane"] = v.lane;
			d["lane_u"] = lane.u[v.lane]; // across the road, from its middle line; the carriageway runs from -right to +left
			d["asphalt_right"] = p.asph_right;
			d["asphalt_left"] = p.asph_left;
			d["lanes"] = t.g.ds[size_t(v.ds)].lanes;
			d["lc_off"] = v.lc_off;
			d["lateral_m"] = (v.cur.x - c.x) * -c.fz + (v.cur.z - c.z) * c.fx; // of the body's middle from the lane's, to its left
			d["heading_off_rad"] = std::atan2(v.cur.fx * c.fz - v.cur.fz * c.fx, v.cur.fx * c.fx + v.cur.fz * c.fz);
			float ahead = FAR_GAP_M;
			for (int o : t.micro_list) {
				const Vehicle &w = t.vehicles[size_t(o)];
				if (o == id || w.ds != v.ds || w.lane != v.lane || w.conn >= 0 || w.s <= v.s) continue;
				ahead = std::min(ahead, w.s - s.catalog.traffic.classes[int(w.type)].length - v.s);
			}
			d["gap_ahead_m"] = ahead;
		}
		out.push_back(d);
	}
	return out;
}

} // namespace city
