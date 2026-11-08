// The era a lot was built in (era.h).
#include "sim/buildings/b40/era.h"

#include "core/seeded.h"

#include <cmath>

namespace city::b40 {

namespace {
constexpr int ERAS = 3; // era_affinity: old, middle, new
constexpr uint32_t SEED_SALT = 0x5eed, ERA_SALT = 0xe2a, HALF_BITS = 16, HALF_MASK = 0xffff;
constexpr float HALF_RANGE = 65536.f;
constexpr uint32_t INFILL_SHIFT = 16; // the lot seed's bits the infill draw reads; the generator's stream starts from the whole seed
} // namespace

int era_at(const Data &data, float x, float z, uint32_t seed) {
	if (float((seed >> INFILL_SHIFT) & HALF_MASK) / HALF_RANGE < data.era_infill) return int((seed >> INFILL_SHIFT) % ERAS);
	const float cell = data.era_cell_m;
	const int ci = int(std::floor(x / cell)), cj = int(std::floor(z / cell));
	float best = 0;
	int era = 0;
	bool first = true;
	for (int di = -1; di <= 1; di++)
		for (int dj = -1; dj <= 1; dj++) {
			const uint32_t h = hash2(ci + di, cj + dj, SEED_SALT);
			const float px = (float(ci + di) + float(h & HALF_MASK) / HALF_RANGE) * cell;
			const float pz = (float(cj + dj) + float(h >> HALF_BITS) / HALF_RANGE) * cell;
			const float d2 = (px - x) * (px - x) + (pz - z) * (pz - z);
			if (first || d2 < best) {
				first = false;
				best = d2;
				era = int(hash2(ci + di, cj + dj, ERA_SALT) % ERAS);
			}
		}
	return era;
}

} // namespace city::b40
