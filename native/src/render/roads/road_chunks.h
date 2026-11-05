// What the road chunks and the verge chunks share (render/roads/road_renderer.h, verge_renderer.h): one mesh a chunk, its
// instance and its material.
#pragma once

#include "render/mesh_queue.h"
#include "render/rs_util.h"

#include <unordered_map>

namespace city {

class RoadChunks : public MeshQueue {
public:
	// Everything drawn: its chunks, one instance and one draw call each, what they hold and the heaviest one's bytes.
	struct Count {
		int64_t chunks = 0, largest = 0;
		MeshCount all;
	};
	void reset();
	void drop_chunk(int64_t key);
	Count count() const;
	void draw(bool on); // for the cost probes: what a frame costs without these chunks

protected:
	void commit_one(BuiltMesh &b) override;
	godot::RID scenario_, material_;
	bool casts_ = false; // the road and its verge lie on the ground: only a structure stands to throw a shadow

private:
	struct Chunk {
		MeshDraw surface;
		MeshCount made;
	};
	std::unordered_map<int64_t, Chunk> chunks_;
	bool drawn_ = true;
};

} // namespace city
