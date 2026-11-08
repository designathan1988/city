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


// The distance (m) from a point to a building's lot rectangle (0 inside it).
float lot_distance(const Building &b, float x, float z);

void cut_roads(Blow &b, float radius); // strike_roads.cpp: a crater that leaves no way past takes the stretch out of the network
void strike_buildings(Blow &b);
void strike_ground(Blow &b); // the crater, the terrain, the roads it cuts, the furniture
void strike_traffic(Blow &b); // the vehicles
void strike_people(Blow &b); // the walkers

} // namespace city
