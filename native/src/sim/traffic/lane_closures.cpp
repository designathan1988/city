// Stretches of lane closed by something off the lanes standing on them (lane_closures.h).
#include "sim/traffic/lane_closures.h"

#include "sim/roads/parking_layout.h"
#include "sim/traffic/lane_graph_internal.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>
#include <map>

namespace city {
namespace traffic {

// The stretches of lane a street part sweeps: the lanes of the gate's segment under the body, with a margin each way.
void sweep_lanes(const CityState &s, const std::vector<AxlePose> &poses, int from, int to, int seg, int owner, std::vector<LaneClosure> &out) {
	const TrafficParams &p = s.catalog.traffic;
	const LaneGraph &g = s.traffic.g;
	const RoadSegment &r = s.roads.segs[size_t(seg)];
	const float half = p.classes[int(VehicleType::TRUCK)].half_width, len = p.classes[int(VehicleType::TRUCK)].length;
	std::map<int, std::pair<float, float>> spans;
	for (int i = from; i <= to && i < int(poses.size()); i += 2) {
		const AxlePose &q = poses[size_t(i)];
		const double c = std::cos(q.angle), sn = std::sin(q.angle), back = len * 0.5 - p.lorry_rear_m, front = len * 0.5 + p.lorry_rear_m;
		for (double u : { -back, (front - back) * 0.5, front })
			for (double v : { -double(half), double(half) }) {
				const double x = q.x + c * u - sn * v, z = q.z + sn * u + c * v;
				double t, dist, cx, cz, dx, dz;
				closest_t(r.q, x, z, t, dist);
				r.q.at(t, cx, cz);
				r.q.deriv(t, dx, dz);
				const float across = float((dz * (x - cx) - dx * (z - cz)) / std::max(FLAT_TANGENT, std::hypot(dx, dz)));
				const float arc = float(arc_of_t(r, t));
				for (int dir = 0; dir < 2; dir++) {
					const DirSeg &d = g.ds[size_t(seg) * 2 + dir];
					if (!d.ok) continue;
					const float at = (arc - d.at0) * d.sign;
					for (int lane = 0; lane < d.lanes; lane++) {
						if (std::fabs(across - d.u[lane]) > p.lorry_lane_half_m) continue;
						auto hit = spans.emplace((seg * 2 + dir) * MAX_LANES + lane, std::make_pair(at, at));
						hit.first->second.first = std::min(hit.first->second.first, at);
						hit.first->second.second = std::max(hit.first->second.second, at);
					}
				}
			}
	}
	for (const auto &kv : spans) out.push_back({ kv.first, kv.second.first - p.lorry_closure_margin_m, kv.second.second + p.lorry_closure_margin_m, owner });
}

// Nobody on a stretch or too near to stop short of it: the near vehicles on its lane, and those on the connectors that end on
// it (all at the node its lane starts from). A near vehicle is on its lane's list or on its connector's, and on no other.
bool lane_clear(const CityState &s, const LaneClosure &c) {
	const TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const int ds = c.key / MAX_LANES, lane = c.key % MAX_LANES;
	const auto stopping = [&](const Vehicle &v) { return v.v * v.v / (2 * driver_of(s, v).b) + p.lorry_clear_margin_m; };
	if (size_t(c.key) < t.lane_vehicles.size())
		for (int id : t.lane_vehicles[size_t(c.key)]) {
			const Vehicle &v = t.vehicles[size_t(id)];
			const float rear = v.s - p.classes[int(v.type)].length;
			if (rear < c.to + p.lorry_clear_margin_m && v.s > c.from - stopping(v)) return false;
		}
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

} // namespace traffic
} // namespace city
