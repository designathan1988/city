// Lots are a pure function of the streets around a 256 m cell (Roadcraft src/jobs/lots.ts + blockLots.ts): closed
// blocks are cut as Roadcraft 4.0 cuts them (rows back to back, equal columns of about 22 m), then the open land
// left along zoned street sides is sliced into strips of the zone's frontage and depth. A lot belongs to the cell
// holding its centre; every cell that sees the same streets cuts them the same way.
#pragma once

#include "sim/roads/geometry.h"
#include "sim/roads/profiles.h"

#include <cstdint>
#include <functional>
#include <vector>

namespace city {

constexpr double LOT_CELL = 256; // lots are computed per cell of this size
constexpr double CELL_MARGIN = 120; // the streets a cell looks at reach this far past it

// The streets around a cell (indices local to this view).
struct LotGraph {
	std::vector<double> nx, nz;
	std::vector<uint8_t> bulb, joint; // dead end with a turning bulb; straight two-arm joint
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
std::vector<BlockLot> block_lots(const LotGraph &g, const std::vector<Profile> &profiles, const std::vector<std::vector<double>> &tables,
		double x0, double z0, const Facing &facing);
// Distance from a point to a curve (12 samples, then halving the step).
double curve_distance(const Quad &q, double x, double z);
} // namespace lot_gen

} // namespace city
