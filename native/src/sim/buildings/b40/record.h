// A building record's body and its lot's frame in the world: the one place the render (BuildingBodies) and the
// simulation (sim/lots/lot_access, the lots' trees) get them from, so both read the same lot.
#pragma once

#include "core/math.h"
#include "sim/buildings/b40/gen.h"
#include "sim/buildings/building_store.h"

#include <vector>

namespace city::b40 {

// The lot's frame on the ground (XZ): `o` the middle of the lot's street front, `e0` along the front (lot x), `e1` away
// from the street (lot y).
struct LotFrame {
	V2 o, e0, e1;
	V2 at(float x, float y) const { return o + e0 * x + e1 * y; }
	V2 dir(float x, float y) const { return e0 * x + e1 * y; }
};
LotFrame lot_frame(const Building &rec);

// The record's body: the version the player edited, or a civic building, an open lot or a grown building made from its seed.
void record_body(const Data &data, const Building &rec, Body &b);
// What names that body: two records with the same key have the same body, wherever they stand.
uint64_t body_key(const Building &rec);

// The ground under a body's ground blocks, from `height(x, z)` in the world: its highest and lowest points (corners and a 1.5 m
// grid), and the floor those give, the highest + the plinth. The render draws on it and a click on a wall is measured from it.
constexpr float LOT_PLINTH_M = 0.15f, LOT_GRID_STEP_M = 1.5f;
constexpr int LOT_GRID_MAX = 24;
template <typename H>
void lot_ground(const Body &b, const Building &rec, const LotFrame &f, H &&height, float &hi, float &lo) {
	hi = -FAR_AWAY;
	lo = FAR_AWAY;
	auto sample = [&](float x, float y) {
		V2 p = f.at(x, y);
		float g = height(p.x, p.y);
		hi = g > hi ? g : hi;
		lo = g < lo ? g : lo;
	};
	for (const Volume &v : b.volumes) {
		if (v.base != 0) continue;
		int nx = int(v.w / LOT_GRID_STEP_M) + 1, ny = int(v.d / LOT_GRID_STEP_M) + 1;
		nx = nx < LOT_GRID_MAX ? nx : LOT_GRID_MAX;
		ny = ny < LOT_GRID_MAX ? ny : LOT_GRID_MAX;
		for (int i = 0; i <= nx; i++)
			for (int j = 0; j <= ny; j++) sample(v.x + v.w * float(i) / float(nx), v.y + v.d * float(j) / float(ny));
	}
	if (hi < lo) sample(0, rec.d / 2);
}
template <typename H>
float lot_floor(const Body &b, const Building &rec, const LotFrame &f, H &&height) {
	float hi = 0, lo = 0;
	lot_ground(b, rec, f, height, hi, lo);
	return hi + LOT_PLINTH_M;
}

// A tree the lot's layout planted, in the world: where, its crown's reach and its height (for the trees' renderer).
struct LotTree {
	V2 at;
	float crown = 0, height = 0;
};
void lot_trees(const Body &b, const LotFrame &f, std::vector<LotTree> &out);

} // namespace city::b40
