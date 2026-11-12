#include "api/prepared_body.h"

#include "sim/buildings/b40/roof_shape.h"
#include "sim/persistence/body_codec.h"

#include <algorithm>
#include <utility>

namespace city {

godot::Ref<PreparedBody> PreparedBody::make(b40::Body body) {
	godot::Ref<PreparedBody> prepared;
	prepared.instantiate();
	float top = 0;
	for (const b40::Volume &v : body.volumes) top = std::max(top, body.level_z(v.top()) + b40::roof_rise(v));
	prepared->extent_ = godot::Vector3(body.plan.W, body.plan.D, top);
	prepared->body_ = std::move(body);
	return prepared;
}

uint32_t PreparedBody::digest() const {
	if (digest_ready_.load(std::memory_order_acquire)) return digest_.load(std::memory_order_relaxed);
	// Cold readers may compute independently; none waits on another reader's body preparation.
	const uint32_t value = body_digest(body_);
	digest_.store(value, std::memory_order_relaxed);
	digest_ready_.store(true, std::memory_order_release);
	return value;
}

} // namespace city
