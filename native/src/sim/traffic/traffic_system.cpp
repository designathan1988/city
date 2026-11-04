// The traffic step called by Simulation::step: applies the lane-graph changes the journal announced (only the segments
// and nodes an edit touched), then runs the far model, demand, lane changes, motion and pedestrians, and computes the
// poses render/agents draws (body centre between the front and rear points, so long vehicles follow curves).
#include "sim/traffic/kerb_stops.h"

#include "sim/traffic/lane_closures.h"

#include "core/laps.h"
#include "core/units.h"
#include "sim/traffic/lane_graph_internal.h"

#include <algorithm>
#include <chrono>

namespace city {
namespace traffic {

namespace {

constexpr float MIN_AXIS_M = 0.5f;

// Front (`front` metres along) and rear points of a vehicle on its lane or connector.
void ends(const CityState &s, const Vehicle &v, float front, float len, Pose &f, Pose &r) {
	if (v.conn >= 0) {
		const Conn &c = s.traffic.g.conns[v.conn];
		conn_pose(c, front, f);
		if (front - len >= 0) conn_pose(c, front - len, r);
		else lane_pose(s, c.from_ds, c.from_lane, s.traffic.g.ds[c.from_ds].len + front - len, r);
		return;
	}
	lane_pose(s, v.ds, v.lane, front, f);
	lane_pose(s, v.ds, v.lane, front - len, r);
	const DirSeg &d = s.traffic.g.ds[v.ds];
	const float off = v.lc_off + cycle_shift(s, v, v.ds, front);
	float lx = d.sign * f.fz * off, lz = -d.sign * f.fx * off; // left of a -> b, as lane offsets
	f.x += lx; f.z += lz;
	r.x += lx; r.z += lz;
}

} // namespace

void despawn_vehicle(CityState &s, int id, bool arrived) {
	TrafficState &t = s.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	if (!v.alive) return;
	transit::bus_gone(s, id);
	lot_car_forget(s, id);
	lorry_gone(s, id);
	kerb_forget(s, id);
	if (v.conn >= 0) conn_del(s, v.conn, id);
	else if (v.micro && v.ds >= 0) lane_del(s, v.ds * MAX_LANES + v.lane, id);
	meso_unlink(s, id);
	micro_remove(t, id);
	t.route_waste += size_t(v.route.cap);
	v = Vehicle();
	t.free_vehicles.push_back(id);
	t.vehicle_count--;
	(arrived ? t.arrived : t.lost)++;
}

void reset_traffic(CityState &s) {
	TrafficState &t = s.traffic;
	// a new map or a load: the whole traffic state starts over (fleet, counters, clock, governor, rng), so nothing of
	// the old city survives in a field this list forgot; the old fleet is gone before the full graph build
	t = TrafficState();
	build_lane_graph(s);
	reset_pedestrians(s);
	t.lane_vehicles.assign(t.g.ds.size() * MAX_LANES, {});
	t.lane_active.assign(t.g.ds.size() * MAX_LANES, 0);
	t.conn_vehicles.assign(t.g.conns.size(), {});
	t.conn_active.assign(t.g.conns.size(), 0);
	meso_clear(t);
	t.light_now.assign(t.g.ds.size(), uint8_t(Light::GREEN));
	reset_admission(s);
	const size_t arms = s.roads.nodes.size() * NODE_ARMS_MAX * CROSS_CACHE_KINDS;
	t.cross_stamp.assign(arms, STAMP_NONE);
	t.cross_walk.assign(arms, 0);
	t.cross_cars.assign(arms, 0);
	t.g.version = s.roads.version;
	s.log.clear_net();
	transit::reset(s);
}

namespace {
// What the signalled approaches within `radius` of (cx, cz) show now, sorted by ds (PoseFrame::signal).
void publish_signals(CityState &s, float cx, float cz, float radius, std::vector<SignalRecord> &out) {
	out.clear();
	const LaneGraph &g = s.traffic.g;
	s.roads.node_hash.query(cx - radius, cz - radius, cx + radius, cz + radius, [&](int n) {
		if (n < 0 || size_t(n) >= g.junctions.size() || g.junctions[size_t(n)].ctl != Control::SIGNAL) return;
		for (int seg : s.roads.nodes[size_t(n)].segs) {
			const int ds = in_ds(s.roads, seg, n);
			if (size_t(ds) >= g.ds.size() || !g.ds[size_t(ds)].ok || g.ds[size_t(ds)].stage < 0) continue;
			out.push_back({ uint32_t(ds), uint8_t(light_for(s, ds)), walk_ok(s, n, seg) });
		}
	});
	std::sort(out.begin(), out.end(), [](const SignalRecord &a, const SignalRecord &b) { return a.ds < b.ds; });
}
} // namespace

void publish_poses(CityState &s, int64_t stamp_ns) {
	const TrafficParams &p = s.catalog.traffic;
	TrafficState &t = s.traffic;
	const int64_t before = t.stamp_ns[POSE_TRAIL - 1];
	for (int i = 0; i + 1 < POSE_TRAIL; i++) t.stamp_ns[i] = t.stamp_ns[i + 1];
	t.stamp_ns[POSE_TRAIL - 1] = stamp_ns;
	Laps lap;
	const float cx = s.camera_x.load(), cz = s.camera_z.load();
	const float r2 = p.publish_radius_m * p.publish_radius_m;
	PoseFrame &f = s.poses.back();
	f.vehicles.clear();
	f.people.clear();
	f.seg_speed_share.assign(s.roads.segs.size(), 1.f);
	std::vector<float> &sum = t.pub_sum, &lim = t.pub_lim;
	std::vector<int> &count = t.pub_count;
	sum.assign(s.roads.segs.size(), 0.f);
	lim.assign(s.roads.segs.size(), 0.f);
	count.assign(s.roads.segs.size(), 0);
	for (Vehicle &v : s.traffic.vehicles) {
		if (!v.alive || (!v.micro && !v.off)) continue; // far vehicles are queues and events, not poses
		if (!v.off) {
			float len = p.classes[int(v.type)].length;
			Pose front, rear;
			ends(s, v, v.conn >= 0 ? v.cs : v.s, len, front, rear);
			V2 axis(front.x - rear.x, front.z - rear.z);
			v.cur.x = (front.x + rear.x) * 0.5f;
			v.cur.z = (front.z + rear.z) * 0.5f;
			v.cur.y = (front.y + rear.y) * 0.5f + p.body_lift_m;
			V2 h = axis.len() > MIN_AXIS_M ? axis.norm() : V2(front.fx, front.fz);
			v.cur.fx = h.x;
			v.cur.fz = h.y;
		}
		v.trail.push(v.cur, stamp_ns, before);
		if (v.ds >= 0 && v.ds < int(t.g.ds.size())) {
			const int seg = t.g.ds[v.ds].seg;
			if (seg >= 0 && seg < int(sum.size())) {
				sum[seg] += v.v;
				lim[seg] += t.g.ds[v.ds].speed;
				count[seg]++;
			}
		}
		const float dx = v.cur.x - cx, dz = v.cur.z - cz;
		if (dx * dx + dz * dz > r2) continue;
		AgentRecord rec;
		for (int i = 0; i < POSE_TRAIL; i++) rec.at[i] = v.trail.at[i];
		rec.id = uint32_t(&v - s.traffic.vehicles.data());
		const bool empty = v.off && lot_car_at_rest(s, int(&v - t.vehicles.data())); // nobody in a car standing in its bay
		rec.look = uint32_t(v.type) | (uint32_t(v.paint) << LOOK_PAINT_SHIFT) | (v.brake ? 1u << LOOK_BRAKE_SHIFT : 0u) | (empty ? 1u << LOOK_EMPTY_SHIFT : 0u) | (uint32_t(v.aboard) << LOOK_ABOARD_SHIFT);
		f.vehicles.push_back(rec);
	}
	for (Pedestrian &w : s.traffic.peds) {
		if (!w.alive) continue;
		w.trail.push(w.cur, stamp_ns, before);
		if (s.traffic.time < w.down_until) continue; // on the ground: the effects draw the body, not the walker
		const float dx = w.cur.x - cx, dz = w.cur.z - cz;
		if (dx * dx + dz * dz > r2) continue;
		AgentRecord rec;
		for (int i = 0; i < POSE_TRAIL; i++) rec.at[i] = w.trail.at[i];
		rec.id = uint32_t(&w - s.traffic.peds.data());
		rec.look = w.look;
		rec.walked = w.walked;
		rec.rest = w.rest;
		rec.age = uint8_t(w.age);
		f.people.push_back(rec);
	}
	for (size_t seg = 0; seg < f.seg_speed_share.size(); seg++)
		if (count[seg] > 0 && lim[seg] > 0) f.seg_speed_share[seg] = sum[seg] / lim[seg];
	publish_signals(s, cx, cz, p.publish_radius_m, f.signals);
	transit::publish(s, f, stamp_ns, before, cx, cz, p.publish_radius_m);
	publish_parked(s, f);
	for (int i = 0; i < POSE_TRAIL; i++) f.stamp_ns[i] = t.stamp_ns[i];
	f.period_ns = int64_t(NS_PER_S / double(s.catalog.tune.sim_tick_hz));
	s.poses.publish();
	t.step_ms[STAGE_PUBLISH] += lap.lap();
}

void step(CityState &s, float dt) {
	TrafficState &t = s.traffic;
	apply_network_changes(s);
	route_compact(t);
	t.time += dt;
	t.tick++;
	update_lights(s);
	check_window(s);
	Laps lap;
	step_parking_sources(s);
	t.step_ms[STAGE_BAYS] += lap.lap();
	t.route_budget = int(s.catalog.traffic.reroute_budget_per_step);
	step_governor(s, dt);
	spawn_vehicles(s, dt);
	meso_process(s);
	change_lanes(s);
	step_vehicles(s, dt);
	step_kerb_stops(s, dt);
	t.step_ms[STAGE_FLEET] += lap.lap();
	step_lots(s, dt);
	t.step_ms[STAGE_LOTS] += lap.lap();
	transit::step(s, dt);
	step_pedestrians(s, dt);
	t.step_ms[STAGE_WALKERS] += lap.lap();
}

} // namespace traffic
} // namespace city
