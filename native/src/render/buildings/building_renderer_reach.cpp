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

// The draws of one sub-chunk hand over to one another at a distance measured to its one box, with no margin on either side: with a margin and
// the fade disabled the engine keeps a state for each instance (docs: a hysteresis), and of two that hand over both could stand drawn, or neither.
void range(const RID &inst, float begin, float end) {
	if (inst.is_valid()) RenderingServer::get_singleton()->instance_geometry_set_visibility_range(inst, begin, end, 0, 0, RenderingServer::VISIBILITY_RANGE_FADE_DISABLED);
}
void range(const MeshDraw &d, float begin, float end) { range(d.inst, begin, end); }
} // namespace

void BuildingRenderer::set_ranges(const CityState &s) {
	state_ = &s;
	float detail = s.catalog.tune.render_building_detail_end_m;
	parts_end_ = detail * look_.parts_share;
	near_end_ = detail * look_.near_share;
	mid_end_ = s.catalog.tune.render_building_mid_end_m;
	bay_end_ = s.catalog.tune.render_building_bay_end_m;
	far_end_ = s.catalog.tune.render_building_far_end_m;
	for (int i = 0; i < BAND_COUNT; i++) {
		const b40::PartBand &pb = look_.part_bands[size_t(std::min(i, int(look_.part_bands.size()) - 1))];
		band_end_[size_t(i)] = std::min(pb.thick_max * px_per_m() / std::max(pb.px_min, MIN_SCREEN_PX), far_end_);
	}
	for (int i = BAND_COUNT; i < BAND_DRAWS; i++) band_end_[size_t(i)] = band_end_[BAND_COUNT - 1];
}

float BuildingRenderer::px_per_m() const {
	const float fov = look_.far_fov_deg > 0 ? look_.far_fov_deg : VIEW_ANGLE_FALLBACK;
	return PROJECTION_HEIGHT / (2 * std::tan(fov * PI_F / float(DEG_PER_TURN)));
}

int BuildingRenderer::band_of(float thick) const {
	int band = 0;
	while (band + 1 < BAND_COUNT && thick > look_.part_bands[size_t(std::min(band, int(look_.part_bands.size()) - 1))].thick_max) band++;
	return band;
}

float BuildingRenderer::part_reach(float thick) const {
	const b40::PartBand &pb = look_.part_bands[size_t(std::min(band_of(thick), int(look_.part_bands.size()) - 1))];
	return thick * px_per_m() / std::max(pb.px_min, MIN_SCREEN_PX);
}

// Read by the far level's jobs: from the tuning and the look, which stand as they are once the game is up. The width that reaches the
// mid level's end by its own band's px_min; the thinnest such, as the bands go.
float BuildingRenderer::far_part_least() const {
	const float mid_end = state_->catalog.tune.render_building_mid_end_m;
	for (const b40::PartBand &pb : look_.part_bands) {
		const float least = mid_end * std::max(pb.px_min, MIN_SCREEN_PX) / px_per_m();
		if (least <= pb.thick_max) return least;
	}
	return NEVER_M; // no band reaches that far: the far level holds no part
}

float BuildingRenderer::far_error_m() const {
	return state_->catalog.tune.render_detail_error_px * state_->catalog.tune.render_building_mid_end_m / px_per_m();
}

// Chunk instances: near and mid hand over at the near end, once the near level stands: until then the mid level draws from
// the eye, or a camera that jumps in sees a hole. They change at that one distance, whole, each in its true place: the two are never drawn
// together, so neither is stepped back nor thinned by noise. The mid level has no end of its own (the chunk is shown or left to the far mesh as a whole: show). The far instance keeps no range (its
// chunk_mask hides the detailed chunks).
void BuildingRenderer::ranges(Chunk &ch) const {
	// A chunk with a building cut open draws its near level at any distance and hides its mid level (the far mesh stands aside).
	for (int i = 0; i < SUB_COUNT; i++) {
		range(ch.near[size_t(i)], 0, ch.cut ? 0 : near_end_ * detail_scale_);
		range(ch.parts[size_t(i)], 0, parts_end_ * detail_scale_);
		range(ch.mid[size_t(i)], ch.cut ? NEVER_M : ch.built[LEVEL_NEAR] ? near_end_ * detail_scale_ : 0, 0);
	}
	for (size_t i = 0; i < SUB_COUNT; i++) {
		// a box throws its shadow as long as it stands; the balcony pieces, fine as they are, to lod.cast_props_m
		for (int k = 0; k < BAND_CASTS; k++) ch.prop_casts[i][size_t(k)].range(k < BAND_COUNT - 1 ? near_end_ * detail_scale_ : look_.cast_props_m);
		// the parts stand on the near shell: past it the mid level draws its own, or they would hang in the air
		for (size_t b = 0; b < BAND_DRAWS; b++) ch.bands[i][b].range(0, (ch.cut ? band_end_[b] : std::min(band_end_[b], near_end_)) * detail_scale_);
	}
	const float mid_from = ch.cut ? NEVER_M : ch.built[LEVEL_NEAR] ? near_end_ * detail_scale_ : 0; // where the near level ends
	for (auto &sub : ch.mid_draws) {
		range(sub[MID_BAYS].inst, mid_from, bay_end_);
		range(sub[MID_BOXES].inst, mid_from, band_end_[MID_SOLID_BAND - 1]); // the thin ones: as far as the widest of their band is drawn
		range(sub[MID_SOLIDS].inst, mid_from, 0);
	}
	for (const std::array<ShadowTwin, SUB_COUNT> *casts : { &ch.mid_casts, &ch.mid_hulls })
		for (const ShadowTwin &d : *casts)
		if (d.inst.is_valid())
			RenderingServer::get_singleton()->instance_geometry_set_visibility_range(d.inst, mid_from, 0, 0, 0, RenderingServer::VISIBILITY_RANGE_FADE_DISABLED);
	// The openings and the flat are the same cells of the same walls in one plane: exactly one of the two is drawn, by the same distance.
	// Until the chunk's openings stand (their own level, streamed by their reach) the flat draws from the mid level's start: no wall is bare.
	const float flat_from = ch.built[LEVEL_BAYS] ? bay_end_ : mid_from;
	for (MeshDraw &d : ch.flat) range(d, ch.cut ? NEVER_M : flat_from, 0);
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
