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

Around around(const TrafficState &t, const std::vector<int> &list, float at, int self) {
	Around a;
	for (int id : list) {
		if (id == self) continue;
		float o = t.vehicles[id].s;
		if (o >= at && (a.lead < 0 || o < t.vehicles[a.lead].s)) a.lead = id;
		if (o < at && (a.follow < 0 || o > t.vehicles[a.follow].s)) a.follow = id;
	}
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
float evaluate(const View &w, int id, const std::vector<int> &cur, const std::vector<int> &dst, float bias, bool force) {
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
	for (int ci : s.traffic.g.ds_out[v.ds]) {
		const Conn &c = s.traffic.g.conns[ci];
		if (c.from_lane == lane && c.to_ds == next) return true;
	}
	return false;
}

void move_to(CityState &s, int id, int target) {
	TrafficState &t = s.traffic;
	Vehicle &v = t.vehicles[id];
	auto &cur = t.lane_vehicles[size_t(v.ds) * MAX_LANES + v.lane];
	auto &dst = t.lane_vehicles[size_t(v.ds) * MAX_LANES + target];
	cur.erase(std::remove(cur.begin(), cur.end(), id), cur.end());
	auto at = std::lower_bound(dst.begin(), dst.end(), v.s, [&](int o, float x) { return t.vehicles[o].s < x; });
	dst.insert(at, id);
	const DirSeg &d = t.g.ds[v.ds];
	v.lc_off += d.u[v.lane] - d.u[target];
	v.lane = target;
	v.lc_cool = s.catalog.traffic.lc_cooldown_s;
}

int choose(const CityState &s, int id) {
	const TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const Vehicle &v = t.vehicles[id];
	const DirSeg &d = t.g.ds[v.ds];
	View w{ s, v.ds };
	auto list = [&](int lane) -> const std::vector<int> & { return t.lane_vehicles[size_t(v.ds) * MAX_LANES + lane]; };
	int ci = pick_connector(s, v);
	if (ci >= 0 && t.g.conns[ci].from_lane != v.lane) {
		int cand = v.lane + (t.g.conns[ci].from_lane > v.lane ? 1 : -1);
		bool force = v.wait > FORCE_AFTER_S;
		return evaluate(w, id, list(v.lane), list(cand), p.lc_mandatory_bias, force) > p.lc_threshold ? cand : -1;
	}
	if (d.len - v.s < p.lc_free_m) return -1;
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
	for (int id = 0; id < int(t.vehicles.size()); id++) {
		const Vehicle &v = t.vehicles[id];
		if (!v.alive || v.ds < 0 || v.lc_cool > 0 || v.s < s.catalog.traffic.mouth_clear_m || (t.tick + uint64_t(id)) % every != 0) continue;
		if (t.g.ds[v.ds].lanes < 2) continue;
		int target = choose(s, id);
		if (target >= 0) move_to(s, id, target);
	}
}

} // namespace traffic
} // namespace city
