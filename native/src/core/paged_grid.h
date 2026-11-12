// Spatial scalar pages over the persistent radix storage. A regional snapshot retains only intersecting tiles;
// a narrow footprint cannot pin the distant remainder of every row. Public indices/copies stay row-major.
#pragma once
#include "core/paged_array.h"

namespace city {
template<class T, int SIDE = 64> class PagedGrid {
	static_assert(SIDE > 0);
	static constexpr size_t VALUES = size_t(SIDE) * SIDE;
	using Storage = PagedArray<T, VALUES * sizeof(T)>;
	static size_t location(int x, int z, int across) {
		return (size_t(z / SIDE) * size_t(across) + size_t(x / SIDE)) * VALUES + size_t(z % SIDE) * SIDE + size_t(x % SIDE);
	}
public:
	class Snapshot;
	class Reader {
	public:
		T at(int x, int z) {
			assert(x >= 0 && z >= 0 && x < side_ && z < side_);
			return values_[location(x, z, across_)];
		}
		T operator[](size_t index) { return at(int(index % size_t(side_)), int(index / size_t(side_))); }
	private:
		friend class PagedGrid;
		friend class Snapshot;
		typename Storage::Reader values_;
		int side_ = 0, across_ = 0;
		Reader(typename Storage::Reader values, int side, int across) : values_(std::move(values)), side_(side), across_(across) {}
	};
	class Snapshot {
	public:
		Snapshot() = default;
		Snapshot(const Snapshot &) = default;
		Snapshot &operator=(const Snapshot &) = default;
		Snapshot(Snapshot &&other) noexcept : values_(std::move(other.values_)), side_(std::exchange(other.side_, 0)), across_(std::exchange(other.across_, 0)) {}
		Snapshot &operator=(Snapshot &&other) noexcept {
			if (this != &other) { values_ = std::move(other.values_); side_ = std::exchange(other.side_, 0); across_ = std::exchange(other.across_, 0); }
			return *this;
		}
		size_t size() const { return size_t(side_) * size_t(side_); }
		bool empty() const { return side_ == 0; }
		Reader reader() const & { return Reader(values_.reader(), side_, across_); }
		Reader reader() && = delete;
		Reader reader() const && = delete;
		T operator[](size_t index) const { auto values = reader(); return values[index]; }
		// Only this rectangle is readable afterwards; HeightPatch owns/clamps the corresponding window metadata.
		Snapshot window(int x, int z, int width, int height) const {
			assert(x >= 0 && z >= 0 && width > 0 && height > 0 && width <= side_ - x && height <= side_ - z);
			const int left = x / SIDE, top = z / SIDE, right = (x + width - 1) / SIDE, bottom = (z + height - 1) / SIDE;
			const size_t first = (size_t(top) * size_t(across_) + size_t(left)) * VALUES;
			return Snapshot(values_.window_rows(first, size_t(right - left + 1) * VALUES, size_t(bottom - top + 1), size_t(across_) * VALUES), side_, across_);
		}
	private:
		friend class PagedGrid;
		typename Storage::Snapshot values_;
		int side_ = 0, across_ = 0;
		Snapshot(typename Storage::Snapshot values, int side, int across) : values_(std::move(values)), side_(side), across_(across) {}
	};
	PagedGrid() = default;
	PagedGrid(const PagedGrid &) = default;
	PagedGrid &operator=(const PagedGrid &) = default;
	PagedGrid(PagedGrid &&other) noexcept : values_(std::move(other.values_)), side_(std::exchange(other.side_, 0)), across_(std::exchange(other.across_, 0)) {}
	PagedGrid &operator=(PagedGrid &&other) noexcept {
		if (this != &other) { values_ = std::move(other.values_); side_ = std::exchange(other.side_, 0); across_ = std::exchange(other.across_, 0); }
		return *this;
	}
	size_t size() const { return size_t(side_) * size_t(side_); }
	bool empty() const { return side_ == 0; }
	Reader reader() const & { return Reader(values_.reader(), side_, across_); }
	Reader reader() && = delete;
	Reader reader() const && = delete;
	Snapshot snapshot() const { return Snapshot(values_.snapshot(), side_, across_); }
	T operator[](size_t index) const { auto values = reader(); return values[index]; }
	void assign(int side, T fill) {
		assert(side >= 0);
		side_ = side; across_ = side / SIDE + (side % SIDE != 0);
		values_.assign(size_t(across_) * size_t(across_) * VALUES, fill);
	}
	bool set(int x, int z, T value) {
		assert(x >= 0 && z >= 0 && x < side_ && z < side_);
		return values_.set(location(x, z, across_), value);
	}
	bool set(size_t index, T value) {
		assert(index < size());
		return set(int(index % size_t(side_)), int(index / size_t(side_)), value);
	}
	template<class Generate> void generate_range(size_t first, size_t count, Generate &&generate) {
		assert(first <= size() && count <= size() - first);
		while (count) {
			const int x = int(first % size_t(side_)), z = int(first / size_t(side_));
			const size_t n = std::min({count, size_t(SIDE - x % SIDE), size_t(side_ - x)}), storage = location(x, z, across_);
			values_.generate_range(storage, n, [&](size_t index) { return generate(first + (index - storage)); });
			first += n; count -= n;
		}
	}
	template<class Generate> void assign_generated(int side, Generate &&generate) {
		assign(side, T{});
		generate_range(0, size(), std::forward<Generate>(generate));
	}
	template<class Generate, class Parallel> void assign_generated_parallel(int side, Generate &&generate, Parallel &&parallel) {
		assign(side, T{});
		values_.assign_blocks_parallel(size_t(across_) * size_t(across_) * VALUES, [&](size_t first, size_t, T *out) {
			const size_t tile = first / VALUES;
			const int x0 = int(tile % size_t(across_)) * SIDE, z0 = int(tile / size_t(across_)) * SIDE;
			for (int z = 0; z < SIDE; ++z)
				for (int x = 0; x < SIDE; ++x)
					out[size_t(z) * SIDE + x] = x0 + x < side_ && z0 + z < side_ ? generate(size_t(z0 + z) * size_t(side_) + size_t(x0 + x)) : T{};
		}, std::forward<Parallel>(parallel));
	}
	void copy_to(T *destination, size_t first, size_t count) const {
		assert(first <= size() && count <= size() - first);
		while (count) {
			const int x = int(first % size_t(side_)), z = int(first / size_t(side_));
			const size_t n = std::min({count, size_t(SIDE - x % SIDE), size_t(side_ - x)});
			values_.copy_to(destination, location(x, z, across_), n);
			first += n; count -= n; destination += n;
		}
	}
	template<class Visit> void each_block(Visit &&visit) const {
		std::array<T, SIDE> row;
		for (int z = 0; z < side_; ++z)
			for (int x = 0; x < side_;) {
				const int n = std::min(SIDE - x % SIDE, side_ - x);
				values_.copy_to(row.data(), location(x, z, across_), size_t(n));
				visit(row.data(), size_t(n)); x += n;
			}
	}
private:
	Storage values_;
	int side_ = 0, across_ = 0;
};
} // namespace city
