// The one admission of a tick's route searches (Roadcraft 4.0 routing/searchScratch.ts SearchBudget, mayRequestSearch, noteSearch):
// every search counts on the tick's budget, those that cannot wait too (a route that broke, a bus's next stop, a lot's call); a search
// that can wait (a new trip, a driver reconsidering or going round a full exit) starts only while some is left, else it asks again
// on a later tick. The budget follows the tick: a new tick starts with none used.
#include "sim/traffic/traffic_system.h"

namespace city {
namespace traffic {

namespace {
SearchBudget &budget(TrafficState &t) {
	SearchBudget &b = t.search_budget;
	if (b.tick != t.tick) {
		b.tick = t.tick;
		b.used = 0;
	}
	return b;
}
} // namespace

bool may_request_search(CityState &s) {
	return budget(s.traffic).used < int(s.catalog.traffic.route_budget_per_step);
}

void note_search(CityState &s) {
	budget(s.traffic).used++;
}

} // namespace traffic
} // namespace city
