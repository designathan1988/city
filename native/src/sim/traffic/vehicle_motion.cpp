// Vehicle motion: improved IDM with an emergency cap, a jerk limit and a speed drift; the head of each lane follows whoever is on its connector or at the tail of the
// next lane, and stops at the line while the junction does not let it in; every vehicle stops for the lights it sees
// down its route (Roadcraft 4.0 signals). Vehicles cross nodes on connectors.
#include "sim/traffic/traffic_system.h"

#include "core/units.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr float IDM_DELTA = 4.f, MIN_V0 = 0.5f, MIN_GAP = 0.02f, SAFE_PAD_M = 0.3f;
constexpr float WAIT_SPEED = 0.5f, STUCK_SPEED = 0.3f, LC_DECAY = 1.f;
constexpr float MOVING_OFF_SHARE = 0.5f; // box_hold: obstacles that leave it this share of its free-road pickup do not hold it

struct Lead {
	float gap = NO_LEADER, v = 0;
	void take(float g, float lv) {
		if (g < gap) { gap = g; v = lv; }
	}
};

float body(const CityState &s, const Vehicle &v) { return s.catalog.traffic.classes[int(v.type)].length; }

// Acceleration after the emergency cap and the jerk limit.
float finish_accel(const CityState &s, const Vehicle &v, float acc, const Lead &l, float dt) {
	const VehicleClass &c = s.catalog.traffic.classes[int(v.type)];
	acc = clampf(acc, -c.b_emergency, c.a);
	// jerk limit from the last acceleration as felt, never harder than an emergency stop: a standing vehicle's is zero,
	// so it moves off as soon as its way clears (as Roadcraft 4.0 drivers do)
	const float foot = clampf(v.acc, -c.b_emergency, c.a);
	if (acc > foot) acc = std::min(acc, foot + s.catalog.traffic.jerk_up * dt);
	if (l.gap < NO_LEADER) {
		float be = c.b_emergency, room = std::max(0.f, l.gap - SAFE_PAD_M + (l.v > 0 ? l.v * l.v / (2 * be) : 0.f));
		float safe = -be * dt + std::sqrt(be * be * dt * dt + 2 * be * room);
		if (v.v + acc * dt > safe) acc = (safe - v.v) / dt;
	}
	return acc;
}

// Moves `v` by the commanded `acc`; keeps the acceleration that speed change really was (as Roadcraft 4.0 does): a
// vehicle held at a standstill is not braking, whatever its driver asks for.
void integrate(const CityState &s, Vehicle &v, float acc, float dt, float &pos) {
	const TrafficParams &p = s.catalog.traffic;
	float nv = std::max(0.f, v.v + acc * dt);
	pos += (v.v + nv) * 0.5f * dt;
	v.acc = (nv - v.v) / dt;
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
// vehicle may cross the line now; `ci` may change with a detour; `released` tells a jam release; `held` tells a light
// down the street that stops it before this junction.
bool gate(CityState &s, int id, int &ci, float dist, bool head, bool held, bool &released) {
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
	if (v.pass == ci) {
		// a commitment is taken back while the nose is still behind the line: the light went against it and it can still
		// stop (at an amber by the amber's rule, at a red comfortably), a light past the junction stops it before it, or it
		// stands and its exit filled meanwhile; else it goes once the box is clear, never into a junction it cannot leave
		// nor against the light (a jammed junction's release lets its one vehicle in whatever its exit)
		const Light light = light_for(s, v.ds);
		const bool can_stop = light == Light::AMBER ? stops_at_amber(s, v, v.ds, line - p.stop_margin_m, ci) || v.amber_hold == v.ds
				: stop_is_comfortable(s, v, line - p.stop_margin_m);
		const bool stale = held || (can_stop && light != Light::GREEN) ||
				(!released && v.v < WAIT_SPEED && !is_splice(s, t.g.conns[ci].node) && !has_exit(s, t.g.conns[ci], id));
		if (!stale) return box_clear(s, ci);
		release(s, id, ci);
		return false;
	}
	if (held) return false;
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
		int ahead = k + 1; // the first of those ahead it follows: a bicycle in its band is beside it
		while (ahead < int(list.size()) && out_of_the_way(s, v, t.vehicles[list[size_t(ahead)]], ds)) ahead++;
		const bool head = ahead == int(list.size());
		if (!head) {
			const Vehicle &o = t.vehicles[list[size_t(ahead)]];
			l.take(o.s - body(s, o) - v.s, o.v);
		}
		const bool through = v.ri + 1 < v.route.len; // it crosses this lane's junction
		const float line_was = L - v.s - t.g.ds[ds].stop_back;
		if (!through) {
			if (v.s >= v.goal_s) { leaving.push_back(-1 - id); continue; }
			if (v.stop_at >= 0) l.take(std::max(0.f, v.stop_at - v.s + p.classes[int(v.type)].min_gap), 0.f); // the IDM stops s0 short
		} else {
			int ci = pick_connector(s, v);
			if (ci < 0) {
				// the route's turn is not reachable from this lane (a connector went, the lanes were rearranged):
				// another lane of the segment, else a new route; without a way on it leaves the scene
				if (!retry_route(s, id)) { leaving.push_back(-1 - id); continue; }
				return; // retry may have changed the lane: the rest of this one waits for the next tick
			}
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
			v0 = std::min(v0, turn_limit(s, v, t.g.conns[ci], dist));
			const LightStop lights = lights_ahead(s, v, v.ri, v.lane, -v.s);
			v.amber_hold = lights.amber_hold;
			l.take(lights.gap, 0.f);
			// Only the lane head arbitrates at the junction; followers keep the IDM distance to their leader.
			if (head) {
				lead_beyond(s, ci, dist, l);
				bool released = false;
				const bool open = gate(s, id, ci, dist, head, lights.here, released);
				const float line = dist - t.g.ds[ds].stop_back; // the stop line is behind the crosswalk
				if (!open && dist < p.look_ahead_m) l.take(line > -p.stop_margin_m ? std::max(0.f, line - p.stop_margin_m) : dist - p.stop_margin_m, 0.f);
				else if (open && v.s >= L) {
					admit(s, t.g.conns[ci].node, released);
					leaving.push_back(id);
					continue;
				}
				if (v.v < WAIT_SPEED && line >= 0 && line < p.at_line_m) v.stood = true;
			}
		}
		float acc = finish_accel(s, v, idm_accel(p.classes[int(v.type)], v.v, v0, l.gap, l.v), l, dt);
		const float was = v.s;
		integrate(s, v, acc, dt, v.s);
		v.s = std::min(v.s, L);
		signal_call(s, ds, was, v.s);
		// over the stop line on red (probe): from a standstill there, the gate let it run the red
		if (through && line_was > 0 && L - v.s - t.g.ds[ds].stop_back <= 0 && light_for(s, ds) == Light::RED) {
			t.red_entries++;
			t.red_last[0] = float(v.type);
			t.red_last[1] = v.v;
			t.red_last[2] = float(ds);
			t.red_last[3] = float(t.time);
			t.red_from_stop += v.stood ? 1 : 0;
		}
	}
	// past a bicycle in its band: the list stays sorted by s
	if (t.g.ds[ds].cycle_shift != 0) std::stable_sort(list.begin(), list.end(), [&](int a, int b) { return t.vehicles[size_t(a)].s < t.vehicles[size_t(b)].s; });
}

void leave_lane(CityState &s, int ds, int lane, std::vector<int> &leaving) {
	TrafficState &t = s.traffic;
	const int key = ds * MAX_LANES + lane;
	for (int m : leaving) {
		int id = m < 0 ? -1 - m : m;
		Vehicle &v = t.vehicles[size_t(id)];
		int ci = m < 0 ? -1 : pick_connector(s, v);
		if (ci < 0) {
			const bool home = m < 0 && v.ri + 1 >= v.route.len;
			lane_del(s, key, id);
			v.ds = -1;
			despawn_vehicle(s, id, home);
			continue;
		}
		lane_del(s, key, id);
		v.cs = v.s - t.g.ds[size_t(ds)].len;
		v.conn = ci;
		v.pass = -1;
		v.stood = false;
		v.ds = -1;
		conn_push(s, ci, id);
	}
	leaving.clear();
}

// What the k-th vehicle on connector `ci` drives against, at its top speed `top` there: the one ahead (on the connector
// or the exit lane's tail) and walkers on the exit crosswalk, a stop at the connector's end (the entry one is behind it).
void connector_leads(const CityState &s, int ci, size_t k, float top, Lead &car, Lead &walkers) {
	const TrafficState &t = s.traffic;
	const auto &list = t.conn_vehicles[ci];
	const Conn &c = t.g.conns[ci];
	const Vehicle &v = t.vehicles[list[k]];
	if (k > 0) {
		const Vehicle &o = t.vehicles[list[k - 1]];
		car.take(o.cs - body(s, o) - v.cs, o.v);
	} else {
		const auto &lane = t.lane_vehicles[size_t(c.to_ds) * MAX_LANES + c.to_lane];
		if (!lane.empty()) {
			const Vehicle &o = t.vehicles[lane.front()];
			car.take(c.len - v.cs + o.s - body(s, o), o.v);
		}
	}
	if (v.cs < c.len && peds_in_way(s, ci, FRONT_PAST, front_time(s, v, c.len - v.cs, top))) walkers.take(c.len - v.cs, 0.f);
}

void step_connector(CityState &s, int ci, float dt) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	auto &list = t.conn_vehicles[ci];
	const Conn &c = t.g.conns[ci];
	const float vt = conn_speed(s, c);
	for (size_t k = 0; k < list.size(); k++) {
		Vehicle &v = t.vehicles[list[k]];
		Lead l, walkers;
		const float top = top_speed(s, v, vt);
		connector_leads(s, ci, k, top, l, walkers);
		l.take(walkers.gap, walkers.v);
		const LightStop lights = lights_ahead(s, v, v.ri + 1, c.to_lane, c.len - v.cs);
		v.amber_hold = lights.amber_hold;
		l.take(lights.gap, 0.f);
		float acc = finish_accel(s, v, idm_accel(p.classes[int(v.type)], v.v, top, l.gap, l.v), l, dt);
		integrate(s, v, acc, dt, v.cs);
	}
	while (!list.empty() && t.vehicles[size_t(list.front())].cs >= c.len) {
		const int id = list.front();
		conn_pop_front(s, ci);
		Vehicle &v = t.vehicles[size_t(id)];
		v.conn = -1;
		v.ri++;
		v.ds = c.to_ds;
		v.lane = c.to_lane;
		v.s = v.cs - c.len;
		lane_add(s, v.ds * MAX_LANES + v.lane, id);
	}
}

} // namespace

BoxHold box_hold(const CityState &s, int id) {
	const TrafficState &t = s.traffic;
	const Vehicle &v = t.vehicles[size_t(id)];
	const auto &on = t.conn_vehicles[size_t(v.conn)];
	const size_t k = size_t(std::find(on.begin(), on.end(), id) - on.begin());
	if (k >= on.size()) return BoxHold::STARTING;
	const VehicleClass &vc = s.catalog.traffic.classes[int(v.type)];
	const Conn &c = t.g.conns[size_t(v.conn)];
	const float top = top_speed(s, v, conn_speed(s, c));
	Lead car, walkers;
	connector_leads(s, v.conn, k, top, car, walkers);
	const float light = lights_ahead(s, v, v.ri + 1, c.to_lane, c.len - v.cs).gap;
	// what takes most of its free-road acceleration holds it (creeping up to a standing leader is being held by it)
	const float by[int(BoxHold::STARTING)] = { idm_accel(vc, v.v, top, car.gap, car.v), idm_accel(vc, v.v, top, walkers.gap, 0.f),
		idm_accel(vc, v.v, top, light, 0.f) };
	const int most = int(std::min_element(by, by + int(BoxHold::STARTING)) - by);
	return by[most] > MOVING_OFF_SHARE * idm_accel(vc, v.v, top, NO_LEADER, 0.f) ? BoxHold::STARTING : BoxHold(most);
}

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

void step_vehicles(CityState &s, float dt) {
	TrafficState &t = s.traffic;
	t.step_lanes = t.active_lanes; // snapshot: lanes come and go as vehicles leave and enter
	for (int key : t.step_lanes) {
		const int ds = key / MAX_LANES, lane = key % MAX_LANES;
		if (ds < 0 || ds >= int(t.g.ds.size()) || !t.g.ds[size_t(ds)].ok || t.lane_vehicles[size_t(key)].empty()) continue;
		std::vector<int> leaving;
		step_lane(s, ds, lane, dt, leaving);
		leave_lane(s, ds, lane, leaving);
	}
	t.step_conns = t.active_conns;
	for (int ci : t.step_conns)
		if (ci >= 0 && ci < int(t.conn_vehicles.size()) && !t.conn_vehicles[size_t(ci)].empty()) step_connector(s, ci, dt);
	for (int i = int(t.micro_list.size()) - 1; i >= 0; i--) {
		const int id = t.micro_list[size_t(i)];
		if (t.vehicles[size_t(id)].alive && t.vehicles[size_t(id)].stuck > s.catalog.traffic.stuck_despawn_s) {
			despawn_vehicle(s, id, false);
			t.stuck_lost++;
		}
	}
}

} // namespace traffic
} // namespace city
