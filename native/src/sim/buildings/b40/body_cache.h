// The bodies of the building records, made once from the generator's tables and kept: the one owner of derived bodies for every
// reader, the simulation's (picking, parking gates, growth, doors, the yard) and the render's (buildings, gardens, walls, canyons).
// record.h record_body and record_mass are its front. Keyed exactly by what a body is made from (body_key), so a kept body is the
// body the generator would make again; the tables' own cache (Data::made), so a new filling of the tables starts empty. An edited
// body is the record's own version, never copied; its mass (the blocks resolved) is kept as a seed body's is. Bounded by memory
// (tuning sim.body_cache_mb), the least recently asked pushed out first; whoever holds a body keeps it past that. How a body looks
// (likeness.h) is read from the kept body and kept beside it by the same key: the look of what stands, never of another body.
// What stands on the lot round the building (its plants, gates, stalls and docks: Body::elements) is kept the same way, as a
// list of its own: the trees' jobs, the kerb's gates and the parking read a lot's elements for every lot within their reach,
// far past the buildings drawn, and asking the whole body for them made bodies kilometres off and pushed the drawn ones out.
#pragma once

#include "core/lru_cache.h"
#include "sim/buildings/b40/body_table.h"
#include "sim/buildings/likeness.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace city {
struct Building;
}

namespace city::b40 {

struct Data;

class BodyCache {
public:
	// The record's body as the player edits it.
	std::shared_ptr<const Body> body(const Data &data, const Building &rec);
	// That body as what stands: its blocks resolved into solid ones (blocks.h).
	std::shared_ptr<const Body> mass(const Data &data, const Building &rec);
	// How the record's body looks (likeness.h body_look of the body above, under the record's function), kept beside it by the
	// same key: the look of what stands, asked by growth for every neighbour of a lot, on any thread.
	std::shared_ptr<const BodyLook> look(const Data &data, const Building &rec);
	// The record's lot elements (the body's `elements`). A body made only for them is not kept: it pushes no kept body out.
	std::shared_ptr<const std::vector<LotElement>> elements(const Data &data, const Building &rec);
	size_t used() const { return kept_.used() + looks_.used() + elements_.used(); } // bytes kept (the memory census)
	void set_budget(size_t bytes) {
		kept_.set_budget(bytes);
		looks_.set_budget(bytes);
		elements_.set_budget(bytes);
	}

private:
	enum class Made : uint8_t { BODY, MASS };
	struct Key {
		BodyKey body;
		Made made = Made::BODY;
		bool operator==(const Key &o) const { return body == o.body && made == o.made; }
	};
	struct KeyHash {
		size_t operator()(const Key &k) const { return size_t(BodyKeyHash::mix(BodyKeyHash()(k.body), uint64_t(k.made))); }
	};
	LruCache<Key, const Body, KeyHash> kept_;
	LruCache<BodyKey, const BodyLook, BodyKeyHash> looks_;
	LruCache<BodyKey, const std::vector<LotElement>, BodyKeyHash> elements_;
};

} // namespace city::b40
