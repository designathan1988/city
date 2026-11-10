// The ground of each chunk the plants read (tree_place.h ChunkGround: its roads, its lots, its ecosystem), gathered once for the
// city's revision (render/kept_by_revision.h) and read by every job that plants the chunk: its near trees, the far cards of its
// superchunk and its ground cover. Bounded by memory (tuning render.ground_cache_mb).
#pragma once

#include "render/kept_by_revision.h"
#include "render/trees/tree_place.h"
#include "sim/state/city_view.h"

#include <cstddef>
#include <cstdint>
#include <memory>

namespace city {

class GroundKept {
public:
	void set_budget(size_t bytes) { kept_.set_budget(bytes); }
	// Chunk `key`'s ground, of the city `s` reads at its revision: the live one (its caller holds the map) or a job's cut, whose
	// ids are the city's own, so a ground made from a cut serves every job of that revision.
	std::shared_ptr<const ChunkGround> of(const CityView &s, int64_t key) {
		return kept_.of(s.revision, key, [&]() { return std::make_shared<const ChunkGround>(chunk_ground(s, key)); },
				[](const ChunkGround &g) { return g.bytes(); });
	}

private:
	KeptByRevision<ChunkGround> kept_;
};

} // namespace city
