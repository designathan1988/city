// The paving a closed block is cut from, and a block's box carried out to its edge (block_paving.h).
#include "sim/citygen/block_paving.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

namespace city::block_paving {

namespace {
// The land's edge lies within a cell's diagonal of the hull of its cells' centres: each side of a block's box is measured to the
// paving within two cells, at points this far apart along it, halving the step EDGE_HALVINGS times (two micrometres).
constexpr double EDGE_REACH = 2 * RASTER_M, SIDE_STEP = 2 * RASTER_M;
constexpr int EDGE_HALVINGS = 20;
constexpr double STRAIGHT_M = 1e-6; // a street whose control point stands this close to its chord's middle is its chord
} // namespace

bool Piece::covers(double x, double z, double ox, double oz) const {
	if (x < x0 || x > x1 || z < z0 || z > z1) return false;
	if (plate) return std::hypot(x - q.x0, z - q.z0) <= r;
	if (straight) // in the block's own frame (ox, oz): its points near, floats keep the millimetres
		return dist_point_segment(V2(float(x - ox), float(z - oz)), V2(float(q.x0 - ox), float(q.z0 - oz)), V2(float(q.x1 - ox), float(q.z1 - oz))) <= r;
	double t, d;
	closest_t(q, x, z, t, d);
	return d <= r;
}

void Paving::street(const Quad &q, double half) {
	Piece p;
	p.q = q;
	p.r = half;
	p.straight = q.is_straight(STRAIGHT_M);
	p.x0 = std::min({ q.x0, q.cx, q.x1 }) - half;
	p.x1 = std::max({ q.x0, q.cx, q.x1 }) + half;
	p.z0 = std::min({ q.z0, q.cz, q.z1 }) - half;
	p.z1 = std::max({ q.z0, q.cz, q.z1 }) + half;
	pieces_.push_back(p);
}

void Paving::plate(double x, double z, double r) {
	Piece p;
	p.q = Quad::straight(x, z, x, z);
	p.r = r;
	p.plate = true;
	p.x0 = x - r;
	p.x1 = x + r;
	p.z0 = z - r;
	p.z1 = z + r;
	pieces_.push_back(p);
}

bool Paving::covers(const std::vector<const Piece *> &near, double x, double z, const Box &box) const {
	for (const Piece *p : near)
		if (p->covers(x, z, box.cx, box.cz)) return true;
	return false;
}

// Each point SIDE_STEP apart along the side (`along` its unit, `length` long) from a point on land to one EDGE_REACH out that is paved.
// The side's corners round the plates: a point there on the paving finds nothing.
double Paving::side_reach(const std::vector<const Piece *> &near, const Box &box, double sx, double sz, double half, double ax, double az, double length) const {
	double reach = 0;
	const int steps = std::max(1, int(std::ceil(length / SIDE_STEP)));
	for (int k = 0; k <= steps; k++) {
		const double a = -length / 2 + length * k / steps;
		const double px = box.cx + sx * half + ax * a, pz = box.cz + sz * half + az * a;
		if (covers(near, px, pz, box) || !covers(near, px + sx * EDGE_REACH, pz + sz * EDGE_REACH, box)) continue;
		double lo = 0, hi = EDGE_REACH;
		for (int h = 0; h < EDGE_HALVINGS; h++) {
			const double mid = (lo + hi) / 2;
			(covers(near, px + sx * mid, pz + sz * mid, box) ? hi : lo) = mid;
		}
		reach = std::max(reach, lo);
	}
	return reach;
}

void Paving::measure(Box &box) const {
	std::vector<const Piece *> near;
	const double grow = std::hypot(box.L, box.D) / 2 + EDGE_REACH;
	for (const Piece &p : pieces_)
		if (p.x1 >= box.cx - grow && p.x0 <= box.cx + grow && p.z1 >= box.cz - grow && p.z0 <= box.cz + grow) near.push_back(&p);
	const double plus_u = side_reach(near, box, box.ux, box.uz, box.L / 2, box.vx, box.vz, box.D);
	const double minus_u = side_reach(near, box, -box.ux, -box.uz, box.L / 2, box.vx, box.vz, box.D);
	const double plus_v = side_reach(near, box, box.vx, box.vz, box.D / 2, box.ux, box.uz, box.L);
	const double minus_v = side_reach(near, box, -box.vx, -box.vz, box.D / 2, box.ux, box.uz, box.L);
	box.cx += box.ux * (plus_u - minus_u) / 2 + box.vx * (plus_v - minus_v) / 2;
	box.cz += box.uz * (plus_u - minus_u) / 2 + box.vz * (plus_v - minus_v) / 2;
	box.L += plus_u + minus_u;
	box.D += plus_v + minus_v;
}

} // namespace city::block_paving
