// The regional style of a table (session-rules.md, 10): what changes from one region to another lives under `styles.<id>`. A city
// does not carry its style yet, so every table is read at the default; when it does, only this file changes.
#pragma once

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/variant.hpp>

namespace city {

constexpr const char *DEFAULT_REGION = "roadcraft";

inline godot::Dictionary region_table(const godot::Dictionary &table) {
	godot::Dictionary styles = table.get("styles", godot::Dictionary());
	return styles.get(DEFAULT_REGION, godot::Dictionary());
}

} // namespace city
