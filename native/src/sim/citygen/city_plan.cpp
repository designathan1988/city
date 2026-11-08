// The plan of a generated city (city_plan.h). The draws keep their order, so a seed lands its centres where the TypeScript game's does.
#include "sim/citygen/city_plan.h"

#include "core/seeded.h"
#include "core/units.h"
#include "sim/citygen/road_gen.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr uint32_t SEED_MUL = 2654435761U;
constexpr double TURN = 6.283185307179586, NO_CENTRE = 1e30;

double range(Xoshiro128 &r, double lo, double hi) { return lo + r.f() * (hi - lo); }
} // namespace

CityPlan plan_city(uint32_t seed, double radius, const CityPlanDef &def) {
	CityPlan p;
	p.def = def;
	p.radius = radius;
	const uint32_t start = seed * SEED_MUL;
	Xoshiro128 r(start != 0 ? start : 1);
	const double x = range(r, -radius, radius) * def.main_offset, y = range(r, -radius, radius) * def.main_offset;
	p.centres.push_back({ x, y });
	for (int i = 1; i < def.centres; i++) {
		const double a = range(r, 0, TURN) + i * def.centre_turn, d = radius * range(r, def.second_from, def.second_to);
		p.centres.push_back({ std::cos(a) * d, std::sin(a) * d });
	}
	range(r, 0, TURN / 2); // the grain of the streets: drawn for the stream, a block grid has none
	p.industry_angle = range(r, 0, TURN);
	return p;
}

int zone_at(const CityPlan &p, double x, double z, bool on_main) {
	const CityPlanDef &d = p.def;
	const double y = -z; // the seed's centres, sector and noise are the map's: read on the world's z they land mirrored
	double near_share = NO_CENTRE;
	for (size_t i = 0; i < p.centres.size(); i++) {
		const double far = std::hypot(x - p.centres[i][0], y - p.centres[i][1]);
		near_share = std::min(near_share, far / (i == 0 ? 1.0 : d.second_pull));
	}
	const double k = near_share / p.radius; // 0 at a centre, about 1 at the edge
	if (k > d.industry_from) {
		double off = std::fmod(std::fabs(std::atan2(y, x) - p.industry_angle), TURN);
		if (off > TURN / 2) off = TURN - off;
		if (off < d.industry_half_angle) return ZONE_IND;
	}
	const double heat = 1 - k + (d.density - 0.5) * d.density_pull + value_noise(x / d.noise_cell_m, y / d.noise_cell_m) * d.noise_heat;
	const int density = heat > d.high_heat ? 2 : heat > d.medium_heat ? 1 : 0;
	return zone_of(heat > d.commerce_heat || (on_main && heat > d.main_commerce_heat), density);
}

int zone_of(bool commercial, int density) {
	static constexpr int DENSITIES = 3, COMMERCIAL[DENSITIES] = { ZONE_COM_LOW, ZONE_COM, ZONE_COM_HIGH }, RESIDENTIAL[DENSITIES] = { ZONE_RES_LOW, ZONE_RES, ZONE_RES_HIGH };
	const int k = std::clamp(density, 0, DENSITIES - 1);
	return commercial ? COMMERCIAL[k] : RESIDENTIAL[k];
}

} // namespace city
