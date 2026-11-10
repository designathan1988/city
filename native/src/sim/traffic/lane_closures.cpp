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

namespace {
// The lanes of segment `seg` under a body whose middle stands at (x, z) heading `angle`, `back` and `front` metres of it behind and
// ahead of that point: each lane's span of arc, widened in `spans`. A lane is under the body where a point of it lies within the
// lane's own width (the profile's); the body is walked along its length and across its width in steps no longer than half the
// narrowest lane, so no lane fits between two points, however the body stands to the street, and a body beside a lane is not on it.
void lanes_under(const CityState &s, double x, double z, double angle, double back, double front, double half, int seg, std::map<int, std::pair<float, float>> &spans) {
	const LaneGraph &g = s.traffic.g;
	const RoadSegment &r = s.roads.segs[size_t(seg)];
	const Profile &pf = s.roads.profile_of(seg);
	double narrowest = 0;
	for (const std::vector<LaneInfo> *lanes : { &pf.fwd, &pf.back })
		for (const LaneInfo &l : *lanes) narrowest = narrowest > 0 ? std::min(narrowest, double(l.w)) : double(l.w);
	if (narrowest <= 0) return; // no lane to stand on
	const double c = std::cos(angle), sn = std::sin(angle), step = narrowest * 0.5;
	const int along = std::max(1, int(std::ceil((back + front) / step))), across = std::max(1, int(std::ceil(2 * half / step)));
	for (int i = 0; i <= along; i++)
		for (int j = 0; j <= across; j++) {
			const double u = -back + (back + front) * i / along, v = -half + 2 * half * j / across;
			const double px = x + c * u - sn * v, pz = z + sn * u + c * v;
			double t, dist, cx, cz, dx, dz;
			closest_t(r.q, px, pz, t, dist);
			r.q.at(t, cx, cz);
			r.q.deriv(t, dx, dz);
			const float off = float((dz * (px - cx) - dx * (pz - cz)) / std::max(FLAT_TANGENT, std::hypot(dx, dz)));
			const float arc = float(arc_of_t(r, t));
			for (int dir = 0; dir < 2; dir++) {
				const DirSeg &d = g.ds[size_t(seg) * 2 + dir];
				if (!d.ok) continue;
				const std::vector<LaneInfo> &lanes = dir == 0 ? pf.fwd : pf.back;
				const float at = (arc - d.at0) * d.sign;
				for (int lane = 0; lane < d.lanes; lane++) {
					if (std::fabs(off - d.u[lane]) > lanes[size_t(lane)].w * 0.5f) continue;
					auto hit = spans.emplace((seg * 2 + dir) * MAX_LANES + lane, std::make_pair(at, at));
					hit.first->second.first = std::min(hit.first->second.first, at);
					hit.first->second.second = std::max(hit.first->second.second, at);
				}
			}
		}
}

void close_spans(const CityState &s, const std::map<int, std::pair<float, float>> &spans, int owner, std::vector<LaneClosure> &out) {
	const float margin = s.catalog.traffic.lorry_closure_margin_m;
	for (const auto &kv : spans) out.push_back({ kv.first, kv.second.first - margin, kv.second.second + margin, owner });
}

// The lanes of every segment near a body (its middle at (x, z) heading `angle`), widened in `spans`.
void lanes_near(const CityState &s, double x, double z, double angle, double half_len, double half_wid, std::map<int, std::pair<float, float>> &spans) {
	const double reach = std::hypot(half_len, half_wid);
	const size_t dirs = s.traffic.g.ds.size();
	s.roads.for_segments_near(x - reach, z - reach, x + reach, z + reach, [&](int seg) {
		if (s.roads.segs[size_t(seg)].alive && size_t(seg) * 2 + 1 < dirs) lanes_under(s, x, z, angle, half_len, half_len, half_wid, seg, spans);
	});
}
} // namespace

// The stretches of lane a lorry's street part sweeps: the lanes under its body at every pose of it, on every segment near, with a
// margin each way.
void sweep_lanes(const CityState &s, const std::vector<AxlePose> &poses, int from, int to, int owner, std::vector<LaneClosure> &out) {
	const TrafficParams &p = s.catalog.traffic;
	const double half = p.classes[int(VehicleType::TRUCK)].half_width, len = p.classes[int(VehicleType::TRUCK)].length;
	std::map<int, std::pair<float, float>> spans;
	for (int i = from; i <= to && i < int(poses.size()); i++) {
		const AxlePose &q = poses[size_t(i)];
		lanes_near(s, q.x + std::cos(q.angle) * p.lorry_rear_m, q.z + std::sin(q.angle) * p.lorry_rear_m, q.angle, len * 0.5, half, spans);
	}
	close_spans(s, spans, owner, out);
}

// The same for a body of `type` along `path` from `from` to `to` metres: poses half a body apart, so the body at one overlaps the
// next and no lane is passed between two.
void sweep_path(const CityState &s, const Manoeuvre &path, float from, float to, VehicleType type, int owner, std::vector<LaneClosure> &out) {
	const VehicleClass &c = s.catalog.traffic.classes[int(type)];
	const float step = std::max(c.half_width, c.length * 0.5f);
	std::map<int, std::pair<float, float>> spans;
	for (float at = from;; at = std::min(to, at + step)) {
		const OffPose q = path.pose_at(at);
		lanes_near(s, q.x, q.z, q.angle, c.length * 0.5, c.half_width, spans);
		if (at >= to) break;
	}
	close_spans(s, spans, owner, out);
}

bool lanes_under_body(const CityState &s, const OffPose &body, VehicleType type) {
	const VehicleClass &c = s.catalog.traffic.classes[int(type)];
	std::map<int, std::pair<float, float>> spans;
	lanes_near(s, body.x, body.z, body.angle, c.length * 0.5, c.half_width, spans);
	return !spans.empty();
}

bool take_lanes(CityState &s, const std::vector<LaneClosure> &stretches) {
	for (const LaneClosure &c : stretches)
		if (!lane_clear(s, c)) return false;
	s.traffic.closures.insert(s.traffic.closures.end(), stretches.begin(), stretches.end());
	return true;
}

// Nobody on a stretch or too near to stop short of it, but the closure's owner itself (a lorry stands on its lane till the street
// is its own), and no other body off the lanes holding it: the near vehicles on its lane, those on the connectors that end on it
// (all at the node its lane starts from), and the others' closures. A near vehicle is on its lane's list or on its connector's.
bool lane_clear(const CityState &s, const LaneClosure &c) {
	const TrafficState &t = s.traffic;
	for (const LaneClosure &o : t.closures)
		if (o.owner != c.owner && o.key == c.key && o.from < c.to && o.to > c.from) return false;
	const TrafficParams &p = s.catalog.traffic;
	const int ds = c.key / MAX_LANES, lane = c.key % MAX_LANES;
	const auto stopping = [&](const Vehicle &v) { return v.v * v.v / (2 * p.classes[int(v.type)].b) + p.lorry_clear_margin_m; };
	if (size_t(c.key) < t.lane_vehicles.size())
		for (int id : t.lane_vehicles[size_t(c.key)]) {
			if (id == c.owner) continue;
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

// A closure holds who comes to it, never who is within it: a vehicle whose front is already past its start has the body that
// closed it beside or behind it and drives out (held there, it and that body would wait for each other for good).
float closure_ahead(const TrafficState &t, int key, float s) {
	float best = -1;
	for (const LaneClosure &c : t.closures)
		if (c.key == key && c.from >= s && (best < 0 || c.from - s < best)) best = c.from - s;
	return best;
}

} // namespace traffic
} // namespace city
