// Everything drawn from CityState. Consumes the ChangeLog (the only dirty channel) on the main thread under the shared
// map lock and the log's own mutex (never the map exclusively: that would wait for every mesh job reading the map).
// Road, furniture and building chunks are streamed around the camera (render.road_end_m,
// render.props_end_m, render.building_far_end_m): built nearest-first within a per-frame budget, freed when the camera leaves, so memory
// follows the view and not the size of the map.
#pragma once

#include <algorithm>
#include <optional>
#include <vector>

#include "render/agents/agent_renderer.h"
#include "render/transit/transit_renderer.h"
#include "render/atmosphere/canyon_field.h"
#include "render/atmosphere/fog_field.h"
#include "render/buildings/body_stage.h"
#include "render/buildings/building_renderer.h"
#include "render/chunk_queue.h"
#include "render/detail.h"
#include "render/frame_admission.h"
#include "render/effects/effect_world.h"
#include "render/overlays/overlay_renderer.h"
#include "render/wear/wear_field.h"
#include "render/props/prop_renderer.h"
#include "render/render_context.h"
#include "render/roads/far_road_renderer.h"
#include "render/roads/road_renderer.h"
#include "render/roads/road_layer.h"
#include "render/terrain_bridge.h"
#include "render/cover/cover_renderer.h"
#include "render/trees/tree_renderer.h"
#include "render/water/shore_field.h"
#include "render/water/water_renderer.h"

#include "core/laps.h"

namespace city {

class RenderWorld {
public:
	// Where the last sync spent the frame, in milliseconds (render_cost probe: a test tells a hitch's cause): waiting
	// for the shared map lock, the change log and chunk streams, the levels and trees chosen for the view, and each
	// renderer's commit (flush: the RenderingServer running what the jobs queued, before the commits; the slowest
	// building chunk and superchunk commit; roads, props and trees: their hand-overs in the frame's one order); and the MiB
	// every upload of the frame paid into its ledger (frame_admission.h UploadBudget).
	struct Cost {
		double lock = 0, stream = 0, levels = 0, flush = 0, roads = 0, props = 0, buildings = 0, trees = 0, uploads = 0;
		double chunk_max = 0, super_max = 0;
		bool backlog = false; // no chunk was asked for this frame: the baked results wait for the upload budget
		int budget_left = 0;
		CommitTimes building; // inside the building commit
	};

	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset(); // frees every instance (new city / load)
	// The core is going: what lives as long as the render does (the wear field, the effects' layers and flash light, made by init)
	void shutdown() {
		wear_.free();
		effects_.release();
	}
	void full_rebuild(CityState &s, godot::Object *terrain3d); // after a new map / load (caller holds the map shared)
	void sync(CityState &s, godot::Object *terrain3d, const godot::Vector3 &cam);
	// The frame's level-of-detail view (render/detail.h), set by CityCore before the frame's updates: every family reads its
	// levels from it.
	void set_view(const DetailView &v) { view_ = v; }
	const DetailView &view() const { return view_; }
	// Vehicles and people near the camera, at the moment `at` of the published poses.
	void update_agents(const PoseFrame &f, const godot::Vector3 &cam, const ViewCull &view, const PoseSample &at) {
		agents_.update(f, cam, view, at);
		transit_.update(f, cam, at);
	}
	int drawn_cars() const { return transit_.drawn_cars(); }
	void transit_xray(bool on) { transit_.set_xray(on); }
	void transit_preview(const CityState &s, const std::vector<V2> &points, TransitMode mode) { transit_.set_preview(s, points, mode); }
	// What the agents of a fresh publication wear (`ticks_s` simulated seconds since the last one), and the window round the view.
	// The tiles it sends are paid into the next sync's upload ledger.
	void update_wear(const PoseFrame &f, bool fresh, float ticks_s, const godot::Vector3 &cam, float frame_s) {
		if (fresh) wear_.wear(f, ticks_s);
		wear_.follow(cam.x, cam.z);
		carried_bytes_ += wear_.upload(frame_s);
	}
	// The signal posts' lit lenses, from the lights the frame published (`fresh`: a new publication).
	void update_signals(const PoseFrame &f, bool fresh) { props_.update_signals(f, fresh); }
	void update_lamps(const godot::Vector3 &eye, const ViewCull &view, float frame_s) {
		props_.update_lamps(eye, view, frame_s);
		agents_.shine_heads(frame_s);
	}
	void set_lamp_budget(int count, int casting, int heads) {
		props_.set_lamp_budget(count, casting);
		agents_.set_head_lights(heads);
	}
	void set_lamp_night(float night) {
		props_.set_lamp_night(night);
		agents_.set_night(night);
	}
	int lit_lamps() const { return props_.lit_lamps(); }
	int lit_heads() const { return agents_.lit_heads(); }
	godot::Vector2 lamps_hand_over() const { return props_.lamps_hand_over(); }
	float shake() const { return effects_.shake(); } // metres the blasts move the camera now
	float since_blast() const { return effects_.since_blast(); } // effect seconds since the last blast
	int ruin_pieces() const { return effects_.ruin_pieces(); }
	int ruin_pieces_flying() const { return effects_.ruin_pieces_flying(); }
	uint32_t ruin_digest(uint32_t seed) const { return effects_.ruin_digest(seed); }
	int bodies() const { return effects_.bodies(); }
	int bodies_moving() const { return effects_.bodies_moving(); }
	int effect_pieces() const { return effects_.pieces(); }
	int broken_buildings() const { return int(effects_.broken().size()); }
	int blood_marks() const { return effects_.blood_marks(); }
	bool effects_busy() const { return effects_.busy(); }
	int lit_lenses() const { return props_.lit_lenses(); }
	int street_instances() const { return props_.instances(); }
	int sign_cells() const { return props_.sign_cells(); }
	int sign_cells_waiting() const { return props_.sign_cells_waiting(); }
	int64_t sign_generation() const { return props_.sign_generation(); }
	godot::Array take_sign_cells() { return props_.take_sign_cells(); }
	godot::Array laid_sign_cells() const { return props_.laid_sign_cells(); }
	void set_pole_preview(const CityState &s, const std::vector<PreviewPole> &poles) {
		props_.set_pole_preview(s, poles);
	}
	int64_t worn_cells() const { return wear_.worn_cells(); }
	int64_t wear_publications() const { return wear_.publications(); }
	int64_t super_bytes_max() const { return buildings_.super_bytes_max(); }
	int drawn_vehicles() const { return agents_.drawn_vehicles(); }
	int drawn_people() const { return agents_.drawn_people(); }
	godot::Array people_clips() const { return agents_.people_clips(); }
	bool show_people_clip(const godot::String &name) { return agents_.show_people_clip(name); }
	godot::Dictionary agent_cost() const {
		godot::Dictionary d = agents_.cost();
		d["trees_near"] = trees_.near_trees();
		d["plants_near"] = trees_.near_plants();
		return d;
	}
	godot::Dictionary building_cost() const { return buildings_.building_cost(); }
	godot::Dictionary bay_census() { return buildings_.bay_census(); }
	void set_detail_scale(float scale) { buildings_.set_detail_scale(scale); }
	void set_vegetation(float share) { trees_.set_share(share); }
	void set_agent_detail(float cabin_m, int cabin_cap, int people_floor) { agents_.set_detail(cabin_m, cabin_cap, people_floor); }
	void set_upload_every(int frames) { upload_every_ = std::max(1, frames); }
	void set_lots_drawn(bool on) { buildings_.set_lots_drawn(on); }
	// A body drawn apart from the city (body_stage.h).
	int stage_show(const b40::Data &data, const b40::Body &body, const godot::RID &scenario, bool ghost) { return stage_.show(data, buildings_.look(), body, scenario, ghost); }
	BodyStage &stage() { return stage_; }
	const BodyStage &stage() const { return stage_; }
	void set_overlay(Overlay o, bool on) { overlays_.set(o, on); }
	void show_every_zone(bool on) { overlays_.show_every_zone(on); }
	bool overlay_on(Overlay o) const { return overlays_.is_on(o); }
	BuildingInteriorCost interior_cost(const CityState &s, int id, int level, int dir) const { return buildings_.interior_cost(s, id, level, dir); }
	// After sync, in the same frame's admission (its deadline, credits and ledger).
	void update_overlays(const CityState &s, const PoseFrame &f, double dt) {
		if (frame_) overlays_.update(s, f, dt, *frame_);
	}
	const Cost &cost() const { return cost_; }
	// The atmosphere's and the shore's fields: advanced by sync with the rest of the render, read by the sky and the terrain views.
	const FogField &fog() const { return fog_; }
	const CanyonField &canyon() const { return canyon_; }
	const ShoreField &shore() const { return shore_; }
	BuildingViewNeed building_need() const { return buildings_.view_need(); }
	godot::Dictionary building_chunk(float x, float z) const { return buildings_.chunk_view(x, z); }
	RoadChunks::Count road_count() const { return roads_.count(); }
	RoadChunks::Count verge_count() const { return verges_.count(); }
	void draw_verges(bool on) { verges_.draw(on); }
	int64_t wear_bytes() const { return wear_.bytes(); }
	void wear_paint(float x0, float z0, float x1, float z1, float value) { wear_.paint(x0, z0, x1, z1, value); }
	int pending() const {
		return road_stream_.pending + raised_stream_.pending + props_stream_.pending + building_stream_.pending + roads_.pending() + far_roads_.pending() + far_roads_.waiting_supers() + verges_.pending() +
				structures_.pending() + props_.pending() + buildings_.pending() + trees_.pending() + cover_.pending() + water_.pending();
	}

private:
	TerrainBridge terrain_;
	RoadRenderer roads_;
	FarRoadRenderer far_roads_;
	RoadLayer verges_{ "verge", false, JobLane::VERGES, emit_verge_chunk };
	RoadLayer structures_{ "transit", true, JobLane::STRUCTURES, emit_structure_chunk };
	PropRenderer props_;
	GroundKept ground_; // the chunks' ground the trees and the cover plant on: before them, so it outlives their jobs
	TreeRenderer trees_;
	CoverRenderer cover_;
	WaterRenderer water_;
	FogField fog_;
	CanyonField canyon_;
	ShoreField shore_;
	BuildingRenderer buildings_;
	BodyStage stage_;
	AgentRenderer agents_;
	TransitRenderer transit_;
	OverlayRenderer overlays_;
	EffectWorld effects_;
	WearField wear_;
	ChunkStream road_stream_, props_stream_, building_stream_;
	ChunkStream raised_stream_; // the chunks with a bridge, a viaduct or a tunnel: those stand out from afar, as far as the buildings
	// The ChangeLog's render lists are swapped into these each frame (capacity kept on both sides).
	std::vector<int64_t> road_keys_, building_keys_, view_keys_, prop_keys_;
	std::vector<int> terrain_rects_, paint_rects_;
	std::vector<int64_t> nature_keys_, water_keys_;
	Cost cost_;
	void drain_log(CityState &s);
	void sync_nature(); // the landscape's dirty chunks and the water
	void mark_changed(const CityState &s); // the chunks the drained edits touched, for every renderer that draws them
	void offer_streams(const CityState &s, const godot::Vector3 &cam, std::vector<FrameAdmission::Unit> &units);
	int64_t baking() const;
	int64_t waiting() const;
	int upload_every_ = 1, upload_frame_ = 0; // the Optimisations tab: uploads only every N frames
	// The frame's admission (frame_admission.h): made at the top of sync, used by the overlays after it; the next frame's ledger
	// starts with the debt it leaves.
	std::optional<FrameAdmission> frame_;
	int64_t upload_debt_ = 0;
	int64_t carried_bytes_ = 0; // uploads made between two syncs (the wear's tiles), paid into the next one's ledger
	std::vector<FrameAdmission::Unit> units_; // the frame's build units (capacity kept)
	std::vector<double> hand_ms_;
	DetailView view_; // this frame's (set_view)
};

} // namespace city
