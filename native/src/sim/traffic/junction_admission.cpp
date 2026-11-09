// Keeping junctions moving (Roadcraft sim/traffic/junctions.ts exitRoom/boxClear/passHolder/claimExit, micro.ts
// jamRelease/admit, traffic/regions.ts governor): nobody enters a junction it cannot leave (room on the exit lane,
// counting whoever is already heading there and what a moving tail frees), a vehicle over its line with the way open
// owns its connector until it is through, the longest waiter at a full exit holds its next free spot, a junction
// nobody has gone through for a while lets one vehicle in at a time with the box clear, and the fleet target eases
// off while too many vehicles stand in full lanes.
#include "sim/traffic/traffic_system.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {

constexpr double CLAIM_FRESH_S = 1.0; // an exit claim lapses unless renewed within this
constexpr int SPLICE_DEGREE = 2;

float body(const CityState &s, int id) { return s.catalog.traffic.classes[int(s.traffic.vehicles[id].type)].length; }
size_t key(int ds, int lane) { return size_t(ds) * MAX_LANES + lane; }

// Where the rear of the last vehicle of lane `cars` (back first) of `ds` comes to rest, BIG when its queue rolls on (Roadcraft lab
// src/sim/intersections/spillback.ts restingRear): from the head back, each one standing stays where it is and each one rolling goes
// up to what stands ahead of it, the stop line under a light other than green for the head, the rest of the one ahead for the others.
float resting_rear(const CityState &s, int ds, const std::vector<int> &cars) {
	const TrafficParams &tp = s.catalog.traffic;
	const DirSeg &d = s.traffic.g.ds[size_t(ds)];
	float limit = light_for(s, ds) == Light::GREEN ? NO_LEADER : d.len - d.stop_back, rear = NO_LEADER;
	for (auto it = cars.rbegin(); it != cars.rend(); ++it) {
		const Vehicle &v = s.traffic.vehicles[size_t(*it)];
		const VehicleClass &c = tp.classes[int(v.type)];
		const float nose = v.v <= tp.tail_moving_ms ? v.s : std::max(v.s, limit);
		if (nose >= NO_LEADER) continue; // it rolls on, and so may whoever follows it
		rear = nose - c.length;
		limit = rear - c.min_gap;
	}
	return rear;
}

} // namespace

bool is_splice(const CityState &s, int node) {
	const Junction &j = s.traffic.g.junctions[node];
	return j.degree <= SPLICE_DEGREE && j.ctl == Control::NONE;
}

bool has_exit(const CityState &s, const Conn &c, int vid) {
	const TrafficState &t = s.traffic;
	const TrafficParams &tp = s.catalog.traffic;
	size_t to = key(c.to_ds, c.to_lane);
	int cl = t.exit_claim[to];
	if (cl >= 0 && cl != vid && t.time - t.exit_claim_t[to] < CLAIM_FRESH_S && t.vehicles[cl].alive) return false;
	const float L = t.g.ds[c.to_ds].len;
	const auto &lane = t.lane_vehicles[to];
	const Vehicle *tail = lane.empty() ? nullptr : &t.vehicles[lane.front()];
	// the last one on the lane, moving, frees room in the time it takes to cross; toward a signal it is rolling up to whatever holds
	// it, very often the red, so the room is what its queue leaves behind where it comes to rest (Roadcraft lab spillback.ts)
	float room = tail ? tail->s - body(s, lane.front()) + std::min(tail->v, tp.tail_credit_cap_ms) * tp.tail_credit_s : L;
	if (tail && t.g.junctions[size_t(t.g.ds[size_t(c.to_ds)].to)].ctl == Control::SIGNAL) room = std::min(L, resting_rear(s, c.to_ds, lane));
	int inbound = 0;
	for (int x = t.g.node_conn[c.node]; x >= 0; x = t.g.conns[size_t(x)].next_at_node) {
		const Conn &o = t.g.conns[size_t(x)];
		if (!o.alive || o.to_ds != c.to_ds || o.to_lane != c.to_lane) continue;
		for (int id : t.conn_vehicles[size_t(x)]) {
			room -= body(s, id) + tp.exit_room_pad_m;
			inbound++;
		}
	}
	// onto a small loop from outside it: not past the share of it a ring can hold and still turn (Roadcraft lab cycles.ts)
	if (ring_full(s, c, body(s, vid) + tp.exit_room_pad_m)) return false;
	if (lane.empty() && inbound == 0) return true;
	// short lane between close junctions: nearly part of the junction; one more fits behind a moving tail
	if (L < tp.short_exit_m && tail && tail->v > tp.tail_moving_ms && inbound == 0) return true;
	if (float(lane.size()) >= std::max(1.f, L / tp.slot_m)) return false;
	return room > std::min(body(s, vid) + tp.exit_room_pad_m, L * tp.reconsider_full_share);
}

int pass_holder(const CityState &s, int ci) {
	const TrafficState &t = s.traffic;
	int u = t.conn_pass[ci];
	if (u < 0 || !t.vehicles[u].alive) return -1;
	const Vehicle &v = t.vehicles[u];
	return v.pass == ci && v.conn < 0 ? u : -1;
}

bool box_clear(const CityState &s, int ci) {
	const TrafficState &t = s.traffic;
	const Conn &c = t.g.conns[ci];
	for (int d : c.conflicts) {
		if (!t.conn_vehicles[d].empty()) return false;
		int u = pass_holder(s, d);
		if (u < 0) continue;
		// another one over its line for a crossing connector since before this one: it goes first
		if (t.conn_pass_t[d] < t.conn_pass_t[ci] || (t.conn_pass_t[d] == t.conn_pass_t[ci] && u < t.conn_pass[ci])) return false;
	}
	return true;
}

void commit(CityState &s, int vid, int ci) {
	TrafficState &t = s.traffic;
	t.vehicles[vid].pass = ci;
	t.conn_pass[ci] = vid;
	t.conn_pass_t[ci] = t.time;
}

void release(CityState &s, int vid, int ci) {
	TrafficState &t = s.traffic;
	if (t.conn_pass[ci] == vid) t.conn_pass[ci] = -1;
	t.vehicles[vid].pass = -1;
}

void claim_exit(CityState &s, const Conn &c, int vid) {
	TrafficState &t = s.traffic;
	size_t to = key(c.to_ds, c.to_lane);
	int cl = t.exit_claim[to];
	if (cl == vid || cl < 0 || t.time - t.exit_claim_t[to] >= CLAIM_FRESH_S || !t.vehicles[cl].alive ||
			t.vehicles[cl].wait < t.vehicles[vid].wait) {
		t.exit_claim[to] = vid;
		t.exit_claim_t[to] = t.time;
	}
}

bool jam_release(CityState &s, int ci, int vid) {
	TrafficState &t = s.traffic;
	const TrafficParams &tp = s.catalog.traffic;
	const Conn &c = t.g.conns[ci];
	const int n = c.node;
	if (t.time - t.last_admit[n] < tp.jam_seconds || t.time < t.release_at[n]) return false;
	if (t.vehicles[vid].wait < tp.jam_seconds) return false;
	if (light_for(s, c.from_ds) == Light::RED) return false; // nobody runs the red
	const auto &on = t.conn_vehicles[ci];
	if (!on.empty() && t.vehicles[on.back()].cs < body(s, on.back()) + tp.exit_room_pad_m) return false;
	if (peds_in_way(s, ci)) return false;
	for (int d : c.conflicts)
		if (!t.conn_vehicles[d].empty() || pass_holder(s, d) >= 0) return false;
	return true;
}

void admit(CityState &s, int node, bool released) {
	TrafficState &t = s.traffic;
	t.last_admit[node] = t.time;
	t.pass_count[node]++;
	if (!released) return;
	t.released++;
	t.release_at[node] = t.time + s.catalog.traffic.release_every_s;
}

void reset_admission(CityState &s) {
	TrafficState &t = s.traffic;
	const size_t lanes = t.g.ds.size() * MAX_LANES, nodes = t.g.junctions.size(), conns = t.g.conns.size();
	t.conn_pass.assign(conns, -1);
	t.conn_pass_t.assign(conns, 0.0);
	t.conn_claim_t.assign(conns, -double(BIG));
	t.exit_claim.assign(lanes, -1);
	t.exit_claim_t.assign(lanes, 0.0);
	t.last_admit.assign(nodes, t.time);
	t.release_at.assign(nodes, 0.0);
	t.pass_count.assign(nodes, 0);
	t.gov = 1;
	t.gov_clock = 0;
}

void step_governor(CityState &s, float dt) {
	TrafficState &t = s.traffic;
	const TrafficParams &tp = s.catalog.traffic;
	t.gov_clock += dt;
	if (t.gov_clock < tp.gov_every_s) return;
	t.gov_clock = 0;
	if (t.vehicle_count < tp.gov_min_fleet) return;
	int jammed = 0;
	for (int ds = 0; ds < int(t.g.ds.size()); ds++) {
		const DirSeg &d = t.g.ds[ds];
		if (!d.ok) continue;
		int n = 0;
		for (int l = 0; l < d.lanes; l++) n += int(t.lane_vehicles[key(ds, l)].size());
		if (float(n) * tp.slot_m >= tp.gov_jammed * d.len * d.lanes) jammed += n;
	}
	float share = float(jammed) / float(t.vehicle_count);
	float over = clampf((share - tp.gov_lo) / (tp.gov_hi - tp.gov_lo), 0.f, 1.f);
	t.gov += (1 - over * (1 - tp.gov_floor) - t.gov) * tp.gov_rate;
}

} // namespace traffic
} // namespace city
