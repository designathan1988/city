// The water that stands above the sea: lakes and the rivers the player draws, one mesh per 256 m chunk over the samples
// of Heightfield::water, each corner at the level of the water round it. The sea itself is the plane of
// project/render/city_world.gd. A chunk is built by a job and baked and handed over in the frame's admission, like the road
// chunks (mesh_queue.h); only the chunks that hold water are asked.
#pragma once

#include "render/frame_admission.h"
#include "render/mesh_queue.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "sim/terrain/ground_patch.h"

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace city {

class WaterRenderer : public MeshQueue {
public:
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	void mark(int64_t key) { dirty_.insert(key); }
	void mark_wet(const Heightfield &hf); // every chunk with a water sample under one of its cells (a new map or a load)
	// The marked chunks as the frame's build units, nearest first in its one order; one with a result on its way waits marked.
	void offer(const CityState &s, const godot::Vector3 &cam, std::vector<FrameAdmission::Unit> &out);
	int pending() const { return int(dirty_.size()) + MeshQueue::pending(); }

protected:
	void commit_one(BuiltMesh &b) override;

private:
	struct Draw {
		godot::RID inst, mesh;
	};
	static void build(const GroundRead &ground, int64_t key, MeshBuilder &mb);
	godot::RID scenario_;
	std::unordered_set<int64_t> dirty_;
	std::unordered_map<int64_t, Draw> chunks_;
};

} // namespace city
