// Area `probes`: what the street shows this tick at the crossings between junctions and the U-turns, for street_crossing_test: who is on a crossing, what
// has stopped for them, how close anything came, and who is turning back.
#include "api/city_probes.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

static constexpr float NOBODY_M = 1e9f;
static constexpr float STOPPED_MS = 0.5f;

Dictionary CityProbes::street_watch(float near_m) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const CityState &s = *state_;
	const TrafficState &t = s.traffic;
	int crossers = 0, near = 0, held = 0, turning = 0;
	float closest = NOBODY_M;
	for (const Pedestrian &w : t.peds) {
		if (!w.alive || w.cross_seg < 0 || w.way_node < 0 || !s.roads.mid_block(w.way_node)) continue;
		crossers++;
		for (const Vehicle &v : t.vehicles) {
			if (!v.alive || !v.micro || v.off) continue;
			const float d = std::hypot(v.cur.x - w.cur.x, v.cur.z - w.cur.z);
			closest = std::min(closest, d);
			if (d > near_m) continue;
			near++;
			held += v.v < STOPPED_MS ? 1 : 0;
		}
	}
	for (const Vehicle &v : t.vehicles) {
		if (!v.alive || !v.micro || v.conn < 0) continue;
		const Conn &c = t.g.conns[size_t(v.conn)];
		turning += c.alive && c.turn == Turn::UTURN && s.roads.nodes[size_t(c.node)].uturn != UTURN_NONE ? 1 : 0;
	}
	Dictionary d;
	d["crossers"] = crossers;
	d["near"] = near;
	d["held"] = held;
	d["closest_m"] = closest;
	d["turning_back"] = turning;
	return d;
}

} // namespace city
