// The records' bodies made once and kept (body_cache.h).
#include "sim/buildings/b40/body_cache.h"

#include "sim/buildings/b40/blocks.h"
#include "sim/buildings/b40/record.h"
#include "sim/buildings/building_store.h"

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

std::shared_ptr<const BodyLook> BodyCache::look(const Data &data, const Building &rec, const Tuning &tn) {
	return looks_.get(body_key(rec), [&]() { return std::make_shared<const BodyLook>(body_look(tn, *body(data, rec), rec.fn)); },
			[](const BodyLook &) { return sizeof(BodyLook); });
}

} // namespace city::b40
