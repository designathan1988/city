// The tallies of what was built under the map and of the exclusive holds by writer (map_hold.h).
#include "sim/state/map_hold.h"

#include "core/job_pool.h"

#include <mutex>

namespace city {
namespace map_hold {

namespace {
std::mutex tally_mutex;
std::map<std::string, int64_t> counts;
std::map<std::string, int64_t> holds;
} // namespace

const char *writer_name(Writer w) {
	switch (w) {
		case Writer::COMMAND: return "command";
		case Writer::TICK: return "tick";
		case Writer::LOAD: return "load";
		case Writer::GENERATION: return "generation";
		case Writer::SAVE: return "save";
		case Writer::PROBE: return "probe";
		default: return "none";
	}
}

void held_by(Writer w) {
	std::lock_guard<std::mutex> lk(tally_mutex);
	holds[writer_name(w)]++;
}

std::map<std::string, int64_t> writers() {
	std::lock_guard<std::mutex> lk(tally_mutex);
	return holds;
}

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
	holds.clear();
}

} // namespace map_hold
} // namespace city
