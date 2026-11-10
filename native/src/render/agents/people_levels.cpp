// Which slot a person is drawn in (people_renderer.h): a mesh level by the distance, the coarsest mesh casting nothing past the
// shadows' distance, the card where the whole figure stands under the card's pixels on the frame's lens; the hold at a slot's edge;
// and what the probes read of the slots.
#include "render/agents/people_renderer.h"

#include <algorithm>

namespace city {

int PeopleRenderer::level_at(float dist) const {
	int lod = std::min(floor_, asset_.lods - 1);
	while (lod < asset_.lods - 1 && dist > lod_m_[size_t(lod)]) lod++;
	return lod;
}

int PeopleRenderer::slot_at(float dist, float height) const {
	// a figure under the card's pixels on this frame's lens: the meshes' error is past a pixel there anyway (agents.json people.card)
	if (view_.ready() && view_.px_of(height, dist) < card_below_px_) return card_slot();
	return dist > blob_from_m_ ? shadowless() : level_at(dist);
}

// A walker keeps its slot until it is past the edge by the hold, or it flickers there.
int PeopleRenderer::held_level(uint64_t key, float dist, float height) {
	const auto at = [&](float d) { return slot_at(d, height); };
	const int now = at(dist), place = keeper_.draw_of(key), was = place < 0 ? now : place % slots();
	if (was == now) return now;
	return now > was ? (at(dist / (1 + level_hold_)) > was ? now : was) : (at(dist * (1 + level_hold_)) < was ? now : was);
}

float PeopleRenderer::tallest_m() const {
	float most = 0;
	for (const PersonVariation &pv : asset_.variations) most = std::max(most, pv.height);
	return most;
}

int PeopleRenderer::shown(int level) const {
	int n = 0;
	for (size_t v = 0; v < asset_.variations.size(); v++) n += bufs_[slot(v, level)].count();
	return n;
}

} // namespace city
