// Everything drawn from CityState. Consumes the ChangeLog (the only dirty channel) on the main thread under the shared
// map lock and the log's own mutex (never the map exclusively: that would wait for every mesh job reading the map).
// Road, furniture and building chunks are streamed around the camera (render.road_end_m,
// render.props_end_m, render.building_far_end_m): built nearest-first within a per-frame budget, freed when the camera leaves, so memory
// follows the view and not the size of the map.
#pragma once

#include <algorithm>

#include "render/agents/agent_renderer.h"
#include "render/transit/transit_renderer.h"
#include "render/atmosphere/canyon_field.h"
#include "render/atmosphere/fog_field.h"
#include "render/buildings/body_stage.h"
#include "render/buildings/building_renderer.h"
#include "render/chunk_queue.h"
#include "render/effects/effect_world.h"
#include "render/overlays/overlay_renderer.h"
#include "render/wear/wear_field.h"
#include "render/props/prop_renderer.h"
#include "render/render_context.h"
#include "render/roads/far_road_renderer.h"
#include "render/roads/road_renderer.h"
#include "render/roads/structure_renderer.h"
#include "render/roads/verge_renderer.h"
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
	// building chunk and superchunk commit); and the MiB of mesh arrays it admitted for upload (UploadBudget).
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
	// Vehicles and people near the camera, at the moment `at` of the published poses.
	void update_agents(const PoseFrame &f, const godot::Vector3 &cam, const ViewCull &view, const PoseSample &at) {
		agents_.update(f, cam, view, at);
		transit_.update(f, cam, at);
	}
	int drawn_cars() const { return transit_.drawn_cars(); }
	void transit_xray(bool on) { transit_.set_xray(on); }
	void transit_preview(const CityState &s, const std::vector<V2> &points, TransitMode mode) { transit_.set_preview(s, points, mode); }
	// What the agents of a fresh publication wear (`ticks_s` simulated seconds since the last one), and the window round the view.
	void update_wear(const PoseFrame &f, bool fresh, float ticks_s, const godot::Vector3 &cam, float frame_s) {
		if (fresh) wear_.wear(f, ticks_s);
		wear_.follow(cam.x, cam.z);
		wear_.upload(frame_s);
	}
	// The signal posts' lit lenses, from the lights the frame published.
	void update_signals(const PoseFrame &f) { props_.update_signals(f); }
	float shake() const { return effects_.shake(); } // metres the blasts move the camera now
	float since_blast() const { return effects_.since_blast(); } // effect seconds since the last blast
	int effect_pieces() const { return effects_.pieces(); }
	int blood_marks() const { return effects_.blood_marks(); }
	bool effects_busy() const { return effects_.busy(); }
	int lit_lenses() const { return props_.lit_lenses(); }
	int street_instances() const { return props_.instances(); }
	int sign_cells() const { return props_.sign_cells(); }
	godot::Array take_sign_cells() { return props_.take_sign_cells(); }
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
	BuildingRenderer::InteriorCost interior_cost(const CityState &s, int id, int level, int dir) const { return buildings_.interior_cost(s, id, level, dir); }
	void update_overlays(const CityState &s, const PoseFrame &f, double dt) { overlays_.update(s, f, dt); }
	const Cost &cost() const { return cost_; }
	// The atmosphere's and the shore's fields: advanced by sync with the rest of the render, read by the sky and the terrain views.
	const FogField &fog() const { return fog_; }
	const CanyonField &canyon() const { return canyon_; }
	const ShoreField &shore() const { return shore_; }
	BuildingRenderer::ViewNeed building_need() const { return buildings_.view_need(); }
	int building_chunk(float x, float z) const { return buildings_.chunk_flags(x, z); }
	RoadChunks::Count road_count() const { return roads_.count(); }
	RoadChunks::Count verge_count() const { return verges_.count(); }
	void draw_verges(bool on) { verges_.draw(on); }
	int64_t wear_bytes() const { return wear_.bytes(); }
	void wear_paint(float x0, float z0, float x1, float z1, float value) { wear_.paint(x0, z0, x1, z1, value); }
	int pending() const {
		return road_stream_.pending + raised_stream_.pending + props_stream_.pending + building_stream_.pending + roads_.pending() + far_roads_.pending() + far_roads_.waiting_supers() + verges_.pending() + props_.pending() +
				buildings_.pending() + trees_.pending() + cover_.pending();
	}

private:
	TerrainBridge terrain_;
	RoadRenderer roads_;
	FarRoadRenderer far_roads_;
	VergeRenderer verges_;
	StructureRenderer structures_;
	PropRenderer props_;
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
	int budget_ = 1;
	// The ChangeLog's render lists are swapped into these each frame (capacity kept on both sides).
	std::vector<int64_t> road_keys_, building_keys_, view_keys_, prop_keys_;
	std::vector<int> terrain_rects_, paint_rects_;
	std::vector<int64_t> nature_keys_, water_keys_;
	Cost cost_;
	void drain_log(CityState &s);
	void sync_nature(CityState &s); // the landscape's dirty chunks and the water
	void mark_changed(const CityState &s); // the chunks the drained edits touched, for every renderer that draws them
	int upload_every_ = 1, upload_frame_ = 0; // the Optimisations tab: uploads only every N frames
};

} // namespace city
