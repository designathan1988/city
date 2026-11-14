// The building stream's view of each frame (building_renderer.h): every superchunk's chunks switched between their own detail and the
// far mesh, the levels each one needs or no longer needs from the eye, and the jobs asked in the frame's budget, nearest first.
#include "render/buildings/building_renderer.h"

#include "render/buildings/building_stream.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

using namespace building_stream;

namespace {
constexpr float HALF = 0.5f, FREED_PAST = 1.25f; // a level is freed past its reach times this

Vector3 super_centre(int64_t key) {
	return Vector3((float(key_x(key)) + HALF) * SUPERCHUNK, 0, (float(key_y(key)) + HALF) * SUPERCHUNK);
}

float horizontal(const Vector3 &cam, const Vector3 &at) {
	float dx = cam.x - at.x, dz = cam.z - at.z;
	return std::sqrt(dx * dx + dz * dz);
}

// fn(key, chunk) for every streamed chunk of a superchunk.
template <typename Map, typename F>
void each_chunk_of(Map &chunks, int64_t super_key, int per_axis, F &&fn) {
	const int sx = key_x(super_key) * per_axis, sz = key_y(super_key) * per_axis;
	for (int j = 0; j < per_axis; j++)
		for (int i = 0; i < per_axis; i++) {
			auto it = chunks.find(cell_key(sx + i, sz + j));
			if (it != chunks.end()) fn(it->first, it->second);
		}
}
} // namespace

// A superchunk, every frame: each chunk's switch between its own detail and the far mesh, the levels it needs or no
// longer needs, and the far mesh's mask of the chunks drawn in detail (never a hole: a chunk leaves the far mesh only
// once its mid level is up, and shows itself whatever the distance while no far mesh stands in).
void BuildingRenderer::visit_super(int64_t key, Super &sp, const Vector3 &cam, float eye, std::vector<Want> &want) {
	const float leave = mid_end_ * (1 + look_.far_hysteresis), mid_reach = mid_end_ + CHUNK * REACH_PAD;
	const float bay_reach = bay_end_ + CHUNK * HALF; // a sub-chunk's range is measured to its own middle, up to half a chunk from the chunk's
	const bool far_missing = !sp.built || !sp.has_content;
	const int sx = key_x(key) * CHUNKS_PER_SUPER, sz = key_y(key) * CHUNKS_PER_SUPER;
	int mask = 0;
	each_chunk_of(chunks_, key, CHUNKS_PER_SUPER, [&](int64_t ck, Chunk &ch) {
		const float dc = horizontal(cam, ch.centre); // streaming of the levels: the ground distance
		const float d = std::sqrt(dc * dc + eye * eye); // the switch: with the eye height, as visibility ranges measure
		ch.detail = ch.cut || (ch.detail ? d < leave : d < mid_end_);
		if (ch.wear_due > 0 && bodies_.now_min() >= ch.wear_due) { // its buildings have aged since it was drawn: its levels in reach are drawn again
			ch.wear_due = 0;
			for (int l = 0; l < LEVELS; l++) ch.stale[l] = ch.built[l];
		}
		uint8_t levels = 0;
		if ((!ch.built[LEVEL_NEAR] || ch.stale[LEVEL_NEAR]) && (dc < near_reach() || ch.cut)) levels = uint8_t(levels | BAND_BIT_NEAR);
		else if (ch.built[LEVEL_NEAR] && !ch.cut && dc > near_reach() * FREED_PAST) drop(ch, LEVEL_NEAR);
		// a chunk drawn in detail keeps its mid level current even in the hysteresis band past mid_reach
		if ((!ch.built[LEVEL_MID] || ch.stale[LEVEL_MID]) && (dc < mid_reach || ch.detail)) {
			levels = uint8_t(levels | BAND_BIT_MID);
		} else if (ch.built[LEVEL_MID] && !ch.detail && dc > mid_reach * FREED_PAST) {
			drop(ch, LEVEL_MID);
			drop_bands(ch);
		}
		// the mid level's openings stand only where they are drawn: with a mid level, built or asked now, within their own reach
		const bool mid_up = ch.built[LEVEL_MID] || (levels & BAND_BIT_MID) != 0;
		if ((!ch.built[LEVEL_BAYS] || ch.stale[LEVEL_BAYS]) && mid_up && dc < bay_reach) levels = uint8_t(levels | BAND_BIT_BAYS);
		else if (ch.built[LEVEL_BAYS] && dc > bay_reach * FREED_PAST) drop(ch, LEVEL_BAYS);
		const bool drawn = ch.built[LEVEL_MID] && (ch.detail || far_missing);
		if (drawn != ch.shown) show(ch, drawn);
		if (drawn) mask |= 1 << ((key_y(ck) - sz) * CHUNKS_PER_SUPER + (key_x(ck) - sx));
		if (levels == 0) return;
		std::lock_guard<std::mutex> lk(queue_mutex_);
		if (!requested_.count(ck)) want.push_back({ dc, ck, levels });
	});
	sp.mask = mask;
	send_mask(sp);
}

void BuildingRenderer::update_view(const Vector3 &cam, BuildBudget &budget) {
	if (!state_) return;
	// RenderWorld::sync calls this under the shared map lock (the ground height is a map read)
	const float eye = std::max(0.f, cam.y - state_->terrain.height_at(cam.x, cam.z));
	bodies_.age_at(state_->clock.minutes.load(), state_->catalog.growth); // the minute every placement of this frame's jobs is aged at
	const float far_reach = far_end_ + eye * look_.far_per_eye; // from up there the ground in view runs on past the data's end
	const double now = now_s();
	std::vector<Want> want, want_super;
	int waiting = 0; // far meshes to redo, each on its clock
	for (auto &[key, sp] : supers_) {
		// the far reach from the superchunk's centre, as the chunks are streamed (a nearest-point reach would hold a
		// ring of extra far meshes, a quarter more memory, for buildings past building_far_end_m)
		const float ground = horizontal(cam, super_centre(key));
		if (ground > far_reach * FREED_PAST && sp.far.inst.is_valid()) drop_far(key, sp);
		visit_super(key, sp, cam, eye, want);
		// the first far mesh at once (nothing stands in yet); a rebuild of a built one on its clock
		const bool due = !sp.built || now >= std::max(sp.due, sp.asked + look_.far_rebuild_interval_s);
		if (sp.dirty && ground <= far_reach) {
			std::lock_guard<std::mutex> lk(queue_mutex_);
			if (!super_requested_.count(key)) {
				if (due) want_super.push_back({ ground, key, 0 });
				else waiting++;
			}
		}
	}
	need_.want = int(want.size() + want_super.size()) + waiting;
	need_.unmet = need_.want;
	std::sort(want.begin(), want.end(), [](const Want &a, const Want &b) { return a.d < b.d; });
	for (const Want &w : want) {
		if (!budget.take()) break;
		submit(*state_, w.key, (w.mask & BAND_BIT_NEAR) != 0, (w.mask & BAND_BIT_MID) != 0, (w.mask & BAND_BIT_BAYS) != 0, w.d);
		need_.unmet--;
	}
	std::sort(want_super.begin(), want_super.end(), [](const Want &a, const Want &b) { return a.d < b.d; });
	for (const Want &w : want_super) {
		if (!budget.take()) break;
		submit_super(*state_, w.key, w.d);
		need_.unmet--;
	}
}

} // namespace city
