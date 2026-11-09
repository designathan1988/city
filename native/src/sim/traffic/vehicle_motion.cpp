// Vehicle motion on the lanes: improved IDM with an emergency cap, a jerk limit and a speed drift (motion_law.h); the head of each lane
// follows whoever is on its connector or at the tail of the next lane, and stops at the line while the junction does not let it in; every
// vehicle stops for the lights it sees down its route (Roadcraft 4.0 signals). Vehicles cross nodes on connectors (connector_motion.cpp).
#include "sim/traffic/kerb_stops.h"
#include "sim/traffic/motion_law.h"
#include "sim/traffic/sight.h"
#include "sim/traffic/lane_shift.h"

#include "sim/traffic/lane_closures.h"

#include "core/units.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

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
		// a commitment is taken back while the nose is still behind the line: the light went against it and it can still
		// stop (at an amber by the amber's rule, at a red comfortably), a light past the junction stops it before it, or it
		// stands and its exit filled meanwhile; else it goes once the box is clear, never into a junction it cannot leave
		// nor against the light (a jammed junction's release lets its one vehicle in whatever its exit)
		const Light light = light_for(s, v.ds);
		const bool can_stop = light == Light::AMBER ? stops_at_amber(s, v, v.ds, line - p.stop_margin_m, ci) || v.amber_hold == v.ds
				: stop_is_comfortable(s, v, line - p.stop_margin_m);
		const bool stale = held || (can_stop && light != Light::GREEN) ||
				(!released && v.v < p.vehicle_wait_speed && !is_splice(s, t.g.conns[ci].node) && !has_exit(s, t.g.conns[ci], id));
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
	if (t.g.ds[v.ds].must_stop) {
		t.stop_entries++;
		t.stop_skipped += v.stood_at == v.ds ? 0 : 1;
	}
	commit(s, id, ci);
	return true;
}

// Whether vehicle `id` stays on its lane this step, and what its trip has it stop for there: false when it leaves the lane here (its goal
// reached, its lane leading nowhere on its way, or its ended trip now out of sight). An ended trip stands short of the lane's end (sight.h).
bool trip_goes_on(CityState &s, int id, bool through, float L, float line_was, float v0, Lead &l) {
	const TrafficParams &p = s.catalog.traffic;
	const Vehicle &v = s.traffic.vehicles[size_t(id)];
	if (v.ended) {
		if (!vehicle_may_show(s, v.cur.x, v.cur.z)) return false;
		l.take(std::max(0.f, L - v.s - p.stop_margin_m), 0.f);
	} else if (!through) {
		if (v.s >= v.goal_s) return false;
		if (v.stop_at >= 0) l.take(std::max(0.f, v.stop_at - v.s + p.classes[int(v.type)].min_gap), 0.f); // the IDM stops s0 short
	} else {
		float hold = -1; // its lane does not lead on: worked across to one that does (lane_shift.h), held short of its end
		if (!way_on(s, id, line_was, v0, hold)) return false;
		if (hold >= 0) l.take(hold, 0.f);
	}
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
		// whoever is changing out of this lane still stands in it till its body has left (lane_shift.h): followed as a leader is
		for (int o : t.lane_shadows[size_t(ds) * MAX_LANES + lane]) {
			const Vehicle &sh = t.vehicles[size_t(o)];
			if (o != id && sh.s > v.s) l.take(sh.s - body(s, sh) - v.s, sh.v);
		}
		if (!t.closures.empty()) {
			const float shut = closure_ahead(t, ds * MAX_LANES + v.lane, v.s);
			if (shut >= 0) l.take(shut, 0.f);
		}
		if (v.kerb >= 0) l.take(kerb_stop_gap(t, v) + p.classes[int(v.type)].min_gap, 0.f); // the IDM stops s0 short
		const bool through = !v.ended && v.ri + 1 < v.route.len; // it crosses this lane's junction
		if (const float hold = transit::hold_ahead(s, v, ds, through, head); hold >= 0) l.take(hold, 0.f);
		const float line_was = L - v.s - t.g.ds[ds].stop_back;
		if (!trip_goes_on(s, id, through, L, line_was, v0, l)) {
			leaving.push_back(-1 - id);
			continue;
		}
		if (through && pick_connector(s, v) >= 0) {
			int ci = pick_connector(s, v);
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
				if (v.v < p.vehicle_wait_speed && line >= 0 && line < p.at_line_m) v.stood = true;
			}
		}
		float acc = finish_accel(s, v, idm_accel(p.classes[int(v.type)], v.v, v0, l.gap, l.v), l, dt);
		const float was = v.s;
		integrate(s, v, acc, dt, v.s);
		v.s = std::min(v.s, L);
		shift_step(s, id, v.s - was); // a lane change's path goes on with the road driven
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
		if (ci < 0 && !v.ended && drives_on(s, v) && new_trip(s, id, ds)) continue; // somewhere else to go: it stays on its lane till its end
		if (ci < 0 && !v.ended && v.bus < 0 && vehicle_may_show(s, v.cur.x, v.cur.z)) {
			trip_ends_here(s, id); // in sight: it stands short of its lane's end until it is out of sight (sight.h)
			continue;
		}
		if (ci < 0) {
			const bool home = m < 0 && (v.ended || v.ri + 1 >= v.route.len);
			lane_del(s, key, id);
			v.ds = -1;
			despawn_vehicle(s, id, home);
			continue;
		}
		lane_del(s, key, id);
		shift_end(t, id); // onto the junction: no lane left behind it to stand in
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
	t.step_lanes = t.active_lanes; // snapshot: lanes come and go as vehicles leave and enter
	for (int key : t.step_lanes) {
		const int ds = key / MAX_LANES, lane = key % MAX_LANES;
		if (ds < 0 || ds >= int(t.g.ds.size()) || !t.g.ds[size_t(ds)].ok || t.lane_vehicles[size_t(key)].empty()) continue;
		std::vector<int> leaving;
		step_lane(s, ds, lane, dt, leaving);
		leave_lane(s, ds, lane, leaving);
		// only this step moves a vehicle along its lane: whether the list is still in order of s is known here (lane_change.cpp)
		const std::vector<int> &list = t.lane_vehicles[size_t(key)];
		t.lane_sorted[size_t(key)] = std::is_sorted(list.begin(), list.end(), [&](int a, int b) { return t.vehicles[size_t(a)].s < t.vehicles[size_t(b)].s; });
	}
	step_connectors(s, dt);
}

} // namespace traffic
} // namespace city
