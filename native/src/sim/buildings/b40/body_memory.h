// Retained allocations of one immutable building body, including nested facade overrides and furnishing.
#pragma once

#include "core/retained_memory.h"
#include "sim/buildings/b40/types.h"

namespace city::b40 {

inline size_t body_retained_bytes(const Body &body) {
	RetainedMemory memory;
	memory.storage(sizeof(Body));
	memory.vector(body.paints);
	memory.vector(body.volumes);
	for (const Volume &v : body.volumes) {
		memory.vector(v.outline); memory.vector(v.fill); memory.vector(v.reliefs); memory.vector(v.gear);
		memory.hash(v.bays); memory.hash(v.side_fill); memory.hash(v.storey_paint);
	}
	memory.vector(body.elements);
	memory.vector(body.cores);
	memory.tree(body.furnishing);
	for (const auto &floor : body.furnishing) memory.vector(floor.second);
	return memory.bytes();
}

} // namespace city::b40
