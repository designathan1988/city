// The broken records as the scripts read them (CityProbes::broken_records) and as CityCore's frame says them (push_error): one
// entry per check, {feature, check, records: PackedInt32Array, dropped, after}, `after` what the first of them was found after
// (a command's label, "ticks"; empty for the whole pass), `dropped` the records found past the room (tuning checks.records_kept).
#pragma once

#include "sim/checks/records.h"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <algorithm>
#include <cstring>
#include <vector>

namespace city {

// The entries of `broken` appended to `out`; `causes` (beside its records) or null.
inline void append_broken(godot::Array &out, const checks::Broken &broken, const std::vector<const char *> *causes) {
	struct Group {
		const char *feature, *check, *after;
		godot::PackedInt32Array records;
	};
	std::vector<Group> groups; // a few: one per check that found something
	const std::vector<checks::BrokenRecord> &records = broken.records();
	for (size_t i = 0; i < records.size(); i++) {
		const checks::BrokenRecord &r = records[i];
		auto g = std::find_if(groups.begin(), groups.end(),
				[&](const Group &x) { return std::strcmp(x.feature, r.feature) == 0 && std::strcmp(x.check, r.check) == 0; });
		if (g == groups.end()) {
			groups.push_back({ r.feature, r.check, causes && i < causes->size() ? (*causes)[i] : "", godot::PackedInt32Array() });
			g = groups.end() - 1;
		}
		g->records.push_back(r.record);
	}
	if (groups.empty() && broken.dropped() > 0) groups.push_back({ "checks", "records_kept", "", godot::PackedInt32Array() });
	for (const Group &g : groups) {
		godot::Dictionary d;
		d["feature"] = godot::String(g.feature);
		d["check"] = godot::String(g.check);
		d["records"] = g.records;
		d["dropped"] = int64_t(broken.dropped());
		d["after"] = godot::String(g.after);
		out.push_back(d);
	}
}

} // namespace city
