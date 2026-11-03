// The traffic step called by Simulation::step: applies the lane-graph changes the journal announced (only the segments
// and nodes an edit touched), then runs the far model, demand, lane changes, motion and pedestrians, and computes the
// poses render/agents draws (body centre between the front and rear points, so long vehicles follow curves).
#include "sim/traffic/traffic_system.h"

#include "core/units.h"
#include "sim/traffic/lane_graph_internal.h"

#include <algorithm>
#include <chrono>

namespace city {
namespace traffic {

namespace {

constexpr float MIN_AXIS_M = 0.5f;
constexpr int LOOK_PAINT_SHIFT = 8, LOOK_BRAKE_SHIFT = 16; // AgentRecord::look: type in the low byte

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
	float lx = d.sign * f.fz * v.lc_off, lz = -d.sign * f.fx * v.lc_off; // left of a -> b, as lane offsets
	f.x += lx; f.z += lz;
	r.x += lx; r.z += lz;
}

} // namespace

void despawn_vehicle(CityState &s, int id, bool arrived) {
	TrafficState &t = s.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	if (!v.alive) return;
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
	// the old fleet goes first: the full graph build must not see it on lanes that no longer exist
	t.vehicles.clear();
	t.free_vehicles.clear();
	t.route_pool.clear();
	t.route_waste = 0;
	t.micro_list.clear();
	t.lane_vehicles.clear();
	t.lane_active.clear();
	t.active_lanes.clear();
	t.conn_vehicles.clear();
	t.conn_active.clear();
	t.active_conns.clear();
	t.meso_head.clear();
	t.meso_tail.clear();
	t.meso_count.clear();
	t.lane_last_exit.clear();
	t.ev_t.clear();
	t.ev_v.clear();
	t.ev_n = 0;
	build_lane_graph(s);
	reset_pedestrians(s);
	t.lane_vehicles.assign(t.g.ds.size() * MAX_LANES, {});
	t.lane_active.assign(t.g.ds.size() * MAX_LANES, 0);
	t.conn_vehicles.assign(t.g.conns.size(), {});
	t.conn_active.assign(t.g.conns.size(), 0);
	meso_clear(t);
	t.light_now.assign(t.g.ds.size(), uint8_t(Light::GREEN));
	reset_admission(s);
	t.spawn_credit = 0;
	t.win_x = 0;
	t.win_z = 0;
	t.win_t = -double(BIG);
	t.ped_metres = 0;
	t.ped_segs.clear();
	const size_t arms = s.roads.nodes.size() * NODE_ARMS_MAX * CROSS_CACHE_KINDS;
	t.cross_stamp.assign(arms, STAMP_NONE);
	t.cross_walk.assign(arms, 0);
	t.cross_cars.assign(arms, 0);
	t.g.version = s.roads.version;
	s.log.clear_net();
}

void publish_poses(CityState &s) {
	const TrafficParams &p = s.catalog.traffic;
	const TrafficState &t = s.traffic;
	const float cx = s.camera_x.load(), cz = s.camera_z.load();
	const float r2 = p.publish_radius_m * p.publish_radius_m;
	PoseFrame &f = s.poses.back();
	f.vehicles.clear();
	f.people.clear();
	f.seg_speed_share.assign(s.roads.segs.size(), 1.f);
	std::vector<float> sum(s.roads.segs.size(), 0.f), lim(s.roads.segs.size(), 0.f);
	std::vector<int> count(s.roads.segs.size(), 0);
	for (Vehicle &v : s.traffic.vehicles) {
		if (!v.alive || !v.micro) continue; // far vehicles are queues and events, not poses
		float len = p.classes[int(v.type)].length;
		Pose front, rear;
		ends(s, v, v.conn >= 0 ? v.cs : v.s, len, front, rear);
		v.prev = v.cur;
		V2 axis(front.x - rear.x, front.z - rear.z);
		v.cur.x = (front.x + rear.x) * 0.5f;
		v.cur.z = (front.z + rear.z) * 0.5f;
		v.cur.y = (front.y + rear.y) * 0.5f + p.body_lift_m;
		V2 h = axis.len() > MIN_AXIS_M ? axis.norm() : V2(front.fx, front.fz);
		v.cur.fx = h.x;
		v.cur.fz = h.y;
		if (!v.placed) v.prev = v.cur;
		v.placed = true;
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
		rec.px = v.prev.x; rec.py = v.prev.y; rec.pz = v.prev.z; rec.pfx = v.prev.fx; rec.pfz = v.prev.fz;
		rec.x = v.cur.x; rec.y = v.cur.y; rec.z = v.cur.z; rec.fx = v.cur.fx; rec.fz = v.cur.fz;
		rec.id = uint32_t(&v - s.traffic.vehicles.data());
		rec.look = uint32_t(v.type) | (uint32_t(v.paint) << LOOK_PAINT_SHIFT) | (v.brake ? 1u << LOOK_BRAKE_SHIFT : 0u);
		f.vehicles.push_back(rec);
	}
	for (const Pedestrian &w : s.traffic.peds) {
		if (!w.alive) continue;
		const float dx = w.cur.x - cx, dz = w.cur.z - cz;
		if (dx * dx + dz * dz > r2) continue;
		AgentRecord rec;
		rec.px = w.prev.x; rec.py = w.prev.y; rec.pz = w.prev.z; rec.pfx = w.prev.fx; rec.pfz = w.prev.fz;
		rec.x = w.cur.x; rec.y = w.cur.y; rec.z = w.cur.z; rec.fx = w.cur.fx; rec.fz = w.cur.fz;
		rec.id = uint32_t(&w - s.traffic.peds.data());
		rec.look = w.look;
		rec.walked = w.walked;
		f.people.push_back(rec);
	}
	for (size_t seg = 0; seg < f.seg_speed_share.size(); seg++)
		if (count[seg] > 0 && lim[seg] > 0) f.seg_speed_share[seg] = sum[seg] / lim[seg];
	f.tick_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	f.period_ns = int64_t(NS_PER_S / double(s.catalog.tune.sim_tick_hz));
	s.poses.publish();
}

void step(CityState &s, float dt) {
	TrafficState &t = s.traffic;
	apply_network_changes(s);
	route_compact(t);
	t.time += dt;
	t.tick++;
	update_lights(s);
	check_window(s);
	t.route_budget = int(s.catalog.traffic.reroute_budget_per_step);
	step_governor(s, dt);
	spawn_vehicles(s, dt);
	meso_process(s);
	change_lanes(s);
	step_vehicles(s, dt);
	step_pedestrians(s, dt);
}

} // namespace traffic
} // namespace city
