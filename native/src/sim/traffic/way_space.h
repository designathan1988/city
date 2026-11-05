// The memory of a yard's searches (way_search.cpp): their poses, their tables of poses by place and heading, and the planner's room
// that one search after another uses (WaySpace). Internal to the yard's ways.
#pragma once

#include "sim/traffic/way_search.h"

#include <algorithm>
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
	void begin(size_t places, int headings) {
		page_of_.assign(places, -1);
		headings_ = headings;
		pages_ = 0;
	}
	int &at(size_t place, int heading) {
		int &page = page_of_[place];
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

private:
	std::vector<int> page_of_;
	std::vector<std::unique_ptr<int[]>> blocks_;
	int headings_ = 0, pages_ = 0;
};

using WayItem = std::pair<double, int>;

// A list in chunks of CHUNK items that never move: pushed onto without copying what it holds (as a vector grown past its room does),
// and cleared without freeing (as a deque does, a block of 4 KB at a time: a fine search's 2.6 million poses were tens of thousands
// of frees in one tick, 4.4 ms, the planner's bench). What it holds goes with it, a few dozen chunks. Its iterator is random access,
// so std::push_heap and std::pop_heap order it exactly as they would a vector.
template <typename T> class WayChunks {
public:
	static constexpr size_t CHUNK = size_t(1) << 16;
	size_t size() const { return size_; }
	bool empty() const { return size_ == 0; }
	void clear() { size_ = 0; }
	T &operator[](size_t i) { return chunks_[i / CHUNK][i % CHUNK]; }
	const T &operator[](size_t i) const { return chunks_[i / CHUNK][i % CHUNK]; }
	T &front() { return (*this)[0]; }
	void push_back(const T &v) {
		if (size_ == chunks_.size() * CHUNK) chunks_.push_back(std::unique_ptr<T[]>(new T[CHUNK]));
		(*this)[size_++] = v;
	}
	void pop_back() { size_--; }

	class iterator {
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T *;
		using reference = T &;
		iterator() = default;
		iterator(WayChunks *c, difference_type i) : c_(c), i_(i) {}
		reference operator*() const { return (*c_)[size_t(i_)]; }
		pointer operator->() const { return &(*c_)[size_t(i_)]; }
		reference operator[](difference_type n) const { return (*c_)[size_t(i_ + n)]; }
		iterator &operator++() { ++i_; return *this; }
		iterator operator++(int) { iterator r = *this; ++i_; return r; }
		iterator &operator--() { --i_; return *this; }
		iterator operator--(int) { iterator r = *this; --i_; return r; }
		iterator &operator+=(difference_type n) { i_ += n; return *this; }
		iterator &operator-=(difference_type n) { i_ -= n; return *this; }
		iterator operator+(difference_type n) const { return iterator(c_, i_ + n); }
		friend iterator operator+(difference_type n, const iterator &it) { return it + n; }
		iterator operator-(difference_type n) const { return iterator(c_, i_ - n); }
		difference_type operator-(const iterator &o) const { return i_ - o.i_; }
		bool operator==(const iterator &o) const { return i_ == o.i_; }
		bool operator!=(const iterator &o) const { return i_ != o.i_; }
		bool operator<(const iterator &o) const { return i_ < o.i_; }
		bool operator>(const iterator &o) const { return i_ > o.i_; }
		bool operator<=(const iterator &o) const { return i_ <= o.i_; }
		bool operator>=(const iterator &o) const { return i_ >= o.i_; }

	private:
		WayChunks *c_ = nullptr;
		difference_type i_ = 0;
	};
	iterator begin() { return iterator(this, 0); }
	iterator end() { return iterator(this, std::ptrdiff_t(size_)); }

private:
	std::vector<std::unique_ptr<T[]>> chunks_;
	size_t size_ = 0;
};
struct WaySpace::Room {
	PoseTable tables[WAY_GRAINS]; // a grain's table keeps that grain's headings from one search to the next
	WayChunks<WayNode> nodes;
	WayChunks<WayItem> heap; // a min-heap by cost (std::push_heap with std::greater)
};


} // namespace traffic
} // namespace city
