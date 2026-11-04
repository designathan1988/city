// What blows and shots have done to the city, kept in CityState (a field of it) so it is saved, undone and read by the
// render: craters in the ground, ruins left by buildings brought down, street furniture and trees knocked out, buildings
// burning. People and vehicles that died are simply gone from the traffic; this is only what stays on the map.
#pragma once

#include "sim/buildings/building_store.h"

#include <cstdint>
#include <vector>

namespace city {

// A hole in the ground (or in the paving: `hard`), as dug by a surface burst.
struct Crater {
	float x = 0, z = 0, radius = 0, depth = 0;
	uint8_t hard = 0;
	uint32_t seed = 0;
};

// What a building brought down leaves: its record (the render breaks the same body into the same pieces) and the lot it
// keeps blocked until the player clears it (demolish) .
struct Ruin {
	float x = 0, z = 0, w = 0, d = 0, rot = 0;
	uint32_t seed = 0;
	Building record;
};

// A pole, signal post, hydrant, bench, bin or tree a blow took out: placed by kind and place, so the street furniture
// derived from the road (sim/roads/street_furniture.h) and the trees skip it.
enum class GoneKind : uint8_t { POLE = 0, POST, HYDRANT, BENCH, BIN, TREE, COUNT };
struct Gone {
	uint8_t kind = 0;
	float x = 0, z = 0;
};

// What a standing building has taken: 0 intact .. 1 brought down (the blows add up: "keep hitting to bring it down").
struct Hurt {
	uint32_t seed = 0; // the building's record seed identifies it across edits (ids move on undo)
	float x = 0, z = 0, amount = 0;
};

// A building on fire until `until` (seconds of DamageState::now).
struct Fire {
	int building = -1;
	float x = 0, z = 0;
	double until = 0;
};

struct DamageState {
	std::vector<Crater> craters;
	std::vector<Ruin> ruins;
	std::vector<Gone> gone;
	std::vector<Hurt> hurt; // buildings that took a blow without falling
	std::vector<Fire> fires; // not saved: a fire does not outlast the session
	double now = 0; // seconds of simulation since the map began (the clock of fires and of the growth hold)
	double growth_hold_until = 0; // nothing grows on the lots while rubble lies (seconds of `now`)
	uint32_t strikes = 0; // blows struck since the map began: the stream every draw of a blow comes from

	void clear() { *this = DamageState(); }
	// The index of the building's record in `hurt`, or -1.
	int hurt_of(uint32_t seed, float x, float z) const;
	// Is there a ruin whose lot covers the point? (index or -1)
	int ruin_at(float x, float z, float pad) const;
	// Was furniture of this kind knocked out within `reach` of the point?
	bool is_gone(GoneKind kind, float x, float z, float reach) const;
};

// One step of the damage clock (caller holds the map exclusively): time passes, fires burn out.
struct CityState;
void damage_step(CityState &s, double dt);

} // namespace city
