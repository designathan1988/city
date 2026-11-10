// Lots are a pure function of the streets around a cell of LOT_CELL (Roadcraft src/jobs/lots.ts + blockLots.ts): closed
// blocks are cut as Roadcraft 4.0 cuts them (rows back to back, equal columns of about the lots' frontage), then the open land
// left along zoned street sides is one strip of a lot's depth, cut in equal lots. A lot belongs to the cell
// holding its centre; every cell that sees the same streets cuts them the same way.
#pragma once

#include "core/math.h"
#include "sim/catalog/gen_shapes.h"
#include "sim/roads/geometry.h"
#include "sim/roads/profiles.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <vector>

namespace city {

struct GenZones;

constexpr double LOT_CELL = 256; // lots are computed per cell of this size
constexpr double CELL_MARGIN = 120; // the streets a cell looks at reach this far past it

// The streets around a cell (indices local to this view).
struct LotGraph {
	std::vector<double> nx, nz;
	std::vector<double> bulb; // a dead end's turning bulb: the radius of its round out to the casing (0 none)
	std::vector<uint8_t> joint; // straight two-arm joint
	std::vector<Quad> quad;
	std::vector<int> sa, sb, profile, zl, zr;
	int nodes() const { return int(nx.size()); }
	int segs() const { return int(quad.size()); }
};

struct Lot {
	double x = 0, z = 0, w = 0, d = 0, rot = 0; // rot: local +z faces the street
	int zone = 0, profile = 0;
	uint32_t seed = 0;
};

// A raster of square cells with its first cell's corner at (ax, az): the cells whose centre is within r of (x, z) in the
// box i0..i1, j0..j1 take v. A row of a disc is one run: its ends found by the test itself, the middle filled.
inline void fill_disc(uint8_t *grid, int w, double ax, double az, double res, double x, double z, double r, const int box[4], uint8_t v) {
	const double r2 = r * r;
	auto in = [&](int i, double dz2) {
		const double dx = ax + (i + 0.5) * res - x;
		return dx * dx + dz2 <= r2;
	};
	const int mid = std::min(box[1], std::max(box[0], ifloor((x - ax) / res)));
	for (int j = box[2]; j <= box[3]; j++) {
		const double dz = az + (j + 0.5) * res - z, dz2 = dz * dz, reach = std::sqrt(std::max(0.0, r2 - dz2)) / res;
		int lo = std::max(box[0], mid - int(reach)), hi = std::min(box[1], mid + int(reach));
		while (lo > box[0] && in(lo - 1, dz2)) lo--;
		while (lo <= hi && !in(lo, dz2)) lo++;
		while (hi < box[1] && in(hi + 1, dz2)) hi++;
		while (hi >= lo && !in(hi, dz2)) hi--;
		if (lo <= hi) std::fill(grid + size_t(j) * w + lo, grid + size_t(j) * w + hi + 1, v);
	}
}

struct LotGround {
	std::function<double(double, double)> height; // ground height
	std::function<double(double, double)> water; // water level (very low where dry)
	// land no lot stands on (a scenic city's parks: the zone read off a street stops at them, a lot as deep as its building needs
	// reached past that read into the park); empty: none
	std::function<bool(double, double)> closed;
};

// The lot a design of its own asks for: its plan's width and depth.
struct GrownPlan {
	double w = 0, d = 0;
};
// The designs of their own the lots grow (b40 Data, the one owner of what they are and how big): by zone code the plans of the
// designs a lot of that zone may grow, and `pick`, the one of them the lot `l` grows by where its front stands (the draw its building
// is made by: b40 quarter_of, grown_pick), its place in by_zone[l.zone], or -1 for a lot that grows none. A lot is cut to the plan of
// the design it grows, every other to the street's size (lot_place.cpp, block_lots.cpp cut_run): a design stands on a lot it fills.
struct GrownPlans {
	std::vector<std::vector<GrownPlan>> by_zone;
	std::function<int(const Lot &l)> pick;
	const std::vector<GrownPlan> &of(int zone) const {
		static const std::vector<GrownPlan> none;
		return pick && zone >= 0 && size_t(zone) < by_zone.size() ? by_zone[size_t(zone)] : none;
	}
};

std::vector<Lot> build_lots(const GenShapes &S, const LotGraph &g, const std::vector<Profile> &profiles, const GenZones &zones, const GrownPlans &grown,
	const LotGround &ground, double cell_x0, double cell_z0);

// Internals shared by the two files.
namespace lot_gen {
struct BlockLot {
	double x, z, w, d, rot;
	int zone, profile;
};
using Facing = std::function<void(double fx, double fz, double ox, double oz, int &zone, int &profile)>;
// A lot's size by the zone it faces: a works lot's room for a lorry to turn in its yard (growth.json shapes.lots works_*), any other
// the street's (shapes.lots; a lot that grows a design of its own is cut to that design's plan instead: GrownPlans). `whole` when that
// size is room its building needs: its lots are never cut narrower than the frontage, and a block lays one row of them rather than two
// too shallow.
struct LotSize {
	double frontage = 0, depth = 0;
	bool whole = false;
	bool same(const LotSize &o) const { return frontage == o.frontage && depth == o.depth && whole == o.whole; }
};
LotSize lot_size(const LotsShape &P, const GenZones &zones, int zone);
// Which design of its own the block lot laid as `l` grows (its place in GrownPlans::of(l.zone)), or -1: asked by the blocks' cutter of
// the strips' (lot_place.cpp), which knows where the lot's front stands once it is brought to the paving.
using DesignAt = std::function<int(const BlockLot &l)>;
std::vector<BlockLot> block_lots(const GenShapes &S, const LotGraph &g, const std::vector<Profile> &profiles, const std::vector<std::vector<double>> &tables,
		double x0, double z0, const Facing &facing, const GenZones &zones, const GrownPlans &grown, const DesignAt &design_at);
// Distance from a point to a curve (12 samples, then halving the step).
double curve_distance(const Quad &q, double x, double z);
} // namespace lot_gen

} // namespace city
