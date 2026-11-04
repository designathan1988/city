// Civic buildings and open lots of the records with a function: the
// body of a school, church, hospital, clinic, gas station, supermarket, mall, city hall, police and fire station
// (civic.cpp, civic_services.cpp), and squares, parks and sports courts laid as lot elements (open_lots.cpp). Colours
// are names of data/render/buildings.json "colors". Bodies come out in the lot frame (x from the lot's middle, y from
// the street), like make_body.
#pragma once

#include "sim/buildings/b40/massing.h"

namespace city::b40 {

// Element helper in any frame: a box element from (x0, y0) to (x1, y1), `h` tall, raised to `z` (hung kinds).
void put(Body &b, Element k, float x0, float y0, float x1, float y1, float h, int paint, float z = 0);
inline void put_at(Body &b, Element k, float x, float y, float w, float d, float h, int paint, float z = 0) {
	put(b, k, x - w / 2, y - d / 2, x + w / 2, y + d / 2, h, paint, z);
}

// True when the record function draws a civic body (not an ordinary building or an open lot).
bool is_civic(int fn);
// True when the function is an open lot (square, park, sports court): no volumes, elements only.
bool is_open_lot(int fn);
void civic_body(const Data &data, Body &b, int fn, int floors, float W, float D, uint32_t seed);
void open_lot_body(const Data &data, Body &b, int fn, float W, float D);
// Where the tree renderer may stand a tree in an open lot (lot frame): on the lawn, off paths, courts and fountains.
bool open_lot_tree_ok(int fn, float W, float D, float x, float y);

// civic_services.cpp: the service buildings (one per function), on a body whose plan, paints and heights are set.
void gas_station(const Data &data, Body &b, Xoshiro128 &r);
void market(const Data &data, Body &b, Xoshiro128 &r, bool mall);
void city_hall(const Data &data, Body &b, Xoshiro128 &r);
void station(const Data &data, Body &b, Xoshiro128 &r, bool fire);

} // namespace city::b40
