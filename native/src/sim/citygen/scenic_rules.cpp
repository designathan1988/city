// The rules and zoning of a scenic city's streets, see scenic_rules.h.
#include "sim/citygen/scenic_rules.h"

#include "core/seeded.h"
#include "sim/citygen/city_plan.h"

namespace city {

Polyline scenic_road_line(const ScenicCity &city, const ScenicRoad &road) {
	if (road.river < 0 || size_t(road.river) >= city.land.rivers.size()) return road.line;
	return polyline_offset(city.land.rivers[size_t(road.river)].line, road.offset_m);
}

ScenicRules::ScenicRules(const ScenicCity &city, const Catalog &catalog, TerrainGen &land) : city_(city), catalog_(catalog), land_(land) {
	for (size_t i = 0; i < city.roads.size(); i++) {
		lines_.push_back(scenic_road_line(city, city.roads[i]));
		if (highway(city.roads[i].profile)) highways_.push_back(i);
	}
}

int ScenicRules::rank(int profile) const { return catalog_.profile(profile).priority; }

bool ScenicRules::highway(int profile) const { return catalog_.profile(profile).road_class == RoadClass::HIGHWAY; }

int ScenicRules::zone_beside(int profile, double x, double z) const {
	if (highway(profile) || catalog_.profile(profile).rail) return catalog_.gen_zones.none;
	return zone_at(V2(float(x), float(z)), rank(profile) > 0);
}

int ScenicRules::district_at(V2 p) const {
	int best = 0;
	float near = BIG;
	for (size_t i = 0; i < city_.districts.size(); i++) {
		const float d = (city_.districts[i].seed - p).len2();
		if (d < near) {
			near = d;
			best = int(i);
		}
	}
	return best;
}

bool ScenicRules::in_park(V2 p) const {
	for (const Polyline &ring : city_.parks)
		if (ring_holds(ring, p)) return true;
	return false;
}

bool ScenicRules::near_highway(V2 p, float reach) const {
	for (size_t i : highways_)
		if (polyline_distance(lines_[i], p) < reach) return true;
	return false;
}

int ScenicRules::zone_at(V2 p, bool on_main) const {
	if (!road_gen::land(land_, p.x, p.y) || in_park(p)) return catalog_.gen_zones.none;
	for (const Polyline &ring : city_.works)
		if (ring_holds(ring, p)) return catalog_.gen_zones.industrial;
	const ScenicDistrict &d = city_.districts[size_t(district_at(p))];
	const ScenicZoning &z = city_.zoning;
	float heat = d.heat, trade = d.trade;
	for (const ScenicHeat &h : city_.heat) {
		const float share = smooth01(1 - polyline_distance(h.line, p) / h.reach_m);
		heat += h.heat * share;
		trade += h.trade * share;
	}
	heat += z.noise_heat * float(value_noise(p.x / z.noise_cell_m, p.y / z.noise_cell_m));
	const int density = heat > z.high_heat ? 2 : heat > z.medium_heat ? 1 : 0;
	return zone_of(catalog_.gen_zones, trade > z.commerce_trade || (on_main && trade > z.main_trade), density);
}

} // namespace city
