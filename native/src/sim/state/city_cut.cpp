// The parts of the city copied into a cut, and the facts it carries (city_cut.h).
#include "sim/state/city_cut.h"

#include "sim/buildings/lot_access.h"
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
	c.seed = s.seed;
	unsigned ground = PATCH_FINAL;
	if (parts & CUT_GROUND_BASE) ground |= PATCH_NATURAL;
	if (parts & CUT_WATER) ground |= PATCH_WATER;
	c.ground = s.terrain.ground_patch(box.minx, box.minz, box.maxx, box.maxz, ground);
	const auto of = [&](unsigned part) { return parts & part ? box : NOWHERE; };
	const CutBox roads = of(CUT_ROADS), street = of(CUT_STREET), buildings = of(CUT_BUILDINGS), nature = of(CUT_NATURE);
	c.roads = s.roads.cut(roads.minx, roads.minz, roads.maxx, roads.maxz);
	c.street = s.street.cut(street.minx, street.minz, street.maxx, street.maxz);
	c.buildings = s.buildings.cut(buildings.minx, buildings.minz, buildings.maxx, buildings.maxz);
	c.nature = s.nature.cut(nature.minx, nature.minz, nature.maxx, nature.maxz);
	return c;
}

void take_pole_reach(CutBox &box, const StreetWorks &street, float x0, float z0, float x1, float z1) {
	street.poles_in(x0, z0, x1, z1, [&](int id) {
		for (int span : street.poles.items[size_t(id)].spans) {
			const UtilitySpan &sp = street.spans.items[size_t(span)];
			for (int end : { sp.a, sp.b }) {
				const UtilityPole &o = street.poles.items[size_t(end)];
				box.take(o.x, o.z, o.x, o.z);
			}
		}
	});
}

void keep_furniture(CityCut &cut, const CityState &live, int sid) {
	if (CHECKS && cut.facts.kinds_kept != FURNITURE_ALL) city_stop("a cut keeping the lamps alone asked for every piece");
	const auto at = cut.facts.furniture.try_emplace(sid);
	if (at.second) traffic::street_furniture_of(live, sid, FURNITURE_ALL, at.first->second);
}

void keep_lamps(CityCut &cut, const CityState &live, int sid) {
	if (CHECKS && cut.facts.kinds_kept != LIT_FURNITURE && !cut.facts.furniture.empty()) city_stop("a cut keeping every piece asked for the lamps alone");
	cut.facts.kinds_kept = LIT_FURNITURE;
	const auto at = cut.facts.furniture.try_emplace(sid);
	if (at.second) traffic::street_furniture_of(live, sid, LIT_FURNITURE, at.first->second, FurnitureRead::PLACED);
}

void keep_node(CityCut &cut, const CityState &live, int node) {
	const auto at = cut.facts.signs.try_emplace(node);
	if (!at.second) return;
	traffic::junction_signs(live, node, at.first->second);
	if (traffic::control_of(live, node) == Control::SIGNAL) cut.facts.signalled.insert(node);
}

void keep_gates(CityCut &cut, const CityState &live, int sid) {
	const auto at = cut.facts.gates.try_emplace(sid);
	if (at.second) street_gates(live.roads, live.buildings, sid, 0.0, at.first->second.data());
}

namespace {
// A segment's end as one key: twice its id, one more at its b end.
int64_t leg_key(int sid, bool at_b) { return int64_t(sid) * 2 + (at_b ? 1 : 0); }
} // namespace

void keep_leg_rules(CityCut &cut, const CityState &live, int sid) {
	const RoadSegment &g = live.roads.segs[size_t(sid)];
	cut.facts.leg_rules[leg_key(sid, false)] = traffic::approach_rule(live, g.a, sid);
	cut.facts.leg_rules[leg_key(sid, true)] = traffic::approach_rule(live, g.b, sid);
}

int leg_rule_of(const CityView &s, int node, int sid) {
	if (s.live) return traffic::approach_rule(*s.live, node, sid);
	const auto at = s.facts->leg_rules.find(leg_key(sid, s.roads.segs[size_t(sid)].b == node));
	if (at == s.facts->leg_rules.end()) {
		if (CHECKS) city_stop("the rule of an approach the job's cut did not keep");
		return RULE_NONE;
	}
	return at->second;
}

void kerb_gates_of(const CityView &s, int sid, std::vector<ArcSpan> out[2]) {
	if (s.live) {
		street_gates(s.roads, s.buildings, s.designs, sid, 0.0, out);
		return;
	}
	const auto at = s.facts->gates.find(sid);
	if (at == s.facts->gates.end()) {
		if (CHECKS) city_stop("the car gates of a segment the job's cut did not keep");
		return;
	}
	out[STREET_RIGHT] = at->second[STREET_RIGHT];
	out[STREET_LEFT] = at->second[STREET_LEFT];
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
	if (CHECKS && (kinds & ~s.facts->kinds_kept)) city_stop("a kind of furniture the job's cut did not keep");
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
