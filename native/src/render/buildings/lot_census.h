// What a placed body's lot draws against its own lines and its open gates, counted for the probes (BuildingRenderer::bay_census): pitched
// roofs and lot parts past the lot's lines, gates standing in their own passage, and parts and ground blocks standing where an open gate's
// leaf stands (render/buildings/lot_emit.cpp draws the lot, data.h LotRules::gate_leaves the leaves).
#pragma once

#include "render/buildings/facade.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <cstdint>

namespace city {

struct LotCensus {
	int64_t roof_past = 0, lot_past = 0, gates = 0, shut = 0, blocked = 0;
	godot::Array lot_past_first, shut_first, blocked_first;
	// One body placed, `id` its record's seed (named in the firsts).
	void add(const b40::Placed &pl, int id);
	// The counts into the census.
	void put(godot::Dictionary &d) const;
};

} // namespace city
