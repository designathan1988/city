// What the road chunks and the verge chunks share (render/roads/road_renderer.h, verge_renderer.h): one mesh a chunk, its
// instance and its material.
#pragma once

#include "render/mesh_queue.h"
#include "render/rs_util.h"

#include <unordered_map>

namespace city {

class RoadChunks : public MeshQueue {
public:
	void reset();
	void drop_chunk(int64_t key);

protected:
	void commit_one(BuiltMesh &b) override;
	godot::RID scenario_, material_;

private:
	struct Chunk {
		MeshDraw surface;
	};
	std::unordered_map<int64_t, Chunk> chunks_;
};

} // namespace city
