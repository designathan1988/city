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
	return kept_.get(Key{ body_key(data, rec), Made::BODY }, [&]() {
		auto b = std::make_shared<Body>();
		make_seed_body(data, rec, *b);
		return std::shared_ptr<const Body>(std::move(b));
	}, cost_of);
}

std::shared_ptr<const Body> BodyCache::mass(const Data &data, const Building &rec) {
	return kept_.get(Key{ body_key(data, rec), Made::MASS }, [&]() {
		auto b = std::make_shared<Body>(*body(data, rec));
		resolve_blocks(*b);
		return std::shared_ptr<const Body>(std::move(b));
	}, cost_of);
}

} // namespace city::b40
