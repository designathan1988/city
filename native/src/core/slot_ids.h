// Free-slot allocator for id-addressed tables: the one owner of which ids are free and in what order they are reused.
// A LIFO stack of free ids; claiming a specific id (undo, redo, load) drops its entry lazily, so put() is O(1) and a
// restore of n records is O(n) instead of O(n^2). The order of reuse is exactly the order an eager erase would give:
// an entry counts only while it is the live entry of its id (`at_[id]` holds its position in the stack).
#pragma once

#include <cstdint>
#include <vector>

class SlotIds {
public:
	void clear() {
		stack_.clear();
		at_.clear();
		live_ = 0;
	}

	// The id add() would use for a table of `size` slots: the most recently freed one, or a new slot at the end.
	int next(int size) {
		drop_stale_top();
		return live_ > 0 ? stack_.back() : size;
	}

	// Frees `id` (its slot stays in the table, dead).
	void release(int id) {
		reach(id);
		if (at_[size_t(id)] >= 0) return; // already free
		at_[size_t(id)] = int32_t(stack_.size());
		stack_.push_back(id);
		live_++;
	}

	// Marks `id` as used. A table of `size` slots that grows to hold it frees the gap slots first, lowest first, as
	// new dead slots. Returns the size the table must have.
	int claim(int id, int size) {
		for (int k = size; k < id; k++) release(k);
		reach(id);
		int32_t &pos = at_[size_t(id)];
		if (pos >= 0) {
			pos = -1;
			live_--;
			if (stack_.size() > 2 * size_t(live_) + COMPACT_SLACK) compact();
		}
		return id >= size ? id + 1 : size;
	}

	bool is_free(int id) const { return id < int(at_.size()) && at_[size_t(id)] >= 0; }
	int free_count() const { return live_; }

private:
	static constexpr size_t COMPACT_SLACK = 64; // stale entries tolerated before a rebuild, so tiny tables never churn

	void reach(int id) {
		if (id >= int(at_.size())) at_.resize(size_t(id) + 1, -1);
	}
	bool live_entry(size_t i) const { return at_[size_t(stack_[i])] == int32_t(i); }
	void drop_stale_top() {
		while (!stack_.empty() && !live_entry(stack_.size() - 1)) stack_.pop_back();
	}
	void compact() { // keeps the relative order of the live entries, so reuse order is unchanged
		size_t w = 0;
		for (size_t i = 0; i < stack_.size(); i++) {
			if (!live_entry(i)) continue;
			stack_[w] = stack_[i];
			at_[size_t(stack_[w])] = int32_t(w);
			w++;
		}
		stack_.resize(w);
	}

	std::vector<int> stack_;
	std::vector<int32_t> at_; // position of each id's live entry in stack_, or -1 when the id is in use
	int live_ = 0;
};
