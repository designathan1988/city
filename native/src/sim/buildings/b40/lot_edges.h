// What a lot's edges meet, read from the streets (sim/commands/lot_sites.h lot_edges), as x along the front from its middle: the stretches
// of its front on a junction's kerb, where no car's gate opens (its corner, its crosswalks and the clearance past them:
// sim/roads/parking_layout.h kerb_standing), and within them those no person's gate opens on either (the crosswalks' landings and the
// signal posts just past them: kerb_walkable); and which of its other lines a street runs along (a side, closed as a front is; the back,
// where an open lot's way may lead). A building record carries
// it (Building::edges), settled with the lot's site; the generator reads it from the plan (types.h LotPlan::edges).
#pragma once

#include <cstdint>

namespace city::b40 {

struct LotEdges {
	static constexpr int SPANS = 2, KINDS = 2; // stretches of each kind; the kinds: a car's gate, a person's
	static constexpr int CAR = 0, WALK = 1;
	static constexpr uint8_t LEFT_ON_STREET = 1, RIGHT_ON_STREET = 2; // the side at lot x < 0, the side at x > 0
	static constexpr uint8_t BACK_ON_STREET = 4; // the line across from the front (a lot that runs through its block)
	float from[KINDS][SPANS] = {}, to[KINDS][SPANS] = {}; // a stretch [from, to] of the front; none where to <= from
	uint8_t street = 0;
	// Whether the front's stretch [x0, x1] meets one no gate of `kind` (CAR, WALK) opens on.
	bool shut(int kind, float x0, float x1) const {
		for (int k = 0; k < SPANS; k++)
			if (to[kind][k] > from[kind][k] && x0 < to[kind][k] && from[kind][k] < x1) return true;
		return false;
	}
	bool on_street(int side) const { return (street & (side == 0 ? LEFT_ON_STREET : RIGHT_ON_STREET)) != 0; } // 0 left, 1 right
	bool back_on_street() const { return (street & BACK_ON_STREET) != 0; }
	// The same edges with the front's stretches `dx` along (into another frame of the front: the envelope's).
	LotEdges moved(float dx) const {
		LotEdges e = *this;
		for (int kind = 0; kind < KINDS; kind++)
			for (int k = 0; k < SPANS; k++) {
				e.from[kind][k] += dx;
				e.to[kind][k] += dx;
			}
		return e;
	}
	bool operator==(const LotEdges &o) const {
		for (int kind = 0; kind < KINDS; kind++)
			for (int k = 0; k < SPANS; k++)
				if (from[kind][k] != o.from[kind][k] || to[kind][k] != o.to[kind][k]) return false;
		return street == o.street;
	}
	bool operator!=(const LotEdges &o) const { return !(*this == o); }
};

} // namespace city::b40
