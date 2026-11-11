// The paving a closed block is cut from (internal to sim/citygen, block_lots.cpp): each street's band (its centre line within its half
// width) and each junction's plate (a disc), kept whole beside the raster block_lots paints from them, and a block's box carried out
// from its land's cells to the paving's real edge on every side.
#pragma once

#include "sim/roads/geometry.h"

#include <vector>

namespace city::block_paving {

constexpr double RASTER_M = 1; // the side of a cell of the raster the blocks are found on

// A block's box: its middle, its unit along (u, the longer side as found) and across (v), its length and depth.
struct Box {
	double cx, cz, ux, uz, vx, vz, L, D;
};

// One piece of the paving: a street's band or a plate.
struct Piece {
	Quad q; // a street's centre line; a plate's centre is (q.x0, q.z0)
	double r = 0; // its half width, or the plate's radius
	bool plate = false, straight = false;
	double x0 = 0, z0 = 0, x1 = 0, z1 = 0; // its box, the band included
	// Whether it covers (x, z); (ox, oz) a point near, the frame a straight band is measured in.
	bool covers(double x, double z, double ox, double oz) const;
};

class Paving {
public:
	void street(const Quad &q, double half);
	void plate(double x, double z, double r);
	// The box's four sides, through its land's cells, each carried out to where the paving begins: the farthest land found along it.
	void measure(Box &box) const;

private:
	std::vector<Piece> pieces_;
	bool covers(const std::vector<const Piece *> &near, double x, double z, const Box &box) const;
	// How far past the side through the box's middle + side * half along `side` the land runs on to the paving, at points along it.
	double side_reach(const std::vector<const Piece *> &near, const Box &box, double sx, double sz, double half, double ax, double az, double length) const;
};

} // namespace city::block_paving
