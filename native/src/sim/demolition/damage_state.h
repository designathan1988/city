// What blows and shots have done to the city, kept in CityState (a field of it) so it is saved, undone and read by the
// render: craters in the ground, ruins left by buildings brought down, street furniture and trees knocked out, buildings
// burning. People and vehicles that died are simply gone from the traffic; this is only what stays on the map.
#pragma once

#include "sim/buildings/building_store.h"
#include "sim/catalog/furniture_types.h"
#include "sim/demolition/damage_event.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <utility>
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
};

// A pole, signal post, hydrant, bench, bin, tree, lighting column, shrub or small box (a post box, a phone, a sign) a blow took
// out: placed by kind and place, so the street furniture derived from the road (sim/roads/street_furniture.h) and the trees skip
// it. Saved: a new kind goes at the end.
enum class GoneKind : uint8_t { POLE = 0, POST, HYDRANT, BENCH, BIN, TREE, COLUMN, SHRUB, BOX, COUNT };
// What a piece of street furniture goes as when a blow knocks it out; a drain lies in the ground and is never knocked out.
bool knockable(FurnitureKind k);
GoneKind gone_kind(FurnitureKind k);
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
};
constexpr float SCAR_REACH_M = 1.4f, SCAR_REACH_PER_FORCE_M = 0.55f; // the 4.0's: how far from where it struck a blow breaks a building
inline float scar_reach_m(float force) { return SCAR_REACH_M + force * SCAR_REACH_PER_FORCE_M; }

// A building on fire until `until` (seconds of DamageState::now).
struct Fire {
	int building = -1;
	float x = 0, z = 0;
	double until = 0;
};

struct DamageState;

// A list of DamageState read like a vector and changed only by DamageState, each change raising its revision: the revision,
// not the count, is what a reader compares to know the list changed (an undo can take one out and a blow put another in).
template <typename T>
class Revised {
public:
	using const_iterator = typename std::vector<T>::const_iterator;
	size_t size() const { return items_.size(); }
	bool empty() const { return items_.empty(); }
	const T &operator[](size_t i) const { return items_[i]; }
	const_iterator begin() const { return items_.begin(); }
	const_iterator end() const { return items_.end(); }
	const std::vector<T> &items() const { return items_; }
	uint64_t revision() const { return revision_; } // raised by every change; never lowered, not even by a clear or a load
	// Raised by every change but an append: while it holds, the items a reader saw are still the first ones (only new ones came).
	uint64_t layout_revision() const { return layout_; }
	// A loaded city taking the place of `before`: its revisions go on from the live ones, so a reader never sees one repeat.
	void follow_on(const Revised &before) {
		revision_ += before.revision_;
		layout_ += before.layout_;
	}

private:
	friend struct DamageState;
	void push(const T &v) {
		items_.push_back(v);
		revision_++;
	}
	void pop() {
		items_.pop_back();
		rewritten();
	}
	void insert(size_t at, const T &v) {
		items_.insert(items_.begin() + std::ptrdiff_t(at), v);
		rewritten();
	}
	void erase(size_t at) {
		items_.erase(items_.begin() + std::ptrdiff_t(at));
		rewritten();
	}
	void replace(std::vector<T> items) {
		items_ = std::move(items);
		rewritten();
	}
	void follow(const Revised &before) { // an empty list in place of `before` (a clear): its revisions go on from there
		revision_ = before.revision_ + 1;
		layout_ = before.layout_ + 1;
	}
	void rewritten() {
		revision_++;
		layout_++;
	}
	std::vector<T> items_;
	uint64_t revision_ = 0, layout_ = 0;
};

struct DamageState {
	Revised<Crater> craters;
	Revised<Ruin> ruins;
	std::vector<Gone> gone;
	std::vector<Hurt> hurt; // buildings that took a blow without falling
	Revised<Scar> scars; // the parts of them broken off, in the order of the blows
	std::vector<Fire> fires; // not saved: a fire does not outlast the session
	double now = 0; // seconds of simulation since the map began (the clock of fires and of the growth hold)
	double growth_hold_until = 0; // nothing grows on the lots while rubble lies (seconds of `now`)
	uint32_t shots = 0; // rounds fired since the map began (the stream of a round)
	uint32_t strikes = 0; // blows struck since the map began: the stream every draw of a blow comes from

	// Everything gone (a new map); the revisions of the lists go on rising, so a reader still sees the change.
	void clear();
	// The index of the building's record in `hurt`, or -1.
	int hurt_of(uint32_t seed, float x, float z) const;
	// Is there a ruin whose lot covers the point? (index or -1)
	int ruin_at(float x, float z, float pad) const;
	// Was furniture of this kind knocked out within `reach` of the point?
	bool is_gone(GoneKind kind, float x, float z, float reach) const;
	// The scars of the building (seed, bx, bz), in the order of the blows, without walking the others'.
	template <typename F>
	void each_scar_of(uint32_t seed, float bx, float bz, F &&f) const {
		const auto it = scars_by_seed_.find(seed);
		if (it == scars_by_seed_.end()) return;
		for (uint32_t i : it->second) {
			const Scar &c = scars[i];
			if (c.bx == bx && c.bz == bz) f(c);
		}
	}
	int scar_count_of(uint32_t seed, float bx, float bz) const;

	// The changes of the three revised lists (sim/commands/journal_damage.cpp journals them; the save's reader restores them).
	// A loaded city taking the place of `before` (sim/state/city_take.cpp): the three lists' revisions continue the live ones.
	void continue_after(const DamageState &before) {
		craters.follow_on(before.craters);
		ruins.follow_on(before.ruins);
		scars.follow_on(before.scars);
	}
	void add_crater(const Crater &c) { craters.push(c); }
	void drop_last_crater() {
		if (!craters.empty()) craters.pop();
	}
	int add_ruin(const Ruin &r); // its index
	void insert_ruin(size_t at, const Ruin &r) { ruins.insert(std::min(at, ruins.size()), r); }
	void erase_ruin(size_t at) {
		if (at < ruins.size()) ruins.erase(at);
	}
	void add_scar(const Scar &c);
	void drop_last_scar();
	void restore(std::vector<Crater> craters_read, std::vector<Ruin> ruins_read, std::vector<Scar> scars_read); // a save read

private:
	void index_scars(); // scars_by_seed_ from the whole list
	std::unordered_map<uint32_t, std::vector<uint32_t>> scars_by_seed_; // by the building's seed: its scars' indices, ascending
};

// One step of the damage clock (caller holds the map exclusively): time passes, fires burn out.
struct CityState;
// Tells the render of a blow, a shot, a fall or a drop of blood (kept when nothing drains them, up to a cap), stamped with the newest
// publication of the poses: the one owner of the events' list (the caller holds the map exclusively, so no publication runs meanwhile).
void emit_damage_event(CityState &s, DamageEvent e);
void damage_step(CityState &s, double dt);
void bleed_step(CityState &s, double dt); // wounds.cpp: the wounded bleed, and die of it

} // namespace city
