// What a body holds of the air past its ground walls: every block that starts above the ground, every roof's reach past its block,
// every part a facade's bay stands out of its wall, every piece hung on the building. The one owner of how far each stands out
// (render/buildings draws them by these: facade_parts.cpp, roof_pitched.cpp, roof_emit.cpp) and of where each hangs, as boxes with
// their underside's height, which the lot's layout keeps its tall parts out of (lot_space.cpp): a tree's crown never stands in a
// roof's edge, under a balcony or in an upper block. A wall's own dress under 0.2 m (sills, plinth, surrounds) is the wall's.
// Lot frame, metres; heights over the building's floor.
#pragma once

#include "sim/buildings/b40/types.h"

#include <vector>

namespace city::b40 {

// A pitched roof: how far its planes reach past the block, and how thick its body is under them.
constexpr float EAVES_OUT_M = 0.45f, ROOF_BODY_M = 0.18f;
// A flat roof's crown, each band's reach, its middle under (or over) the eaves and its height: a cornice; a stepped crown's upper and
// lower band; a slab standing over the roof's edge.
constexpr float CORNICE_OUT_M = 0.16f, CORNICE_DROP_M = 0.2f, CORNICE_H_M = 0.36f;
constexpr float STEP_OUT_M = 0.55f, STEP_H_M = 0.55f, STEP_LOW_OUT_M = 0.28f, STEP_LOW_DROP_M = 1.05f, STEP_LOW_H_M = 0.32f;
constexpr float CROWN_SLAB_OUT_M = 0.45f, CROWN_SLAB_UP_M = 0.73f, CROWN_SLAB_H_M = 0.26f;
// A balcony's slab (a shopfront's awning reaches as far) and its thickness under the floor.
constexpr float BALCONY_OUT_M = 1.3f, BALCONY_SLAB_M = 0.18f;
// A balcony with side walls and a beam: the farthest any of it stands out, and the lowest any of it hangs under its floor.
constexpr float DEEP_BALCONY_OUT_M = 1.65f, DEEP_BALCONY_UNDER_M = 0.307f;
constexpr float SUN_RACK_OUT_M = 0.6f; // a rack of sun blades before a bay, from its floor to its head
constexpr float DOOR_CANOPY_OUT_M = 0.9f, WIDE_DOOR_CANOPY_OUT_M = 1.1f; // the canopy over a ground door, over a double one
// What hangs over a ground-floor opening (a door's canopy, a shopfront's awning) hangs over a door's clear height, 2.10 m (ABNT NBR
// 9050:2020, 6.11.2.4): no part taller than that stands under it.
constexpr float GROUND_HANG_UNDER_M = 2.1f;

// A side of a block in plan: from `a` to `b` seen from outside left to right, `out` its outward normal; the bay i of n runs from
// a + (b - a) * i / n. (Side 0 of a rectangle is its front, along +x; 1 its right; 2 its back, from +x to -x; 3 its left.)
struct FaceRun {
	int side = 0;
	Point2 a, b, out;
	float length = 0;
};
std::vector<FaceRun> face_runs(const Volume &v);

// How far a pitched roof reaches past each side of its block (lot frame: -x, +x, the front, the back): its eaves, but never past the
// lot's own line (a wall on it ends flush, as a party wall does) nor into another block of the body that stands as high.
struct RoofReach {
	float left = EAVES_OUT_M, right = EAVES_OUT_M, front = EAVES_OUT_M, back = EAVES_OUT_M;
};
RoofReach roof_reach(const Body &b, const Volume &v);

// A box of the body over the ground: its rectangle in plan and the height of its underside. Nothing stands in it from there up.
struct Overhang {
	float x0 = 0, y0 = 0, x1 = 0, y1 = 0, under = 0;
	bool over(float px0, float py0, float px1, float py1) const { return px0 < x1 && x0 < px1 && py0 < y1 && y0 < py1; }
};
// Every one of the body: blocks above the ground, roofs' reaches, bays' parts by storey, hung pieces among its elements.
std::vector<Overhang> body_overhangs(const Body &b);

} // namespace city::b40
