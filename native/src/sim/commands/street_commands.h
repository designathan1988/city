// The Street tab's commands (street_snap.cpp, street_commands.cpp, pole_runs.cpp): where an item or a barrier may stand, placing
// and taking away, the kit a road carries through an edit, and the pole runs. The commands' callers hold map_mutex exclusively;
// snap_item is also the preview's, under the shared lock (its FurnitureRead says which it is).
#pragma once

#include "sim/commands/commands.h"
#include "sim/roads/furniture_kept.h"

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace city {

struct ItemSnap {
	bool ok = false;
	std::string reason; // strings.csv key when refused
	double x = 0, z = 0;
};
// Where an item of `kind` put at (x, z) would stand, or why it cannot (Roadcraft landscape.ts snapLandscape).
ItemSnap snap_item(const CityState &s, ItemKind kind, double x, double z, double reach, FurnitureRead read);
// A point of a barrier run on the footway's back, or at the kerb for a guardrail and a railing.
void snap_barrier_point(const CityState &s, BarrierKind kind, double &x, double &z);
// Why a run cannot be built: "" or a strings.csv key.
std::string barrier_problem(const CityState &s, BarrierKind kind, const std::vector<float> &pts);
// A road's rule at a junction (SegRule), from the inspector: the junction goes by priority, as a plate makes it.
EditResult set_approach_rule(CityState &s, int node, int seg, int rule);
EditResult place_item(CityState &s, ItemKind kind, SignType sign, const std::string &text, double x, double z, double reach);
// A row of items along the sidewalk from a to b, each where the tool would accept it, every_m apart (a drag).
EditResult place_item_row(CityState &s, ItemKind kind, double ax, double az, double bx, double bz, double every_m);
EditResult build_barrier(CityState &s, BarrierKind kind, const std::vector<float> &pts);
// Takes what stands nearest (x, z) within reach: an item, a pole and its wires, a barrier or a piece of the street kit.
bool remove_street_at(CityState &s, double x, double z, double reach);
// A road going: what stands on its sidewalks goes with it.
void remove_street_kit(CityState &s, int seg);
// What stands on a road being reprofiled, kept by its place along the kerb, and put back where the new sidewalks lie.
struct CarriedPiece {
	int item = -1, pole = -1, side = 0;
	double v = 0;
	float from_kerb = 0;
};
std::vector<CarriedPiece> street_kit_of(const CityState &s, int seg);
// Where a carried piece stands now: a point of a footway, or false when its road has none there any more (the piece goes).
using KitPlace = std::function<bool(const CarriedPiece &, double &x, double &z)>;
// The pieces put where `place` says, the poles' wires strung again between their new places (Roadcraft streetKit.ts
// carryStreetKit).
void carry_kit(CityState &s, const std::vector<CarriedPiece> &kit, const KitPlace &place);
// The kit back on `seg` reprofiled: the same place along the kerb, the new sidewalk.
void carry_street_kit(CityState &s, int seg, const std::vector<CarriedPiece> &kit);
// The kit of a road `old_len` long laid again as `laid` along `q` (Move): each piece at the same share of the length, on the
// piece of `laid` there, the same side and distance from the kerb.
void carry_kit_along(CityState &s, const std::vector<CarriedPiece> &kit, double old_len, const Quad &q, const std::vector<int> &laid);

// What a pole run would build (Roadcraft poles.ts PoleRunPlan), the same plan for the preview and the command.
enum class LampMode : uint8_t { NONE = 0, ALTERNATE, ALL };
struct PlannedPole {
	double x = 0, z = 0;
	int existing = -1; // the pole standing here, which the run reuses
	bool lamp = false;
	PoleKind kind = PoleKind::URBAN; // a pole the run reuses keeps its own
};
struct PoleRunPlan {
	std::vector<PlannedPole> poles;
	std::string refused; // strings.csv key when nothing would be built
	bool from_free = false, to_free = false;
	double from_x = 0, from_z = 0, to_x = 0, to_z = 0;
};
PoleRunPlan plan_pole_run(const CityState &s, double ax, double az, double bx, double bz, double reach, LampMode lamps, PoleKind kind);
// The run planned and strung; `last`, when given, takes the place of its last pole (where the next run goes on from).
EditResult build_pole_run(CityState &s, double ax, double az, double bx, double bz, double reach, LampMode lamps, PoleKind kind,
		PlannedPole *last = nullptr);

} // namespace city
