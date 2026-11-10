// The tally of what was built under the map (map_hold.h).
#include "sim/state/map_hold.h"

#include "core/job_pool.h"

#include <mutex>

namespace city {
namespace map_hold {

namespace {
std::mutex tally_mutex;
std::map<std::string, int64_t> counts;
} // namespace

void leave() {
	Held &h = mine();
	if (--h.depth > 0 || h.built == 0) return;
	JobKey key;
	const char *who = JobPool::running(key) ? job_lane_name(key.lane) : "outside";
	std::lock_guard<std::mutex> lk(tally_mutex);
	counts[who] += h.built;
	h.built = 0;
}

std::map<std::string, int64_t> tally() {
	std::lock_guard<std::mutex> lk(tally_mutex);
	return counts;
}

void clear() {
	std::lock_guard<std::mutex> lk(tally_mutex);
	counts.clear();
}

} // namespace map_hold
} // namespace city
