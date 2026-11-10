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

// The street side and the quarter a lot belongs to, for its building's style (Roadcraft zoning.ts quarterOf): `character` the same
// for the lots of one side of a street within shapes.facade street_m (one frontage reads as one), `era` the same over a quarter of
// quarter_m, its period: old towards the middle of the map more often, new towards the edges. `front` is the middle of the lot's
// street front and `inward` the way into the lot, on the ground (x, z). `lot` is the lot's own draw in [0, 1), from where its front
// stands: what a share of the lots is drawn on (data.h grown_share), and `design` a second draw of the same place: which design of
// its own such a lot grows (grown_pick). Both are the same for every candidate body the growth draws for the lot, so the share and
// the design are the lot's, known where the lot is cut (sim/citygen GrownPlans: the lot is cut to that design's plan), and the
// candidates only choose among the looks of what the lot grows.
struct Quarter {
	uint32_t character = 0;
	int era = 0;
	double lot = 0, design = 0;
};
Quarter quarter_of(const Data &data, V2 front, V2 inward);
// The design of its own a lot of `kind` with these draws grows: its place in data.grown_designs, or -1 (the lot is not of the share
// that grows one, or the kind has none). By the designs' weights alone: where the lot stands decides, not what it measures.
int grown_pick(const Data &data, Kind kind, const Quarter &q);

LotPlan plan_lot(const Data &data, Kind kind, float W, float D, Xoshiro128 &r, LotCorner corner = CORNER_NONE);
// The building for the envelope of `b.plan` (volumes in the envelope frame, x from its left edge).
void made_to_measure(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r);
// Facade grammar (facade_compose.cpp).
// `street`: half the time the street's style (its character's point on the weights), as the lab draws every building; a tower or an
// office chooses freely (the owner's order: a downtown of many kinds), the low-rise keeps its street.
int style_for(const Data &data, Kind kind, Xoshiro128 &r, uint32_t character, int era, bool street);
void paints_of(const Data &data, int style, Xoshiro128 &r, Paint &wall, Paint &trim);
int columns_for(float length, float module);
int bays_of(const Volume &v, int side);
float side_length(const Volume &v, int side);
void compose_facades(const Data &data, Body &b, Xoshiro128 &r);
void dress_building(const Data &data, Body &b, Xoshiro128 &r, float entrance, int front);
// Every length of a body times `k` (a design read off a document in another unit, brought to metres); shares and counts stay.
void scale_body(Body &b, float k);
// The lot round the building (lot_layout.h): grounds, drives and garages, yards, bays and docks, boundary, gates; lot frame.
void lay_out_lot(const Data &data, Body &b, Xoshiro128 &r);
// The whole body of a record: kind from the zone, its style from its street and quarter, all steps, shifted to the lot frame (x from
// the lot's middle), its corner side a front too. It chooses its own storeys; `floors` only caps a middling block, so the storeys it answers with give the same body back.
void make_body(const Data &data, Body &b, int zone, int floors, float W, float D, uint32_t seed, const Quarter &quarter,
		LotCorner corner = CORNER_NONE);

} // namespace city::b40
