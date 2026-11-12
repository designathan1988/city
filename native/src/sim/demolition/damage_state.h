// What blows and shots have done to the city, kept in CityState (a field of it) so it is saved, undone and read by the
// render: craters in the ground, ruins left by buildings brought down, street furniture and trees knocked out, buildings
// burning. People and vehicles that died are simply gone from the traffic; this is only what stays on the map.
#pragma once

#include "sim/buildings/building_store.h"
#include "core/generation.h"
#include "core/bounds_index.h"

#include <cstdint>
#include <cstddef>
#include <array>
#include <map>
#include <tuple>
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
	float blow_x = 0, blow_y = 0, blow_z = 0, blow_force = 0; // the blow that brought it down: how it fell, so how it lies
	bool operator==(const Ruin &o) const {
		return x == o.x && z == o.z && w == o.w && d == o.d && rot == o.rot && seed == o.seed && record == o.record
				&& blow_x == o.blow_x && blow_y == o.blow_y && blow_z == o.blow_z && blow_force == o.blow_force;
	}
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

// A blow that broke part of a standing building (destruction.ts `strike`): what it reached falls, and what hung on that with it;
// the rest stands. `seed`, `bx`, `bz` name the building as `Hurt` does; x y z is where it struck, `force` how hard. One blow, one scar.
struct Scar {
	uint32_t seed = 0;
	float bx = 0, bz = 0, x = 0, y = 0, z = 0, force = 0;
	bool operator==(const Scar &o) const {
		return seed == o.seed && bx == o.bx && bz == o.bz && x == o.x && y == o.y && z == o.z && force == o.force;
	}
};
constexpr float SCAR_REACH_M = 1.4f, SCAR_REACH_PER_FORCE_M = 0.55f; // the 4.0's: how far from where it struck a blow breaks a building
inline float scar_reach_m(float force) { return SCAR_REACH_M + force * SCAR_REACH_PER_FORCE_M; }

// A building on fire until `until` (seconds of DamageState::now).
struct Fire {
	int building = -1;
	float x = 0, z = 0;
	double until = 0;
};

// Decode-only value boundary: restore publishes records and their derived indices together.
struct DamageRecords {
	std::vector<Ruin> ruins;
	std::vector<b40::BodyRef> ruin_bodies;
	std::vector<Gone> gone;
	std::vector<Hurt> hurt;
	std::vector<Scar> scars;
};

struct DamageState {
	uint64_t craters_revision = next_generation(), ruins_revision = next_generation(), scars_revision = next_generation(); // derived-state identity, never serialized
	std::vector<Crater> craters;
	const std::vector<Ruin> &ruins() const { return records_.ruins; }
	const std::vector<b40::BodyRef> &ruin_bodies() const { return records_.ruin_bodies; }
	const std::vector<Gone> &gone() const { return records_.gone; }
	const std::vector<Hurt> &hurt() const { return records_.hurt; }
	const std::vector<Scar> &scars() const { return records_.scars; }
	void restore(DamageRecords records);
	void insert_ruin(size_t at, const Ruin &ruin, b40::BodyRef body);
	void remove_ruin(size_t at);
	void add_gone(const Gone &record);
	void pop_gone();
	void set_hurt(const Hurt &record, float amount);
	void add_scar(const Scar &record);
	void pop_scar();
	const std::vector<int> &scar_ids(uint32_t seed, float x, float z) const;
	std::vector<int> ruins_near(float x, float z, float reach) const;
	std::vector<Fire> fires; // not saved: a fire does not outlast the session
	double now = 0; // seconds of simulation since the map began (the clock of fires and of the growth hold)
	double growth_hold_until = 0; // nothing grows on the lots while rubble lies (seconds of `now`)
	uint32_t shots = 0; // rounds fired since the map began (the stream of a round)
	uint32_t strikes = 0; // blows struck since the map began: the stream every draw of a blow comes from

	void clear() { *this = DamageState(); }
	// The index of the building's record in `hurt`, or -1.
	int hurt_of(uint32_t seed, float x, float z) const;
	// Is there a ruin whose lot covers the point? (index or -1)
	int ruin_at(float x, float z, float pad) const;
	// Was furniture of this kind knocked out within `reach` of the point?
	bool is_gone(GoneKind kind, float x, float z, float reach) const;
private:
	using Identity = std::tuple<uint32_t, float, float>;
	DamageRecords records_;
	std::map<Identity, std::vector<int>> scar_rows_, hurt_rows_;
	BoundsIndex ruin_index_;
	std::array<BoundsIndex, size_t(GoneKind::COUNT)> gone_index_;
	void rebuild_ruins();
	void rebuild_hurt();
};

// One step of the damage clock (caller holds the map exclusively): time passes, fires burn out.
struct CityState;
void damage_step(CityState &s, double dt);
void bleed_step(CityState &s, double dt); // wounds.cpp: the wounded bleed, and die of it

} // namespace city
