// Spatial preparation follows fog edits, never camera movement. Dabs retain chronological evaluation order.
#include "sim/nature/fog_paint.h"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace city {
Bounds2 FogPaint::bounds(const FogDab &d) {
	// Preserve the rasterizer's bucket admission, including boundary buckets. One extra bucket expands float division edges.
	return {(std::floor((d.x - d.radius) / BUCKET_M) - 1.0) * BUCKET_M,
		(std::floor((d.z - d.radius) / BUCKET_M) - 1.0) * BUCKET_M,
		(std::floor((d.x + d.radius) / BUCKET_M) + 2.0) * BUCKET_M,
		(std::floor((d.z + d.radius) / BUCKET_M) + 2.0) * BUCKET_M};
}
bool FogPaint::indexed(const Bounds2 &b) {
	return std::isfinite(b.minx) && std::isfinite(b.minz) && std::isfinite(b.maxx) && std::isfinite(b.maxz) &&
		b.minx <= b.maxx && b.minz <= b.maxz;
}
void FogPaint::push_back(const FogDab &dab) {
	const int id = int(dabs_.size());
	dabs_.push_back(dab);
	const Bounds2 box = bounds(dab);
	if (indexed(box)) index_.insert(id, box);
}
void FogPaint::pop_back() {
	assert(!dabs_.empty());
	const Bounds2 box = bounds(dabs_.back());
	if (indexed(box)) index_.erase(int(dabs_.size() - 1), box);
	dabs_.pop_back();
}
void FogPaint::clear() {
	std::vector<FogDab>().swap(dabs_);
	index_.clear();
}
std::vector<FogDab> FogPaint::take() {
	std::vector<FogDab> result;
	result.swap(dabs_);
	index_.clear();
	return result;
}
void FogPaint::restore(std::vector<FogDab> dabs) {
	clear();
	dabs_ = std::move(dabs);
	for (size_t i = 0; i < dabs_.size(); i++) {
		const Bounds2 box = bounds(dabs_[i]);
		if (indexed(box)) index_.insert(int(i), box);
	}
}
std::vector<FogDab> FogPaint::window(float x0, float z0, float x1, float z1) const {
	std::vector<int> ids;
	index_.query({x0, z0, x1, z1}, [&](int id) { ids.push_back(id); });
	std::sort(ids.begin(), ids.end());
	std::vector<FogDab> result;
	result.reserve(ids.size());
	for (int id : ids) result.push_back(dabs_[size_t(id)]);
	return result;
}
} // namespace city
