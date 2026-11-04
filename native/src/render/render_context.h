// What every streamed renderer receives at startup: the scenario, the render data tables (project/data/render/*.json,
// read by DataTables), the materials made from data/render/materials.json by render/city_world.gd, by name, and the
// simulation's catalog (loaded before the render: what the render draws where the simulation owns it).
#pragma once

#include "sim/catalog/catalog.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <string>
#include <unordered_map>

namespace city {

struct RenderContext {
	godot::RID scenario;
	godot::Dictionary tables; // file stem -> parsed JSON (e.g. "palette", "streets", "buildings")
	std::unordered_map<std::string, godot::RID> materials;
	const Catalog *catalog = nullptr;

	// The material of that name, or an error when data/render/materials.json does not define it.
	godot::RID material(const char *name, godot::PackedStringArray &errors) const {
		auto it = materials.find(name);
		if (it != materials.end()) return it->second;
		errors.push_back(godot::String("material ausente: ") + name);
		return godot::RID();
	}
	godot::Dictionary table(const char *name, godot::PackedStringArray &errors) const {
		godot::String key(name);
		if (tables.has(key)) return tables.get(key, godot::Dictionary());
		errors.push_back(godot::String("tabela de render ausente: ") + name);
		return godot::Dictionary();
	}
};

} // namespace city
