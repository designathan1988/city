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
const char *const KEEP_PARAM = "level_keep", *const BACK_PARAM = "level_back"; // building_level.gdshaderinc

// Compatibility has no dissolve (building_level.gdshaderinc). A value set on an instance takes a place in that renderer's few, whether its shader declares it or not
// (Godot instance_uniforms.cpp materials_finish).
void put_param(const RID &inst, const char *name, float value) {
	static const bool has_them = RenderingServer::get_singleton()->get_current_rendering_method() != "gl_compatibility";
	if (has_them && inst.is_valid()) RenderingServer::get_singleton()->instance_geometry_set_shader_parameter(inst, name, value);
}

void range(const MeshDraw &d, float begin, float end, float begin_margin = 0) {
	if (d.inst.is_valid())
		RenderingServer::get_singleton()->instance_geometry_set_visibility_range(d.inst, begin, end, begin_margin, 0,
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
		range(ch.mid[size_t(i)], ch.cut ? NEVER_M : ch.built[LEVEL_NEAR] ? near_end_ * detail_scale_ - look_.level_fade_m : 0, 0);
	}
	for (size_t i = 0; i < SUB_COUNT; i++) {
		// a box throws its shadow as long as it stands; the balcony pieces, fine as they are, to lod.cast_props_m
		for (int k = 0; k < BAND_CASTS; k++) ch.prop_casts[i][size_t(k)].range(k < BAND_COUNT - 1 ? near_end_ * detail_scale_ : look_.cast_props_m);
		// the parts stand on the near shell: past it the mid level draws its own, or they would hang in the air
		for (size_t b = 0; b < BAND_DRAWS; b++) ch.bands[i][b].range(0, (ch.cut ? band_end_[b] : std::min(band_end_[b], near_end_)) * detail_scale_);
	}
	// the mid level stands up a band before the near one ends: the near one dissolves over it (fade_levels)
	const float mid_from = ch.cut ? NEVER_M : ch.built[LEVEL_NEAR] ? near_end_ * detail_scale_ - look_.level_fade_m : 0;
	for (auto &sub : ch.mid_draws) {
		sub[MID_BAYS].range(mid_from, bay_end_);
		sub[MID_BOXES].range(mid_from, 0);
		sub[MID_SOLIDS].range(mid_from, 0);
		for (const MultiMeshDraw &d : sub) put_param(d.inst, BACK_PARAM, look_.level_back_m);
	}
	for (const MeshDraw &d : ch.mid) put_param(d.inst, BACK_PARAM, look_.level_back_m);
	for (const std::array<ShadowTwin, SUB_COUNT> *casts : { &ch.mid_casts, &ch.mid_hulls })
		for (const ShadowTwin &d : *casts)
		if (d.inst.is_valid())
			RenderingServer::get_singleton()->instance_geometry_set_visibility_range(d.inst, mid_from, 0, 0, 0, RenderingServer::VISIBILITY_RANGE_FADE_DISABLED);
	// Where the bays end, by the margin of theirs: the engine's hysteresis moves each edge by the way the eye comes, and with no margin here a
	// camera coming in lost the flat before the bays were back, and for those metres the plain side behind them stood bare.
	for (MeshDraw &d : ch.flat) range(d, ch.cut ? NEVER_M : bay_end_, 0, bay_end_ * RANGE_LINGER_SHARE);
	ch.flags.range(0, ch.cut ? 0 : near_end_ * detail_scale_); // the masts' cloths, with the near level they fly from
}

// The renderer compiles the pipelines of a surface when an instance takes it on, whether or not it is ever seen: the three twins are worn once here, on
// instances out of every range, so the first sub-chunk to enter a band finds them ready.
void BuildingRenderer::warm_fades(const Chunk &ch) {
	if (fade_warmers_[0].is_valid()) return;
	RenderingServer *rs = RenderingServer::get_singleton();
	for (size_t i = 0; i < SUB_COUNT; i++) {
		if (!ch.near[i].mesh.is_valid() || ch.wall_at[i] < 0 || ch.glass_at[i] < 0) continue;
		const RID walls = rs->instance_create2(ch.near[i].mesh, scenario_);
		rs->instance_set_surface_override_material(walls, ch.wall_at[i], material_fade_);
		rs->instance_set_surface_override_material(walls, ch.glass_at[i], material_glass_fade_);
		fade_warmers_[0] = walls;
		for (const MultiMeshDraw &d : ch.bands[i])
			if (d.mm.is_valid() && !fade_warmers_[1].is_valid()) {
				fade_warmers_[1] = rs->instance_create2(d.mm, scenario_);
				rs->instance_geometry_set_material_override(fade_warmers_[1], material_parts_fade_);
			}
		if (ch.parts[i].mesh.is_valid()) {
			fade_warmers_[2] = rs->instance_create2(ch.parts[i].mesh, scenario_);
			rs->instance_geometry_set_material_override(fade_warmers_[2], material_fade_);
		}
		for (const RID &w : fade_warmers_)
			if (w.is_valid()) rs->instance_geometry_set_visibility_range(w, NEVER_M, 0, 0, 0, RenderingServer::VISIBILITY_RANGE_FADE_DISABLED);
		return;
	}
}

void BuildingRenderer::fade_near(Chunk &ch, size_t sub, bool on, float keep) {
	RenderingServer *rs = RenderingServer::get_singleton();
	const RID inst = ch.near[sub].inst;
	const float end = near_end_ * detail_scale_;
	if (on != ch.fading[sub]) {
		ch.fading[sub] = on;
		if (inst.is_valid() && ch.wall_at[sub] >= 0) rs->instance_set_surface_override_material(inst, ch.wall_at[sub], on ? material_fade_ : RID());
		if (inst.is_valid() && ch.glass_at[sub] >= 0) rs->instance_set_surface_override_material(inst, ch.glass_at[sub], on ? material_glass_fade_ : RID());
		for (size_t b = 0; b < BAND_COUNT; b++)
			if (ch.bands[sub][b].inst.is_valid() && band_end_[b] * detail_scale_ >= end)
				rs->instance_geometry_set_material_override(ch.bands[sub][b].inst, on ? material_parts_fade_ : material_parts_);
	}
	put_param(inst, KEEP_PARAM, keep);
	for (size_t b = 0; b < BAND_COUNT; b++)
		if (band_end_[b] * detail_scale_ >= end) put_param(ch.bands[sub][b].inst, KEEP_PARAM, keep);
}

void BuildingRenderer::fade_parts(Chunk &ch, size_t sub, bool on, float keep) {
	const RID inst = ch.parts[sub].inst;
	if (on != ch.parts_fading[sub]) {
		ch.parts_fading[sub] = on;
		if (inst.is_valid()) RenderingServer::get_singleton()->instance_geometry_set_material_override(inst, on ? material_fade_ : material_);
	}
	put_param(inst, KEEP_PARAM, keep);
}

// Each sub-chunk by the distance its visibility range measures, to the middle of its box: inside a band it wears the twin that dissolves, and only
// then (a shader that discards loses the depth prepass). Nothing is sent for a sub-chunk that stays out of the bands.
void BuildingRenderer::fade_levels(Chunk &ch, const Vector3 &cam) {
	const float band = std::max(look_.level_fade_m, MIN_SCREEN_PX);
	for (size_t i = 0; i < SUB_COUNT; i++) {
		const float d = cam.distance_to(ch.reach_box[i].get_center());
		const float keep = std::clamp((near_end_ * detail_scale_ - d) / band, 0.f, 1.f), parts = std::clamp((parts_end_ * detail_scale_ - d) / band, 0.f, 1.f);
		const bool in_band = keep > 0 && keep < 1, parts_in = parts > 0 && parts < 1;
		if (in_band || ch.fading[i]) fade_near(ch, i, in_band, in_band ? keep : 1.f);
		if (parts_in || ch.parts_fading[i]) fade_parts(ch, i, parts_in, parts_in ? parts : 1.f);
	}
}

float BuildingRenderer::near_reach() const { return near_end_ * detail_scale_ + CHUNK * REACH_PAD; }

// The graphics tier's reach of the near level and its parts: the ranges of the chunks in place change now, the stream
// follows at the next sync (near_reach).
void BuildingRenderer::set_detail_scale(float scale) {
	detail_scale_ = scale;
	for (auto &[key, ch] : chunks_) ranges(ch);
}

} // namespace city
