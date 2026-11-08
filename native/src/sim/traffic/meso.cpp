// The far (meso) traffic model, ported from Roadcraft src/sim/traffic/meso.ts: one queue per lane in order of entry,
// one event per vehicle at the end of its lane, and no loop over the fleet per step. A vehicle leaves its lane when
// its event is due: it picks the next lane (waiting at a red or behind a full queue), enters the micro window as a
// drawn vehicle when it gets there, and otherwise queues on the next lane with a fresh event. The pace is the near
// model's (meso_alpha) and a full lane slows it (meso_crowd), so the camera does not change how fast the city drives.
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr size_t EVENT_HEAP_MIN = 64; // the event heap's first room: it doubles from there

void schedule(TrafficState &t, int id, double at) {
	t.vehicles[size_t(id)].t_exit = at;
	if (t.ev_n >= t.ev_t.size()) {
		const size_t cap = std::max<size_t>(t.ev_t.size() * 2, EVENT_HEAP_MIN);
		t.ev_t.resize(cap);
		t.ev_v.resize(cap);
	}
	size_t i = t.ev_n++;
	while (i > 0) {
		const size_t p = (i - 1) / 2;
		if (t.ev_t[p] <= at) break;
		t.ev_t[i] = t.ev_t[p];
		t.ev_v[i] = t.ev_v[p];
		i = p;
	}
	t.ev_t[i] = at;
	t.ev_v[i] = id;
}

int pop_event(TrafficState &t) {
	const int id = t.ev_v[0];
	const double lt = t.ev_t[--t.ev_n];
	const int lv = t.ev_v[t.ev_n];
	size_t i = 0;
	for (;;) {
		size_t c = 2 * i + 1;
		if (c >= t.ev_n) break;
		if (c + 1 < t.ev_n && t.ev_t[c + 1] < t.ev_t[c]) c++;
		if (t.ev_t[c] >= lt) break;
		t.ev_t[i] = t.ev_t[c];
		t.ev_v[i] = t.ev_v[c];
		i = c;
	}
	t.ev_t[i] = lt;
	t.ev_v[i] = lv;
	return id;
}

// Plans again from the lane the vehicle is on to its destination (the last segment of its route).
bool replan(CityState &s, Vehicle &v) {
	TrafficState &t = s.traffic;
	const int goal = v.route.len > 0 ? route_at(t.route_pool, v.route, v.route.len - 1) : -1;
	if (goal < 0 || v.ds < 0) return false;
	std::vector<int> path;
	if (!find_route(s, v.ds, goal, path) || path.empty()) return false;
	route_set(t.route_pool, v.route, t.route_waste, path.data(), int(path.size()));
	v.ri = 0;
	t.reroutes++;
	return true;
}

// The least full lane of `to_ds` that can make the turn after it (all lanes when the route has no next turn).
int pick_lane(const CityState &s, int to_ds, int after, int fallback) {
	const TrafficState &t = s.traffic;
	const DirSeg &d = t.g.ds[size_t(to_ds)];
	int best = -1, best_n = int(BIG);
	for (int lane = 0; lane < d.lanes; lane++) {
		if (after >= 0 && !lane_reaches(s, to_ds, lane, after)) continue;
		const int n = lane_load(t, to_ds * MAX_LANES + lane);
		if (n < best_n) { best_n = n; best = lane; }
	}
	if (best < 0 && fallback >= 0 && fallback < d.lanes) return fallback;
	return best;
}

// The connection the lane should take, changing lane inside the same segment when that lane makes the turn.
int take_conn(CityState &s, int id, int next) {
	Vehicle &v = s.traffic.vehicles[size_t(id)];
	int ci = conn_toward(s, v.ds, v.lane, next);
	if (ci >= 0) return ci;
	const int alt = lane_toward(s, v.ds, next, v.lane);
	if (alt < 0 || alt == v.lane) return -1;
	meso_unlink(s, id);
	v.lane = alt;
	meso_link(s, id, v.ds * MAX_LANES + alt);
	return conn_toward(s, v.ds, alt, next);
}

void advance(CityState &s, int id);

} // namespace

void meso_clear(TrafficState &t) {
	const size_t lanes = t.g.ds.size() * MAX_LANES;
	t.meso_head.assign(lanes, -1);
	t.meso_tail.assign(lanes, -1);
	t.meso_count.assign(lanes, 0);
	t.lane_last_exit.assign(lanes, 0.0);
	t.ev_t.clear();
	t.ev_v.clear();
	t.ev_n = 0;
}

void meso_link(CityState &s, int id, int key) {
	if (key < 0) return;
	TrafficState &t = s.traffic;
	if (size_t(key) >= t.meso_tail.size()) return;
	Vehicle &v = t.vehicles[size_t(id)];
	if (v.ml == key) return;
	meso_unlink(s, id);
	v.ml = key;
	v.mnext = -1;
	v.mprev = t.meso_tail[size_t(key)];
	if (v.mprev >= 0) t.vehicles[size_t(v.mprev)].mnext = id;
	else t.meso_head[size_t(key)] = id;
	t.meso_tail[size_t(key)] = id;
	t.meso_count[size_t(key)]++;
}

void meso_unlink(CityState &s, int id) {
	TrafficState &t = s.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	const int key = v.ml;
	if (key < 0) return;
	if (size_t(key) < t.meso_head.size()) {
		if (v.mprev >= 0) t.vehicles[size_t(v.mprev)].mnext = v.mnext;
		else t.meso_head[size_t(key)] = v.mnext;
		if (v.mnext >= 0) t.vehicles[size_t(v.mnext)].mprev = v.mprev;
		else t.meso_tail[size_t(key)] = v.mprev;
		if (t.meso_count[size_t(key)] > 0) t.meso_count[size_t(key)]--;
	}
	v.ml = -1;
	v.mnext = -1;
	v.mprev = -1;
}

void meso_enter(CityState &s, int id, int key, bool spawned) {
	const TrafficParams &p = s.catalog.traffic;
	TrafficState &t = s.traffic;
	const int ds = key / MAX_LANES;
	if (ds < 0 || ds >= int(t.g.ds.size()) || !t.g.ds[size_t(ds)].ok || key % MAX_LANES >= t.g.ds[size_t(ds)].lanes) return;
	Vehicle &v = t.vehicles[size_t(id)];
	// a drawn vehicle leaving the window also leaves its lane list (else a ghost stays there and is stepped forever)
	if (v.micro && v.conn < 0 && v.ds >= 0) lane_del(s, v.ds * MAX_LANES + v.lane, id);
	micro_remove(t, id);
	v.micro = false;
	v.conn = -1;
	v.pass = -1;
	v.ds = ds;
	v.lane = key % MAX_LANES;
	meso_link(s, id, key);
	const DirSeg &d = t.g.ds[size_t(ds)];
	const float L = d.len, lanes = float(std::max(1, d.lanes));
	v.t_in = t.time;
	v.s0 = spawned ? std::max(0.f, std::min(v.s, L)) : 0.f;
	const float rest = spawned ? std::max(0.f, L - v.s0) : L;
	float wait = 0;
	int next = v.ri + 1 < v.route.len ? route_at(t.route_pool, v.route, v.ri + 1) : -1;
	int ci = next >= 0 ? conn_toward(s, ds, v.lane, next) : -1;
	if (ci < 0 && next >= 0) {
		const int alt = lane_toward(s, ds, next, v.lane);
		if (alt >= 0) ci = conn_toward(s, ds, alt, next);
	}
	if (ci >= 0) wait = junction_cost(s, t.g.conns[size_t(ci)]);
	const bool lit = d.to >= 0 && d.to < int(t.g.junctions.size()) && t.g.junctions[size_t(d.to)].ctl == Control::SIGNAL;
	const float headway = lit ? p.meso_headway_light_s : wait >= p.meso_wait_long_s ? p.meso_headway_long_s : wait >= p.meso_wait_mid_s ? p.meso_headway_mid_s : p.meso_headway_base_s;
	const float occ = std::min(1.f, float(lane_load(t, key)) * s.catalog.traffic.slot_m / (L * lanes));
	const float run = rest / std::max(d.speed * v.vf, p.meso_speed_floor_ms) * (1 + s.catalog.traffic.meso_crowd * occ * occ) + wait;
	const double due = std::max(t.time + double(run * s.catalog.traffic.meso_alpha), t.lane_last_exit[size_t(key)] + double(headway / lanes));
	t.lane_last_exit[size_t(key)] = due;
	schedule(t, id, due);
}

void meso_process(CityState &s) {
	TrafficState &t = s.traffic;
	while (t.ev_n > 0 && t.ev_t[0] <= t.time) {
		const double at = t.ev_t[0];
		const int id = pop_event(t);
		if (id < 0 || id >= int(t.vehicles.size())) continue;
		Vehicle &v = t.vehicles[size_t(id)];
		if (!v.alive || v.micro || v.t_exit != at) continue;
		advance(s, id);
	}
}

namespace {

// End of the lane in meso mode: pick the next lane, wait for a red or a full queue, or hand the vehicle to the near
// model when the lane it enters is inside the window (Roadcraft meso.ts advance).
void advance(CityState &s, int id) {
	const TrafficParams &p = s.catalog.traffic;
	TrafficState &t = s.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	const LaneGraph &g = t.g;
	int ds = v.ds;
	const int key = v.ml >= 0 ? v.ml : ds * MAX_LANES + v.lane;
	if (ds < 0 || ds >= int(g.ds.size()) || !g.ds[size_t(ds)].ok) { despawn_vehicle(s, id, false); return; }
	if (v.ri + 1 >= v.route.len) { despawn_vehicle(s, id, true); return; } // the lane's end is past its goal: arrived
	if (in_window(s, ds) && !t.lane_vehicles[size_t(key)].empty()) { // wait behind the drawn queue, never pass it unseen
		materialize(s, key);
		if (!v.alive || v.micro) return;
		schedule(t, id, t.time + p.meso_window_retry_s);
		return;
	}
	int next = route_at(t.route_pool, v.route, v.ri + 1);
	if (next < 0) { despawn_vehicle(s, id, false); return; }
	int ci = take_conn(s, id, next);
	if (ci < 0) {
		if (!replan(s, v)) { despawn_vehicle(s, id, false); return; }
		next = v.ri + 1 < v.route.len ? route_at(t.route_pool, v.route, v.ri + 1) : -1;
		if (next < 0) { despawn_vehicle(s, id, false); return; }
		ci = take_conn(s, id, next);
	}
	if (ci < 0) { despawn_vehicle(s, id, false); return; }
	ds = v.ds;
	if (light_for(s, ds) != Light::GREEN) { // the far model obeys the lights: wait for its next green
		schedule(t, id, std::max(t.time + double(p.meso_min_wait_s), next_green(s, ds, t.time)));
		return;
	}
	signal_call_far(s, ds);
	const Conn &c = g.conns[size_t(ci)];
	const int to = c.to_ds;
	if (to < 0 || to >= int(g.ds.size()) || !g.ds[size_t(to)].ok) { despawn_vehicle(s, id, false); return; }
	const int after = v.ri + 2 < v.route.len ? route_at(t.route_pool, v.route, v.ri + 2) : -1;
	const int to_lane = pick_lane(s, to, after, c.to_lane);
	if (to_lane < 0) { despawn_vehicle(s, id, false); return; }
	const int to_key = to * MAX_LANES + to_lane;
	const float L = g.ds[size_t(to)].len;
	const int room = std::max(1, int(L / s.catalog.traffic.slot_m)), fit = std::max(room, int(L / p.meso_fit_m));
	const int load = lane_load(t, to_key);
	if (load >= room) { // a full lane holds the vehicle at the end of its own
		v.wait += float(p.meso_queue_retry_s);
		schedule(t, id, t.time + (load >= fit ? double(p.meso_window_retry_s) : double(p.meso_queue_retry_s)));
		return;
	}
	const bool near = in_window(s, to);
	const int tail = t.lane_vehicles[size_t(to_key)].empty() ? -1 : t.lane_vehicles[size_t(to_key)].back();
	if (near && tail >= 0) { // behind drawn vehicles: it waits to fit behind them (never passes them unseen)
		const Vehicle &o = t.vehicles[size_t(tail)];
		if (o.s < s.catalog.traffic.classes[int(o.type)].length + s.catalog.traffic.meso_s0_m + p.meso_handover_pad_m) {
			schedule(t, id, t.time + double(p.meso_handover_retry_s));
			return;
		}
	}
	meso_unlink(s, id);
	v.wait = 0;
	v.held = 0;
	count_entry(s.traffic, v.ds = to);
	v.lane = to_lane;
	v.ri++;
	if (near) { // enters the window: becomes micro at the start of the lane
		v.s = 0;
		v.conn = -1;
		float speed = g.ds[size_t(to)].speed * v.vf;
		if (tail >= 0) {
			const Vehicle &o = t.vehicles[size_t(tail)];
			const float gap = o.s - s.catalog.traffic.classes[int(o.type)].length;
			speed = std::min(speed, std::max(p.meso_min_handover_ms, gap / s.catalog.traffic.meso_head_s));
		} else {
			speed = std::min(speed, p.meso_free_handover_ms) * p.meso_handover_share;
		}
		v.v = speed;
		micro_add(t, id);
		lane_add(s, to_key, id);
		return;
	}
	v.s = 0;
	meso_enter(s, id, to_key, false);
}

} // namespace
} // namespace traffic
} // namespace city
