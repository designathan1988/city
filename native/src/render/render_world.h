// Everything drawn from CityState. Consumes the ChangeLog (the only dirty channel) on the main thread under the shared
// map lock and the log's own mutex (never the map exclusively: that would wait for every mesh job reading the map).
// Road, furniture and building chunks are streamed around the camera (render.road_end_m,
// render.props_end_m, render.building_far_end_m): built nearest-first within a per-frame budget, freed when the camera leaves, so memory
// follows the view and not the size of the map.
#pragma once

#include "render/agents/agent_renderer.h"
#include "render/buildings/body_stage.h"
#include "render/buildings/building_renderer.h"
#include "render/chunk_queue.h"
#include "render/overlays/overlay_renderer.h"
#include "render/wear/wear_field.h"
#include "render/props/prop_renderer.h"
#include "render/render_context.h"
#include "render/roads/road_renderer.h"
#include "render/terrain_bridge.h"
#include "render/trees/tree_renderer.h"

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
		CommitTimes building; // inside the building commit
	};

	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset(); // frees every instance (new city / load)
	void shutdown() { wear_.free(); } // the core is going: what lives as long as the render does
	void full_rebuild(CityState &s, godot::Object *terrain3d); // after a new map / load (caller holds the map shared)
	void sync(CityState &s, godot::Object *terrain3d, const godot::Vector3 &cam);
	// Vehicles and people near the camera, at the moment `at` of the published poses.
	void update_agents(const PoseFrame &f, const godot::Vector3 &cam, const PoseSample &at) { agents_.update(f, cam, at); }
	// What the agents of a fresh publication wear (`ticks_s` simulated seconds since the last one), and the window round the view.
	void update_wear(const PoseFrame &f, bool fresh, float ticks_s, const godot::Vector3 &cam, float frame_s) {
		if (fresh) wear_.wear(f, ticks_s);
		wear_.follow(cam.x, cam.z);
		wear_.upload(frame_s);
	}
	// The signal posts' lit lenses, from the lights the frame published.
	void update_signals(const PoseFrame &f) { props_.update_signals(f); }
	int lit_lenses() const { return props_.lit_lenses(); }
	int64_t worn_cells() const { return wear_.worn_cells(); }
	int64_t wear_publications() const { return wear_.publications(); }
	int64_t super_bytes_max() const { return buildings_.super_bytes_max(); }
	int drawn_vehicles() const { return agents_.drawn_vehicles(); }
	int drawn_people() const { return agents_.drawn_people(); }
	godot::Dictionary agent_cost() const { return agents_.cost(); }
	// A body drawn apart from the city (body_stage.h).
	int stage_show(const b40::Data &data, const b40::Body &body, const godot::RID &scenario, bool ghost) { return stage_.show(data, buildings_.look(), body, scenario, ghost); }
	BodyStage &stage() { return stage_; }
	const BodyStage &stage() const { return stage_; }
	void set_overlay(Overlay o, bool on) { overlays_.set(o, on); }
	bool overlay_on(Overlay o) const { return overlays_.is_on(o); }
	void update_overlays(const CityState &s, const PoseFrame &f, double dt) { overlays_.update(s, f, dt); }
	const Cost &cost() const { return cost_; }
	int pending() const {
		return road_stream_.pending + props_stream_.pending + building_stream_.pending + roads_.pending() + props_.pending() +
				buildings_.pending() + trees_.pending();
	}

private:
	TerrainBridge terrain_;
	RoadRenderer roads_;
	PropRenderer props_;
	TreeRenderer trees_;
	BuildingRenderer buildings_;
	BodyStage stage_;
	AgentRenderer agents_;
	OverlayRenderer overlays_;
	WearField wear_;
	ChunkStream road_stream_, props_stream_, building_stream_;
	int budget_ = 1;
	// The ChangeLog's render lists are swapped into these each frame (capacity kept on both sides).
	std::vector<int64_t> road_keys_, building_keys_;
	std::vector<int> terrain_rects_;
	Cost cost_;
	void drain_log(CityState &s);
};

} // namespace city
