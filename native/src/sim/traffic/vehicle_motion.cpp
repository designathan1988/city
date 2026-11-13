// Vehicle motion: each driver's own following law (driver.h, vehicle_speed.cpp follow_accel) with an emergency cap, a pedal that
// moves at a driver's pace and a speed drift (motion_law.h); it slows for the bends of its lane and of the turn and street ahead;
// the head of each lane follows whoever is on its connector or at the tail of the next lane (lane_leads.cpp), and stops at the line
// while the junction does not let it in; every vehicle stops for the lights it sees down its route. Vehicles cross nodes on
// connectors (connector_motion.cpp). A car the camera's window shows never vanishes from it: with no trip to make it drives on by
// a way its lane has, or stands, and leaves once out of sight (unseen_exit.cpp).
#include "sim/traffic/kerb_stops.h"

#include "sim/traffic/driver.h"
#include "sim/traffic/light_rule.h"
#include "sim/traffic/motion_law.h"
#include "sim/traffic/unseen_exit.h"

#include "core/units.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace city {
namespace traffic {

namespace {
constexpr float SAFE_PAD_M = 0.3f;
constexpr float WAIT_SPEED = 0.5f, STUCK_SPEED = 0.3f;
} // namespace

float finish_accel(const CityState &s, const Vehicle &v, float acc, const Lead &l, float dt) {
	const TrafficParams &p = s.catalog.traffic;
	const VehicleClass c = driver_of(p, v);
	acc = clampf(acc, -c.b_emergency, c.a);
	// the pedal moves at a driver's pace from the last acceleration as felt (a standing vehicle's is zero, so it moves off as
	// soon as its way clears): easing on, and in ordinary braking; a brake past the comfortable one, or at a crawl, is not held back
	const float foot = clampf(v.acc, -c.b_emergency, c.a);
	if (acc > foot) acc = std::min(acc, foot + p.jerk_up * dt);
	else if (acc >= -c.b && v.v >= p.follow_crawl_ms) acc = std::max(acc, foot - p.jerk_brake * dt);
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
	const float moved = (v.v + nv) * 0.5f * dt;
	pos += moved;
	v.acc = (nv - v.v) / dt;
	v.v = nv;
	v.brake = acc < p.brake_light_accel || nv < p.brake_light_speed;
	v.stuck = nv < STUCK_SPEED ? v.stuck + dt : 0.f;
	v.wait = nv < WAIT_SPEED ? v.wait + dt : 0.f;
	v.held = nv < WAIT_SPEED ? v.held + dt : 0.f;
	// the lane change's path: toward its new lane by the metres it drove, ending there exactly; standing, it stays where it is
	v.lc_off = v.lc_off > 0 ? std::max(0.f, v.lc_off - v.lc_rate * moved) : std::min(0.f, v.lc_off + v.lc_rate * moved);
	v.lc_cool = std::max(0.f, v.lc_cool - dt);
	v.lc_lead = std::max(0.f, v.lc_lead - dt);
	v.moved_tick = s.traffic.tick;
}

namespace {
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
		if (v.wait > patience(p, v, p.detour_after_s) && v.lc_cool <= 0 && t.g.conns[ci].from_lane == v.lane && !has_exit(s, t.g.conns[ci], id)) {
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
	const float b = driver_of(p, v).b, commit_at = std::max(p.commit_min_m, v.v * v.v / (2 * b));
	if (line >= commit_at) return true;
	if (!box_clear(s, ci, id)) return false; // it commits only into a clear box
	if (t.g.ds[v.ds].must_stop) {
		t.stop_entries++;
		t.stop_skipped += v.stood_at == v.ds ? 0 : 1;
	}
	commit(s, id, ci);
	return true;
}

// Over the stop line on red (probe): counted, with who and where, and whether from a standstill there (the gate let it run the red).
void note_red_entry(TrafficState &t, const Vehicle &v, int ds) {
	t.red_entries++;
	t.red_last[0] = float(v.type);
	t.red_last[1] = v.v;
	t.red_last[2] = float(ds);
	t.red_last[3] = float(t.time);
	t.red_from_stop += v.stood ? 1 : 0;
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
		const VehicleClass drv = driver_of(p, v);
		float curve = bend_ahead(s, v, ds, lane, v.s); // the braking curve of the bends ahead: its lane's, its turn's, the next street's
		float v0 = std::min(desired_speed(s, v, ds), curve);
		int onto = -1; // the connector it takes at the lane's end
		bool open = false, released = false; // the head: its junction lets it in (by a jam's release)
		const bool head = lane_leads(s, ds, lane, list, k, l);
		bool through = v.ri + 1 < v.route.len; // it crosses this lane's junction
		if (!through && v.s >= v.goal_s) {
			// its trip ends here, on a street in the window: it drives on, on its next trip from here (a trip ends out of the
			// window, in the far model, or in a lot); a car with nowhere to go (goal_s 0) leaves here. With no trip found it
			// leaves only out of sight: seen, it drives on by a way its lane has, or stands at its lane's end, asking again on
			// its turn (never every tick)
			if (v.goal_s <= 0) { leaving.push_back(-1 - id); continue; }
			const bool asks = v.v >= WAIT_SPEED || (t.tick + uint64_t(id)) % uint64_t(std::max(1.f, p.lc_every_ticks)) == 0;
			if (asks && new_trip(s, id, ds)) t.arrived++;
			else if (!window_shows(s, v)) { leaving.push_back(-1 - id); continue; }
			else if (!asks || !drive_on(s, id)) l.take(std::max(0.f, L - v.s), 0.f);
			through = v.ri + 1 < v.route.len;
		}
		if (const float hold = transit::hold_ahead(s, v, ds, through, head); hold >= 0) l.take(hold, 0.f);
		const float line_was = L - v.s - t.g.ds[ds].stop_back;
		if (!through) {
			if (v.stop_at >= 0) l.take(std::max(0.f, v.stop_at - v.s + drv.min_gap), 0.f); // the IDM stops its standing gap short
		} else {
			int ci = pick_connector(s, v);
			const float dist = L - v.s;
			// stopped long in the queue of a full lane: reconsider the route, avoiding what is full
			if (ci >= 0 && !head && v.held >= patience(p, v, p.reconsider_after_s)) {
				v.held = 0;
				if (float(list.size()) >= p.reconsider_full_share * L / p.slot_m && reconsider(s, id)) ci = pick_connector(s, v);
			}
			if (ci >= 0 && t.g.conns[ci].from_lane != v.lane && v.wait > p.claim_after_s && dist < p.look_ahead_m) {
				v.wait = 0;
				if (detour(s, id, -1, false)) ci = pick_connector(s, v);
			}
			if (ci < 0) {
				// its lane does not make the route's turn: it works across to the lane that does (lane_change.cpp, signalled), no
				// faster than lets those changes fit in the room it has left, and never over its line; with no such lane (a
				// connector went, the lanes were rearranged) its way is looked for once every lane has moved
				const int needed = lane_toward(s, ds, route_at(t.route_pool, v.route, v.ri + 1), v.lane);
				if (needed < 0 || needed == v.lane) retry.push_back(id);
				else v0 = std::min(v0, std::max(p.lc_crawl_ms, (dist - t.g.ds[ds].stop_back) / (float(std::abs(needed - v.lane)) * p.lc_duration_s)));
				l.take(std::max(0.f, stop_room(p, dist - t.g.ds[ds].stop_back)), 0.f);
			} else {
				onto = ci;
				const Conn &c = t.g.conns[size_t(ci)];
				curve = std::min(curve, std::min(turn_limit(s, v, c, dist), bend_ahead(s, v, c.to_ds, c.to_lane, -(dist + c.len))));
				v0 = std::min(v0, curve);
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
		float acc = held_to(finish_accel(s, v, follow_accel(drv, p.follow_coolness, v.v, v0, l.gap, l.v, l.a), l, dt), v.v, curve, drv.b, dt);
		const float was = v.s;
		integrate(s, v, acc, dt, v.s);
		// over its lane's end with the way open: onto its connector this tick, the metres past the end with it (leave_lane)
		const bool out = open && v.s >= L;
		if (!out) v.s = std::min(v.s, L);
		signal_call(s, ds, was, std::min(v.s, L));
		if (through && line_was > 0 && L - v.s - t.g.ds[ds].stop_back <= 0 && light_for(s, ds) == Light::RED) note_red_entry(t, v, ds);
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
		v.lc_want = -1;
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
	for (int id : retry) {
		const Vehicle &v = t.vehicles[size_t(id)];
		if (!v.alive || v.ds < 0) continue;
		// standing with no way found, it looks again on its turn, not every tick (a search each)
		if (v.v < WAIT_SPEED && (t.tick + uint64_t(id)) % uint64_t(std::max(1.f, s.catalog.traffic.lc_every_ticks)) != 0) continue;
		if (retry_route(s, id)) continue;
		if (!window_shows(s, v)) despawn_vehicle(s, id, false);
		else drive_on(s, id); // seen, it drives on by a way its lane has, or stands at its line
	}
	t.step_conns = t.active_conns;
	for (int ci : t.step_conns)
		if (ci >= 0 && ci < int(t.conn_vehicles.size()) && !t.conn_vehicles[size_t(ci)].empty()) step_connector(s, ci, dt);
	for (int i = int(t.micro_list.size()) - 1; i >= 0; i--) {
		const int id = t.micro_list[size_t(i)];
		const Vehicle &v = t.vehicles[size_t(id)];
		if (v.alive && v.stuck > s.catalog.traffic.stuck_despawn_s && !window_shows(s, v)) { // a car seen stays: it goes once out of sight
			despawn_vehicle(s, id, false);
			t.stuck_lost++;
		}
	}
}

} // namespace traffic
} // namespace city
