// Streets by the block, in one gesture (road_blocks.cpp).
#pragma once

#include "sim/commands/commands.h"

namespace city {

// A grid of streets laid in one gesture (road_blocks.cpp): `cols` by `rows` blocks of `block_m`, turned by `angle`, centred on (x, z).
struct BlockGrid {
	int cols = 1, rows = 1;
	double block_m = 0, angle = 0;
};
struct BlockLine {
	StrokePoint a, b;
};
std::vector<BlockLine> block_lines(double x, double z, const BlockGrid &g);
EditResult lay_blocks(CityState &s, double x, double z, const BlockGrid &g, int profile);

} // namespace city
