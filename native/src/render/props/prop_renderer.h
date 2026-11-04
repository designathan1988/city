// Street furniture chunks: every small prop and plate of a 256 m chunk (render/props/street_props.h, street_signs.h) merged
// into one mesh drawn with the "prop" material (data/render/materials.json), and its poles, wires, barrier boxes and lamp
// light as one MultiMesh each. Rebuilt with the road chunks and when the player places something there. The chunk's signal
// posts go with its mesh to SignalLamps, which lights their lenses.
#pragma once

#include "core/job_pool.h"
#include "render/mesh_builder.h"
#include "render/mesh_queue.h"
#include "render/props/barrier_boxes.h"
#include "render/props/prop_kit.h"
#include "render/props/sign_atlas.h"
#include "render/props/signal_lamps.h"
#include "render/props/street_props.h"
#include "render/props/street_signs.h"
#include "render/render_context.h"
#include "render/rs_util.h"

#include <chrono>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace city {

class PropRenderer : public MeshQueue {
public:
	bool init(const RenderContext &ctx, const StreetLook &look, godot::PackedStringArray &errors);
	void reset();
	void submit(const CityState &s, int64_t key, JobPool &pool, float priority);
	void drop_chunk(int64_t key);
	void update_signals(const PoseFrame &f) { lamps_.update(f); }
	int lit_lenses() const { return lamps_.lit(); }
	// The run under the pointer, drawn as it will stand; empty lists take it away. Main thread, map held.
	void set_pole_preview(const CityState &s, const std::vector<godot::Vector3> &at, const std::vector<bool> &lamp, const std::vector<bool> &standing);
	godot::Array take_sign_cells() { return atlas_.take_pending(); }
	int instances() const { return instances_; } // poles, wires, boxes and lights drawn (render_cost probe)
	int sign_cells() const { return atlas_.cells_used(); }

private:
	// What a chunk draws as instances.
	struct Draws {
		MultiMeshDraw poles, wires_near, wires_far, boxes, light;
		int count = 0;
		void free();
	};
	void build_cpu(const CityState &s, int64_t key, MeshBuilder &mb) const;
	void commit_one(BuiltMesh &b) override;
	void show(Draws &d, const StreetInstances &inst, const godot::AABB &box);
	godot::RID scenario_, material_, glow_material_;
	const StreetLook *look_ = nullptr;
	FurnitureLook furniture_;
	BarrierLook barriers_;
	SignLook signs_;
	PropKit kit_;
	mutable SignAtlas atlas_;
	OwnedMesh pole_mesh_, wire_near_mesh_, wire_far_mesh_, box_mesh_, light_mesh_;
	std::unordered_map<int64_t, MeshDraw> chunks_;
	std::unordered_map<int64_t, Draws> draws_;
	Draws preview_;
	SignalLamps lamps_;
	int instances_ = 0;
	// what each chunk's newest job placed, handed over with its mesh (a stale result never commits)
	mutable std::mutex placed_mutex_;
	mutable std::unordered_map<int64_t, StreetInstances> placed_;
};

} // namespace city
