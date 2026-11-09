// Lane changes, as the Roadcraft lab decides them (sim/vehicles/laneChange.ts): mandatory toward the lane with a connector to the next
// road (or the lane of the place it stops at), discretionary by MOBIL (own IDM gain, the new follower's braking, a keep-right bias over a
// threshold). The driver signals first (lc_signal_s) and moves only into room: every body of the lane it goes to, shadows of those still
// changing out of it too, far enough off, and the road of the change ahead free of anybody slower and inside the lane. The move is a path
// (lane_shift.h), never a jump: no gap is ever forced.
#include "sim/traffic/traffic_system.h"
#include "sim/traffic/lane_shift.h"

#include <cmath>

#include <algorithm>

namespace city {
namespace traffic {

namespace {


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
	// IDM acceleration of `me` behind `lead` (-1 = free road).
	float accel(int me, int lead) const {
		const Vehicle &v = s.traffic.vehicles[me];
		const VehicleClass &c = s.catalog.traffic.classes[int(v.type)];
		float v0 = desired_speed(s, v, ds);
		if (lead < 0) return idm_accel(c, v.v, v0, NO_LEADER, 0.f);
		const Vehicle &l = s.traffic.vehicles[lead];
		return idm_accel(c, v.v, v0, l.s - len(lead) - v.s, l.v);
	}
};

// The room a body needs from another at `speed` (the lab's gapIsSafe): the larger of the jam gap and its standstill gap, and a share of
// its headway at its speed.
float room_for(const TrafficParams &p, const Vehicle &v, float speed) {
	const VehicleClass &c = p.classes[int(v.type)];
	return std::max(p.lc_jam_gap_m, c.min_gap) + speed * c.headway * p.lc_follow_share;
}

// Whether lane `dst` has room beside `id` now (the lab's gapIsSafe): every body in it, those changing out of it as well; the road of the
// change and a margin free of anybody slower and inside the lane.
bool room_beside(const View &w, int id, int dst) {
	const TrafficState &t = w.s.traffic;
	const TrafficParams &p = w.s.catalog.traffic;
	const Vehicle &v = t.vehicles[size_t(id)];
	const DirSeg &d = t.g.ds[size_t(v.ds)];
	const int lane = dst - v.ds * MAX_LANES;
	const float rear = v.s - w.len(id);
	const float finish = v.s + shift_length(p, d.u[v.lane] - d.u[lane], v.v, w.len(id)) + p.lc_finish_margin_m;
	if (finish > d.len) return false;
	const auto clear_of = [&](int o) {
		if (o == id) return true;
		const Vehicle &b = t.vehicles[size_t(o)];
		const float front = b.s, back = b.s - w.len(o);
		if (front <= rear) return rear - front >= room_for(p, b, b.v);
		if (back >= v.s) return back - v.s >= room_for(p, v, v.v) && !(back < finish && b.v < std::max(v.v, p.lc_crawl_ms));
		return false; // beside its own body: no room at all
	};
	for (int o : t.lane_vehicles[size_t(dst)])
		if (!clear_of(o)) return false;
	for (int o : t.lane_shadows[size_t(dst)])
		if (!clear_of(o)) return false;
	return true;
}

// Incentive of moving `id` into `dst` (or -BIG when there is no room or the new follower would brake too hard).
float evaluate(const View &w, int id, int cur, int dst, float bias) {
	const TrafficState &t = w.s.traffic;
	const TrafficParams &p = w.s.catalog.traffic;
	const Vehicle &v = t.vehicles[id];
	if (!room_beside(w, id, dst)) return -BIG;
	Around n = around(t, dst, v.s, id), o = around(t, cur, v.s, id);
	if (n.follow >= 0 && w.accel(n.follow, id) < -p.lc_safe_brake) return -BIG;
	return w.accel(id, n.lead) - w.accel(id, o.lead) + bias;
}

bool reaches_next(const CityState &s, const Vehicle &v, int lane) {
	if (v.ri + 1 >= v.route.len) return true;
	const int next = route_at(s.traffic.route_pool, v.route, v.ri + 1);
	return lane_reaches(s, v.ds, lane, next);
}

void move_to(CityState &s, int id, int target) {
	move_vehicle_lane(s, id, target);
	s.traffic.vehicles[size_t(id)].lc_cool = s.catalog.traffic.lc_cooldown_s;
}

// The lane of its segment the route asks for, when its own does not lead to the next road; -1 when it does or none does.
int mandatory_lane(const CityState &s, const Vehicle &v) {
	if (v.ri + 1 >= v.route.len || pick_connector(s, v) >= 0) return -1;
	return lane_toward(s, v.ds, route_at(s.traffic.route_pool, v.route, v.ri + 1), v.lane);
}

int choose(const CityState &s, int id) {
	const TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const Vehicle &v = t.vehicles[id];
	const DirSeg &d = t.g.ds[v.ds];
	View w{ s, v.ds };
	auto list = [&](int lane) { return v.ds * MAX_LANES + lane; };
	// mandatory: its lane does not lead to the route's next road; a step toward the nearest that does (laneChange.ts mandatory)
	const int to = mandatory_lane(s, v);
	if (to >= 0) {
		const int cand = v.lane + (to > v.lane ? 1 : -1);
		return evaluate(w, id, list(v.lane), list(cand), p.lc_mandatory_bias) > p.lc_threshold ? cand : -1;
	}
	if (v.stop_at >= 0 && v.ri + 1 >= v.route.len && v.lane != v.stop_lane) {
		const int cand = v.lane + (v.stop_lane > v.lane ? 1 : -1);
		return evaluate(w, id, list(v.lane), list(cand), p.lc_mandatory_bias) > p.lc_threshold ? cand : -1;
	}
	if (d.len - v.s < p.lc_free_m || v.type == VehicleType::BICYCLE) return -1;
	const float bias[2] = { p.lc_bias_right, -p.lc_bias_right };
	const int cands[2] = { v.lane - 1, v.lane + 1 };
	for (int k = 0; k < 2; k++) {
		int c = cands[k];
		if (c < 0 || c >= d.lanes || !reaches_next(s, v, c)) continue;
		if (evaluate(w, id, list(v.lane), list(c), bias[k]) > p.lc_threshold) return c;
	}
	return -1;
}

} // namespace

// Whether `v` is pinned to its lane (the lab's decideLaneChanges): its body still between two lanes, a junction granted, at the kerb, or
// pulling in to the place it stops at on this lane.
bool pinned(const TrafficParams &p, const Vehicle &v) {
	const bool pulling_in = v.stop_at >= 0 && v.lane == v.stop_lane && v.stop_at - v.s < p.lc_hold_m;
	return std::fabs(v.lat) > p.lc_settled_m || v.pass >= 0 || v.kerb >= 0 || pulling_in;
}

void change_lanes(CityState &s) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const uint64_t every = uint64_t(std::max(1.f, p.lc_every_ticks));
	for (int id : t.micro_list) { // only near vehicles change lanes; the far model picks its lane on entry
		Vehicle &v = t.vehicles[size_t(id)];
		if (!v.alive || v.ds < 0 || v.lc_cool > 0 || v.s < p.mouth_clear_m || t.g.ds[size_t(v.ds)].lanes < 2) continue;
		if (pinned(p, v)) {
			v.lc_intent = -1;
			continue;
		}
		if ((t.tick + uint64_t(id)) % every == 0) { // weighed on its own ticks; between them the intent signalled stands
			const int wanted = choose(s, id);
			if (wanted != v.lc_intent) {
				v.lc_intent = wanted;
				v.lc_intent_t = float(t.time);
			}
		}
		if (v.lc_intent < 0) continue;
		// signal first; move once the lead has run and the room is still there
		const bool mandatory = mandatory_lane(s, v) >= 0;
		const float lead = mandatory && v.v < 2 * p.lc_crawl_ms ? p.lc_signal_crawl_s : p.lc_signal_s;
		if (float(t.time) - v.lc_intent_t < lead) continue;
		const View w{ s, v.ds };
		if (!room_beside(w, id, v.ds * MAX_LANES + v.lc_intent)) continue;
		const int target = v.lc_intent;
		v.lc_intent = -1;
		move_to(s, id, target);
	}
}

} // namespace traffic
} // namespace city
