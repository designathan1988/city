// Converts the parsed data tables (project/data/catalog/*.json, read once by game/data/data_tables.gd)
// into the C++ Catalog. Every key is required: a missing or mistyped key is reported and loading fails,
// so no default value can silently replace the data.
#pragma once

#include "sim/catalog/catalog.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

namespace city {

class CatalogLoader {
public:
	// tables: { "tuning", "maps", "road_profiles", "zones", "growth", "traffic", "street_furniture", "blast", "nature", "scenic" } (parsed JSON)
	static bool load(const godot::Dictionary &tables, Catalog &out, godot::PackedStringArray &errors);
	static uint32_t parse_color(const godot::String &hex); // "#RRGGBB" or "#RRGGBBAA" -> RGBA8
};

// data/catalog/traffic.json (api/traffic_loader.cpp).
bool load_traffic_params(const godot::Dictionary &t, TrafficParams &p, godot::PackedStringArray &errors);

// data/catalog/street_furniture.json (api/furniture_loader.cpp).
bool load_street_furniture(const godot::Dictionary &t, StreetFurnitureDef &d, godot::PackedStringArray &errors);

// data/catalog/blast.json (api/blast_loader.cpp).
bool load_blast_params(const godot::Dictionary &t, BlastParams &p, godot::PackedStringArray &errors);
// data/catalog/nature.json (api/nature_loader.cpp).
bool load_nature(const godot::Dictionary &t, NatureDef &n, godot::PackedStringArray &errors);
// data/catalog/scenic.json (api/scenic_loader.cpp), read after the profiles and the nature tables; then each map that names a
// scenic city (`names`, by map, empty for none) takes its index.
bool load_scenic(const godot::Dictionary &t, Catalog &c, godot::PackedStringArray &errors);
bool link_scenic_maps(const std::vector<std::string> &names, Catalog &c, godot::PackedStringArray &errors);

} // namespace city
