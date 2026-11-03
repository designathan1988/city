// The traffic step called by Simulation::step: keeps the lane graph in step with the road network (a road edit
// rebuilds it and restarts the fleet), runs demand, lane changes, motion and pedestrians, and computes the poses
// render/agents draws (body centre between the front and rear points, so long vehicles follow curves).
#include "sim/traffic/traffic_system.h"

#include "core/units.h"

#include <algorithm>
#include <chrono>

namespace city {
namespace traffic {

namespace {

constexpr float MIN_AXIS_M = 0.5f;

void rebuild(CityState &s) {
	TrafficState &t = s.traffic;
	build_lane_graph(s);
	t.lane_vehicles.assign(t.g.ds.size() * MAX_LANES, {});
	t.conn_vehicles.assign(t.g.conns.size(), {});
	reset_admission(s);
	// Rebuild the graph around the fleet instead of restarting it: a road edit must not delete every trip.
	for (int id = 0; id < int(t.vehicles.size()); id++) {
		Vehicle &v = t.vehicles[id];
		if (!v.alive) continue;
		const bool lane_ok = v.conn < 0 && v.ds >= 0 && v.ds < int(t.g.ds.size()) && t.g.ds[v.ds].ok && v.lane < t.g.ds[v.ds].lanes;
		if (!lane_ok) {
			despawn_vehicle(s, id, false);
			continue;
		}
		auto &list = t.lane_vehicles[size_t(v.ds) * MAX_LANES + v.lane];
		auto at = std::lower_bound(list.begin(), list.end(), v.s, [&](int o, float x) { return t.vehicles[o].s < x; });
		list.insert(at, id);
	}
	t.node_crossers.assign(s.roads.nodes.size(), {});
	for (Pedestrian &w : t.peds) {
		if (!w.alive) continue;
		w.cross_seg = -1;
		w.way_node = -1;
		w.wn = w.wi = 0;
		w.mode = PedMode::SIDEWALK;
	}
	t.spawn_credit = 0;
}

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
	Vehicle &v = t.vehicles[id];
	if (!v.alive) return;
	if (v.conn >= 0) {
		auto &l = t.conn_vehicles[v.conn];
		l.erase(std::remove(l.begin(), l.end(), id), l.end());
	} else if (v.ds >= 0) {
		auto &l = t.lane_vehicles[size_t(v.ds) * MAX_LANES + v.lane];
		l.erase(std::remove(l.begin(), l.end(), id), l.end());
	}
	t.route_waste += size_t(v.route.cap);
	v = Vehicle();
	t.free_vehicles.push_back(id);
	t.vehicle_count--;
	(arrived ? t.arrived : t.lost)++;
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
		if (!v.alive) continue;
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
		rec.look = uint32_t(v.type) | (uint32_t(v.paint) << 8) | (v.brake ? 1u << 16 : 0u);
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
	if (t.g.version != s.roads.version) rebuild(s);
	route_compact(t);
	t.time += dt;
	t.tick++;
	update_lights(s);
	t.route_budget = int(s.catalog.traffic.reroute_budget_per_step);
	step_governor(s, dt);
	spawn_vehicles(s, dt);
	change_lanes(s);
	step_vehicles(s, dt);
	step_pedestrians(s, dt);
}

} // namespace traffic
} // namespace city
