// How far each level of the building stream reaches from the eye: the ends the data gives (near, parts, the part bands, mid, far),
// the graphics tier's share of the near reach and the visibility ranges of the chunks' instances that follow from them.
#include "render/buildings/building_renderer.h"

#include "core/units.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float NEVER_M = 1e9f;
constexpr float PROJECTION_HEIGHT = 1080.f;
constexpr float VIEW_ANGLE_FALLBACK = 90.f, MIN_SCREEN_PX = 1e-3f;

void range(const MeshDraw &d, float begin, float end) {
	if (d.inst.is_valid())
		RenderingServer::get_singleton()->instance_geometry_set_visibility_range(d.inst, begin, end, 0, 0,
				RenderingServer::VISIBILITY_RANGE_FADE_DISABLED);
}
} // namespace

void BuildingRenderer::set_ranges(const CityState &s) {
	state_ = &s;
	float detail = s.catalog.tune.render_building_detail_end_m;
	parts_end_ = detail * look_.parts_share;
	near_end_ = detail * look_.near_share;
	mid_end_ = s.catalog.tune.render_building_mid_end_m;
	bay_end_ = s.catalog.tune.render_building_bay_end_m;
	far_end_ = s.catalog.tune.render_building_far_end_m;
	float fov = look_.far_fov_deg > 0 ? look_.far_fov_deg : VIEW_ANGLE_FALLBACK;
	float half_tan = std::tan(fov * PI_F / float(DEG_PER_TURN));
	for (int i = 0; i < BAND_COUNT; i++) {
		const b40::PartBand &pb = look_.part_bands[size_t(std::min(i, int(look_.part_bands.size()) - 1))];
		float px = std::max(pb.px_min, MIN_SCREEN_PX);
		band_end_[size_t(i)] = std::min(pb.thick_max * PROJECTION_HEIGHT / (2 * half_tan * px), far_end_);
	}
	for (int i = BAND_COUNT; i < BAND_DRAWS; i++) band_end_[size_t(i)] = band_end_[BAND_COUNT - 1];
}

// Chunk instances: near and mid hand over at the near end, once the near level stands: until then the mid level draws from
// the eye, or a camera that jumps in sees a hole. The mid level has no end of its own (the chunk is shown or left to the far
// mesh as a whole: show). The far instance keeps no range (its chunk_mask hides the detailed chunks).
void BuildingRenderer::ranges(Chunk &ch) const {
	// A chunk with a building cut open draws its near level at any distance and hides its mid level (the far mesh stands aside).
	for (int i = 0; i < SUB_COUNT; i++) {
		range(ch.near[size_t(i)], 0, ch.cut ? 0 : near_end_ * detail_scale_);
		range(ch.parts[size_t(i)], 0, parts_end_ * detail_scale_);
		range(ch.mid[size_t(i)], ch.cut ? NEVER_M : ch.built[LEVEL_NEAR] ? near_end_ * detail_scale_ : 0, 0);
	}
	for (ShadowTwin &d : ch.prop_casts) d.range(look_.cast_props_m);
	for (int b = 0; b < BAND_DRAWS; b++) ch.bands[size_t(b)].range(0, band_end_[size_t(b)] * detail_scale_);
	const float mid_from = ch.cut ? NEVER_M : ch.built[LEVEL_NEAR] ? near_end_ * detail_scale_ : 0;
	ch.mid_draws[MID_BAYS].range(mid_from, bay_end_);
	ch.mid_draws[MID_BOXES].range(mid_from, 0);
	for (MeshDraw &d : ch.flat) range(d, ch.cut ? NEVER_M : bay_end_, 0);
	ch.flags.range(0, ch.cut ? 0 : near_end_ * detail_scale_); // the masts' cloths, with the near level they fly from
}

float BuildingRenderer::near_reach() const { return near_end_ * detail_scale_ + CHUNK * REACH_PAD; }

// The graphics tier's reach of the near level and its parts: the ranges of the chunks in place change now, the stream
// follows at the next sync (near_reach).
void BuildingRenderer::set_detail_scale(float scale) {
	detail_scale_ = scale;
	for (auto &[key, ch] : chunks_) ranges(ch);
}

} // namespace city
