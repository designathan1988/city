// data/catalog/models.json -> the Build > Models catalogue kept with the generator's tables (Data::models).
#pragma once

#include "sim/buildings/b40/data.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

namespace city {

bool load_models(const godot::Dictionary &t, b40::Data &d, godot::PackedStringArray &errors);

} // namespace city
