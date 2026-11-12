#include "sim/citygen/lot_raster.h"
#include "sim/citygen/lot_gen.h"
#include "core/job_pool.h"
#include "core/retained_memory.h"
#include <cmath>
#include <cstring>
#include <limits>

namespace city {
namespace {
uint64_t bits(double v) { uint64_t out; static_assert(sizeof(out) == sizeof(v)); std::memcpy(&out, &v, sizeof out); return out; }
}
bool LotRasterCache::fill(uint8_t *grid, int width, double ax, double az, double res, double x, double z, double radius,
		const int box[4], uint8_t value, size_t limit) const {
	if constexpr (!std::numeric_limits<double>::is_iec559 || std::numeric_limits<double>::radix != 2 || std::numeric_limits<double>::digits < 53) return false;
	if (!limit || width <= 0 || (res != 1 && res != 2) || !std::isfinite(ax) || !std::isfinite(az) ||
		!std::isfinite(x) || !std::isfinite(z) || !std::isfinite(radius) || radius < 0) return false;
	// The source's centres are exact half/integer lattice values at game coordinates. Keep unknown lattices uncached.
	const double ox = ax / res, oz = az / res;
	if (ox != std::floor(ox) || oz != std::floor(oz) || ox < std::numeric_limits<int>::min() || ox > std::numeric_limits<int>::max() ||
		oz < std::numeric_limits<int>::min() || oz > std::numeric_limits<int>::max()) return false;
	if (box[0] > box[1] || box[2] > box[3]) return true;
	const double raw[4] = {std::floor((x - radius) / res), std::ceil((x + radius) / res), std::floor((z - radius) / res), std::ceil((z + radius) / res)};
	for (double bound : raw) if (!std::isfinite(bound) || bound <= std::numeric_limits<int>::min() || bound >= std::numeric_limits<int>::max()) return false;
	// Cache preparation never expands a very large disc beyond the caller's window-sized amount of row work.
	if (raw[3] - raw[2] + 1 > width || radius / res > width) return false;
	const size_t overhead = sizeof(Spans) + RetainedMemory::shared_control_bytes() + sizeof(Entry) + 2 * sizeof(Key) + 9 * sizeof(void *);
	if (limit < overhead || size_t(raw[3] - raw[2] + 1) > (limit - overhead) / sizeof(Row)) return false;
	const Key key{bits(x), bits(z), bits(radius), bits(res)};
	std::shared_ptr<const Spans> spans;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		const auto found = entries_.find(key);
		if (found != entries_.end()) { recent_.splice(recent_.end(), recent_, found->second.recent); spans = found->second.value; }
	}
	if (!spans) {
		auto made = std::make_shared<Spans>();
		made->rows.reserve(size_t(raw[3] - raw[2] + 1));
		const int bounds[4] = {int(raw[0]), int(raw[1]), int(raw[2]), int(raw[3])};
		disc_rows(0, 0, res, x, z, radius, bounds, [&](int row, int from, int to) { made->rows.push_back({row, from, to}); });
		const size_t bytes = overhead + made->rows.capacity() * sizeof(Row);
		spans = made;
		if (bytes <= limit && !JobPool::cancelled()) {
			std::vector<std::shared_ptr<const Spans>> released;
			{
				std::lock_guard<std::mutex> lock(mutex_);
				const auto found = entries_.find(key);
				if (found != entries_.end()) { recent_.splice(recent_.end(), recent_, found->second.recent); spans = found->second.value; }
				else {
					while (!recent_.empty() && retained_ > limit - bytes) {
						const auto old = entries_.find(recent_.front()); retained_ -= old->second.bytes;
						released.push_back(std::move(old->second.value)); entries_.erase(old); recent_.pop_front();
					}
					recent_.push_back(key); entries_.emplace(key, Entry{spans, bytes, std::prev(recent_.end())}); retained_ += bytes;
				}
			}
		}
	}
	const int64_t ix = int64_t(ox), iz = int64_t(oz);
	const int64_t first = iz + std::max(0, box[2]), last = iz + std::min(width - 1, box[3]);
	auto row = std::lower_bound(spans->rows.begin(), spans->rows.end(), first, [](const Row &r, int64_t at) { return r.at < at; });
	for (; row != spans->rows.end() && row->at <= last; ++row) {
		const int64_t from = std::max({int64_t(row->from) - ix, int64_t(box[0]), int64_t(0)});
		const int64_t to = std::min({int64_t(row->to) - ix, int64_t(box[1]), int64_t(width - 1)});
		if (from <= to) std::fill(grid + size_t(int64_t(row->at) - iz) * width + size_t(from), grid + size_t(int64_t(row->at) - iz) * width + size_t(to) + 1, value);
	}
	return true;
}
} // namespace city
