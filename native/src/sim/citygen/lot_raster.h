// Immutable circle row spans reused before clipping them to neighbouring lot windows.
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

namespace city {
class LotRasterCache {
public:
	// False selects the caller's unchanged fill_disc path (unsupported lattice or deliberately uncached footprint).
	bool fill(uint8_t *grid, int width, double ax, double az, double res, double x, double z, double radius,
		const int box[4], uint8_t value, size_t limit) const;
private:
	using Key = std::array<uint64_t, 4>; // exact x/z/radius/res object bits, not a geometry digest
	struct Row { int at, from, to; };
	struct Spans { std::vector<Row> rows; };
	struct Entry { std::shared_ptr<const Spans> value; size_t bytes; std::list<Key>::iterator recent; };
	mutable std::mutex mutex_;
	mutable std::map<Key, Entry> entries_;
	mutable std::list<Key> recent_;
	mutable size_t retained_ = 0;
};
} // namespace city
