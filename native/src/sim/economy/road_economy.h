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
	void add(const BuildLengths &o);
};

// The way a stretch of road is built, by where its deck stands over the natural ground.
enum class BuildWay : uint8_t { GROUND, EMBANKMENT, TRENCH, BRIDGE, TUNNEL };
// The way at a station whose deck stands `over` metres over the ground (negative: under it), on a road built as
// `structure`: a raised road is on piers and a sunken one bored all along; at grade it is bored under the cover a bore
// needs (the depth the render bores at: render/roads/road_structures.cpp), on piers past economy.on_piers_from_m (where
// the ground stops being shaped to it: sim/terrain/road_shape.cpp), filled or cut past economy.fill_from_m.
BuildWay way_at(const Tuning &tn, Structure structure, float over);
// The metres built each way along a road's stations (a station stands for the half step either side of it).
BuildLengths lengths_along(const Tuning &tn, Structure structure, const AloneStations &st);
// What those metres are worth: metres of the same road on plain ground (the catalogue's factor a way).
double worth_of(const CityState &s, const BuildLengths &l);

// A road as it is built when laid by itself along `q` between the heights its two ends stand at: its stations, the metres
// each way and its worth. The one function the road tool's draft and the build both call: the click costs what the draft
// showed.
struct Quad;
struct BuiltAlone {
	AloneStations stations;
	BuildLengths lengths;
	double worth = 0;
};
BuiltAlone built_alone(const CityState &s, const Quad &q, const std::vector<double> &table, int profile, uint16_t flags, float stand_a, float stand_b,
		bool authored);
// A station of a road built alone, where it stands on the map: its deck, the natural ground under it and the way it is
// built there. What the road tool's draft draws between the road and the ground.
struct DraftStation {
	float x = 0, z = 0, deck = 0, ground = 0;
	BuildWay way = BuildWay::GROUND;
	Structure structure = Structure::GROUND; // of the piece it is a station of
};
// Adds the stations of a road built alone along `q` to `out`, each placed and told its way of building.
void place_stations(const Tuning &tn, const Quad &q, const std::vector<double> &table, Structure structure, const AloneStations &st,
		std::vector<DraftStation> &out);
// The worth of standing segment `seg` from its solved heights (a road whose heights the player changed is worth what it
// now is); its recorded worth when it has no solved profile.
double worth_solved(const CityState &s, int seg);
// What standing segment `seg` is worth as it stands recorded: its worth, or with none (a generated road, an older save's)
// its authored heights' bands (build_lengths), as it was always valued.
double worth_recorded(const CityState &s, int seg);

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
