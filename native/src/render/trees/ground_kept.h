// The ground of each chunk the plants read (tree_place.h ChunkGround: its roads, its lots, its ecosystem), gathered once for the
// city's revision (render/kept_by_revision.h) and read by every job that plants the chunk: its near trees, the far cards of its
// superchunk and its ground cover. Bounded by memory (tuning render.ground_cache_mb).
#pragma once

#include "render/kept_by_revision.h"
#include "render/trees/tree_place.h"
#include "sim/state/city_state.h"

#include <cstddef>
#include <cstdint>
#include <memory>

namespace city {

class GroundKept {
public:
	void set_budget(size_t bytes) { kept_.set_budget(bytes); }
	// Chunk `key`'s ground; the caller holds the map's shared lock.
	std::shared_ptr<const ChunkGround> of(const CityState &s, int64_t key) {
		return kept_.of(s.revision, key, [&]() { return std::make_shared<const ChunkGround>(chunk_ground(s, key)); },
				[](const ChunkGround &g) { return g.bytes(); });
	}

private:
	KeptByRevision<ChunkGround> kept_;
};

} // namespace city
