// The money of road edits (Roadcraft world/economy.ts, editor/roads/economy.ts): what a road is worth by how it is
// built, what an edit costs, and whether it can be paid. Roads are all the player pays for; demolishing gives a share
// of the price back, generated roads included.
#pragma once

#include <cstdint>

namespace city {

struct CityState;
struct EditResult;
struct Tuning;

// The strings.csv key of an edit refused for lack of money.
constexpr const char *REFUSED_FUNDS = "hint.rule.funds";

enum class RoadStructure : uint8_t { GROUND, ELEVATED, BRIDGE, TUNNEL };

// How a road stands: its structure and its authored height over the ground at each end (metres).
struct RoadBuild {
	RoadStructure structure = RoadStructure::GROUND;
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
// What the command in progress added to the value of the roads, read from its journal entry.
double edit_value_change(const CityState &s);
// Charges the command in progress. False when it cannot be paid: `r` says why and the caller rolls the journal back.
bool settle_road_edit(CityState &s, EditResult &r);

} // namespace city
