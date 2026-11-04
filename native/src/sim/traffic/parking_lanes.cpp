// What the parked side asks of the road and of the walkers: the point of a segment's frame, the lane a gate opens onto
// and who stands near a point (the walkers by cell, filed at most once a tick).
#include "sim/traffic/traffic_system.h"

#include "sim/roads/parking_layout.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {
constexpr float WALKER_CELL_M = 8.f;
constexpr int CELL_ROW_BITS = 32;
constexpr double FLAT_TANGENT = 1e-9; // below this the curve has no direction

int64_t cell_of(float x, float z) {
	return (int64_t(std::floor(x / WALKER_CELL_M)) << CELL_ROW_BITS) ^ int64_t(uint32_t(int32_t(std::floor(z / WALKER_CELL_M))));
}

void file_walkers(CityState &s) {
	TrafficState &t = s.traffic;
	if (t.walkers_filed == t.tick) return;
	t.walkers_filed = t.tick;
	for (auto &kv : t.walker_cells) kv.second.clear();
	for (int id = 0; id < int(t.peds.size()); id++)
		if (t.peds[size_t(id)].alive) t.walker_cells[cell_of(t.peds[size_t(id)].cur.x, t.peds[size_t(id)].cur.z)].push_back(id);
}
} // namespace

void seg_point(const CityState &s, int seg, float arc, float u, Pose &out) {
	const RoadSegment &g = s.roads.segs[size_t(seg)];
	const double t = t_at_length(g.table, std::max(0.0, std::min(g.len, double(arc))));
	double x, z, dx, dz;
	g.q.at(t, x, z);
	g.q.deriv(t, dx, dz);
	const double l = std::max(FLAT_TANGENT, std::hypot(dx, dz));
	dx /= l;
	dz /= l;
	out.x = float(x + dz * u);
	out.z = float(z - dx * u);
	out.y = float(s.roads.seg_y(seg, arc));
	out.fx = float(dx);
	out.fz = float(dz);
}

bool gate_lane(const CityState &s, const V2 &gate, LaneSpot &out) {
	const TrafficParams &p = s.catalog.traffic;
	const LaneGraph &g = s.traffic.g;
	const float reach = p.gate_lane_reach_m;
	float best = reach;
	bool found = false;
	s.roads.for_segments_near(gate.x - reach, gate.y - reach, gate.x + reach, gate.y + reach, [&](int seg) {
		const RoadSegment &r = s.roads.segs[size_t(seg)];
		double t, dist, x, z, dx, dz;
		closest_t(r.q, gate.x, gate.y, t, dist);
		if (dist >= best) return;
		r.q.at(t, x, z);
		r.q.deriv(t, dx, dz);
		// the lot on the kerb side of the way it drives; on a one-way street, whichever kerb the lot is on
		const int side = (dz * (gate.x - x) - dx * (gate.y - z) > 0 ? 1 : 0) ^ p.drive_side;
		int ds = seg * 2 + side;
		int lane = 0;
		if (!g.ds[size_t(ds)].ok) {
			ds = seg * 2 + 1 - side;
			lane = g.ds[size_t(ds)].lanes - 1;
		}
		const DirSeg &d = g.ds[size_t(ds)];
		if (!d.ok || d.len < p.gate_lane_min_m) return;
		const float foot = d.sign > 0 ? float(arc_of_t(r, t)) - d.at0 : d.at0 - float(arc_of_t(r, t));
		const float lo = p.gate_lane_end_m, hi = d.len - p.gate_lane_end_m;
		const float at = clampf(foot + p.gate_join_past_m, lo, hi), entry = clampf(foot - p.gate_stop_before_m, lo, hi);
		if (at - entry < p.gate_span_min_m) return; // the gate must lie between where a car stops and where one joins
		Pose q;
		lane_pose(s, ds, lane, at, q);
		out.ds = ds;
		out.lane = lane;
		out.at = at;
		out.entry = entry;
		out.p = V2(q.x, q.z);
		out.t = V2(q.fx, q.fz);
		best = float(dist);
		found = true;
	});
	return found;
}

bool walker_within(CityState &s, float x, float z, float r) {
	file_walkers(s);
	const TrafficState &t = s.traffic;
	const int reach = int(std::ceil(r / WALKER_CELL_M));
	for (int j = -reach; j <= reach; j++)
		for (int i = -reach; i <= reach; i++) {
			auto it = t.walker_cells.find(cell_of(x + float(i) * WALKER_CELL_M, z + float(j) * WALKER_CELL_M));
			if (it == t.walker_cells.end()) continue;
			for (int id : it->second) {
				const Pose &q = t.peds[size_t(id)].cur;
				if ((q.x - x) * (q.x - x) + (q.z - z) * (q.z - z) < r * r) return true;
			}
		}
	return false;
}

} // namespace traffic
} // namespace city
