// The ground under a building's lot, graded to it (sim/commands/lot_sites.h makes it): the lot's turned rectangle at the footway's
// top less the lot's plate, following the footway's profile along the frontage, an apron at that level all round, then a bank back
// to the natural ground at tune.lot_bank_batter, at most tune.lot_bank_reach_m long; the lot and its own ground, never the street:
// under a road the road's ground lies over it by the road's weight (lot_shape.cpp, compose.cpp). Derived from the building and the
// street it faces, never journaled nor saved. The height grid (4 m) cannot hold a kerb's step, so the ground's height at a point of
// the lot itself is the lot's own level, exactly (holds: Heightfield::height_at and the jobs' patches ask it first): every reader of
// the ground (the floors, the lot's covers, its bays, doors and gardens) stands on one level that meets the footway's top, the grid
// under it.
#pragma once

#include "core/grid.h"

#include <vector>

namespace city {

struct LotShape {
	bool alive = false;
	float ox = 0, oz = 0; // the middle of the lot's front
	float ax = 0, az = 0; // unit along the front (the lot's x)
	float bx = 0, bz = 0; // unit into the lot, away from the street (the lot's y)
	float w = 0, d = 0; // frontage and depth
	std::vector<float> levels; // the level at evenly spaced points along the front, from x = -w/2 to +w/2 (two at least)
	float rx0 = 0, rz0 = 0, rx1 = 0, rz1 = 0; // the box of the lot's own rectangle: where holds() can answer
	float minx = 0, minz = 0, maxx = 0, maxz = 0; // where it can grade: the rectangle grown by the apron and the longest bank

	// The lot's level `x` metres along the front from its middle (held at the end levels past the corners).
	float level_at(float x) const {
		const size_t n = levels.size();
		if (n == 0) return 0;
		if (n == 1 || w <= 0) return levels[0];
		float t = (x / w + HALF) * float(n - 1);
		t = t < 0 ? 0 : t > float(n - 1) ? float(n - 1) : t;
		const size_t k = size_t(t) < n - 1 ? size_t(t) : n - 2;
		return levels[k] + (levels[k + 1] - levels[k]) * (t - float(k));
	}
	// The ground at world (x, z) when it lies on the lot itself (its rectangle, not the apron).
	bool holds(float x, float z, float &h) const {
		if (!alive || x < rx0 || x > rx1 || z < rz0 || z > rz1) return false;
		const float px = x - ox, pz = z - oz, lx = px * ax + pz * az, ly = px * bx + pz * bz;
		if (lx < -w * HALF || lx > w * HALF || ly < 0 || ly > d) return false;
		h = level_at(lx);
		return true;
	}

private:
	static constexpr float HALF = 0.5f;
};

// The lots a reader of the ground holds (the live land's or a job's copy of a window): the lowest id among those whose lot holds the
// point decides, as the composition lets the first of equals stand.
struct LotLevels {
	std::vector<LotShape> shapes; // by id
	SpatialHash hash; // each live shape filed by its own rectangle's box
	bool level_at(float x, float z, float &h) const {
		int best = -1;
		float hb = 0;
		hash.query(x, z, x, z, [&](int id) {
			float hh;
			if ((best < 0 || id < best) && shapes[size_t(id)].holds(x, z, hh)) {
				best = id;
				hb = hh;
			}
		});
		if (best >= 0) h = hb;
		return best >= 0;
	}
};

} // namespace city
