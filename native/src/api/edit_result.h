// Shared by the CityCore command files: an EditResult as the script Dictionary, and running a command inside an undo
// entry (sim/commands/edit_journal.h): kept when it succeeds, rolled back when it fails half way.
#pragma once

#include "api/command_hold.h"
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
	d["placed"] = r.placed;
	return d;
}

// The answer to a command given while no city is open: refused, with the key that says so.
inline godot::Dictionary no_city_dict() {
	EditResult r;
	r.error = "hint.noCity";
	return result_dict(r);
}

// Locks the world, records the command under `label` (a strings.csv key) and returns its result. With no city open the
// command is refused with its key, never answered with an empty Dictionary. The hold's end checks what the command changed, and
// after a refusal that the rollback left the city as it was (api/command_hold.h, the tests' DLL).
template <typename F>
godot::Dictionary run_edit(CityState &s, const char *label, F &&fn) {
	CommandHold hold(s, label);
	if (!s.ready) return no_city_dict();
	s.history.begin(s, label);
	EditResult r = fn(s);
	s.history.end(s, r.ok);
	if (!r.ok) hold.refused();
	return result_dict(r);
}

} // namespace city
