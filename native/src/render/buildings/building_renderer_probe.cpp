// What the tests read of the building levels: what the view still asks and how a chunk stands.
#include "render/buildings/building_renderer.h"

#include "core/grid.h"

namespace city {

int BuildingRenderer::pending() const {
	std::lock_guard<std::mutex> lk(queue_mutex_);
	return need_.unmet + int(requested_.size() + done_.size() + super_requested_.size() + super_done_.size());
}

BuildingRenderer::ViewNeed BuildingRenderer::view_need() const {
	ViewNeed n = need_;
	std::lock_guard<std::mutex> lk(queue_mutex_);
	n.requested = int(requested_.size() + super_requested_.size());
	n.done = int(done_.size() + super_done_.size());
	return n;
}

int BuildingRenderer::chunk_flags(float x, float z) const {
	const int64_t key = chunk_of(x, z);
	auto it = chunks_.find(key);
	if (it == chunks_.end()) return 0;
	const Chunk &ch = it->second;
	constexpr int NEAR_BIT = 1, MID_BIT = 2, SHOWN_BIT = 4, DETAIL_BIT = 8, KNOWN_BIT = 16, FAR_BIT = 32;
	int flags = KNOWN_BIT | (ch.built[LEVEL_NEAR] ? NEAR_BIT : 0) | (ch.built[LEVEL_MID] ? MID_BIT : 0) | (ch.shown ? SHOWN_BIT : 0) | (ch.detail ? DETAIL_BIT : 0);
	auto sp = supers_.find(chunk_of(ch.centre.x, ch.centre.z, SUPERCHUNK));
	if (sp != supers_.end() && sp->second.far.inst.is_valid()) flags |= FAR_BIT;
	return flags;
}

} // namespace city
