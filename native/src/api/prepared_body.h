// Immutable body carried by the modelling UI. Bytes remain the file boundary, not the preview handoff.
#pragma once

#include "sim/buildings/b40/types.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <atomic>

namespace city {

class PreparedBody : public godot::RefCounted {
	GDCLASS(PreparedBody, godot::RefCounted)

public:
	static godot::Ref<PreparedBody> make(b40::Body body);
	const b40::Body &body() const { return body_; }
	const godot::Vector3 &extent() const { return extent_; }
	uint32_t digest() const;

protected:
	static void _bind_methods() {} // opaque to scripts: no mutable body or constructor API

private:
	b40::Body body_;
	godot::Vector3 extent_;
	mutable std::atomic<uint32_t> digest_{0};
	mutable std::atomic<bool> digest_ready_{false};
};

} // namespace city
