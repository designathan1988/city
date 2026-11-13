// The bomb (Roadcraft actionsWiring.ts strikeAt / explodeAt): a blow of the player's force at a point. The blast model
// (sim/demolition/blast_model.h) says how hard it hits at each distance; this applies it to the city through the
// journal, so the whole blow is one undo step: buildings brought down or worn, the crater in the ground, the roads it
// cuts, the furniture it knocks out, the vehicles and walkers it reaches. The render is told by events.
#include "sim/commands/strike_internal.h"

#include "sim/commands/road_edit.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr size_t EVENT_CAP = 8192; // events kept when nothing drains them (a headless run)
constexpr float ROOF_MIN_M = 0.5f; // a burst this high over the ground, off a street's deck, is on a building
} // namespace

void emit_damage_event(CityState &s, const DamageEvent &e) {
	if (s.log.damage_events.size() < EVENT_CAP) s.log.damage_events.push_back(e);
}

void Blow::emit(const DamageEvent &e) const { emit_damage_event(s, e); }

float lot_distance(const Building &b, float x, float z) {
	const float c = std::cos(b.rot), sn = std::sin(b.rot), dx = x - b.x, dz = z - b.z;
	const float u = std::fabs(dx * c - dz * sn) - b.w * 0.5f, v = std::fabs(dx * sn + dz * c) - b.d * 0.5f;
	return std::hypot(std::max(u, 0.f), std::max(v, 0.f));
}

EditResult strike_at(CityState &s, double x, double z, double strength, double height) {
	EditResult r;
	const BlastParams &p = s.catalog.blast;
	if (!s.terrain.in_bounds(float(x), float(z)) || strength < p.strength_min) {
		r.error = "tool.failed";
		return r;
	}
	const float st = float(std::min(strength, double(p.strength_max)));
	const float ground = s.terrain.height_at(float(x), float(z));
	double t, d;
	const int street = s.roads.find_segment_near(x, z, s.catalog.tune.road_pick_m, t, d);
	// on the street, whatever the ground under it: the terrain lies SHAPE_DROP_M under every street (sim/terrain/road_shape.cpp), and the
	// player's click lands on the deck (RoadNetwork::ray_deck from just over the point, straight down, meets it within ROOF_MIN_M)
	double deck_t = 0;
	const bool on_street = s.roads.ray_deck(x, height + ROOF_MIN_M, z, 0, -1, 0, 2 * ROOF_MIN_M, deck_t);
	const bool on_building = !on_street && height > ground + ROOF_MIN_M;
	Blow b{ s, p, float(x), float(z), ground, float(std::max(height, double(ground))), st, blast::charge_kg(p, st),
		street >= 0 && !on_building ? blast::Surface::HARD : blast::Surface::EARTH, on_building, STRIKE_SALT ^ s.damage.strikes, r };
	const uint32_t struck = s.damage.strikes++; // undone with the blow: the next one draws what this one drew
	j_nature(s, sizeof(struck), [struck](CityState &st, bool fwd) { st.damage.strikes = fwd ? struck + 1 : struck; });
	DamageEvent e;
	e.kind = DamageEvent::BLAST;
	e.x = b.x; e.y = b.height; e.z = b.z;
	e.dx = st;
	e.a = blast::range_at_pressure(p, b.kg, p.collapse_kpa);
	e.b = b.kg;
	e.c = on_building ? 2.f : b.soil == blast::Surface::HARD ? 1.f : 0.f;
	e.d = blast::range_at_pressure(p, b.kg, p.light_kpa);
	b.emit(e);
	strike_buildings(b);
	strike_ground(b);
	strike_traffic(b);
	strike_people(b);
	s.damage.growth_hold_until = std::max(s.damage.growth_hold_until, s.damage.now + double(p.hold_growth_s));
	r.ok = true;
	return r;
}

} // namespace city
