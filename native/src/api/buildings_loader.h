// data/catalog/buildings.json -> the building generator's tables kept by the simulation (BuildingStore::designs).
#pragma once

#include "render/data_reader.h"
#include "sim/catalog/catalog.h"
#include "sim/buildings/b40/data.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <string>

namespace city {

bool load_buildings(const godot::Dictionary &t, b40::Data &d, godot::PackedStringArray &errors);
// Each zone's building kind (zones.json "building") and whether it is a low shop street (commercial at low density), into the tables.
bool set_zone_kinds(const Catalog &catalog, b40::Data &d, godot::PackedStringArray &errors);
// The names of the table, for the tables that build on it: an unknown name is an error on the caller's reader (its path).
uint8_t bay_named(const std::string &s, DataReader &r);
b40::Kind kind_named(const std::string &s, DataReader &r);
// [finish, '#rrggbb'].
b40::Paint paint_named(const godot::Variant &pair, DataReader r);

} // namespace city
