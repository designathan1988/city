// data/render/interiors.json -> the interior tables kept by the simulation (BuildingStore::designs).
#pragma once

#include "sim/buildings/interior/tables.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

namespace city {

bool load_interiors(const godot::Dictionary &t, b40::InteriorTables &out, godot::PackedStringArray &errors);

} // namespace city
