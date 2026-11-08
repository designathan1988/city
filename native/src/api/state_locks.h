// The protocol's read locks (sim/state/city_state.h): the map shared for what commands change, and for traffic the map shared
// and then traffic_mutex, the order the simulation takes them in. One owner, so no reader takes the traffic lock alone (a command
// changes traffic under the map's exclusive lock without it) or the two the other way round.
#pragma once

#include "sim/state/city_state.h"

#include <mutex>
#include <shared_mutex>

namespace city {

struct TrafficRead {
	std::shared_lock<std::shared_mutex> map;
	std::lock_guard<std::mutex> traffic;
	explicit TrafficRead(const CityState &s) : map(s.map_mutex), traffic(s.traffic_mutex) {}
};

} // namespace city
