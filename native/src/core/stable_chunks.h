// Stable raw chunks with logical clear and random-access heap order; reserve storage, construct only pushed values.
#pragma once
#include <cstddef>
#include <array>
#include <limits>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

namespace city {
template <typename T, size_t BLOCK_ITEMS = 256> class StableChunks {
	static_assert(BLOCK_ITEMS > 0 && std::is_trivially_destructible_v<T>, "logical clear keeps only scalar/non-owning values");
public:
	static constexpr size_t CHUNK = BLOCK_ITEMS;
	StableChunks() = default;
	StableChunks(const StableChunks &) = delete;
	StableChunks &operator=(const StableChunks &) = delete;
	StableChunks(StableChunks &&other) : root_(std::move(other.root_)), blocks_(std::exchange(other.blocks_, 0)), level_(std::exchange(other.level_, 0)), size_(std::exchange(other.size_, 0)) {}
	StableChunks &operator=(StableChunks &&other) {
		if (this != &other) { root_ = std::move(other.root_); blocks_ = std::exchange(other.blocks_, 0); level_ = std::exchange(other.level_, 0); size_ = std::exchange(other.size_, 0); }
		return *this;
	}
	size_t size() const { return size_; }
	bool empty() const { return size_ == 0; }
	void clear() { size_ = 0; }
	T &operator[](size_t i) { return block(i / CHUNK).data[i % CHUNK]; }
	const T &operator[](size_t i) const { return block(i / CHUNK).data[i % CHUNK]; }
	T &front() { return (*this)[0]; }
	T &back() { return (*this)[size_ - 1]; }
	const T &back() const { return (*this)[size_ - 1]; }
	void push_back(const T &v) {
		if (size_ == std::numeric_limits<size_t>::max()) std::abort();
		if (size_ / CHUNK == blocks_) append_block();
		Block &block = this->block(size_ / CHUNK);
		const size_t at = size_ % CHUNK;
		if (at == block.constructed) {
			::new (static_cast<void *>(block.data + at)) T(v);
			block.constructed++;
		} else block.data[at] = v;
		size_++;
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
		iterator(StableChunks *c, difference_type i) : c_(c), i_(i) {}
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
		StableChunks *c_ = nullptr;
		difference_type i_ = 0;
	};
	iterator begin() { return iterator(this, 0); }
	iterator end() { return iterator(this, std::ptrdiff_t(size_)); }

private:
	struct Block {
		std::allocator<T> allocator;
		T *data = allocator.allocate(CHUNK); // reserve storage, never default-construct thousands of unused poses
		size_t constructed = 0;
		Block() = default;
		Block(const Block &) = delete;
		Block &operator=(const Block &) = delete;
		~Block() {
			if constexpr (!std::is_trivially_destructible_v<T>)
				for (size_t i = 0; i < constructed; i++) std::allocator_traits<std::allocator<T>>::destroy(allocator, data + i);
			allocator.deallocate(data, CHUNK);
		}
	};
	static constexpr unsigned BITS = 5;
	static constexpr size_t FANOUT = size_t(1) << BITS, MASK = FANOUT - 1;
	struct Directory {
		Directory() {} // unique_ptr constructs null links once; no preliminary value-initialization pass
		std::array<std::unique_ptr<Directory>, FANOUT> next;
		std::array<std::unique_ptr<Block>, FANOUT> blocks;
	};
	void append_block() {
		size_t high = blocks_ / FANOUT;
		unsigned wanted = 0;
		while (high) { ++wanted; high /= FANOUT; }
		if (!root_) root_ = std::make_unique<Directory>();
		while (level_ < wanted) { auto parent = std::make_unique<Directory>(); parent->next[0] = std::move(root_); root_ = std::move(parent); ++level_; }
		Directory *at = root_.get();
		for (unsigned level = level_; level; --level) {
			auto &child = at->next[(blocks_ >> (level * BITS)) & MASK];
			if (!child) child = std::make_unique<Directory>();
			at = child.get();
		}
		at->blocks[blocks_ & MASK] = std::make_unique<Block>(); ++blocks_;
	}
	Block &block(size_t index) {
		Directory *at = root_.get();
		for (unsigned level = level_; level; --level) at = at->next[(index >> (level * BITS)) & MASK].get();
		return *at->blocks[index & MASK];
	}
	const Block &block(size_t index) const {
		const Directory *at = root_.get();
		for (unsigned level = level_; level; --level) at = at->next[(index >> (level * BITS)) & MASK].get();
		return *at->blocks[index & MASK];
	}
	std::unique_ptr<Directory> root_;
	size_t blocks_ = 0;
	unsigned level_ = 0;
	size_t size_ = 0;
};
} // namespace city
