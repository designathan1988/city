// What render jobs derive from the city for one place (a chunk, a node, a segment), made once for the city's revision and read by
// every job that needs it. The revision is the one its inputs move with: CityData::revision, which every change to the map moves
// (touch(), a new map, a load, a building grown), or CityData::street_revision for what reads the streets alone, and
// the jobs ask under the map's shared lock, when it cannot move, so a kept value is the one the place has now. Bounded by
// core/lru_cache.h: the least recently read pushed out first, older revisions with them; a job holds what it reads.
#pragma once

#include "core/lru_cache.h"
#include "core/random.h"

#include <cstddef>
#include <cstdint>
#include <memory>

namespace city {

template <typename T>
class KeptByRevision {
public:
	void set_budget(size_t budget) { kept_.set_budget(budget); }
	// The value of `place` at `revision`: the kept one, or the one `make()` returns (a shared_ptr<const T>), kept at `cost(value)`.
	template <typename Make, typename Cost>
	std::shared_ptr<const T> of(uint64_t revision, int64_t place, Make &&make, Cost &&cost) {
		return kept_.get(Key{ place, revision }, make, cost);
	}

private:
	struct Key {
		int64_t place = 0;
		uint64_t revision = 0;
		bool operator==(const Key &o) const { return place == o.place && revision == o.revision; }
	};
	struct KeyHash {
		static constexpr int SHIFT = 31; // the product's high half folded onto its low (a 64-bit mix)
		size_t operator()(const Key &k) const {
			const uint64_t h = (uint64_t(k.place) ^ k.revision) * GOLDEN_GAMMA_64;
			return size_t(h ^ (h >> SHIFT));
		}
	};
	LruCache<Key, const T, KeyHash> kept_;
};

} // namespace city
