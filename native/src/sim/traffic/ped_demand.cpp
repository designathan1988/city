// How many people the land use puts on its street at an hour and which way they go. By kind of place (home, shop, work,
// civic): people per metre of street at full bustle, times how busy that kind is at the hour; of those, the share coming out
// of the place follows the people there: the share of everybody working or shopping at each hour (American Time Use Survey,
// table A-3) rises as they go in and falls as they come out, and a stay turns over in its own time (a working day, an errand).
// A home sends out those arriving elsewhere and takes back those leaving. Tables in data/catalog/traffic.json.
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr float M_PER_TABLE = 100.f; // people_kind_m counts people per this many metres of street
constexpr float HALF_HOUR = 0.5f, EVEN_SHARE = 0.5f;

float at_hour(const float *table, float hour) {
	const float h = std::fmod(std::fmod(hour, float(HOURS)) + float(HOURS), float(HOURS)), u = h - std::floor(h);
	const int i = int(h);
	return table[i] * (1 - u) + table[(i + 1) % HOURS] * u;
}

struct Flow {
	float in = 0, out = 0;
};

Flow stay_flow(const float *table, float stay_h, float hour) {
	const float there = at_hour(table, hour), rise = at_hour(table, hour + HALF_HOUR) - at_hour(table, hour - HALF_HOUR);
	return { std::max(0.f, rise) + there / stay_h, std::max(0.f, -rise) + there / stay_h };
}

Flow flow_of(const TrafficParams &p, int kind, float hour) {
	const Flow work = stay_flow(p.people_working, p.work_stay_h, hour), shop = stay_flow(p.people_purchasing, p.errand_stay_h, hour);
	if (kind == int(PlaceKind::WORK)) return work;
	if (kind == int(PlaceKind::SHOP) || kind == int(PlaceKind::CIVIC)) return shop;
	return { work.out + shop.out, work.in + shop.in };
}

} // namespace

PeopleNow people_now(const TrafficParams &p, float hour) {
	PeopleNow now;
	for (int k = 0; k < PLACE_KINDS; k++) {
		now.per_m[k] = p.people_kind_m[k] / M_PER_TABLE * at_hour(p.people_busy + k * HOURS, hour);
		const Flow f = flow_of(p, k, hour);
		now.out[k] = f.in + f.out > 0 ? f.out / (f.in + f.out) : EVEN_SHARE;
		now.peak = std::max(now.peak, now.per_m[k]);
	}
	return now;
}

double people_wanted(const CityState &s, const PeopleNow &now) {
	double sum = 0;
	for (int k = 0; k < PLACE_KINDS; k++) sum += s.traffic.ped_kind_m[k] * double(now.per_m[k]);
	return sum * double(s.catalog.traffic.people_share);
}

float zone_people(const TrafficParams &p, const PeopleNow &now, int zone) {
	float sum = 0;
	for (int k = 0; k < PLACE_KINDS; k++) sum += p.zone_kinds[zone * PLACE_KINDS + k] * now.per_m[k];
	return sum;
}

} // namespace traffic
} // namespace city
