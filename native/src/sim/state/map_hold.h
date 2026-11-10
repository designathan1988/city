// What a thread builds while it holds the map (CityState::map_mutex shared, taken through MapRead: city_state.h). A mesh job copies
// what it reads under the lock and builds with the lock let go (sim/state/city_cut.h): a vertex or an instance a builder takes
// while its thread holds the map is counted here, in the tests' DLL (core/checks.h), by the lane of the job that took it, so the
// map_hold probe says which producer still builds under the lock.
#pragma once

#include "core/checks.h"

#include <cstdint>
#include <map>
#include <string>

namespace city {
namespace map_hold {

struct Held {
	int depth = 0; // MapRead guards open on this thread
	int64_t built = 0; // what it built under them, not yet handed to the tally
};
inline Held &mine() {
	thread_local Held held;
	return held;
}

inline void enter() { mine().depth++; }
void leave(); // the last guard of the thread hands its count to the tally
// One vertex or one instance taken by a builder: counted when the calling thread holds the map.
inline void built() {
	if (!CHECKS) return;
	Held &h = mine();
	if (h.depth > 0) h.built++;
}

// What was built under the map since the last clear: by the lane of the keyed job that built it (core/job_pool.h job_lane_name),
// "outside" for a thread in no keyed job (the frame's thread).
std::map<std::string, int64_t> tally();
void clear();

} // namespace map_hold
} // namespace city
