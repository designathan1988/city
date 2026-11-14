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
	int density = 1; // the zone's density (zone_codes.h zone_step): a works' hall is taller the denser its district
	// The blocks alone, with their roofs and paints: all a look reads (sim/buildings/likeness.h) and all that says how tall the body
	// is. No facade is composed, nothing is dressed (massing.h finish_body stops before them).
	bool blocks_only = false;
	// The front's stretches no gate opens on (LotEdges) in the envelope's frame (x from its left edge), and half the person's gate that
	// stands in line with the door: the entrance is put where that gate opens off a person's shut stretches. 0: no front zone, no gate.
	LotEdges shut;
	float gate_half = 0;
};

// What a record's seed says of where it stands, written there when it grows (sim/buildings/growth.h quarter_seed) and read here: the
// character of its street's side in the seed's low half, its quarter's era (0 old, 1 middle, 2 new) by the high half.
constexpr int ERAS = 3;
constexpr uint32_t SEED_CHARACTER_MASK = 0xffff, SEED_ERA_SHIFT = 16;
constexpr uint32_t seed_character(uint32_t seed) { return seed & SEED_CHARACTER_MASK; }
constexpr int seed_era(uint32_t seed) { return int((seed >> SEED_ERA_SHIFT) % ERAS); }

// The plan of a lot by its kind and size, its drives' gates kept off the front's shut stretches and its street sides kept open (`edges`).
LotPlan plan_lot(const LotRules &l, Kind kind, float W, float D, Xoshiro128 &r, const LotEdges &edges);
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
// storeys; `floors` only caps a middling block, so the storeys it answers with give the same body back. `edges`: what the lot's edges
// meet (Building::edges). `blocks_only`: the plan and the blocks alone (Envelope::blocks_only), the lot not laid out: the same blocks,
// roofs and paints as the whole body's at a fraction of its cost, for what only looks at them (growth's candidates, a record's look).
void make_body(const Data &data, Body &b, int zone, int floors, float W, float D, uint32_t seed, const LotEdges &edges, bool blocks_only = false);

} // namespace city::b40
