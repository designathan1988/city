// Vehicle motion (Roadcraft drivers.ts driveAccel + sim/traffic/micro.ts): improved IDM with an emergency cap, a
// jerk limit and a speed drift; the head of each lane follows whoever is on its connector or at the tail of the
// next lane, and stops at the line while the junction does not let it in. Vehicles cross nodes on connectors.
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr float IDM_DELTA = 4.f, MIN_V0 = 0.5f, MIN_GAP = 0.02f, SAFE_PAD_M = 0.3f;
constexpr float WAIT_SPEED = 0.5f, STUCK_SPEED = 0.3f, DRIFT_RATE = 0.1f, LC_DECAY = 1.f;
constexpr float KMH_PER_MS = 3.6f;

struct Lead {
	float gap = NO_LEADER, v = 0;
	void take(float g, float lv) {
		if (g < gap) { gap = g; v = lv; }
	}
};

float body(const CityState &s, const Vehicle &v) { return s.catalog.traffic.classes[int(v.type)].length; }

// Acceleration after the emergency cap and the jerk limit; also sets the brake light.
float finish_accel(const CityState &s, Vehicle &v, float acc, const Lead &l, float dt) {
	const VehicleClass &c = s.catalog.traffic.classes[int(v.type)];
	acc = clampf(acc, -c.b_emergency, c.a);
	// jerk limit from the last acceleration as felt (an emergency cap can leave a huge negative one: drivers.ts foot)
	const float foot = clampf(v.acc, -c.b_emergency, c.a);
	if (acc > foot) acc = std::min(acc, foot + s.catalog.traffic.jerk_up * dt);
	if (l.gap < NO_LEADER) {
		float be = c.b_emergency, room = std::max(0.f, l.gap - SAFE_PAD_M + (l.v > 0 ? l.v * l.v / (2 * be) : 0.f));
		float safe = -be * dt + std::sqrt(be * be * dt * dt + 2 * be * room);
		if (v.v + acc * dt > safe) acc = (safe - v.v) / dt;
	}
	v.acc = acc;
	return acc;
}

void integrate(const CityState &s, Vehicle &v, float acc, float dt, float &pos) {
	const TrafficParams &p = s.catalog.traffic;
	float nv = std::max(0.f, v.v + acc * dt);
	pos += (v.v + nv) * 0.5f * dt;
	v.v = nv;
	v.brake = acc < p.brake_light_accel || nv < p.brake_light_speed;
	v.stuck = nv < STUCK_SPEED ? v.stuck + dt : 0.f;
	v.wait = nv < WAIT_SPEED ? v.wait + dt : 0.f;
	v.held = nv < WAIT_SPEED ? v.held + dt : 0.f;
	v.lc_off *= std::max(0.f, 1.f - dt * LC_DECAY / std::max(p.lc_duration_s, dt));
	v.lc_cool = std::max(0.f, v.lc_cool - dt);
}

// What the lane head sees beyond the stop line: the connector's last vehicle or the next lane's tail.
void lead_beyond(const CityState &s, int ci, float dist, Lead &l) {
	const TrafficState &t = s.traffic;
	const Conn &c = t.g.conns[ci];
	const auto &on = t.conn_vehicles[ci];
	if (!on.empty()) {
		const Vehicle &b = t.vehicles[on.back()];
		l.take(dist + b.cs - body(s, b), b.v);
		return;
	}
	const auto &lane = t.lane_vehicles[size_t(c.to_ds) * MAX_LANES + c.to_lane];
	if (!lane.empty()) {
		const Vehicle &b = t.vehicles[lane.front()];
		l.take(dist + c.len + b.s - body(s, b), b.v);
	}
}

// The lane head at its junction (Roadcraft micro.ts): detour round a full exit, claim it after a long wait, jammed
// junction release, commitment at the line with the way open (then only the box can stop it). Returns whether the
// vehicle may cross the line now; `ci` may change with a detour; `released` tells a jam release.
bool gate(CityState &s, int id, int &ci, float dist, bool head, bool &released) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	Vehicle &v = t.vehicles[id];
	released = false;
	const float line = dist - t.g.ds[v.ds].stop_back;
	if (head && line < p.at_line_m && ci >= 0) {
		if (v.wait > p.detour_after_s && v.lc_cool <= 0 && t.g.conns[ci].from_lane == v.lane && !has_exit(s, t.g.conns[ci], id)) {
			v.lc_cool = p.lc_cooldown_s;
			if (detour(s, id, ci, true)) ci = pick_connector(s, v);
		}
		if (ci >= 0 && v.wait > p.claim_after_s && light_for(s, v.ds) != Light::RED) t.conn_claim_t[ci] = t.time; // claims the turn
		if (ci >= 0 && v.wait > p.starve_seconds) claim_exit(s, t.g.conns[ci], id);
		if (ci >= 0 && v.wait > p.jam_seconds) released = jam_release(s, ci, id);
	}
	if (ci < 0 || t.g.conns[ci].from_lane != v.lane || dist >= p.look_ahead_m) return false;
	if (v.pass == ci) return box_clear(s, ci);
	if (released) {
		commit(s, id, ci); // the box is its own until it is through
		return true;
	}
	if (!may_enter(s, id, ci, dist)) return false;
	const float b = p.classes[int(v.type)].b, commit_at = std::max(p.commit_min_m, v.v * v.v / (2 * b));
	if (line >= commit_at) return true;
	if (!box_clear(s, ci)) return false; // it commits only into a clear box
	commit(s, id, ci);
	return true;
}

// Moves one lane (list sorted by s, back first). Returns ids leaving it: >= 0 onto connectors, -1 - id arrived.
void step_lane(CityState &s, int ds, int lane, float dt, std::vector<int> &leaving) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	auto &list = t.lane_vehicles[size_t(ds) * MAX_LANES + lane];
	const float L = t.g.ds[ds].len;
	for (int k = int(list.size()) - 1; k >= 0; k--) {
		const int id = list[k];
		Vehicle &v = t.vehicles[id];
		Lead l;
		float v0 = desired_speed(s, v, ds);
		const bool head = k + 1 == int(list.size());
		if (!head) {
			const Vehicle &o = t.vehicles[list[k + 1]];
			l.take(o.s - body(s, o) - v.s, o.v);
		}
		if (v.ri + 1 >= v.route.len) {
			if (v.s >= v.goal_s) { leaving.push_back(-1 - id); continue; }
		} else {
			int ci = pick_connector(s, v);
			if (ci < 0) { leaving.push_back(-1 - id); continue; }
			float dist = L - v.s;
			// stopped long in the queue of a full lane: reconsider the route, avoiding what is full
			if (!head && v.held >= p.reconsider_after_s) {
				v.held = 0;
				if (float(list.size()) >= p.reconsider_full_share * L / p.slot_m && reconsider(s, id)) ci = pick_connector(s, v);
				if (ci < 0) continue;
			}
			if (t.g.conns[ci].from_lane != v.lane && v.wait > p.claim_after_s && dist < p.look_ahead_m) {
				v.wait = 0;
				if (detour(s, id, -1, false)) ci = pick_connector(s, v);
				if (ci < 0) continue;
			}
			if (t.g.conns[ci].turn != Turn::STRAIGHT) {
				float vt = p.turn_speed_kmh / KMH_PER_MS, b = p.classes[int(v.type)].b;
				v0 = std::min(v0, std::sqrt(vt * vt + 2 * b * std::max(0.f, dist - p.stop_margin_m)));
			}
			// Only the lane head arbitrates at the junction; followers keep the IDM distance to their leader.
			if (head) {
				lead_beyond(s, ci, dist, l);
				bool released = false;
				const bool open = gate(s, id, ci, dist, head, released);
				const float line = dist - t.g.ds[ds].stop_back; // the stop line is behind the crosswalk
				if (!open && dist < p.look_ahead_m) l.take(line > -p.stop_margin_m ? std::max(0.f, line - p.stop_margin_m) : dist - p.stop_margin_m, 0.f);
				else if (open && v.s >= L) {
					admit(s, t.g.conns[ci].node, released);
					leaving.push_back(id);
					continue;
				}
			}
		}
		float acc = finish_accel(s, v, idm_accel(p.classes[int(v.type)], v.v, v0, l.gap, l.v), l, dt);
		integrate(s, v, acc, dt, v.s);
		v.s = std::min(v.s, L);
	}
}

void leave_lane(CityState &s, int ds, int lane, std::vector<int> &leaving) {
	TrafficState &t = s.traffic;
	auto &list = t.lane_vehicles[size_t(ds) * MAX_LANES + lane];
	for (int m : leaving) {
		int id = m < 0 ? -1 - m : m;
		list.erase(std::remove(list.begin(), list.end(), id), list.end());
		Vehicle &v = t.vehicles[id];
		int ci = m < 0 ? -1 : pick_connector(s, v);
		if (ci < 0) {
			v.ds = -1;
			despawn_vehicle(s, id, m < 0 && v.ri + 1 >= v.route.len);
			continue;
		}
		v.cs = v.s - t.g.ds[ds].len;
		v.conn = ci;
		v.pass = -1;
		v.ds = -1;
		t.conn_vehicles[ci].push_back(id);
	}
	leaving.clear();
}

void step_connector(CityState &s, int ci, float dt) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	auto &list = t.conn_vehicles[ci];
	const Conn &c = t.g.conns[ci];
	float vt = c.turn == Turn::STRAIGHT ? t.g.ds[c.to_ds].speed : p.turn_speed_kmh / KMH_PER_MS;
	for (size_t k = 0; k < list.size(); k++) {
		Vehicle &v = t.vehicles[list[k]];
		Lead l;
		if (k > 0) {
			const Vehicle &o = t.vehicles[list[k - 1]];
			l.take(o.cs - body(s, o) - v.cs, o.v);
		} else {
			const auto &lane = t.lane_vehicles[size_t(c.to_ds) * MAX_LANES + c.to_lane];
			if (!lane.empty()) {
				const Vehicle &o = t.vehicles[lane.front()];
				l.take(c.len - v.cs + o.s - body(s, o), o.v);
			}
		}
		if (v.cs < c.len && peds_in_way(s, ci, 0.f, front_time(v, c.len - v.cs))) l.take(c.len - v.cs, 0.f); // walkers on the exit crosswalk
		float acc = finish_accel(s, v, idm_accel(p.classes[int(v.type)], v.v, vt * v.vf, l.gap, l.v), l, dt);
		integrate(s, v, acc, dt, v.cs);
	}
	while (!list.empty() && t.vehicles[list.front()].cs >= c.len) {
		int id = list.front();
		list.erase(list.begin());
		Vehicle &v = t.vehicles[id];
		v.conn = -1;
		v.ri++;
		v.ds = c.to_ds;
		v.lane = c.to_lane;
		v.s = v.cs - c.len;
		auto &lane = t.lane_vehicles[size_t(v.ds) * MAX_LANES + v.lane];
		auto at = std::lower_bound(lane.begin(), lane.end(), v.s, [&](int o, float x) { return t.vehicles[o].s < x; });
		lane.insert(at, id);
	}
}

} // namespace

float idm_accel(const VehicleClass &c, float v, float v0, float gap, float vl) {
	v0 = std::max(v0, MIN_V0);
	float free;
	if (v <= v0) {
		const float r = v / v0, r2 = r * r;
		free = c.a * (1 - r2 * r2); // IDM_DELTA is 4: no pow in the per-vehicle path
	} else {
		free = -c.b * (1 - std::pow(v0 / v, c.a * IDM_DELTA / c.b));
	}
	if (gap >= NO_LEADER) return free;
	float s_star = c.min_gap + std::max(0.f, v * c.headway + v * (v - vl) / (2 * std::sqrt(c.a * c.b)));
	float z = s_star / std::max(gap, MIN_GAP);
	if (v <= v0) return z >= 1 ? c.a * (1 - z * z) : (free > 0 ? free * (1 - std::pow(z, 2 * c.a / free)) : free);
	return z >= 1 ? free + c.a * (1 - z * z) : free;
}

float desired_speed(const CityState &s, const Vehicle &v, int ds) {
	const DirSeg &d = s.traffic.g.ds[ds];
	float drift = s.catalog.traffic.speed_drift * std::sin(v.drift + float(s.traffic.time) * DRIFT_RATE);
	return d.speed * v.vf * (1 + drift);
}

void step_vehicles(CityState &s, float dt) {
	TrafficState &t = s.traffic;
	std::vector<int> leaving;
	for (int ds = 0; ds < int(t.g.ds.size()); ds++) {
		if (!t.g.ds[ds].ok) continue;
		for (int lane = 0; lane < t.g.ds[ds].lanes; lane++) {
			if (t.lane_vehicles[size_t(ds) * MAX_LANES + lane].empty()) continue;
			step_lane(s, ds, lane, dt, leaving);
			leave_lane(s, ds, lane, leaving);
		}
	}
	for (int ci = 0; ci < int(t.conn_vehicles.size()); ci++)
		if (!t.conn_vehicles[ci].empty()) step_connector(s, ci, dt);
	for (int id = 0; id < int(t.vehicles.size()); id++)
		if (t.vehicles[id].alive && t.vehicles[id].stuck > s.catalog.traffic.stuck_despawn_s) {
			despawn_vehicle(s, id, false);
			t.stuck_lost++;
		}
}

} // namespace traffic
} // namespace city
