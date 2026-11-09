// The site a grown building stands on (Roadcraft zoning.ts tryOnLot's ground and ring): the street floor the lot's front gives
// it, the ground under the lot about that floor on a fixed grid, and the lot's own outline when it is not its rectangle. Read
// once from the terrain when the building grows (sim/simulation/growth.cpp read_site), held by the record, shared and never
// changed, and saved with it: the body is made from it (split_level.cpp, lot_fit.cpp) and keyed by it (record.h body_key),
// so every reader, the simulation's and the render's, gets the same body from the same record.
#pragma once

#include "core/math.h"

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace city::b40 {

// The ground grid across the lot (x along the front) and back into it (y): the lab's stepToSlope reads five points across a block
// and five back; five across the lot and nine back read a lot of up to ~40 m to that density. A saved record's layout: fixed.
constexpr int SITE_COLS = 5, SITE_ROWS = 9;
constexpr float SITE_QUANTUM_M = 0.01f; // the ground is kept to the centimetre: the key of a body made from it

struct LotSite {
	float floor = 0; // the street floor, world height (m): the ground floor's level
	std::array<int16_t, SITE_COLS * SITE_ROWS> ground{}; // the ground over the floor, row by row from the front, in SITE_QUANTUM_M
	bool sloped = false; // any ground point off the floor's level: what makes a split level possible
	std::vector<V2> ring; // the lot in its own frame (lot x, lot y) when it is not its rectangle; empty: the rectangle
	uint64_t hash = 0; // what the body is made from (the ground and the ring, never the floor's height): body_key's part

	// The ground over the floor at a point of the lot (lot frame, metres), bilinear on the grid over a lot w wide and d deep.
	float ground_at(float x, float y, float w, float d) const;
	void seal(); // sets `sloped` and `hash` from the ground and the ring
};
using SiteRef = std::shared_ptr<const LotSite>;

struct Data;
struct Body;
// What a site makes of a body the generator laid on the lot's rectangle (lot frame, the lot w x d): its back stepped to the ground
// with the lot's walls, flights and the garage's apron (split_level.cpp), then fitted to the lot's own outline (lot_fit.cpp).
void follow_slope(const Data &data, Body &b, const LotSite &site, float w, float d);
void fit_to_lot(const Data &data, Body &b, const LotSite &site);

// The site of a lot w x d in its frame: `height(x, z)` the world's ground (the street's paving along the front, where the road
// shapes it); the floor `threshold` over the highest paving of `front_points` along the front (zoning.ts streetFloor); `ring_world`
// the lot's outline (world XZ, its first side the front), or empty for its rectangle.
SiteRef read_site(V2 o, V2 e0, V2 e1, float w, float d, const std::function<float(float, float)> &height, float threshold,
		const std::vector<V2> &ring_world, int front_points);

} // namespace city::b40
