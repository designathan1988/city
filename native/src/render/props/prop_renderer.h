// Street furniture chunks: every small prop and plate of a 256 m chunk (render/props/street_props.h, street_signs.h) merged
// into one mesh drawn with the "prop" material (data/render/materials.json), and its poles, wires, barrier boxes and lamp
// beams as one MultiMesh each. Rebuilt with the road chunks and when the player places something there. The chunk's signal
// posts go with its mesh to SignalLamps, which lights their lenses, and its street lamps to LampLights, which lights the street.
#pragma once

#include "core/job_pool.h"
#include "render/detail.h"
#include "render/mesh_builder.h"
#include "render/mesh_queue.h"
#include "render/props/barrier_boxes.h"
#include "render/props/lamp_lights.h"
#include "render/props/prop_kit.h"
#include "render/props/sign_atlas.h"
#include "render/props/signal_lamps.h"
#include "render/props/street_props.h"
#include "render/props/street_signs.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "sim/state/city_cut.h"

#include <chrono>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace city {

class PropRenderer : public MeshQueueOf<StreetInstances> {
public:
	bool init(const RenderContext &ctx, const StreetLook &look, godot::PackedStringArray &errors);
	void reset();
	void submit(const CityState &s, int64_t key, JobPool &pool, float priority);
	void drop_chunk(int64_t key);
	// The frame's view (render/detail.h): the level of every chunk's wires and of its poles' shadows is picked by it (pick).
	void update_view(const DetailView &view);
	void update_signals(const PoseFrame &f, bool fresh) { lamps_.update(f, fresh); }
	int lit_lenses() const { return lamps_.lit(); }
	void update_lamps(const godot::Vector3 &eye, const ViewCull &view, float frame_s) { lights_.update(eye, view, frame_s); }
	void set_lamp_budget(int count, int casting) { lights_.set_budget(count, casting); }
	void set_lamp_night(float night) { lights_.set_night(night); }
	int lit_lamps() const { return lights_.lit(); }
	int lamp_budget() const { return lights_.budget(); }
	int lamp_casting() const { return lights_.casting(); }
	const LightLook &lamp_look() const { return lights_.look(); }
	godot::Vector2 lamps_hand_over() const { return lights_.handed_over(); }
	const FurnitureLook &furniture_look() const { return furniture_; } // the lamps' lenses and pools as streets.json draws them
	void lenses_near(float x, float z, float r, godot::PackedVector3Array &out) const { lights_.lenses_near(x, z, r, out); }
	// The run under the pointer, drawn as it will stand; empty lists take it away. Main thread, map held.
	void set_pole_preview(const CityState &s, const std::vector<PreviewPole> &poles);
	godot::Array take_sign_cells() { return atlas_.take_pending(); }
	int64_t sign_generation() const { return atlas_.generation(); } // moves when the sheets start empty again (a new city)
	int instances() const { return instances_; } // poles, wires, boxes and lights drawn (render_cost probe)
	// Tests: each chunk's spans and which levels of its wires and of its poles' shadows the engine is told to draw (probe wire_draws).
	godot::Array wire_draws() const;
	int sign_cells() const { return atlas_.cells_used(); }

private:
	// What a chunk draws as instances.
	struct Draws {
		MultiMeshDraw poles, wires_near, wires_far, boxes, beams, light;
		MultiMeshDraw pole_shadow_near, pole_shadow_far; // the poles' shadow: the whole mesh in the near chunks, the light one past them
		godot::RID cast; // the chunk's mesh in the shadow maps alone, as far as a thin thing's shadow reads
		godot::AABB box; // the chunk's, as its draws were given it: what the pick measures the eye's distance to
		float wire_error = 0; // how far the coarse wires are off the chunk's deepest span, in metres (WireLook::chord_miss)
		bool casting = true; // `cast` as the engine was told last (an instance is made drawn)
		int count = 0;
		void free();
	};
	// The one decision of which level of a chunk's wires and of its poles' shadows stands, by view_ (prop_renderer.cpp).
	void pick(Draws &d) const;
	CityCut gather(const CityState &s, int64_t key) const;
	void build_cpu(const CityView &s, int64_t key, MeshBuilder &mb, StreetInstances &inst) const;
	void commit_one(Built &b) override;
	void show(Draws &d, StreetInstances &inst, const godot::AABB &box);
	void cast_chunk(Draws &d, const godot::RID &mesh, const godot::AABB &box);
	DetailView view_; // the last frame's (update_view): a chunk committed between two frames is picked by it
	godot::RID scenario_, material_, glow_material_;
	const StreetLook *look_ = nullptr;
	FurnitureLook furniture_;
	BarrierLook barriers_;
	SignLook signs_;
	PropKit kit_;
	mutable SignAtlas atlas_;
	OwnedMesh pole_mesh_, pole_cast_mesh_, wire_near_mesh_, wire_far_mesh_, box_mesh_, beam_mesh_, light_mesh_;
	std::unordered_map<int64_t, MeshDraw> chunks_;
	std::unordered_map<int64_t, Draws> draws_;
	Draws preview_;
	SignalLamps lamps_;
	LampLights lights_;
	int instances_ = 0;
};

} // namespace city
