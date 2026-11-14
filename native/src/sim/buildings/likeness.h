// How alike two buildings look at a glance (Roadcraft 4.0 world/buildings/procedural.ts bodySignature and signatureDistance, its
// behaviour), and which of a lot's candidates looks least like what stands round it (world/growth/zoning.ts unlikeness): a street of
// grown buildings does not repeat. Mitchell's best candidate, in the space of what a building looks like.
#pragma once

#include "sim/buildings/b40/types.h"

#include <cstdint>
#include <vector>

namespace city {

// What the eye reads of a body: the arrangement of its volumes, the paint of its main wall, its main roof and how tall it is.
struct BodyLook {
	uint64_t form = 0, roof = 0;
	uint32_t colour = 0;
	int storeys = 0;
	// The least side of the room its lot's plan left its blocks (b40 LotPlan's envelope), metres; 0 for a body with no plan. Not a
	// thing the eye compares (look_distance leaves it out): growth asks it of a candidate before it builds on a lot.
	float room = 0;
};
BodyLook body_look(const b40::Body &body, int fn);
// How unlike two looks are: 0 the same building, up to about 4.
float look_distance(const BodyLook &a, const BodyLook &b);

// A building standing near a lot, and how far.
struct NearLook {
	BodyLook look;
	float distance = 0;
};
constexpr int LOOK_CANDIDATES = 4; // bodies drawn for a lot
constexpr float LOOK_REACH_M = 100; // how far round a lot the buildings it should not look like are read
// The score of a candidate: how unlike the nearest look-alike it is, a far one counting for less.
float unlikeness(const BodyLook &mine, const std::vector<NearLook> &near);

} // namespace city
