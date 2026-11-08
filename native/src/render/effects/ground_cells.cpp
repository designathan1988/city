// The ground the effects lie on (ground_cells.h).
#include "render/effects/ground_cells.h"

#include "sim/transit/transit_ground.h"

#include <cmath>

namespace city {

namespace {
constexpr float PITCH_M = 0.25f; // a cell of the kept ground: under the kerb strip's width (0.15 m) the step shows as a slope at most
constexpr size_t CELLS_MOST = size_t(1) << 18; // past this many kept, all are let go (a blast reads a few thousand)
constexpr int64_t CELL_BIAS = int64_t(1) << 31; // a cell's index made positive for its key
constexpr uint64_t LOW_HALF = 0xffffffffULL; // the key's lower half: the cell's row
constexpr int COLUMN_BITS = 32; // the column in the upper half
constexpr float HALF = 0.5f;

uint64_t cell_key(float x, float z) {
	const int64_t i = int64_t(std::floor(x / PITCH_M)) + CELL_BIAS, j = int64_t(std::floor(z / PITCH_M)) + CELL_BIAS;
	return (uint64_t(i) << COLUMN_BITS) | (uint64_t(j) & LOW_HALF);
}

bool paved(const CityState &s, float x, float z, float &y) {
	bool carriageway = false;
	return transit_paved(s, V2(x, z), s.catalog.furniture.sidewalk.height_m, y, carriageway);
}
} // namespace

float GroundCells::surface(const CityState &s, float x, float z) {
	float y = 0;
	return paved(s, x, z, y) ? y : s.terrain.height_at(x, z);
}

float GroundCells::laid(const CityState &s, float x, float z) {
	float y = 0;
	return paved(s, x, z, y) ? y : s.terrain.base_height_at(x, z);
}

float GroundCells::at(const CityState &s, float x, float z) {
	if (revision_ != s.revision || cells_.size() > CELLS_MOST) {
		cells_.clear();
		revision_ = s.revision;
	}
	const uint64_t key = cell_key(x, z);
	const auto it = cells_.find(key);
	if (it != cells_.end()) return it->second;
	const float y = surface(s, (std::floor(x / PITCH_M) + HALF) * PITCH_M, (std::floor(z / PITCH_M) + HALF) * PITCH_M);
	cells_.emplace(key, y);
	return y;
}

} // namespace city
