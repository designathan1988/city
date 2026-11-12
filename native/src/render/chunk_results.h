// Completion storage shared by chunk renderers. Producer-side heap insertion replaces frame-wide rescans/sorts;
// blocked CPU results age into the next cycle without a bulk tail reinsertion. Caller owns synchronization.
#pragma once
#include "core/generation.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <cmath>
#include <utility>
#include <cstdlib>
#include <deque>
#include <memory>
#include <limits>

namespace city {
template<class Built> class ChunkResults {
public:
	struct Entry {
		std::shared_ptr<Built> value;
		int64_t bytes = 0;
		uint64_t order = 0, not_before = 0;
		float priority = 0;
		bool baked = false, retryable = false; // terminal bake receipt; retryable means its work never started
	};
	void begin_cycle() { if (++cycle_ == 0) std::abort(); }
	void push_back(std::shared_ptr<Built> value) {
		const bool baked = value->baked;
		const int64_t bytes = baked ? 0 : value->bytes();
		const float priority = std::isnan(value->priority) ? std::numeric_limits<float>::infinity() : value->priority;
		push(Entry{std::move(value), bytes, next_generation(), cycle_, priority, baked});
	}
	void push_bake_result(std::shared_ptr<Built> value, bool never_started) {
		const float priority = std::isnan(value->priority) ? std::numeric_limits<float>::infinity() : value->priority;
		push(Entry{std::move(value), 0, next_generation(), 0, priority, true, never_started});
	}
	bool take(Entry &out) {
		const bool cold = !heaps_[0].empty() && heaps_[0].front().not_before <= cycle_, baked = !heaps_[1].empty();
		if (!cold && !baked) return false;
		const bool use_baked = baked && (prefer_baked_ || !cold);
		prefer_baked_ = !use_baked;
		auto &heap = heaps_[size_t(use_baked)];
		std::pop_heap(heap.begin(), heap.end(), Compare{use_baked});
		out = std::move(heap.back()); heap.pop_back();
		if (!out.baked) waiting_ -= out.bytes;
		return true;
	}
	void defer(Entry value) { if (cycle_ == std::numeric_limits<uint64_t>::max()) std::abort(); value.not_before = cycle_ + 1; push(std::move(value)); }
	int64_t waiting() const { return waiting_; }
	size_t size() const { return heaps_[0].size() + heaps_[1].size(); }
	void clear() { heaps_[0].clear(); heaps_[1].clear(); waiting_ = 0; }
private:
	struct Compare {
		bool baked;
		bool operator()(const Entry &a, const Entry &b) const {
			if (!baked && a.not_before != b.not_before) return a.not_before > b.not_before;
			if (a.priority != b.priority) return a.priority > b.priority;
			return a.order > b.order;
		}
	};
	std::array<std::deque<Entry>, 2> heaps_;
	uint64_t cycle_ = 0;
	int64_t waiting_ = 0;
	bool prefer_baked_ = true;
	void push(Entry value) {
		if (!value.baked) waiting_ += value.bytes;
		const bool baked = value.baked;
		auto &heap = heaps_[size_t(baked)]; heap.push_back(std::move(value));
		std::push_heap(heap.begin(), heap.end(), Compare{baked});
	}
};
template<class Built> using Results = ChunkResults<Built>;

// Completion-only producers share incremental priority admission, without imposing a bake/byte interface on Built.
// An entry keeps its publication order when returned after the post-lock deadline check.
template<class Built> class CompletedResults {
public:
	struct Entry {
		Built value;
		uint64_t order = 0;
		float priority = 0;
	};
	void push_back(Built value) {
		const float priority = std::isnan(value.priority) ? std::numeric_limits<float>::infinity() : value.priority;
		restore(Entry{std::move(value), next_generation(), priority});
	}
	bool take(Entry &out) {
		if (heap_.empty()) return false;
		std::pop_heap(heap_.begin(), heap_.end(), Later{});
		out = std::move(heap_.back()); heap_.pop_back(); return true;
	}
	void restore(Entry value) { heap_.push_back(std::move(value)); std::push_heap(heap_.begin(), heap_.end(), Later{}); }
	size_t size() const { return heap_.size(); }
	void clear() { heap_.clear(); }
private:
	struct Later {
		bool operator()(const Entry &a, const Entry &b) const {
			if (a.priority != b.priority) return a.priority > b.priority;
			return a.order > b.order;
		}
	};
	std::deque<Entry> heap_;
};
} // namespace city
