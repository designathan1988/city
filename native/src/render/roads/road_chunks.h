// What the road chunks and the road layers share (render/roads/road_renderer.h, road_layer.h): one mesh a chunk, its
// instance and its material.
#pragma once

#include "render/mesh_queue.h"
#include "render/rs_util.h"

#include <unordered_map>

namespace city {

// A chunk's box (`size` a chunk or a superchunk): its column (rs_util.h AABB_MIN_Y, AABB_H) grown to the vertices its job emitted
// (a segment is drawn whole by the chunk of its middle, and reaches past it).
godot::AABB chunk_bounds(int64_t key, float size, const godot::AABB &emitted);

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
	uint64_t revision() const { return revision_; } // moves when a chunk comes or goes
	template <typename F>
	void each_standing(F &&fn) const {
		for (const auto &kv : chunks_) fn(kv.first);
	}

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
	uint64_t revision_ = 1;
};

} // namespace city
