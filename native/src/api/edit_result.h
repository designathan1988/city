// Shared by the CityCore command files: an EditResult as the script Dictionary, and running a command inside an undo
// entry (sim/commands/edit_journal.h): kept when it succeeds, rolled back when it fails half way.
#pragma once

#include "sim/commands/commands.h"
#include "sim/state/city_state.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>

#include <mutex>

namespace city {

inline godot::Dictionary result_dict(const EditResult &r) {
	godot::Dictionary d;
	d["ok"] = r.ok;
	d["error"] = godot::String(r.error.c_str());
	d["money"] = r.money;
	d["segments"] = r.segments;
	d["buildings_removed"] = r.buildings_removed;
	d["zoned_sides"] = r.zoned_sides;
	d["note"] = godot::String(r.note.c_str());
	return d;
}

// Locks the world, records the command under `label` (a strings.csv key) and returns its result.
template <typename F>
godot::Dictionary run_edit(CityState &s, const char *label, F &&fn) {
	std::unique_lock<std::shared_mutex> lk(s.map_mutex);
	if (!s.ready) return godot::Dictionary();
	s.history.begin(label);
	EditResult r = fn(s);
	s.history.end(s, r.ok);
	return result_dict(r);
}

} // namespace city
