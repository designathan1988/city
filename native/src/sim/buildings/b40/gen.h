// The building generator, step by step on one Xoshiro128 stream seeded with the record's seed:
// plan_lot (lot_plan.cpp) -> made_to_measure (massing_*.cpp) -> lay_out_lot (lot_layout.h). make_body runs them.
#pragma once

#include "sim/buildings/b40/data.h"
#include "sim/buildings/b40/types.h"

#include "core/seeded.h"

namespace city::b40 {

// What the massing is told about its lot: the envelope's size, a back door, the street's character and the era.
struct Envelope {
	float W = 0, D = 0;
	bool back_door = false;
	uint32_t character = 0;
	int era = 0;
	int cap = 0; // the most storeys its neighbours allow a middling block (0: no limit)
	bool low = false; // a low-density commercial lot
};

LotPlan plan_lot(const LotRules &l, Kind kind, float W, float D, Xoshiro128 &r);
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
// The lot round the building (lot_layout.h): grounds, drives and garages, yards, bays and docks, boundary, gates; lot frame.
void lay_out_lot(const Data &data, Body &b, Xoshiro128 &r);
// The whole body of a record: kind from the zone, all steps, shifted to the lot frame (x from the lot's middle). It chooses its own
// storeys; `floors` only caps a middling block, so the storeys it answers with give the same body back.
void make_body(const Data &data, Body &b, int zone, int floors, float W, float D, uint32_t seed);

} // namespace city::b40
