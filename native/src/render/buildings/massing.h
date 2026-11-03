// Shared steps of the 4.0 massing (jobs/b40/massing.ts) used by every kind: adding a block, the pitched-roof shape,
// roof paints, the second skin of twin/pair forms and the common finish (membranes, facades, door, dress).
#pragma once

#include "render/buildings/b40_gen.h"
#include "render/buildings/b40_paint.h"

namespace city::b40 {

struct BlockSpec {
	float x = 0, y = 0, w = 0, d = 0;
	int base = 0, storeys = 1;
	Roof roof = ROOF_FLAT;
	uint8_t fill = BAY_WINDOW, ground = BAY_NONE;
	const std::vector<Point2> *shape = nullptr;
};

int add_block(Body &b, const BlockSpec &s);
// Pitch and ridge (4.0 roofShape). `ridge_to_street` < 0 draws the default float (even for a flat roof).
void roof_shape(Volume &v, Xoshiro128 &r, int ridge_to_street = -1);
Paint roof_of(Xoshiro128 &r, const std::vector<Paint> &list);
std::vector<Paint> clay_of(const Data &data);
Paint other_skin(const Data &data, int style, Xoshiro128 &r, const Paint &wall);
void look(Body &b, const Paint &wall, const Paint &roof, const Paint &trim);
// Pitches still unset, roof membranes, facades, entrance and back door, dress (4.0 finish).
void finish_body(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r, uint8_t door_kind, int door_block);

void house(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r);
void flats(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r, bool high);
void shop(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r);
void office(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r);
void works(const Data &data, Body &b, const Envelope &env, Xoshiro128 &r);

} // namespace city::b40
