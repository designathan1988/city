// The 4.0 building generator, step by step on one Xoshiro128 stream seeded with the record seed:
// plan_lot (lot_plan.cpp) -> made_to_measure (massing_*.cpp) -> furnish_lot (lot_furnish.cpp). make_body runs them.
#pragma once

#include "sim/buildings/b40/data.h"
#include "sim/buildings/b40/types.h"

#include "core/seeded.h"

namespace city::b40 {

// What the massing is told about its lot (4.0 B40Envelope).
struct Envelope {
	float W = 0, D = 0;
	bool back_door = false;
	uint32_t character = 0;
	int era = 0;
};

LotPlan plan_lot(Kind kind, float W, float D, Xoshiro128 &r);
// The building for the envelope of `b.plan` (volumes in the envelope frame, x from its left edge).
void made_to_measure(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r);
// Facade grammar (facade_compose.cpp).
int style_for(const Data &data, Kind kind, Xoshiro128 &r, uint32_t character, int era);
void paints_of(const Data &data, int style, Xoshiro128 &r, Paint &wall, Paint &trim);
int columns_for(float length, float module);
int bays_of(const Volume &v, int side);
float side_length(const Volume &v, int side);
void compose_facades(const Data &data, Body &b, Xoshiro128 &r);
void dress_building(const Data &data, Body &b, Xoshiro128 &r, float entrance, int front);
// Yard, walls, gates, parking (lot_furnish.cpp), in the lot frame.
void furnish_lot(const Data &data, Body &b, Xoshiro128 &r);
// The whole body of a record: kind from use/floors, all steps, shifted to the lot frame (x from the lot's middle).
void make_body(const Data &data, Body &b, int zone, int floors, float W, float D, uint32_t seed);

} // namespace city::b40
