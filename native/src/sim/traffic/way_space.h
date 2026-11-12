// The memory of a yard's searches (way_search.cpp): their poses, their tables of poses by place and heading, and the planner's room
// that one search after another uses (WaySpace). Internal to the yard's ways.
#pragma once

#include "sim/traffic/way_search.h"
#include "core/generation.h"
#include "core/stable_chunks.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <new>
#include <type_traits>
#include <cstddef>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

namespace city {
namespace traffic {

constexpr int WAY_COARSE = 0, WAY_FINE = 1, WAY_GRAINS = 2; // the grains of a search's places (way_search.cpp GRAINS)

struct WayNode {
	AxlePose pose;
	double g = 0, kappa = 0;
	int parent = -1;
	bool closed = false;
	int grain = WAY_COARSE; // the grain of the step that reached it: the table it is kept in
};

// The node standing for each place and heading: a page of headings for each place touched, in blocks that never move. A search runs
// on the simulation's tick, so nothing here grows by copying all it holds (a hash table rehashing took 78 ms of one tick).
class PoseTable {
public:
	// A search's start: every place empty again, the blocks of the last search kept for this one's pages (WaySpace).
	void begin(size_t /*places*/, int headings) {
		epoch_ = next_generation();
		headings_ = headings;
		pages_ = 0;
	}
	int &at(size_t place, int heading) {
		if (!root_) { root_ = std::make_unique<IndexNode>(); indexed += NODE_CELLS; }
		IndexNode *node = root_.get();
		// Fixed-depth radix links: no grid-wide initialization, vector relocation or hash rehash in a tick.
		for (int shift = int((sizeof(size_t) - 1) * BYTE_BITS); shift >= 2 * BYTE_BITS; shift -= BYTE_BITS) {
			auto &next = node->next[(place >> shift) & BYTE_MASK];
			if (!next) { next = std::make_unique<IndexNode>(); indexed += NODE_CELLS; }
			node = next.get();
		}
		auto &leaf = node->leaf[(place >> BYTE_BITS) & BYTE_MASK];
		if (!leaf) leaf = std::make_unique<IndexPage>();
		if (leaf->epoch != epoch_) {
			leaf->page.fill(-1);
			leaf->epoch = epoch_;
			indexed += INDEX_CELLS;
		}
		int &page = leaf->page[place & BYTE_MASK];
		if (page < 0) {
			page = pages_++;
			taken++;
			// not zeroed: a page is set to -1 when it is taken, below
			if (size_t(page / BLOCK_PAGES) == blocks_.size()) blocks_.push_back(std::unique_ptr<int[]>(new int[size_t(BLOCK_PAGES) * headings_]));
			std::fill_n(&blocks_[size_t(page / BLOCK_PAGES)][size_t(page % BLOCK_PAGES) * headings_], headings_, -1);
		}
		return blocks_[size_t(page / BLOCK_PAGES)][size_t(page % BLOCK_PAGES) * headings_ + heading];
	}

private:
	static constexpr int BLOCK_PAGES = 256;

public:
	int64_t taken = 0; // the pages taken, ever: a search reads what it took
	int64_t indexed = 0; // actual index initialization in int-sized write units, ever

private:
	static constexpr int BYTE_BITS = 8, INDEX_CELLS = 1 << BYTE_BITS;
	static constexpr size_t BYTE_MASK = INDEX_CELLS - 1;
	struct IndexPage {
		IndexPage() : epoch(0) {} // the entries initialize only when reached
		uint64_t epoch;
		std::array<int, INDEX_CELLS> page;
	};
	struct IndexNode {
		IndexNode() {} // construct null links once, without a preliminary value-initialization pass
		std::array<std::unique_ptr<IndexNode>, INDEX_CELLS> next;
		std::array<std::unique_ptr<IndexPage>, INDEX_CELLS> leaf;
	};
	static constexpr int NODE_CELLS = int((2 * INDEX_CELLS * sizeof(void *) + sizeof(int) - 1) / sizeof(int));
	std::unique_ptr<IndexNode> root_;
	uint64_t epoch_ = 0;
	std::vector<std::unique_ptr<int[]>> blocks_;
	int headings_ = 0, pages_ = 0;
};

using WayItem = std::pair<double, int>;

// A list in chunks of CHUNK items that never move: pushed onto without copying what it holds (as a vector grown past its room does),
// and cleared without freeing (as a deque does, a block of 4 KB at a time: a fine search's 2.6 million poses were tens of thousands
// of frees in one tick, 4.4 ms, the planner's bench). What it holds goes with it, a few dozen chunks. Its iterator is random access,
// so std::push_heap and std::pop_heap order it exactly as they would a vector.
template<typename T> using WayChunks = ::city::StableChunks<T, size_t(1) << 16>;

struct WaySpace::Room {
	PoseTable tables[WAY_GRAINS]; // a grain's table keeps that grain's headings from one search to the next
	WayChunks<WayNode> nodes;
	WayChunks<WayItem> heap; // a min-heap by cost (std::push_heap with std::greater)
};


} // namespace traffic
} // namespace city
