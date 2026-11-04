// Walkers among walkers (Roadcraft 4.0 walk.ts, after SUMO's striping model): on a sidewalk the footway is a few
// stripes wide; each walker looks ahead, takes the stripe with the most room (aside costs, off its own side of the
// way costs, a change must be clearly better and the last one must have held), steps aside at a share of its pace and
// keeps its gap behind the body ahead in its stripe. At a crosswalk the waiting take places along the kerb and in rows
// back from it, and cross keeping that place across, so nobody stands or walks inside another.
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr int MAX_STRIPES = 8;
constexpr int MAX_PLACES = 32; // bits of TrafficState::kerb_slots
constexpr float ONE_WAY_CLOSING = 0.5f; // a body coming the other way closes the gap at both paces: half the room
constexpr float ZERO_SPAN_M = 1e-4f; // a way across shorter than this has no direction
constexpr int KERB_NODE_BITS = 32; // a kerb's key: its node above these bits, the arm crossed and which way below

int sidewalk_key(const Pedestrian &w) { return w.seg * 2 + w.side; }
float own_u(const Pedestrian &w) { return w.dir == 0 ? -w.track : w.track; } // its own side of the way

struct Decision {
	float u = 0, u_goal = 0, held = 0, pace = 0;
};

// The stripe, pace and step aside of walker `id` (index k of its sidewalk's `list`, sorted by arc).
Decision decide(const CityState &s, const std::vector<int> &list, size_t k, float dt) {
	const TrafficParams &p = s.catalog.traffic;
	const TrafficState &t = s.traffic;
	const Pedestrian &w = t.peds[size_t(list[k])];
	const float half = walk_half(s, w.seg, w.side), touch = 2 * p.ped_body_m, ahead_sign = w.dir == 0 ? 1.f : -1.f;
	const int n = std::clamp(1 + int(2 * half / p.ped_stripe_m), 1, MAX_STRIPES);
	float centre[MAX_STRIPES], room[MAX_STRIPES];
	for (int m = 0; m < n; m++) {
		centre[m] = n == 1 ? 0.f : -half + 2 * half * float(m) / float(n - 1);
		room[m] = p.ped_look_m;
	}
	float lead_gap = p.ped_look_m;
	auto see = [&](const Pedestrian &o) {
		const float d = (o.s - w.s) * ahead_sign;
		if (d <= 0) return; // behind: theirs to keep the gap
		const float gap = (o.dir == w.dir ? d : d * ONE_WAY_CLOSING) - touch;
		for (int m = 0; m < n; m++)
			if (std::fabs(o.u - centre[m]) < touch) room[m] = std::min(room[m], gap);
		if (std::fabs(o.u - w.u) < touch) lead_gap = std::min(lead_gap, gap);
	};
	for (size_t j = k + 1; j < list.size() && std::fabs(t.peds[size_t(list[j])].s - w.s) < p.ped_look_m; j++) see(t.peds[size_t(list[j])]);
	for (size_t j = k; j-- > 0 && std::fabs(t.peds[size_t(list[j])].s - w.s) < p.ped_look_m;) see(t.peds[size_t(list[j])]);
	auto score = [&](int m) {
		return room[m] - p.ped_lateral_cost * std::fabs(centre[m] - w.u) - p.ped_keep_right_cost * std::fabs(centre[m] - own_u(w));
	};
	int best = 0, cur = 0;
	for (int m = 0; m < n; m++) {
		if (score(m) > score(best)) best = m;
		if (std::fabs(centre[m] - w.u_goal) < std::fabs(centre[cur] - w.u_goal)) cur = m;
	}
	Decision out;
	out.held = w.stripe_held + dt;
	out.u_goal = centre[cur];
	const bool shut = room[cur] < p.ped_gap_m; // the way ahead in its stripe is closed: no waiting out the hold
	if (best != cur && score(best) > score(cur) + p.ped_stripe_gain_m && (out.held >= p.ped_stripe_hold_s || shut)) {
		out.u_goal = centre[best];
		out.held = 0;
	}
	// the gap behind the body ahead in its own span; aside at a share of its own pace (it can step aside standing)
	const float want = std::clamp((lead_gap - p.ped_gap_m) / p.ped_headway_s, 0.f, w.speed);
	out.pace = std::min(want, w.pace + p.ped_accel * dt);
	const float aside = p.ped_lateral_share * w.speed * dt;
	out.u = w.u + std::clamp(out.u_goal - w.u, -aside, aside);
	return out;
}

} // namespace

float walk_half(const CityState &s, int seg, int side) {
	const Profile &pf = s.roads.profile_of(seg);
	if (pf.edges.empty()) return 0.f;
	const StripEdge &e = side == 0 ? pf.edges.front() : pf.edges.back();
	return std::max(0.f, (e.u1 - e.u0) * 0.5f - s.catalog.traffic.ped_margin_m);
}

void crowd_step(CityState &s, float dt) {
	TrafficState &t = s.traffic;
	const size_t sides = s.roads.segs.size() * 2;
	if (t.crowd.size() != sides) {
		t.crowd.assign(sides, {});
		t.crowd_keys.clear();
	}
	for (int key : t.crowd_keys) t.crowd[size_t(key)].clear();
	t.crowd_keys.clear();
	for (int id = 0; id < int(t.peds.size()); id++) {
		const Pedestrian &w = t.peds[size_t(id)];
		if (!w.alive || w.mode != PedMode::SIDEWALK) continue;
		auto &list = t.crowd[size_t(sidewalk_key(w))];
		if (list.empty()) t.crowd_keys.push_back(sidewalk_key(w));
		list.push_back(id);
	}
	std::vector<Decision> next;
	for (int key : t.crowd_keys) {
		auto &list = t.crowd[size_t(key)];
		std::sort(list.begin(), list.end(), [&](int a, int b) { return t.peds[size_t(a)].s < t.peds[size_t(b)].s; });
		next.resize(list.size());
		for (size_t k = 0; k < list.size(); k++) next[k] = decide(s, list, k, dt); // all read the step's start
		for (size_t k = 0; k < list.size(); k++) {
			Pedestrian &w = t.peds[size_t(list[k])];
			w.u = next[k].u;
			w.u_goal = next[k].u_goal;
			w.stripe_held = next[k].held;
			w.pace = next[k].pace;
		}
	}
}

void on_sidewalk(Pedestrian &w) {
	w.u = w.u_goal = own_u(w);
	w.stripe_held = 0;
	w.pace = w.speed;
}

void take_kerb_place(CityState &s, int id) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	Pedestrian &w = t.peds[size_t(id)];
	if (w.wi + 1 >= w.wn || w.ways[w.wi].cross >= 0 || w.ways[w.wi + 1].cross < 0) return;
	free_kerb_place(t, id);
	Waypoint &kerb = w.ways[w.wi], &far = w.ways[w.wi + 1];
	V2 d(far.x - kerb.x, far.z - kerb.z);
	const float len = d.len();
	if (len < ZERO_SPAN_M) return;
	d = d * (1.f / len);
	const V2 right(-d.y, d.x); // the right of the way across (left is (d.z, -d.x), lane_geometry.cpp)
	const int64_t key = (int64_t(w.way_node) << KERB_NODE_BITS) | (int64_t(far.cross) << 1) | ((d.x > 0 || (d.x == 0 && d.y > 0)) ? 1 : 0);
	const int per_row = 2 * int(p.kerb_wait_half_m / p.kerb_wait_gap_m) + 1;
	const int places = std::min(MAX_PLACES, per_row * int(p.kerb_wait_rows));
	uint32_t &taken = t.kerb_slots[key];
	int k = 0;
	while (k < places && (taken >> k) & 1u) k++;
	if (k >= places) return; // every place taken: it waits behind them at the kerb point
	taken |= 1u << k;
	w.slot_key = key;
	w.slot = k;
	const int in_row = k % per_row;
	// the middle first, then either side of it; the whole row a half gap to the right so the two ways do not meet
	const float lateral = float((in_row + 1) / 2) * p.kerb_wait_gap_m * (in_row % 2 ? 1.f : -1.f) + p.kerb_wait_gap_m * 0.5f;
	const float back = float(k / per_row) * p.kerb_wait_gap_m;
	kerb.x += right.x * lateral - d.x * back;
	kerb.z += right.y * lateral - d.y * back;
	far.x += right.x * lateral;
	far.z += right.y * lateral;
}

void free_kerb_place(TrafficState &t, int id) {
	Pedestrian &w = t.peds[size_t(id)];
	if (w.slot < 0) return;
	auto it = t.kerb_slots.find(w.slot_key);
	if (it != t.kerb_slots.end()) {
		it->second &= ~(1u << w.slot);
		if (it->second == 0) t.kerb_slots.erase(it);
	}
	w.slot = -1;
	w.slot_key = -1;
}

} // namespace traffic
} // namespace city
