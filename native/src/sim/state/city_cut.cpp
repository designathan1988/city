// The parts of the city copied into a cut, and the facts it carries (city_cut.h).
#include "sim/state/city_cut.h"

#include "sim/traffic/ped_system.h"
#include "sim/traffic/traffic_system.h"

namespace city {

namespace {
// A box that holds nothing: the cut of a store over it answers for no place and holds no record (a part the job did not ask).
constexpr CutBox NOWHERE{ 1.f, 1.f, -1.f, -1.f };
} // namespace

CityCut cut_held(const CityState &s, const CutBox &box, unsigned parts) {
	if (CHECKS && map_hold::mine().depth == 0) city_stop("a cut of the city made without the map held");
	CityCut c;
	c.catalog = &s.catalog;
	c.designs = &s.buildings.designs;
	c.marking_style = s.marking_style;
	c.revision = s.revision;
	c.street_revision = s.street_revision;
	unsigned ground = GROUND_FINAL;
	if (parts & CUT_GROUND_BASE) ground |= GROUND_BASE;
	if (parts & CUT_WATER) ground |= GROUND_WATER;
	c.ground = s.terrain.ground_patch(box.minx, box.minz, box.maxx, box.maxz, ground);
	const auto of = [&](unsigned part) { return parts & part ? box : NOWHERE; };
	const CutBox roads = of(CUT_ROADS), street = of(CUT_STREET), buildings = of(CUT_BUILDINGS);
	c.roads = s.roads.cut(roads.minx, roads.minz, roads.maxx, roads.maxz);
	c.street = s.street.cut(street.minx, street.minz, street.maxx, street.maxz);
	c.buildings = s.buildings.cut(buildings.minx, buildings.minz, buildings.maxx, buildings.maxz);
	return c;
}

void keep_furniture(CityCut &cut, const CityState &live, int sid) {
	const auto at = cut.facts.furniture.try_emplace(sid);
	if (at.second) traffic::street_furniture_of(live, sid, FURNITURE_ALL, at.first->second);
}

void keep_node(CityCut &cut, const CityState &live, int node) {
	const auto at = cut.facts.signs.try_emplace(node);
	if (!at.second) return;
	traffic::junction_signs(live, node, at.first->second);
	if (traffic::control_of(live, node) == Control::SIGNAL) cut.facts.signalled.insert(node);
}

void furniture_of(const CityView &s, int sid, uint32_t kinds, std::vector<FurnitureSpot> &out) {
	if (s.live) {
		traffic::street_furniture_of(*s.live, sid, kinds, out);
		return;
	}
	const auto at = s.facts->furniture.find(sid);
	if (at == s.facts->furniture.end()) {
		if (CHECKS) city_stop("the furniture of a segment the job's cut did not keep");
		return;
	}
	for (const FurnitureSpot &f : at->second)
		if (kinds & furniture_bit(f.kind)) out.push_back(f);
}

bool node_signalled(const CityView &s, int node) {
	if (s.live) return traffic::control_of(*s.live, node) == Control::SIGNAL;
	if (CHECKS && !s.facts->signs.count(node)) city_stop("the control of a node the job's cut did not keep");
	return s.facts->signalled.count(node) != 0;
}

void node_signs(const CityView &s, int node, std::vector<SignStand> &out) {
	if (s.live) {
		traffic::junction_signs(*s.live, node, out);
		return;
	}
	const auto at = s.facts->signs.find(node);
	if (at == s.facts->signs.end()) {
		if (CHECKS) city_stop("the plates of a node the job's cut did not keep");
		return;
	}
	out.insert(out.end(), at->second.begin(), at->second.end());
}

} // namespace city
