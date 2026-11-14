// Lane changes (MOBIL: Kesting, Treiber, Helbing 2007, "General lane-changing model MOBIL for car-following models"). A change is
// safe when the gaps ahead and behind are open and the new follower need not brake past lc_safe_brake; it is worth it when the
// driver's own gain, plus its politeness times what the two followers gain or lose by it, plus the bias, passes the driver's
// threshold (driver.h). Mandatory changes move toward the lane that has a connector to the next road (after waiting long enough at
// the line any physical gap will do); changes by choice keep right. A change is signalled first (Vehicle::lc_want, lc_lead) and
// made once the signal has run and it is still safe; a mandatory one is signalled while its gap is still waited for.
#include "sim/traffic/traffic_system.h"

#include "sim/traffic/driver.h"
#include "sim/traffic/lane_closures.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr float FORCE_AFTER_S = 5.f, FORCE_GAP_M = 0.5f;

struct Around {
	int lead = -1, follow = -1;
};

// The nearest vehicle at or ahead of `at` and the nearest behind it on lane `key`, `self` left out; of several at the same
// place, the first on the list. A list the step left in order of s (TrafficState::lane_sorted) is searched by place: the lead is
// the first from where `at` would go, the follower the first of the run at the greatest place behind it; any other is walked whole.
Around around(const TrafficState &t, int key, float at, int self) {
	Around a;
	const std::vector<int> &list = t.lane_vehicles[size_t(key)];
	const auto s_of = [&](int i) { return t.vehicles[size_t(list[size_t(i)])].s; };
	if (size_t(key) >= t.lane_sorted.size() || !t.lane_sorted[size_t(key)]) {
		for (int id : list) {
			if (id == self) continue;
			float o = t.vehicles[id].s;
			if (o >= at && (a.lead < 0 || o < t.vehicles[a.lead].s)) a.lead = id;
			if (o < at && (a.follow < 0 || o > t.vehicles[a.follow].s)) a.follow = id;
		}
		return a;
	}
	const int n = int(list.size());
	const int first = int(std::lower_bound(list.begin(), list.end(), at, [&](int o, float x) { return t.vehicles[size_t(o)].s < x; }) - list.begin());
	for (int i = first; i < n && a.lead < 0; i++)
		if (list[size_t(i)] != self) a.lead = list[size_t(i)];
	int j = first - 1;
	while (j >= 0 && list[size_t(j)] == self) j--;
	if (j < 0) return a;
	const float most = s_of(j);
	int k = j;
	for (int i = j - 1; i >= 0 && s_of(i) == most; i--)
		if (list[size_t(i)] != self) k = i;
	a.follow = list[size_t(k)];
	return a;
}

struct View {
	const CityState &s;
	int ds;
	float len(int id) const { return s.catalog.traffic.classes[int(s.traffic.vehicles[id].type)].length; }
	// IDM acceleration of `me`, as its own driver drives, behind `lead` (-1 = free road).
	float accel(int me, int lead) const {
		const Vehicle &v = s.traffic.vehicles[me];
		const VehicleClass c = driver_of(s.catalog.traffic, v);
		float v0 = desired_speed(s, v, ds);
		if (lead < 0) return idm_accel(c, v.v, v0, NO_LEADER, 0.f);
		const Vehicle &l = s.traffic.vehicles[lead];
		return idm_accel(c, v.v, v0, l.s - len(lead) - v.s, l.v);
	}
};

// Whether lane key `dst` leaves `front` metres ahead of the body of `id` and `back` behind it: nobody there, nor a stretch a body
// off the lanes holds, nor a body that changed out of that lane and still stands over it (TrafficState::sliders: it is on its new
// lane's list, its body not yet off the old one).
bool gaps_open(const View &w, int id, int dst, float front, float back) {
	const TrafficState &t = w.s.traffic;
	const Vehicle &v = t.vehicles[id];
	if (stretch_held(t, dst, v.s - w.len(id) - back, v.s + front)) return false;
	for (int o : t.sliders) {
		const Vehicle &u = t.vehicles[size_t(o)];
		if (o != id && u.slid_from == dst && u.s - w.len(o) < v.s + front && u.s > v.s - w.len(id) - back) return false;
	}
	const Around n = around(t, dst, v.s, id);
	if (n.lead >= 0 && t.vehicles[n.lead].s - w.len(n.lead) - v.s < front) return false;
	return n.follow < 0 || v.s - w.len(id) - t.vehicles[n.follow].s >= back;
}

// Incentive of moving `id` into `dst` (or -BIG when unsafe); `force` keeps only the physical gaps.
float evaluate(const View &w, int id, int cur, int dst, float bias, bool force) {
	const TrafficState &t = w.s.traffic;
	const TrafficParams &p = w.s.catalog.traffic;
	const Vehicle &v = t.vehicles[id];
	Around n = around(t, dst, v.s, id), o = around(t, cur, v.s, id);
	if (!gaps_open(w, id, dst, force ? FORCE_GAP_M : p.lc_gap_front_m, force ? FORCE_GAP_M : p.lc_gap_back_m)) return -BIG;
	// it must get its body out of its own lane before it reaches whoever is ahead of it there: the road its path takes until it
	// is a body's width off that lane's line (lane_list.cpp move_vehicle_lane: the path's length, never steeper than lc_slope)
	if (o.lead >= 0) {
		const DirSeg &d = t.g.ds[size_t(w.ds)];
		const float across = std::fabs(d.u[cur % MAX_LANES] - d.u[dst % MAX_LANES]);
		const float path = std::max(v.v * p.lc_duration_s, across / p.lc_slope);
		const float out = across > 0 ? 2 * p.classes[int(v.type)].half_width * path / across : 0.f;
		if (t.vehicles[o.lead].s - w.len(o.lead) - v.s < out) return -BIG;
	}
	if (force) return BIG;
	// what the change does to the two it touches: the one it would pull in front of, and the one it stops holding back
	float others = 0;
	if (n.follow >= 0) {
		const float after = w.accel(n.follow, id);
		if (after < -p.lc_safe_brake) return -BIG;
		others += after - w.accel(n.follow, n.lead);
	}
	if (o.follow >= 0) others += w.accel(o.follow, o.lead) - w.accel(o.follow, id);
	return w.accel(id, n.lead) - w.accel(id, o.lead) + politeness(p, v) * others + bias;
}

bool reaches_next(const CityState &s, const Vehicle &v, int lane) {
	if (v.ri + 1 >= v.route.len) return true;
	const int next = route_at(s.traffic.route_pool, v.route, v.ri + 1);
	return lane_reaches(s, v.ds, lane, next);
}

void move_to(CityState &s, int id, int target) {
	move_vehicle_lane(s, id, target);
	Vehicle &v = s.traffic.vehicles[size_t(id)];
	v.lc_cool = s.catalog.traffic.lc_cooldown_s;
	v.lc_want = -1;
}

// What the vehicle would change to: -1 for nothing. A mandatory change is wanted whether its gap is there or not (`ready` says
// whether it is; `forced`: it waited long enough at its line for any physical gap); one by choice only while it is worth making.
int choose(const CityState &s, int id, bool &ready, bool &forced) {
	const TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const Vehicle &v = t.vehicles[id];
	const DirSeg &d = t.g.ds[v.ds];
	View w{ s, v.ds };
	auto list = [&](int lane) { return v.ds * MAX_LANES + lane; };
	ready = forced = false;
	// the lane it needs: the nearest one its route's next turn leaves from, when its own does not; the lane of its stop
	int needed = -1;
	// a bus by its kind: a line's bus and the city's own alike (a bus lane is public transport's: traffic.json _lanes_doc)
	const bool bus = v.type == VehicleType::BUS, cycle = v.type == VehicleType::BICYCLE;
	const bool on_entry = d.len - v.s <= p.lane_entry_m; // the stretch before the junction where a lane's line lets one in
	bool for_turn = false;
	if (v.ri + 1 < v.route.len) {
		const int next = route_at(t.route_pool, v.route, v.ri + 1);
		if (!lane_reaches(s, v.ds, v.lane, next)) needed = lane_toward(s, v.ds, next, v.lane);
		for_turn = needed >= 0;
	} else if (v.stop_at >= 0 && v.lane != v.stop_lane) {
		needed = v.stop_lane;
	}
	// a bus lane is the buses': who is no bus leaves it, but on the entry stretch when it is the lane of its turn, and a
	// bicycle on its kerb edge; a bus off it takes the nearest one that leads where it goes: its line bars the others,
	// not whom the lane is for (traffic.json _lanes_doc)
	const bool leaves = !bus && !cycle && d.for_buses(v.lane) && !(on_entry && reaches_next(s, v, v.lane));
	const bool joins = bus && d.bus_only != 0 && !d.for_buses(v.lane);
	if (needed < 0 && v.stop_at < 0 && (leaves || joins)) {
		for (int off = 1; off < d.lanes && needed < 0; off++)
			for (int at : { v.lane - off, v.lane + off })
				if (needed < 0 && at >= 0 && at < d.lanes && d.for_buses(at) == joins && (leaves || reaches_next(s, v, at))) needed = at;
	}
	if (needed >= 0 && needed != v.lane) {
		const int cand = v.lane + (needed > v.lane ? 1 : -1);
		// towards its turn a driver crosses a continuous line, or enters a bus lane, only on the entry stretch
		if (for_turn && !on_entry && (d.line_between(v.lane, cand) || (!bus && d.for_buses(cand)))) return -1;
		forced = v.wait > FORCE_AFTER_S;
		// before a signal's stop line the lanes' lines are continuous (traffic.json approach_line_m): the change that was
		// not made in time is made only by who has waited at the line, never on the move
		const bool held = d.stage >= 0 && d.len - v.s <= d.stop_back + p.approach_line_m;
		ready = (!held || forced) && evaluate(w, id, list(v.lane), list(cand), p.lc_mandatory_bias, forced) > p.lc_threshold;
		forced = forced && ready;
		return cand;
	}
	if (d.len - v.s < p.lc_free_m || cycle) return -1;
	const float bias[2] = { p.lc_bias_right, -p.lc_bias_right };
	const int cands[2] = { v.lane - 1, v.lane + 1 };
	for (int k = 0; k < 2; k++) {
		int c = cands[k];
		if (c < 0 || c >= d.lanes || !reaches_next(s, v, c)) continue;
		if (d.line_between(v.lane, c) || (!bus && d.for_buses(c))) continue; // no change by choice across a continuous line
		if (evaluate(w, id, list(v.lane), list(c), bias[k], false) > change_threshold(p, v)) {
			ready = true;
			return c;
		}
	}
	return -1;
}

} // namespace

bool lane_room(const CityState &s, int id, int target) {
	const Vehicle &v = s.traffic.vehicles[size_t(id)];
	return gaps_open(View{ s, v.ds }, id, v.ds * MAX_LANES + target, FORCE_GAP_M, FORCE_GAP_M);
}

void change_lanes(CityState &s) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const uint64_t every = uint64_t(std::max(1.f, p.lc_every_ticks));
	for (int id : t.micro_list) { // only near vehicles change lanes; the far model picks its lane on entry
		Vehicle &v = t.vehicles[size_t(id)];
		if (!v.alive || v.ds < 0 || v.lc_cool > 0 || v.s < p.mouth_clear_m || (t.tick + uint64_t(id)) % every != 0) continue;
		if (t.g.ds[size_t(v.ds)].lanes < 2) {
			v.lc_want = -1;
			continue;
		}
		bool ready = false, forced = false;
		const int target = choose(s, id, ready, forced);
		if (target < 0) {
			v.lc_want = -1;
			continue;
		}
		if (forced) {
			move_to(s, id, target);
			t.lane_forced++;
			continue;
		}
		if (v.lc_want != target) { // the signal goes on now; the move waits for it
			v.lc_want = int8_t(target);
			v.lc_lead = p.lc_signal_s;
			t.lane_signals++;
			continue;
		}
		if (ready && v.lc_lead <= 0) {
			move_to(s, id, target);
			t.lane_changes++;
		}
	}
}

} // namespace traffic
} // namespace city
