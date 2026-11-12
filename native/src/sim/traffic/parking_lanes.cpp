// What the parked side asks of the road and of the walkers: the point of a segment's frame, the lane a gate opens onto
// and who stands near a point (the walkers by cell, filed at most once a tick).
#include "sim/traffic/lane_graph_internal.h"
#include "sim/traffic/traffic_system.h"

#include "sim/buildings/b40/record.h"
#include "sim/buildings/lot_access.h"
#include "sim/roads/parking_layout.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {
constexpr float WALKER_CELL_M = 8.f;
constexpr int CELL_ROW_BITS = 32;

int64_t cell_of(float x, float z) {
	return (int64_t(std::floor(x / WALKER_CELL_M)) << CELL_ROW_BITS) ^ int64_t(uint32_t(int32_t(std::floor(z / WALKER_CELL_M))));
}

void file_walkers(CityState &s) {
	TrafficState &t = s.traffic;
	if (t.walkers_filed == t.tick) return;
	t.walkers_filed = t.tick;
	auto &old = t.walker_spare_cells;
	old.swap(t.walker_active_cells);
	t.walker_active_cells.clear();
	for (int64_t key : old) {
		auto it = t.walker_cells.find(key);
		if (it != t.walker_cells.end()) it->second.clear();
	}
	for (int id : t.live_peds)
		if (t.peds[size_t(id)].alive) {
			const int64_t key = cell_of(t.peds[size_t(id)].cur.x, t.peds[size_t(id)].cur.z);
			auto &list = t.walker_cells[key];
			if (list.empty()) t.walker_active_cells.push_back(key);
			list.push_back(id);
		}
	for (int64_t key : old) {
		auto it = t.walker_cells.find(key);
		if (it != t.walker_cells.end() && it->second.empty()) t.walker_cells.erase(it);
	}
	old.clear();
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

// Through the gate square on: a curve cut across the wall's line would swing the car's corners into the posts. The way's end is
// replaced by a straight run along the gate's axis, a car's half length either side, and a point beyond each end so the curves
// (manoeuvre.cpp) leave it in line.
static void through_gate(const TrafficParams &tp, const LotGate &gate, std::vector<V2> &via) {
	float half = 0;
	for (int t = int(VehicleType::SEDAN); t <= int(VehicleType::PICKUP); t++) half = std::max(half, tp.classes[t].length * 0.5f);
	const float run = half + tp.parker_gate_margin_m, beyond = run + tp.parker_gate_lead_m;
	while (via.size() > 1 && (via.back() - gate.at).len() < beyond + tp.parker_gate_lead_m) via.pop_back();
	via.push_back(gate.at + gate.in * beyond);
	via.push_back(gate.at + gate.in * run);
	via.push_back(gate.at - gate.in * run);
	via.push_back(gate.at - gate.in * beyond);
}

bool read_bay_way(CityState &s, int b) {
	ParkingState &p = s.traffic.park;
	if (p.bays[size_t(b)].way_read) return p.bays[size_t(b)].reached;
	const int source = p.bays[size_t(b)].source;
	const Building &rec = s.buildings.items[size_t(source)];
	b40::Body body;
	b40::record_body(s.buildings.designs, rec, body);
	LotAccess acc;
	const TrafficParams &tp = s.catalog.traffic;
	// room to spare along the way: its curves stray from the grid's line and a car's corners swing wider than its sides. Each bay
	// keeps the most room that still reaches a gate; a lot too tight for any keeps its exact fit.
	const float half = car_half_width(tp), lorry = lorry_half_width(tp);
	lot_access(s.buildings.designs, body, b40::lot_frame(rec), half + tp.parker_way_wide_m, lorry, acc);
	for (float room : { tp.parker_way_room_m, 0.f }) {
		if (acc.reached() == int(acc.bays.size())) break;
		LotAccess tighter;
		lot_access(s.buildings.designs, body, b40::lot_frame(rec), half + room, lorry, tighter);
		for (size_t i = 0; i < acc.bays.size() && i < tighter.bays.size(); i++)
			if (acc.bays[i].gate < 0) acc.bays[i] = tighter.bays[i];
	}
	const std::vector<int> &ours = p.lots[source].bays;
	for (size_t i = 0; i < ours.size() && i < acc.bays.size(); i++) {
		Bay &o = p.bays[size_t(ours[i])];
		const LotBay &lb = acc.bays[i];
		o.way_read = true;
		o.reached = lb.gate >= 0 && !lb.way.empty() && gate_lane(s, acc.gates[size_t(lb.gate)].at, o.lane);
		if (o.reached) {
			o.via = lb.way;
			if (!o.dock) through_gate(tp, acc.gates[size_t(lb.gate)], o.via);
		}
		refile_bay(s, ours[i], false);
	}
	return p.bays[size_t(b)].reached;
}

} // namespace traffic
} // namespace city
