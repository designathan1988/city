// data/render/buildings.json -> the building generator's tables kept by the simulation (BuildingStore::designs).
#pragma once

#include "sim/buildings/b40/data.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

namespace city {

bool load_buildings(const godot::Dictionary &t, b40::Data &d, godot::PackedStringArray &errors);

} // namespace city
