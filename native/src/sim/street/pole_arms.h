// Which way a pole's cross-arms lie: one
// arm square to a straight line or on the bisector of a gentle bend, and at a corner (poles.corner_deflection_deg or more)
// or a branching an arm for each line leaving it, two lines straight through each other sharing theirs.
#pragma once

#include "sim/street/street_works.h"

#include <vector>

namespace city {

class RoadNetwork;
struct StreetFurnitureDef;

struct ArmDir {
	float x = 0, z = 1;
};

struct PoleArms {
	std::vector<ArmDir> arms; // one per physical arm
	std::vector<ArmDir> by_span; // per span of the pole, in its order: the arm it hangs from, on the left of the span's a -> b
};

// `idle`: the arm of a pole with no wire yet, square to its street.
PoleArms pole_arms(const StreetWorks &w, const PoleDef &def, int pole, ArmDir idle);

// Which way a pole's arms reach (Roadcraft 4.0 utilities.ts poleToward): toward the street, the way to its kerb (`kerb`) when it
// stands by one; else a way fixed in the world, its first arm turned positive along its larger axis, the same at every pole of a
// line however it wanders, so its wires never cross from one side to the other between two poles.
ArmDir pole_toward(const ArmDir *kerb, ArmDir arm);
// An arm turned to reach toward the street (streetArm): itself or its opposite. The wires' seats are measured along it.
ArmDir street_arm(ArmDir arm, ArmDir toward);

// A pole the player built as it stands: its arms, the way they reach (pole_toward) and the footway it stands on, if any. The one
// frame its wires hang from: render/props/street_props.cpp draws them from it, and a blow tears them from it (strike_ground.cpp).
struct PoleStand {
	PoleArms arms;
	ArmDir toward;
	int seg = -1; // the street it stands by, and where along it
	double v = 0;
};
PoleStand pole_stand(const StreetWorks &w, const RoadNetwork &roads, const StreetFurnitureDef &def, int id);
// The seat line of span `at` (its place in the pole's list): the arm that span hangs from, turned toward the street.
inline ArmDir seat_arm(const PoleStand &p, size_t at) { return street_arm(p.arms.by_span[at], p.toward); }

} // namespace city
