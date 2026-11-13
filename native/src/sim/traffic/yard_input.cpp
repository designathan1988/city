// What a yard is made from, copied from the world under the locks the step holds (yard.h): the lot and what is solid in it, the
// carriageways round it, the gates with the lane each opens onto, the docks. Nothing here searches.
#include "sim/traffic/traffic_system.h"

#include "sim/buildings/b40/record.h"
#include "sim/buildings/lot_access.h"
#include "sim/traffic/yard.h"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>

namespace city {
namespace traffic {

namespace {
constexpr float HALF = 0.5f, ASPHALT_STEP_M = 4.f;
constexpr float UNBOUNDED = std::numeric_limits<float>::max();

std::vector<V2> rect_ring(const b40::LotFrame &f, float x0, float y0, float x1, float y1) {
	return { f.at(x0, y0), f.at(x1, y0), f.at(x1, y1), f.at(x0, y1) };
}

// The carriageway of every segment near, in short quads along its curve, with what of a lorry's body may stand over the kerb: only its wheels are held by it.
void asphalt_near(const CityState &s, double x0, double z0, double x1, double z1, YardInput &out) {
	out.reads.read(s.roads, x0, z0, x1, z1);
	s.roads.for_segments_near(x0, z0, x1, z1, [&](int seg) {
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		const Profile &p = s.roads.profile_of(seg);
		const int steps = std::max(1, int(std::ceil(g.len / ASPHALT_STEP_M)));
		const float over = s.catalog.traffic.lorry_kerb_over_m;
		Pose l0, r0;
		seg_point(s, seg, 0, p.asph_left + over, l0);
		seg_point(s, seg, 0, -p.asph_right - over, r0);
		for (int i = 1; i <= steps; i++) {
			Pose l1, r1;
			const float arc = float(g.len) * float(i) / float(steps);
			seg_point(s, seg, arc, p.asph_left + over, l1);
			seg_point(s, seg, arc, -p.asph_right - over, r1);
			out.open.push_back({ V2(l0.x, l0.z), V2(r0.x, r0.z), V2(r1.x, r1.z), V2(l1.x, l1.z) });
			l0 = l1;
			r0 = r1;
		}
	});
}

// The lane each car gate opens onto, the stop before it and the join past it; its mouth across the footway.
void gates_of(const CityState &s, const LotAccess &acc, YardInput &out) {
	const TrafficParams &p = s.catalog.traffic;
	const LaneGraph &g = s.traffic.g;
	for (const LotGate &gate : acc.gates) {
		LaneSpot spot;
		if (!gate.car || !gate_lane(s, gate.at, spot, &out.reads)) continue;
		const DirSeg &d = g.ds[size_t(spot.ds)];
		const float foot = (spot.at - p.gate_join_past_m + spot.entry + p.gate_stop_before_m) * HALF;
		YardGate yg;
		yg.ds = spot.ds;
		yg.lane = spot.lane;
		// Both are where the body's middle stands, and the body stands whole on its lane: half its length and the lane's end margin
		// from either end. A middle held only by the margin left half a lorry past the end of a street with no way on, where
		// nothing is paved: a pose that never fits, and every dock of the lot without its way out.
		const float whole = p.classes[int(VehicleType::TRUCK)].length * HALF + p.gate_lane_end_m;
		yg.stop_at = std::max(whole, foot - p.lorry_stop_before_m);
		yg.join_at = std::min(d.len - whole, foot + p.lorry_join_past_m);
		Pose stop, join;
		lane_pose(s, spot.ds, spot.lane, yg.stop_at, stop);
		lane_pose(s, spot.ds, spot.lane, yg.join_at, join);
		const double rear = p.lorry_rear_m;
		yg.stop = { stop.x - stop.fx * rear, stop.z - stop.fz * rear, std::atan2(double(stop.fz), double(stop.fx)), false };
		yg.join = { join.x - join.fx * rear, join.z - join.fz * rear, std::atan2(double(join.fz), double(join.fx)), false };
		yg.inner = gate.at + gate.in * p.lorry_gate_in_m;
		yg.outer = spot.p;
		out.gates.push_back(yg);
		// the paved mouth in front of the gate a lorry may cross the footway by: the gate and a flare either side
		const V2 across(-gate.in.y, gate.in.x), a = yg.inner, b = yg.outer;
		const float half = gate.w * HALF + p.lorry_mouth_flare_m;
		out.open.push_back({ a - across * half, a + across * half, b + across * half, b - across * half });
	}
}
} // namespace

void YardReads::read(const RoadNetwork &net, double x0, double z0, double x1, double z1) {
	boxes.push_back({ x0, z0, x1, z1 });
	net.for_segments_near(x0, z0, x1, z1, [&](int seg) { segs.push_back(seg); });
}

bool YardReads::touched_by(const RoadNetwork &net, const std::vector<int> &removed, const std::vector<int> &changed) const {
	for (const std::vector<int> *list : { &removed, &changed })
		for (int seg : *list)
			if (std::binary_search(segs.begin(), segs.end(), seg)) return true;
	for (int seg : changed) { // come into a box (for_segments_near's own test)
		if (seg < 0 || size_t(seg) >= net.segs.size() || !net.segs[size_t(seg)].alive) continue;
		const RoadSegment &g = net.segs[size_t(seg)];
		for (const std::array<double, 4> &b : boxes)
			if (!(g.maxx < b[0] || g.minx > b[2] || g.maxz < b[1] || g.minz > b[3])) return true;
	}
	return false;
}

void forget_yards_near(CityState &s, const std::vector<int> &removed, const std::vector<int> &changed) {
	ParkingState &pk = s.traffic.park;
	for (auto it = pk.yards.begin(); it != pk.yards.end();) it = it->second->reads.touched_by(s.roads, removed, changed) ? pk.yards.erase(it) : std::next(it);
	if (pk.planner && pk.planner->plan.reads.touched_by(s.roads, removed, changed)) pk.planner.reset();
}

bool yard_input(const CityState &s, int building, YardInput &out) {
	const TrafficParams &p = s.catalog.traffic;
	const Building &rec = s.buildings.items[size_t(building)];
	if (!rec.alive) return false;
	const std::shared_ptr<const b40::Body> body_held = b40::record_body(s.buildings.designs, rec);
	const b40::Body &body = *body_held;
	const b40::LotFrame f = b40::lot_frame(rec);
	LotAccess acc;
	lot_bays(body.elements, f, acc);
	out.body = { p.classes[int(VehicleType::TRUCK)].length, p.classes[int(VehicleType::TRUCK)].half_width * 2, p.lorry_rear_m, p.lorry_radius_m, p.lorry_margin_m };
	out.lot = rect_ring(f, -rec.w * HALF, 0, rec.w * HALF, rec.d);
	float lo_x = UNBOUNDED, lo_z = UNBOUNDED, hi_x = -UNBOUNDED, hi_z = -UNBOUNDED;
	for (const V2 &q : out.lot) {
		lo_x = std::min(lo_x, q.x);
		lo_z = std::min(lo_z, q.y);
		hi_x = std::max(hi_x, q.x);
		hi_z = std::max(hi_z, q.y);
	}
	const float reach = p.lorry_street_reach_m;
	out.cell = p.lorry_cell_m;
	out.x0 = lo_x - reach;
	out.z0 = lo_z - reach;
	out.nx = int(std::ceil((hi_x - lo_x + 2 * reach) / p.lorry_cell_m));
	out.nz = int(std::ceil((hi_z - lo_z + 2 * reach) / p.lorry_cell_m));
	out.street_window = p.lorry_street_window_m;
	out.dock_gap = p.lorry_dock_gap_m;
	out.pre_dock = p.lorry_pre_dock_m;
	out.pre_step = p.lorry_pre_step_m;
	out.reach = p.lorry_reach_m;
	out.turn = p.lorry_turn_rad;
	out.settle_step = p.lorry_settle_step_m;
	out.fit_margin = p.lorry_fit_margin_m;
	asphalt_near(s, out.x0, out.z0, out.x0 + out.nx * out.cell, out.z0 + out.nz * out.cell, out);
	gates_of(s, acc, out);
	s.buildings.query(float(out.x0), float(out.z0), float(out.x0 + out.nx * out.cell), float(out.z0 + out.nz * out.cell), [&](int id) {
		if (id == building) return;
		const Building &o = s.buildings.items[size_t(id)];
		out.shut.push_back(rect_ring(b40::lot_frame(o), -o.w * HALF, 0, o.w * HALF, o.d));
	});
	lot_solids(s.buildings.designs, body, f, p.lorry_clear_h_m, out.shut);
	std::sort(out.reads.segs.begin(), out.reads.segs.end());
	out.reads.segs.erase(std::unique(out.reads.segs.begin(), out.reads.segs.end()), out.reads.segs.end());
	const ParkingState &pk = s.traffic.park;
	const auto lot = pk.lots.find(building);
	if (lot == pk.lots.end()) return false;
	for (int place = 0; place < int(lot->second.bays.size()); place++) { // a dock by its place among the lot's bays (yard.h)
		const Bay &bay = pk.bays[size_t(lot->second.bays[size_t(place)])];
		if (bay.dock) out.docks.push_back({ place, bay.at - bay.open * bay.room, bay.open }); // the closed end, a half depth behind its middle
	}
	return !out.gates.empty() && !out.docks.empty();
}

} // namespace traffic
} // namespace city
