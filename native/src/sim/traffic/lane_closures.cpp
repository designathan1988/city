// Stretches of lane closed by something off the lanes standing on them (lane_closures.h).
#include "sim/traffic/lane_closures.h"

#include "sim/street/footway.h"
#include "sim/traffic/lane_graph_internal.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>
#include <map>

namespace city {
namespace traffic {

namespace {
constexpr float BESIDE_M = 0.05f; // a point this near a segment's end along it still stands beside it

// Each lane under point (x, z), with the metre along it: the lanes of any segment the point stands beside, between its mouths,
// within half the lane's width of the lane's line.
template <typename F>
void lanes_under(const CityState &s, float x, float z, F &&take) {
	const LaneGraph &g = s.traffic.g;
	const float r = s.roads.cat->widest_half();
	s.roads.for_segments_near(x - r, z - r, x + r, z + r, [&](int seg) {
		if (size_t(seg) * 2 + 1 >= g.ds.size()) return;
		double arc;
		float across, past;
		segment_place(s.roads, seg, x, z, arc, across, past);
		if (past > BESIDE_M) return;
		const Profile &pr = s.roads.profile_of(seg);
		for (int dir = 0; dir < 2; dir++) {
			const DirSeg &d = g.ds[size_t(seg) * 2 + dir];
			if (!d.ok) continue;
			const float at = (float(arc) - d.at0) * d.sign;
			if (at < 0 || at > d.len) continue;
			const std::vector<LaneInfo> &lanes = dir == 0 ? pr.fwd : pr.back;
			for (int lane = 0; lane < d.lanes && lane < int(lanes.size()); lane++)
				if (std::fabs(across - d.u[lane]) <= lanes[size_t(lane)].w * 0.5f) take((seg * 2 + dir) * MAX_LANES + lane, at);
		}
	});
}

// The points of a body (middle, heading, half length and width) that find the lanes under it: its corners and the middles of
// its long sides.
template <typename F>
void body_points(const OffPose &q, float half_len, float half_w, F &&take) {
	const V2 f = v2_from_angle(q.angle), l(-f.y, f.x), m(q.x, q.z);
	for (const float a : { -half_len, 0.f, half_len })
		for (const float b : { -half_w, half_w }) take(m + f * a + l * b);
}
} // namespace

void sweep_body(const CityState &s, float half_len, float half_w, const std::vector<OffPose> &poses, float margin, int owner, std::vector<LaneClosure> &out) {
	std::map<int, std::pair<float, float>> spans;
	for (const OffPose &q : poses)
		body_points(q, half_len, half_w, [&](const V2 &p) {
			lanes_under(s, p.x, p.y, [&](int key, float at) {
				auto hit = spans.emplace(key, std::make_pair(at, at));
				hit.first->second.first = std::min(hit.first->second.first, at);
				hit.first->second.second = std::max(hit.first->second.second, at);
			});
		});
	for (const auto &kv : spans) out.push_back({ kv.first, kv.second.first - margin, kv.second.second + margin, owner });
}

bool on_lanes(const CityState &s, float half_len, float half_w, const OffPose &q) {
	bool on = false;
	body_points(q, half_len, half_w, [&](const V2 &p) {
		if (!on) lanes_under(s, p.x, p.y, [&](int, float) { on = true; });
	});
	return on;
}

// The stretches of lane a lorry's swing sweeps: its body at every other axle pose of the stretch.
void sweep_lanes(const CityState &s, const std::vector<AxlePose> &poses, int from, int to, int owner, std::vector<LaneClosure> &out) {
	const TrafficParams &p = s.catalog.traffic;
	const VehicleClass &c = p.classes[int(VehicleType::TRUCK)];
	std::vector<OffPose> bodies;
	for (int i = from; i <= to && i < int(poses.size()); i += 2) {
		const AxlePose &q = poses[size_t(i)];
		bodies.push_back({ float(q.x + std::cos(q.angle) * p.lorry_rear_m), float(q.z + std::sin(q.angle) * p.lorry_rear_m), float(q.angle) });
	}
	sweep_body(s, c.length * 0.5f, c.half_width, bodies, p.lorry_closure_margin_m, owner, out);
}

// Nobody on a stretch or too near to stop short of it: no other body off the lanes holding it (a stretch it closed: that body
// drives over it on no lane list), the near vehicles on its lane, and those on the connectors that end on it (all at the node its
// lane starts from) or committed to one. A near vehicle is on its lane's list or on its connector's, and on no other. The stretch's
// owner standing on that lane (a car turning off it into its lot) is not in its own way, and whoever is behind its body there follows
// it (it holds them, then the stretch does): only what is ahead of its rear can stand on the stretch.
bool lane_clear(const CityState &s, const LaneClosure &c) {
	const TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const int ds = c.key / MAX_LANES, lane = c.key % MAX_LANES;
	for (const LaneClosure &held : t.closures)
		if (held.key == c.key && held.owner != c.owner && held.from < c.to + p.lorry_clear_margin_m && held.to > c.from - p.lorry_clear_margin_m) return false;
	const auto stopping = [&](const Vehicle &v) { return v.v * v.v / (2 * p.classes[int(v.type)].b) + p.lorry_clear_margin_m; };
	const Vehicle *owner = c.owner >= 0 && size_t(c.owner) < t.vehicles.size() ? &t.vehicles[size_t(c.owner)] : nullptr;
	const bool owner_on_it = owner && !owner->off && owner->conn < 0 && owner->ds * MAX_LANES + owner->lane == c.key;
	const float behind = owner_on_it ? owner->s - p.classes[int(owner->type)].length : -BIG;
	if (size_t(c.key) < t.lane_vehicles.size())
		for (int id : t.lane_vehicles[size_t(c.key)]) {
			const Vehicle &v = t.vehicles[size_t(id)];
			if (id == c.owner || v.s <= behind) continue;
			const float rear = v.s - p.classes[int(v.type)].length;
			if (rear < c.to + p.lorry_clear_margin_m && v.s > c.from - stopping(v)) return false;
		}
	if (owner_on_it) return true; // the connectors onto its lane end behind it
	const LaneGraph &g = t.g;
	const int from = size_t(ds) < g.ds.size() ? g.ds[size_t(ds)].from : -1;
	if (from < 0 || size_t(from) >= g.node_conn.size()) return true;
	for (int ci = g.node_conn[size_t(from)]; ci >= 0; ci = g.conns[size_t(ci)].next_at_node) {
		const Conn &k = g.conns[size_t(ci)];
		if (k.to_ds != ds || k.to_lane != lane || size_t(ci) >= t.conn_vehicles.size()) continue;
		for (int id : t.conn_vehicles[size_t(ci)]) {
			const Vehicle &v = t.vehicles[size_t(id)];
			if (k.len - v.cs + c.from < stopping(v)) return false;
		}
		// a lane head committed to this connector has taken the way over it: as near as one already on it
		const int u = pass_holder(s, ci);
		if (u >= 0) {
			const Vehicle &v = t.vehicles[size_t(u)];
			if (v.ds >= 0 && g.ds[size_t(v.ds)].len - v.s + k.len + c.from < stopping(v)) return false;
		}
	}
	return true;
}

void open_lanes(TrafficState &t, int owner) {
	auto &all = t.closures;
	all.erase(std::remove_if(all.begin(), all.end(), [&](const LaneClosure &c) { return c.owner == owner; }), all.end());
}

float closure_ahead(const TrafficState &t, int key, float s) {
	float best = -1;
	for (const LaneClosure &c : t.closures)
		if (c.key == key && c.to > s && (best < 0 || c.from - s < best)) best = std::max(0.f, c.from - s);
	return best;
}

bool stretch_held(const TrafficState &t, int key, float from, float to) {
	const float ahead = closure_ahead(t, key, from);
	return ahead >= 0 && ahead < to - from;
}

} // namespace traffic
} // namespace city
