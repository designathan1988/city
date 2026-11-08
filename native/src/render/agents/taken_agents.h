// The walkers and vehicles a blow or a shot took off the map, drawn no more from that moment: the published poses
// (sim/state/pose_buffer.h) still hold them until the simulation publishes again, which it does only when it steps, never
// while the game stands paused. Each is hidden until a publication newer than the one standing when it was taken
// (DamageEvent::published), which no longer holds it; then the list lets it go (an id is given to somebody new only by a step).
#pragma once

#include "sim/demolition/damage_event.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city {

struct TakenAgents {
	std::unordered_map<uint32_t, int64_t> walkers, vehicles; // id -> the publication standing when it was taken

	void take(const std::vector<DamageEvent> &played) {
		for (const DamageEvent &e : played) {
			if (e.kind == DamageEvent::CASUALTY && e.id >= 0 && e.id < VEHICLE_BODY_ID_BASE) walkers[uint32_t(e.id)] = e.published;
			if ((e.kind == DamageEvent::VEHICLE_BLOWN || e.kind == DamageEvent::VEHICLE_DROP) && e.id >= 0) vehicles[uint32_t(e.id)] = e.published;
		}
	}
	// The newest publication drawn is `newest`: what was taken before it is no longer in it.
	void seen(int64_t newest) {
		for (auto *list : { &walkers, &vehicles })
			for (auto it = list->begin(); it != list->end();) it = newest > it->second ? list->erase(it) : std::next(it);
	}
	bool walker(uint32_t id) const { return !walkers.empty() && walkers.count(id) != 0; }
	bool vehicle(uint32_t id) const { return !vehicles.empty() && vehicles.count(id) != 0; }
	void clear() {
		walkers.clear();
		vehicles.clear();
	}
};

} // namespace city
