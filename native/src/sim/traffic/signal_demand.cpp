// What a signal's controller asks of its plan (signal_plan.h), each from what is already kept, never by walking the map: a stage's
// demand (a call still standing, or a vehicle inside the detection reach of one of its approaches' lanes: the lane's own list from
// its head back; a turn stage: a head bound for a turn that gives way; a walker waiting at a crossing it walks), its queue, a bus
// near, the people on its crossings, the longest wait at its kerbs, a crossing's lamp. And the cell a kerb keeps of who waits there.
#include "sim/traffic/signal_plan.h"

#include "sim/traffic/traffic_system.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {

constexpr double KERB_FRESH_S = 0.5; // a walker held at a kerb asks again 5 times a second (pedestrians.cpp): two words missed and it is gone

// Where the detection reach of approach `d` starts, metres along it.
float reach_from(const DirSeg &d) { return d.len - d.stop_back - d.detector; }

// The vehicles of approach `ds` inside its detection reach, a lane's list read from its head back; `only_bus` counts buses alone.
int in_reach(const CityState &s, int ds, bool only_bus) {
	const TrafficState &t = s.traffic;
	const DirSeg &d = t.g.ds[size_t(ds)];
	const float from = reach_from(d);
	int found = 0;
	for (int lane = 0; lane < d.lanes; lane++) {
		const std::vector<int> &list = t.lane_vehicles[size_t(ds) * MAX_LANES + size_t(lane)];
		for (auto it = list.rbegin(); it != list.rend() && t.vehicles[size_t(*it)].s >= from; ++it)
			found += !only_bus || t.vehicles[size_t(*it)].type == VehicleType::BUS ? 1 : 0;
	}
	return found;
}

// A head of approach `ds`, inside its reach, bound for a turn that gives way all through its stage: what its turn stage is for.
bool turn_asked(const CityState &s, int ds) {
	const TrafficState &t = s.traffic;
	const DirSeg &d = t.g.ds[size_t(ds)];
	for (int lane = 0; lane < d.lanes; lane++) {
		const std::vector<int> &list = t.lane_vehicles[size_t(ds) * MAX_LANES + size_t(lane)];
		if (list.empty() || t.vehicles[size_t(list.back())].s < reach_from(d)) continue;
		const int ci = pick_connector(s, t.vehicles[size_t(list.back())]);
		if (ci >= 0 && t.g.conns[size_t(ci)].gives_way) return true;
	}
	return false;
}

int kerb_slot(const CityState &s, int node, int arm_seg) {
	const int slot = arm_slot(s, node, arm_seg);
	return slot < 0 ? -1 : node * NODE_ARMS_MAX + slot;
}

bool walks_in(const Junction &j, int k, int arm_seg) {
	const std::vector<int> &walks = j.walks[size_t(k)];
	return std::find(walks.begin(), walks.end(), arm_seg) != walks.end();
}

} // namespace

bool stage_asked(const CityState &s, int n, const Junction &j, int k) {
	const TrafficState &t = s.traffic;
	if (j.turn_of[size_t(k)] >= 0) return turn_asked(s, j.turn_of[size_t(k)]);
	for (int ds : j.approaches[size_t(k)]) {
		if (size_t(ds) < t.green_wanted.size() && t.green_wanted[size_t(ds)] > t.time) return true;
		if (in_reach(s, ds, false) > 0) return true;
	}
	return walkers_wait(s, n, j, k) >= 0;
}

float stage_queue(const CityState &s, const Junction &j, int k) {
	int queued = 0;
	for (int ds : j.approaches[size_t(k)]) queued += in_reach(s, ds, false);
	return float(queued);
}

bool bus_near(const CityState &s, const Junction &j, int k) {
	for (int ds : j.approaches[size_t(k)])
		if (in_reach(s, ds, true) > 0) return true;
	return false;
}

bool people_on(const CityState &s, int n, const Junction &j, int k) {
	const TrafficState &t = s.traffic;
	if (size_t(n) >= t.node_crossers.size()) return false;
	for (int id : t.node_crossers[size_t(n)])
		if (walks_in(j, k, t.peds[size_t(id)].cross_seg)) return true;
	return false;
}

float walkers_wait(const CityState &s, int n, const Junction &j, int k) {
	float longest = -1;
	for (int seg : j.walks[size_t(k)]) longest = std::max(longest, kerb_waited(s, n, seg));
	return longest;
}

// Walk while the running stage walks that arm and its green lasts: the controller keeps the green over whoever is on a crossing
// (signal_timing.cpp, ped_hold_limit_s); under a fixed plan, which follows the clock, only while what is left of the green and its
// intergreen lets the walker over. Flash once the green is over (who is on it finishes); stop in every other stage.
WalkLamp walk_lamp(const CityState &s, int node, int arm_seg) {
	const Junction &j = s.traffic.g.junctions[size_t(node)];
	if (j.green.empty()) return WalkLamp::WALK;
	const size_t k = size_t(j.run);
	if (!walks_in(j, j.run, arm_seg)) return WalkLamp::STOP;
	if (j.run_green >= 0) return WalkLamp::FLASH;
	if (!j.fixed) return WalkLamp::WALK;
	const double left = double(j.green[k]) - (s.traffic.time - j.run_start) + j.amber[k] + j.all_red[k];
	return left >= ped_clearance(s, arm_seg) + s.catalog.traffic.ped_all_red_s ? WalkLamp::WALK : WalkLamp::FLASH;
}

void kerb_word(CityState &s, int node, int arm_seg, float waited) {
	TrafficState &t = s.traffic;
	const int slot = kerb_slot(s, node, arm_seg);
	if (slot < 0 || size_t(slot) >= t.junction.kerb.size()) return;
	KerbAsking &cell = t.junction.kerb[size_t(slot)];
	if (t.time - cell.seen > KERB_FRESH_S) cell.longest = 0; // nobody had been waiting: this one's wait starts the count
	cell.seen = t.time;
	cell.longest = std::max(cell.longest, waited);
}

float kerb_waited(const CityState &s, int node, int arm_seg) {
	const TrafficState &t = s.traffic;
	const int slot = kerb_slot(s, node, arm_seg);
	if (slot < 0 || size_t(slot) >= t.junction.kerb.size()) return -1;
	const KerbAsking &cell = t.junction.kerb[size_t(slot)];
	return t.time - cell.seen > KERB_FRESH_S ? -1.f : cell.longest;
}

} // namespace traffic
} // namespace city
