// Everything drawn from CityState. Consumes the ChangeLog (the only dirty channel) on the main thread while the
// caller holds world_mutex. Road, furniture and building chunks are streamed around the camera (render.road_end_m,
// render.props_end_m, render.building_far_end_m): built nearest-first within a per-frame budget, freed when the camera leaves, so memory
// follows the view and not the size of the map.
#pragma once

#include "render/agents/agent_renderer.h"
#include "render/buildings/building_renderer.h"
#include "render/chunk_queue.h"
#include "render/overlays/overlay_renderer.h"
#include "render/props/prop_renderer.h"
#include "render/render_context.h"
#include "render/roads/road_renderer.h"
#include "render/terrain_bridge.h"
#include "render/trees/tree_renderer.h"

namespace city {

class RenderWorld {
public:
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset(); // frees every instance (new city / load)
	void full_rebuild(CityState &s, godot::Object *terrain3d); // after a new map / load
	void sync(CityState &s, godot::Object *terrain3d, const godot::Vector3 &cam);
	// Vehicles and people near the camera; alpha = share of the sim tick elapsed (interpolation).
	void update_agents(const PoseFrame &f, const godot::Vector3 &cam, float alpha) { agents_.update(f, cam, alpha); }
	int drawn_vehicles() const { return agents_.drawn_vehicles(); }
	int drawn_people() const { return agents_.drawn_people(); }
	void set_overlay(Overlay o, bool on) { overlays_.set(o, on); }
	bool overlay_on(Overlay o) const { return overlays_.is_on(o); }
	void update_overlays(const CityState &s, const PoseFrame &f, double dt) { overlays_.update(s, f, dt); }
	int pending() const { return road_stream_.pending + props_stream_.pending + building_stream_.pending + buildings_.pending() + trees_.pending(); }

private:
	TerrainBridge terrain_;
	RoadRenderer roads_;
	PropRenderer props_;
	TreeRenderer trees_;
	BuildingRenderer buildings_;
	AgentRenderer agents_;
	OverlayRenderer overlays_;
	ChunkStream road_stream_, props_stream_, building_stream_;
	int budget_ = 1;
};

} // namespace city
