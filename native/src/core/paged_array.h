// Persistent scalar pages: snapshots retain an immutable radix root in O(1); a writer copies only its shared path/page.
// Source owners serialize writers and snapshot acquisition. Readers of published snapshots need no lock or mutable cache.
#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace city {
template<class T, size_t PAGE_BYTES = 16 * 1024> class PagedArray {
	static_assert(std::is_trivially_copyable_v<T> && sizeof(T) <= PAGE_BYTES);
	static constexpr unsigned BITS = 5;
	static constexpr size_t FANOUT = size_t(1) << BITS, MASK = FANOUT - 1;
	static constexpr size_t VALUES = PAGE_BYTES / sizeof(T);
	struct Page {
		std::array<T, VALUES> values;
		explicit Page(const T &fill) { values.fill(fill); }
	};
	struct Node {
		std::array<std::shared_ptr<Node>, FANOUT> children;
		std::array<std::shared_ptr<Page>, FANOUT> pages;
	};
	static const std::shared_ptr<Page> *page_ref(const Node *root, unsigned level, size_t page) {
		while (root && level) {
			root = root->children[(page >> (level * BITS)) & MASK].get();
			--level;
		}
		return root ? &root->pages[page & MASK] : nullptr;
	}
	static const Page *page_at(const Node *root, unsigned level, size_t page) {
		const auto *found = page_ref(root, level, page);
		return found ? found->get() : nullptr;
	}
	static bool same(const T &a, const T &b) { return std::memcmp(&a, &b, sizeof(T)) == 0; }
public:
	class Snapshot;
	// A caller-local cursor avoids repeated tree walks for adjacent samples without sharing mutable reader state.
	class Reader {
	public:
		T operator[](size_t index) {
			assert(index < size_);
			const size_t page = index / VALUES;
			if (page != cached_id_) { cached_ = page_at(root_, level_, page); cached_id_ = page; }
			return cached_ ? cached_->values[index % VALUES] : fill_;
		}
	private:
		friend class PagedArray;
		friend class Snapshot;
		const Node *root_ = nullptr;
		size_t size_ = 0, cached_id_ = std::numeric_limits<size_t>::max();
		unsigned level_ = 0;
		T fill_{};
		const Page *cached_ = nullptr;
		Reader(const Node *root, size_t size, unsigned level, T fill) : root_(root), size_(size), level_(level), fill_(fill) {}
	};
	class Snapshot {
	public:
		Snapshot() = default;
		Snapshot(const Snapshot &) = default;
		Snapshot &operator=(const Snapshot &) = default;
		Snapshot(Snapshot &&other) noexcept : root_(std::move(other.root_)), size_(std::exchange(other.size_, 0)), level_(std::exchange(other.level_, 0)), fill_(other.fill_) {}
		Snapshot &operator=(Snapshot &&other) noexcept {
			if (this != &other) { root_ = std::move(other.root_); size_ = std::exchange(other.size_, 0); level_ = std::exchange(other.level_, 0); fill_ = other.fill_; }
			return *this;
		}
		// Retains only pages intersecting these rows; callers may read only these rows afterwards.
		// HeightPatch clamps every read to its window. Build the page lease outside the source lock when possible.
		Snapshot window_rows(size_t first, size_t width, size_t rows, size_t stride) const {
			return PagedArray::window(*this, first, width, rows, stride);
		}
		size_t size() const { return size_; }
		bool empty() const { return size_ == 0; }
		Reader reader() const & { return Reader(root_.get(), size_, level_, fill_); }
		Reader reader() && = delete;
		Reader reader() const && = delete;
		T operator[](size_t index) const { auto cursor = reader(); return cursor[index]; }
	private:
		friend class PagedArray;
		std::shared_ptr<const Node> root_;
		size_t size_ = 0;
		unsigned level_ = 0;
		T fill_{};
		Snapshot(std::shared_ptr<const Node> root, size_t size, unsigned level, T fill) :
				root_(std::move(root)), size_(size), level_(level), fill_(fill) {}
	};
	PagedArray() = default;
	PagedArray(const PagedArray &) = default;
	PagedArray &operator=(const PagedArray &) = default;
	PagedArray(PagedArray &&other) noexcept : root_(std::move(other.root_)), size_(std::exchange(other.size_, 0)), level_(std::exchange(other.level_, 0)), fill_(other.fill_) {}
	PagedArray &operator=(PagedArray &&other) noexcept {
		if (this != &other) { root_ = std::move(other.root_); size_ = std::exchange(other.size_, 0); level_ = std::exchange(other.level_, 0); fill_ = other.fill_; }
		return *this;
	}
	size_t size() const { return size_; }
	bool empty() const { return size_ == 0; }
	Reader reader() const & { return Reader(root_.get(), size_, level_, fill_); }
	Reader reader() && = delete;
	Reader reader() const && = delete;
	Snapshot snapshot() const { return Snapshot(root_, size_, level_, fill_); }
	T operator[](size_t index) const { auto cursor = reader(); return cursor[index]; }
	void assign(size_t size, T fill) {
		root_.reset(); size_ = size; fill_ = fill; level_ = 0;
		size_t last_page = size ? (size - 1) / VALUES : 0;
		while (last_page >= FANOUT) { last_page /= FANOUT; ++level_; }
	}
	// Growing keeps all old values/versions; newly addressable slots retain the original default without dense filling.
	void grow(size_t size) {
		if (size <= size_) return;
		size_t last_page = (size - 1) / VALUES;
		unsigned wanted = 0;
		while (last_page >= FANOUT) { last_page /= FANOUT; ++wanted; }
		while (level_ < wanted) {
			if (root_) { auto parent = std::make_shared<Node>(); parent->children[0] = std::move(root_); root_ = std::move(parent); }
			++level_;
		}
		size_ = size;
	}
	bool set(size_t index, T value) {
		assert(index < size_);
		if (same((*this)[index], value)) return false;
		writable_page(index / VALUES)->values[index % VALUES] = value;
		return true;
	}
	// The generator supplies values in index order and must not mutate/snapshot this array inside the callback.
	// Grouping page writes preserves scalar arithmetic/order while avoiding a COW tree walk per sample.
	template<class Generate> void generate_range(size_t first, size_t count, Generate &&generate) {
		assert(first <= size_ && count <= size_ - first);
		while (count) {
			const size_t page = first / VALUES, offset = first % VALUES, n = std::min(count, VALUES - offset);
			const Page *before = page_at(root_.get(), level_, page);
			Page *write = nullptr;
			for (size_t i = 0; i < n; ++i) {
				const T value = generate(first + i);
				const T old = write ? write->values[offset + i] : before ? before->values[offset + i] : fill_;
				if (same(old, value)) continue;
				if (!write) write = writable_page(page);
				write->values[offset + i] = value;
			}
			first += n; count -= n;
		}
	}
	template<class Generate> void assign_generated(size_t size, Generate &&generate) {
		assign(size, T{});
		generate_range(0, size, std::forward<Generate>(generate));
	}
	// The adapter joins its independent block fills before private page metadata is attached by the caller.
	template<class Fill, class Parallel> void assign_blocks_parallel(size_t size, Fill &&fill, Parallel &&parallel) {
		assign(size, T{});
		const size_t count = size / VALUES + (size % VALUES != 0);
		std::vector<std::shared_ptr<Page>> prepared(count);
		parallel(count, [&](size_t page) {
			auto values = std::make_shared<Page>(fill_);
			const size_t first = page * VALUES, n = std::min(VALUES, size - first);
			fill(first, n, values->values.data()); prepared[page] = std::move(values);
		});
		for (size_t page = 0; page < count; ++page) page_slot(page) = std::move(prepared[page]);
	}
	void copy_to(T *destination, size_t first, size_t count) const {
		assert(first <= size_ && count <= size_ - first);
		while (count) {
			const size_t offset = first % VALUES, n = std::min(count, VALUES - offset);
			const Page *page = page_at(root_.get(), level_, first / VALUES);
			if (page) std::memcpy(destination, page->values.data() + offset, n * sizeof(T));
			else std::fill_n(destination, n, fill_);
			first += n; count -= n; destination += n;
		}
	}
	template<class Visit> void each_block(Visit &&visit) const {
		std::unique_ptr<Page> uniform;
		for (size_t first = 0; first < size_;) {
			const size_t n = std::min(VALUES, size_ - first);
			const Page *page = page_at(root_.get(), level_, first / VALUES);
			if (!page && !uniform) uniform = std::make_unique<Page>(fill_);
			visit(page ? page->values.data() : uniform->values.data(), n);
			first += n;
		}
	}
private:
	static Snapshot window(const Snapshot &source, size_t first, size_t width, size_t rows, size_t stride) {
		if (!width || !rows) return {};
		assert(stride && width <= stride && first <= source.size_ && width <= source.size_ - first);
		assert(rows - 1 <= (source.size_ - first - width) / stride);
		if (!source.root_ || (first == 0 && width == stride && source.size_ % stride == 0 && rows == source.size_ / stride)) return source;
		PagedArray subset;
		subset.size_ = source.size_; subset.level_ = source.level_; subset.fill_ = source.fill_;
		for (size_t row = 0; row < rows; ++row) {
			const size_t start = first + row * stride, last = (start + width - 1) / VALUES;
			for (size_t page = start / VALUES; page <= last; ++page) {
				const auto *value = page_ref(source.root_.get(), source.level_, page);
				if (value && *value) subset.page_slot(page) = *value;
			}
		}
		return subset.snapshot();
	}
	std::shared_ptr<Node> root_;
	size_t size_ = 0;
	unsigned level_ = 0;
	T fill_{};
	static void fork(std::shared_ptr<Node> &node) {
		if (!node) node = std::make_shared<Node>();
		else if (node.use_count() != 1) node = std::make_shared<Node>(*node);
	}
	std::shared_ptr<Page> &page_slot(size_t page) {
		fork(root_);
		Node *node = root_.get();
		for (unsigned level = level_; level; --level) {
			auto &next = node->children[(page >> (level * BITS)) & MASK];
			fork(next); node = next.get();
		}
		return node->pages[page & MASK];
	}
	Page *writable_page(size_t page) {
		auto &data = page_slot(page);
		if (!data) data = std::make_shared<Page>(fill_);
		else if (data.use_count() != 1) data = std::make_shared<Page>(*data);
		return data.get();
	}
};
} // namespace city
