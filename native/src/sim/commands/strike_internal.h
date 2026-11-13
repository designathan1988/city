// One blow of the bomb (strike_commands.cpp) and the steps that carry it out, one file each. Internal to sim/commands.
#pragma once

#include "sim/commands/commands.h"
#include "sim/demolition/blast_model.h"
#include "sim/demolition/damage_event.h"
#include "sim/state/city_state.h"

#include "core/random.h"

namespace city {

constexpr uint32_t STRIKE_SALT = 0x5b10f7d3U; // the stream of draws of one blow: hash(salt, blows so far, who, what)

struct Blow {
	CityState &s;
	const BlastParams &p;
	float x, z, ground, height; // where it burst on the map, the ground there, the height of the burst
	float strength, kg;
	blast::Surface soil;
	bool on_building;
	uint32_t stream;
	EditResult &result;

	// A draw in 0..1 for `what` about `who`, the same for the same blow whatever the order of the steps.
	float roll(uint32_t what, uint32_t who) const { return hash_float(hash_combine(hash_combine(stream, what), who)); }
	uint32_t bits(uint32_t what, uint32_t who) const { return hash_u32(hash_combine(hash_combine(stream, what), who)); }
	float pressure(float r) const { return blast::airblast(p, kg, r).pressure_kpa; }
	void emit(const DamageEvent &e) const;
};

// Tells the render of a blow, a shot or a fall (kept when nothing drains them, up to a cap).
void emit_damage_event(CityState &s, const DamageEvent &e);

// The distance (m) from a point to a building's lot rectangle (0 inside it).
float lot_distance(const Building &b, float x, float z);

void cut_roads(Blow &b, float radius); // strike_roads.cpp: a crater that leaves no way past takes the stretch out of the network
void strike_buildings(Blow &b);
void strike_ground(Blow &b); // the crater, the terrain, the roads it cuts, the furniture
void strike_traffic(Blow &b); // the vehicles
void strike_people(Blow &b); // the walkers

// shoot_people.cpp: a grave wound, a round's or a blast's (half the health gone, or a limb lost: Pedestrian::lost already holds it):
// down for good the way event `e` fells them (damage_event.h fall_way), bleeding by the worst of their wounds (a leg gone, a limb off,
// else hurt: blast.json shot.bleed_*) until they die of it (wounds.cpp). One rule for both.
constexpr float LIE_FOR_GOOD_S = 600.f; // down until they bleed out (the longest any bleeding lasts)
struct Pedestrian;
void wound_for_good(CityState &s, Pedestrian &w, uint32_t severed, const DamageEvent &e);

} // namespace city
