// Lane changes (Roadcraft sim/traffic/laneChanges.ts, MOBIL-like): safety = gaps ahead/behind and the new follower's
// braking; incentive = own IDM gain + bias over a fixed threshold. Mandatory changes move toward the lane that has a
// connector to the next road (after waiting long enough at the line any physical gap will do); discretionary
// changes keep right.
#include "sim/traffic/traffic_system.h"

#include <algorithm>

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

// Incentive of moving `id` into `dst` (or -BIG when unsafe); `force` keeps only the physical gaps.
float evaluate(const View &w, int id, int cur, int dst, float bias, bool force) {
	const TrafficState &t = w.s.traffic;
	const TrafficParams &p = w.s.catalog.traffic;
	const Vehicle &v = t.vehicles[id];
	Around n = around(t, dst, v.s, id), o = around(t, cur, v.s, id);
	float front = force ? FORCE_GAP_M : p.lc_gap_front_m, back = force ? FORCE_GAP_M : p.lc_gap_back_m;
	if (n.lead >= 0 && t.vehicles[n.lead].s - w.len(n.lead) - v.s < front) return -BIG;
	if (n.follow >= 0 && v.s - w.len(id) - t.vehicles[n.follow].s < back) return -BIG;
	if (force) return BIG;
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

int choose(const CityState &s, int id) {
	const TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const Vehicle &v = t.vehicles[id];
	const DirSeg &d = t.g.ds[v.ds];
	View w{ s, v.ds };
	auto list = [&](int lane) { return v.ds * MAX_LANES + lane; };
	int ci = pick_connector(s, v);
	if (ci >= 0 && t.g.conns[ci].from_lane != v.lane) {
		int cand = v.lane + (t.g.conns[ci].from_lane > v.lane ? 1 : -1);
		bool force = v.wait > FORCE_AFTER_S;
		return evaluate(w, id, list(v.lane), list(cand), p.lc_mandatory_bias, force) > p.lc_threshold ? cand : -1;
	}
	if (v.stop_at >= 0 && v.ri + 1 >= v.route.len && v.lane != v.stop_lane) {
		const int cand = v.lane + (v.stop_lane > v.lane ? 1 : -1);
		return evaluate(w, id, list(v.lane), list(cand), p.lc_mandatory_bias, v.wait > FORCE_AFTER_S) > p.lc_threshold ? cand : -1;
	}
	if (d.len - v.s < p.lc_free_m || v.type == VehicleType::BICYCLE) return -1;
	const float bias[2] = { p.lc_bias_right, -p.lc_bias_right };
	const int cands[2] = { v.lane - 1, v.lane + 1 };
	for (int k = 0; k < 2; k++) {
		int c = cands[k];
		if (c < 0 || c >= d.lanes || !reaches_next(s, v, c)) continue;
		if (evaluate(w, id, list(v.lane), list(c), bias[k], false) > p.lc_threshold) return c;
	}
	return -1;
}

} // namespace

void change_lanes(CityState &s) {
	TrafficState &t = s.traffic;
	const uint64_t every = uint64_t(std::max(1.f, s.catalog.traffic.lc_every_ticks));
	for (int id : t.micro_list) { // only near vehicles change lanes; the far model picks its lane on entry
		const Vehicle &v = t.vehicles[size_t(id)];
		if (!v.alive || v.ds < 0 || v.lc_cool > 0 || v.s < s.catalog.traffic.mouth_clear_m || (t.tick + uint64_t(id)) % every != 0) continue;
		if (t.g.ds[size_t(v.ds)].lanes < 2) continue;
		int target = choose(s, id);
		if (target >= 0) move_to(s, id, target);
	}
}

} // namespace traffic
} // namespace city
