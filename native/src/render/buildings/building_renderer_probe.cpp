// What the tests read of the building levels: what the view still asks and how a chunk stands.
#include "render/buildings/building_renderer.h"

#include "core/grid.h"

using namespace godot;

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

Dictionary BuildingRenderer::building_cost() const {
	static const char *const NAMES[HELD_KINDS] = { "near_walls", "near_panes", "near_guards", "near_cards", "near_pieces", "mid_walls", "mid_panes", "mid_cards" };
	std::array<Held, HELD_KINDS> sum;
	Held far;
	int64_t near_chunks = 0, mid_chunks = 0, instances = 0, props = 0;
	for (const auto &[key, ch] : chunks_) {
		near_chunks += ch.built[LEVEL_NEAR] ? 1 : 0;
		mid_chunks += ch.built[LEVEL_MID] ? 1 : 0;
		instances += ch.built[LEVEL_NEAR] ? ch.instances : 0;
		props += ch.built[LEVEL_NEAR] ? ch.props : 0;
		for (int k = 0; k < HELD_KINDS; k++) {
			if (!ch.built[k < HELD_MID_WALLS ? LEVEL_NEAR : LEVEL_MID]) continue;
			sum[size_t(k)].triangles += ch.held[size_t(k)].triangles;
			sum[size_t(k)].bytes += ch.held[size_t(k)].bytes;
		}
	}
	for (const auto &[key, sp] : supers_) {
		far.triangles += sp.held.triangles;
		far.bytes += sp.held.bytes;
	}
	Dictionary d;
	auto put_held = [&d](const char *name, const Held &h) {
		Dictionary one;
		one["triangles"] = h.triangles;
		one["bytes"] = h.bytes;
		d[name] = one;
	};
	for (int k = 0; k < HELD_KINDS; k++) put_held(NAMES[k], sum[size_t(k)]);
	put_held("far", far);
	d["near_chunks"] = near_chunks;
	d["mid_chunks"] = mid_chunks;
	d["instances"] = instances;
	d["props"] = props;
	return d;
}

} // namespace city
