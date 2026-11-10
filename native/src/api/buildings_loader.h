// data/render/buildings.json -> the building generator's tables kept by the simulation (BuildingStore::designs).
#pragma once

#include "sim/buildings/b40/data.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <string>

namespace city {

bool load_buildings(const godot::Dictionary &t, b40::Data &d, godot::PackedStringArray &errors);
// The names of the table, for the tables that build on it.
uint8_t bay_named(const std::string &s, godot::PackedStringArray &errors);
b40::Kind kind_named(const std::string &s, godot::PackedStringArray &errors);
b40::Paint paint_named(const godot::Variant &pair, godot::PackedStringArray &errors);

} // namespace city
