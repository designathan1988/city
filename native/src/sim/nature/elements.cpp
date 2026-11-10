// The store of the elements (see elements.h).
#include "sim/nature/elements.h"

#include "core/checks.h"
#include "core/grid.h"

#include <algorithm>
#include <cmath>

namespace city {

void Elements::clear() {
	cells_.clear();
	count_ = 0;
}

void Elements::add(const ElementItem &e) {
	cells_[chunk_of(e.x, e.z)].push_back(e);
	count_++;
}

bool Elements::remove(const ElementItem &e) {
	auto it = cells_.find(chunk_of(e.x, e.z));
	if (it == cells_.end()) return false;
	auto at = std::find(it->second.begin(), it->second.end(), e);
	if (at == it->second.end()) return false;
	*at = it->second.back();
	it->second.pop_back();
	count_--;
	if (it->second.empty()) cells_.erase(it);
	return true;
}

Elements Elements::cut(float minx, float minz, float maxx, float maxz) const {
	Elements c;
	c.windowed_ = true;
	c.win_x0_ = int(std::floor(minx / CHUNK));
	c.win_z0_ = int(std::floor(minz / CHUNK));
	c.win_x1_ = int(std::floor(maxx / CHUNK));
	c.win_z1_ = int(std::floor(maxz / CHUNK));
	for_chunks(minx, minz, maxx, maxz, [&](int64_t key) {
		const auto it = cells_.find(key);
		if (it == cells_.end()) return;
		c.count_ += it->second.size();
		c.cells_.emplace(key, it->second);
	});
	return c;
}

const std::vector<ElementItem> *Elements::in_chunk(int64_t key) const {
	if (CHECKS && windowed_ && (key_x(key) < win_x0_ || key_y(key) < win_z0_ || key_x(key) > win_x1_ || key_y(key) > win_z1_))
		city_stop("the elements of a chunk past the box of the map a job copied");
	auto it = cells_.find(key);
	return it == cells_.end() ? nullptr : &it->second;
}

std::vector<int64_t> Elements::chunks_near(float x, float z, float r) const {
	std::vector<int64_t> out;
	for_chunks(x - r, z - r, x + r, z + r, [&](int64_t k) { out.push_back(k); });
	return out;
}

std::vector<ElementItem> Elements::all() const {
	std::vector<ElementItem> out;
	out.reserve(count_);
	for (const auto &kv : cells_) out.insert(out.end(), kv.second.begin(), kv.second.end());
	return out;
}

} // namespace city
