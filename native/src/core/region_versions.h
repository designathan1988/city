// Local dependency stamps for immutable regional derivatives. Map writers hold their source lock;
// readers query only metadata. Reset changes the epoch without walking the whole world's cells.
#pragma once
#include "core/generation.h"
#include "core/grid.h"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <limits>

namespace city {
class RegionVersions {
public:
	void reset() { regions_.clear(); epoch_ = latest_ = next_generation(); }
	void touch(float x0, float z0, float x1, float z1) {
		const uint64_t revision = latest_ = next_generation();
		if (!for_cells(x0, z0, x1, z1, [&](int64_t key) { regions_[key] = revision; })) reset();
	}
	uint64_t stamp(float x0, float z0, float x1, float z1) const {
		uint64_t result = epoch_;
		const bool valid = for_cells(x0, z0, x1, z1, [&](int64_t key) {
			const auto it = regions_.find(key);
			if (it != regions_.end()) result = std::max(result, it->second);
		});
		return valid ? result : latest_;
	}
private:
	uint64_t epoch_ = next_generation(), latest_ = epoch_;
	std::unordered_map<int64_t, uint64_t> regions_;
	template<class F> static bool for_cells(float x0, float z0, float x1, float z1, F fn) {
		for (float value : {x0, z0, x1, z1})
			if (!std::isfinite(value) || std::fabs(double(value) / CHUNK) >= double(std::numeric_limits<int>::max() - 1)) return false;
		const int left = int(std::floor(double(x0) / CHUNK)), right = int(std::floor(double(x1) / CHUNK));
		const int top = int(std::floor(double(z0) / CHUNK)), bottom = int(std::floor(double(z1) / CHUNK));
		for (int z = top; z <= bottom; z++)
			for (int x = left; x <= right; x++) fn(cell_key(x, z));
		return true;
	}
};
} // namespace city
