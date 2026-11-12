// Lots are a pure function of the streets around a 256 m cell (Roadcraft src/jobs/lots.ts + blockLots.ts): closed
// blocks are cut as Roadcraft 4.0 cuts them (rows back to back, equal columns of about 22 m), then the open land
// left along zoned street sides is one strip of a lot's depth, cut in equal lots. A lot belongs to the cell
// holding its centre; every cell that sees the same streets cuts them the same way.
#pragma once

#include "core/math.h"
#include "sim/roads/geometry.h"
#include "sim/roads/profiles.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <functional>
#include <vector>

namespace city {

class LotRasterCache;

constexpr double LOT_CELL = 256; // lots are computed per cell of this size
constexpr double CELL_MARGIN = 120; // the streets a cell looks at reach this far past it
constexpr int LOT_ARC_STEPS = 24; // a distinct sampling contract from the road renderer's stations

// The streets around a cell (indices local to this view).
struct LotGraph {
	const LotRasterCache *raster = nullptr; // shared immutable disc spans; standalone callers retain the exact uncached path
	size_t raster_bytes = 0;
	std::vector<double> nx, nz;
	std::vector<uint8_t> bulb, joint; // dead end with a turning bulb; straight two-arm joint
	std::vector<Quad> quad;
	std::vector<const std::vector<double> *> prepared_arcs; // optional borrowed station tables; source map stays pinned through build_lots
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
template<class Emit> void disc_rows(double ax, double az, double res, double x, double z, double r, const int box[4], Emit emit) {
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
		if (lo <= hi) emit(j, lo, hi);
	}
}
inline void fill_disc(uint8_t *grid, int w, double ax, double az, double res, double x, double z, double r, const int box[4], uint8_t v) {
	disc_rows(ax, az, res, x, z, r, box, [&](int row, int from, int to) {
		std::fill(grid + size_t(row) * w + from, grid + size_t(row) * w + to + 1, v);
	});
}

struct LotGround {
	std::function<double(double, double)> height; // ground height
	std::function<double(double, double)> water; // water level (very low where dry)
};

std::vector<Lot> build_lots(const LotGraph &g, const std::vector<Profile> &profiles, const LotGround &ground, double cell_x0, double cell_z0);

// Internals shared by the two files.
namespace lot_gen {
struct BlockLot {
	double x, z, w, d, rot;
	int zone, profile;
};
using Facing = std::function<void(double fx, double fz, double ox, double oz, int &zone, int &profile)>;
std::vector<BlockLot> block_lots(const LotGraph &g, const std::vector<Profile> &profiles, const std::vector<const std::vector<double> *> &tables,
		double x0, double z0, const Facing &facing);
// Distance from a point to a curve (12 samples, then halving the step).
double curve_distance(const Quad &q, double x, double z);
} // namespace lot_gen

} // namespace city
