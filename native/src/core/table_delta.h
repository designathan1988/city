// Ordered record edits over an indexed table. Replay keeps record order and ids; undo reverses both edits and direction.
#pragma once

#include <cassert>
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

namespace city {

template <typename T>
struct TableDelta {
	struct Edit {
		size_t index = 0;
		std::optional<T> before, after;
	};
	std::vector<Edit> edits;
	bool empty() const { return edits.empty(); }
	void add(size_t at, T value) { edits.push_back({ at, {}, std::move(value) }); }
	void remove(size_t at, T value) { edits.push_back({ at, std::move(value), {} }); }
	void replace(size_t at, T before, T after) { edits.push_back({ at, std::move(before), std::move(after) }); }
	void apply(std::vector<T> &table, bool forward) const {
		auto one = [&](const Edit &edit) {
			const auto &from = forward ? edit.before : edit.after, &to = forward ? edit.after : edit.before;
			if (from && to) {
				assert(edit.index < table.size());
				table[edit.index] = *to;
			} else if (to) {
				assert(edit.index <= table.size());
				table.insert(table.begin() + std::ptrdiff_t(edit.index), *to);
			} else {
				assert(from && edit.index < table.size());
				table.erase(table.begin() + std::ptrdiff_t(edit.index));
			}
		};
		if (forward) for (const Edit &edit : edits) one(edit);
		else for (auto it = edits.rbegin(); it != edits.rend(); ++it) one(*it);
	}
};

} // namespace city
