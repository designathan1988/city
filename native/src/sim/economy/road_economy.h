// The money of road edits (Roadcraft world/economy.ts, editor/roads/economy.ts): what a road is worth by how it is
// built, what an edit costs, and whether it can be paid. Roads are all the player pays for; demolishing gives a share
// of the price back, generated roads included.
#pragma once

#include "sim/roads/road_grade.h"

#include <cstdint>

namespace city {

struct CityState;
struct EditResult;
struct Tuning;

// The strings.csv key of an edit refused for lack of money.
constexpr const char *REFUSED_FUNDS = "hint.rule.funds";

// How a road stands: its structure and its authored height over the ground at each end (metres).
struct RoadBuild {
	Structure structure = Structure::GROUND; // the segment's (sim/roads/road_grade.h, the one owner)
	double height_a = 0, height_b = 0;
};

// Metres of a road built each way.
struct BuildLengths {
	double ground = 0, embankment = 0, trench = 0, bridge = 0, tunnel = 0;
};

// What an edit costs: money taken (positive) or given back (negative), the balance after it, and whether it can be paid.
struct RoadCharge {
	double amount = 0, balance = 0;
	bool affordable = true;
};

BuildLengths build_lengths(const Tuning &tn, const RoadBuild &b, double len);
// What `len` metres of a profile cost, built as `b`.
double build_value(const CityState &s, int profile, double len, const RoadBuild &b);
// The same at grade: the price of a road in hand.
double road_value(const CityState &s, int profile, double len);

// The one rule: turning roads worth `before` into roads worth `after` with `balance` in hand.
RoadCharge charge_for(double before, double after, double balance, double refund_share);
// The rule for a change of value in this city, whose unlimited money pays anything.
RoadCharge charge_of(const CityState &s, double value_change);
// What the ops recorded in the open entry from index `from` on added to the value of the roads.
double edit_value_change(const CityState &s, size_t from);
// Charges the command in progress. False when it cannot be paid: `r` says why and the caller rolls the journal back.
bool settle_road_edit(CityState &s, EditResult &r);
// Charges what was recorded from op `from` on: one road of a command that lays several, each paid for on its own against
// the map as it stood before it (Roadcraft editor/blocks.ts commitBlockGrid lays each line by commitRoadPath, whose
// layCopy settles that line alone: settleRoadEdit(roadsBefore(doc), work)).
bool settle_road_edit(CityState &s, EditResult &r, size_t from);

} // namespace city
