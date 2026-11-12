// Vehicle motion: improved IDM with an emergency cap, a jerk limit and a speed drift (motion_law.h); the head of each lane follows
// whoever is on its connector or at the tail of the next lane, and stops at the line while the junction does not let it in; every
// vehicle stops for the lights it sees down its route (Roadcraft 4.0 signals). Vehicles cross nodes on connectors
// (connector_motion.cpp).
#include "sim/traffic/kerb_stops.h"

#include "sim/traffic/lane_closures.h"
#include "sim/traffic/light_rule.h"
#include "sim/traffic/motion_law.h"

#include "core/units.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {
constexpr float SAFE_PAD_M = 0.3f;
constexpr float WAIT_SPEED = 0.5f, STUCK_SPEED = 0.3f;
} // namespace

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
	// the lane change's slide: toward its new lane at its own rate, ending there exactly
	v.lc_off = v.lc_off > 0 ? std::max(0.f, v.lc_off - v.lc_rate * dt) : std::min(0.f, v.lc_off + v.lc_rate * dt);
	v.lc_cool = std::max(0.f, v.lc_cool - dt);
	v.moved_tick = s.traffic.tick;
}

namespace {
// What the lane head sees beyond the stop line: a stretch held on the next lane by a body off the lanes, and the connector's last
// vehicle or the next lane's tail.
void lead_beyond(const CityState &s, int ci, float dist, Lead &l) {
	const TrafficState &t = s.traffic;
	const Conn &c = t.g.conns[ci];
	const float shut = closure_ahead(t, c.to_ds * MAX_LANES + c.to_lane, 0.f);
	if (shut >= 0) l.take(dist + c.len + shut, 0.f);
	const auto &on = t.conn_vehicles[ci];
	if (!on.empty()) {
		const Vehicle &b = t.vehicles[on.back()];
		l.take(dist + b.cs - body_length(s, b), b.v);
		return;
	}
	const auto &lane = t.lane_vehicles[size_t(c.to_ds) * MAX_LANES + c.to_lane];
	if (!lane.empty()) {
		const Vehicle &b = t.vehicles[lane.front()];
		l.take(dist + c.len + b.s - body_length(s, b), b.v);
	}
}

// What the k-th vehicle of a lane's list drives against on the lane: the first ahead not out of its way (a bicycle in its band
// is beside it), a closure of the lane ahead, a body changing out of the lane that still stands over it, its kerb stop.
// Returns whether nobody is ahead of it on the lane: it heads the lane.
bool lane_leads(const CityState &s, int ds, int lane, const std::vector<int> &list, int k, Lead &l) {
	const TrafficState &t = s.traffic;
	const int id = list[size_t(k)];
	const Vehicle &v = t.vehicles[size_t(id)];
	int ahead = k + 1;
	while (ahead < int(list.size()) && out_of_the_way(s, v, t.vehicles[size_t(list[size_t(ahead)])], ds)) ahead++;
	const bool head = ahead == int(list.size());
	if (!head) {
		const Vehicle &o = t.vehicles[size_t(list[size_t(ahead)])];
		l.take(o.s - body_length(s, o) - v.s, o.v);
	}
	if (!t.closures.empty()) {
		const float shut = closure_ahead(t, ds * MAX_LANES + v.lane, v.s);
		if (shut >= 0) l.take(shut, 0.f);
	}
	for (int o : t.sliders) {
		const Vehicle &w = t.vehicles[size_t(o)];
		if (o != id && w.slid_from == ds * MAX_LANES + lane && w.s > v.s) l.take(w.s - body_length(s, w) - v.s, w.v);
	}
	if (v.kerb >= 0) l.take(kerb_stop_gap(t, v) + s.catalog.traffic.classes[int(v.type)].min_gap, 0.f); // the IDM stops s0 short
	return head;
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
	if (head && line < p.at_line_m && t.g.ds[v.ds].must_stop && v.v <= p.stop_sign_ms) v.stood_at = v.ds;
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
		// a commitment is taken back while the nose is still behind the line: the light went against it and it can still be
		// kept behind its line (light_rule.h light_recalls: the amber's rule, a red an amber with nothing left; standing behind
		// the line it has stopped), a light past the junction stops it before it, or it stands and its exit filled meanwhile;
		// else it goes once the box is clear, never into a junction it cannot leave nor against the light (a jammed junction's
		// release lets its one vehicle in whatever its exit)
		const bool stale = held || light_recalls(s, v, v.ds, line, ci) ||
				(!released && v.v < WAIT_SPEED && !is_splice(s, t.g.conns[ci].node) && !has_exit(s, t.g.conns[ci], id));
		if (!stale) return box_clear(s, ci, id);
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
	if (!box_clear(s, ci, id)) return false; // it commits only into a clear box
	if (t.g.ds[v.ds].must_stop) {
		t.stop_entries++;
		t.stop_skipped += v.stood_at == v.ds ? 0 : 1;
	}
	commit(s, id, ci);
	return true;
}

// Moves one lane (list sorted by s, back first). Returns ids leaving it: >= 0 onto connectors, -1 - id arrived; and in `retry`
// those whose route's turn this lane cannot make: their way is looked for again once every lane has moved (the search may
// move one to another lane, which this walk must not see changing under it).
void step_lane(CityState &s, int ds, int lane, float dt, std::vector<int> &leaving, std::vector<int> &retry) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	auto &list = t.lane_vehicles[size_t(ds) * MAX_LANES + lane];
	const float L = t.g.ds[ds].len;
	for (int k = int(list.size()) - 1; k >= 0; k--) {
		const int id = list[k];
		Vehicle &v = t.vehicles[id];
		Lead l;
		float v0 = desired_speed(s, v, ds);
		int onto = -1; // the connector it takes at the lane's end
		bool open = false, released = false; // the head: its junction lets it in (by a jam's release)
		const bool head = lane_leads(s, ds, lane, list, k, l);
		bool through = v.ri + 1 < v.route.len; // it crosses this lane's junction
		if (!through && v.s >= v.goal_s) {
			// its trip ends here, on a street in the window: it drives on, on its next trip from here (a trip ends out of the
			// window, in the far model, or in a lot); only a car with nowhere to go (goal_s 0, or no trip found) leaves here
			if (v.goal_s <= 0 || !new_trip(s, id, ds)) { leaving.push_back(-1 - id); continue; }
			t.arrived++;
			through = v.ri + 1 < v.route.len;
		}
		if (const float hold = transit::hold_ahead(s, v, ds, through, head); hold >= 0) l.take(hold, 0.f);
		const float line_was = L - v.s - t.g.ds[ds].stop_back;
		if (!through) {
			if (v.stop_at >= 0) l.take(std::max(0.f, v.stop_at - v.s + p.classes[int(v.type)].min_gap), 0.f); // the IDM stops s0 short
		} else {
			int ci = pick_connector(s, v);
			const float dist = L - v.s;
			// stopped long in the queue of a full lane: reconsider the route, avoiding what is full
			if (ci >= 0 && !head && v.held >= p.reconsider_after_s) {
				v.held = 0;
				if (float(list.size()) >= p.reconsider_full_share * L / p.slot_m && reconsider(s, id)) ci = pick_connector(s, v);
			}
			if (ci >= 0 && t.g.conns[ci].from_lane != v.lane && v.wait > p.claim_after_s && dist < p.look_ahead_m) {
				v.wait = 0;
				if (detour(s, id, -1, false)) ci = pick_connector(s, v);
			}
			if (ci < 0) {
				// the route's turn is not reachable from this lane (a connector went, the lanes were rearranged): it drives on
				// toward its line, which it does not cross, and its way is looked for once every lane has moved
				retry.push_back(id);
				l.take(std::max(0.f, stop_room(p, dist - t.g.ds[ds].stop_back)), 0.f);
			} else {
				onto = ci;
				v0 = std::min(v0, turn_limit(s, v, t.g.conns[ci], dist));
				const LightStop lights = lights_ahead(s, v, v.ri, v.lane, -v.s);
				v.amber_hold = lights.amber_hold;
				l.take(lights.gap, 0.f);
				// Only the lane head arbitrates at the junction; followers keep the IDM distance to their leader.
				if (head) {
					lead_beyond(s, ci, dist, l);
					fork_leads(s, ci, v, -dist, l); // a body that took another branch out of this lane, not yet clear of this one's way
					open = gate(s, id, ci, dist, head, lights.here, released);
					const float line = dist - t.g.ds[ds].stop_back; // the stop line is behind the crosswalk
					if (!open && dist < p.look_ahead_m) l.take(line > -p.stop_margin_m ? std::max(0.f, stop_room(p, line)) : dist - p.stop_margin_m, 0.f);
					if (v.v < WAIT_SPEED && line >= 0 && line < p.at_line_m) v.stood = true;
				}
			}
		}
		float acc = finish_accel(s, v, idm_accel(p.classes[int(v.type)], v.v, v0, l.gap, l.v), l, dt);
		const float was = v.s;
		integrate(s, v, acc, dt, v.s);
		// over its lane's end with the way open: onto its connector this tick, the metres past the end with it (leave_lane)
		const bool out = open && v.s >= L;
		if (!out) v.s = std::min(v.s, L);
		signal_call(s, ds, was, std::min(v.s, L));
		// over the stop line on red (probe): from a standstill there, the gate let it run the red
		if (through && line_was > 0 && L - v.s - t.g.ds[ds].stop_back <= 0 && light_for(s, ds) == Light::RED) {
			t.red_entries++;
			t.red_last[0] = float(v.type);
			t.red_last[1] = v.v;
			t.red_last[2] = float(ds);
			t.red_last[3] = float(t.time);
			t.red_from_stop += v.stood ? 1 : 0;
		}
		if (out) {
			admit(s, t.g.conns[size_t(onto)].node, released);
			leaving.push_back(id);
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
		v.stood_at = -1;
		count_entry(s, ci);
		v.pass = -1;
		v.stood = false;
		v.ds = -1;
		conn_push(s, ci, id);
	}
	leaving.clear();
}

} // namespace

void step_vehicles(CityState &s, float dt) {
	TrafficState &t = s.traffic;
	// the bodies changing lane that have slid off their old lane, or left it (onto a connector, out of the scene), drop out
	t.sliders.erase(std::remove_if(t.sliders.begin(), t.sliders.end(), [&](int id) {
		Vehicle &v = t.vehicles[size_t(id)];
		const bool over = v.alive && v.conn < 0 && v.ds >= 0 && v.slid_from >= 0 && v.slid_from / MAX_LANES == v.ds && std::fabs(v.lc_off) > v.lc_clear;
		if (!over) v.slid_from = -1;
		return !over;
	}), t.sliders.end());
	t.step_lanes = t.active_lanes; // snapshot: lanes come and go as vehicles leave and enter
	std::vector<int> retry;
	for (int key : t.step_lanes) {
		const int ds = key / MAX_LANES, lane = key % MAX_LANES;
		if (ds < 0 || ds >= int(t.g.ds.size()) || !t.g.ds[size_t(ds)].ok || t.lane_vehicles[size_t(key)].empty()) continue;
		std::vector<int> leaving;
		step_lane(s, ds, lane, dt, leaving, retry);
		leave_lane(s, ds, lane, leaving);
		// only this step moves a vehicle along its lane: whether the list is still in order of s is known here (lane_change.cpp)
		const std::vector<int> &list = t.lane_vehicles[size_t(key)];
		t.lane_sorted[size_t(key)] = std::is_sorted(list.begin(), list.end(), [&](int a, int b) { return t.vehicles[size_t(a)].s < t.vehicles[size_t(b)].s; });
	}
	// every lane has moved once: another lane of the segment (the change slides it there), else a new route, else it leaves
	for (int id : retry)
		if (t.vehicles[size_t(id)].alive && t.vehicles[size_t(id)].ds >= 0 && !retry_route(s, id)) despawn_vehicle(s, id, false);
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
