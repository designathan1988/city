// The basins of water a placed body's lot draws (types.h EL_POOL: a house's pool, a fountain's basin), read from the mesh
// lot_props.cpp writes of each, drawn alone: looking out from the basin's middle every way, its water runs to its element's outline
// (sim/buildings/b40/element_shape.h) and its rim stands right past it, over the water: a rim closed all round, its corners too.
#pragma once

#include "render/buildings/facade.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <cstdint>

namespace city {

struct BasinCensus {
	int64_t basins = 0, round = 0, looks = 0, dry = 0, open = 0;
	godot::Array first_dry, first_open;
	// One body placed, `id` named in the firsts.
	void add(const b40::Placed &pl, int id);
	// The counts into the answer.
	void put(godot::Dictionary &d) const;
};

} // namespace city
