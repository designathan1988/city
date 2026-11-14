// The records' bodies made once and kept (body_cache.h).
#include "sim/buildings/b40/body_cache.h"

#include "sim/buildings/b40/blocks.h"
#include "sim/buildings/b40/record.h"

namespace city::b40 {

namespace {
size_t cost_of(const Body &b) { return sizeof(Body) + heap_bytes(b); }
} // namespace

std::shared_ptr<const Body> BodyCache::body(const Data &data, const Building &rec) {
	if (rec.body) return rec.body.shared(); // the version itself: nothing to make
	return kept_.get(Key{ body_key(rec), Made::BODY }, [&]() {
		auto b = std::make_shared<Body>();
		make_seed_body(data, rec, *b);
		return std::shared_ptr<const Body>(std::move(b));
	}, cost_of);
}

std::shared_ptr<const Body> BodyCache::mass(const Data &data, const Building &rec) {
	return kept_.get(Key{ body_key(rec), Made::MASS }, [&]() {
		auto b = std::make_shared<Body>(*body(data, rec));
		resolve_blocks(*b);
		return std::shared_ptr<const Body>(std::move(b));
	}, cost_of);
}

std::shared_ptr<const BodyLook> BodyCache::look(const Data &data, const Building &rec) {
	return looks_.get(body_key(rec), [&]() {
		if (rec.body) return std::make_shared<const BodyLook>(body_look(*rec.body.shared(), rec.fn)); // the player's version: as it stands
		Body blocks; // a look reads the blocks, their roofs and paints: nothing the facades or the lot's layout make, so they are not made
		make_seed_body(data, rec, blocks, true);
		return std::make_shared<const BodyLook>(body_look(blocks, rec.fn));
	}, [](const BodyLook &) { return sizeof(BodyLook); });
}

std::shared_ptr<const std::vector<LotElement>> BodyCache::elements(const Data &data, const Building &rec) {
	if (rec.body) { // the version itself holds them
		const std::shared_ptr<const Body> own = rec.body.shared();
		return std::shared_ptr<const std::vector<LotElement>>(own, &own->elements);
	}
	const BodyKey key = body_key(rec);
	return elements_.get(key, [&]() {
		if (const std::shared_ptr<const Body> kept = kept_.find(Key{ key, Made::BODY })) return std::make_shared<const std::vector<LotElement>>(kept->elements);
		Body b; // made for its elements alone and let go
		make_seed_body(data, rec, b);
		b.elements.shrink_to_fit();
		return std::make_shared<const std::vector<LotElement>>(std::move(b.elements));
	}, [](const std::vector<LotElement> &e) { return sizeof(e) + e.capacity() * sizeof(LotElement); });
}

} // namespace city::b40
