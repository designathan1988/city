// What the road chunks and the road layers share (render/roads/road_renderer.h, road_layer.h, ground_seam_layer.h): one mesh a
// chunk, its instance and its material; `Extra` what a chunk's result carries besides its surface (mesh_queue.h), handed over with
// it by `adopt_extra` and let go with the chunk by `drop_extra`.
#pragma once

#include "core/grid.h"
#include "render/mesh_queue.h"
#include "render/render_layers.h"
#include "render/rs_util.h"

#include <godot_cpp/classes/rendering_server.hpp>

#include <algorithm>
#include <unordered_map>

namespace city {

// A chunk's box (`size` a chunk or a superchunk): its column (rs_util.h AABB_MIN_Y, AABB_H) grown to the vertices its job emitted
// (a segment is drawn whole by the chunk of its middle, and reaches past it).
godot::AABB chunk_bounds(int64_t key, float size, const godot::AABB &emitted);

template <typename Extra>
class RoadChunksOf : public MeshQueueOf<Extra> {
public:
	using Built = BuiltMeshOf<Extra>;
	// Everything drawn: its chunks, one instance and one draw call each, what they hold and the heaviest one's bytes.
	struct Count {
		int64_t chunks = 0, largest = 0;
		MeshCount all;
	};
	void reset() {
		this->clear_queue();
		for (auto &kv : chunks_) kv.second.surface.free();
		chunks_.clear();
		revision_++;
		forget_extras();
	}
	void drop_chunk(int64_t key) {
		this->forget(key);
		auto it = chunks_.find(key);
		if (it == chunks_.end()) return;
		it->second.surface.free();
		chunks_.erase(it);
		revision_++;
		drop_extra(key);
	}
	Count count() const {
		Count n;
		for (const auto &kv : chunks_) {
			const MeshCount &m = kv.second.made;
			if (m.triangles == 0) continue;
			n.chunks++;
			n.all.triangles += m.triangles;
			n.all.vertices += m.vertices;
			n.all.bytes += m.bytes;
			n.largest = std::max(n.largest, m.bytes);
		}
		return n;
	}
	void draw(bool on) { // for the cost probes: what a frame costs without these chunks
		drawn_ = on;
		for (auto &kv : chunks_) kv.second.surface.visible(on && kv.second.made.triangles > 0);
	}
	uint64_t revision() const { return revision_; } // moves when a chunk comes or goes
	template <typename F>
	void each_standing(F &&fn) const {
		for (const auto &kv : chunks_) fn(kv.first);
	}

protected:
	void commit_one(Built &b) override {
		Chunk &c = chunks_[b.key];
		const bool any = b.mesh.valid();
		c.surface.adopt(scenario_, material_, casts_, b.mesh.release(), LAYER_ROADS);
		if (any) godot::RenderingServer::get_singleton()->instance_set_custom_aabb(c.surface.inst, chunk_bounds(b.key, CHUNK, b.bounds));
		c.surface.visible(any && drawn_);
		c.made = b.count;
		revision_++;
		adopt_extra(b);
	}
	// What a layer does with a chunk's extra as its mesh is handed over, as the chunk goes, and when every chunk goes.
	virtual void adopt_extra(Built &) {}
	virtual void drop_extra(int64_t) {}
	virtual void forget_extras() {}
	godot::RID scenario_, material_;
	bool casts_ = false; // the road and the ground beside it lie on the ground: only a structure stands to throw a shadow

private:
	struct Chunk {
		MeshDraw surface;
		MeshCount made;
	};
	std::unordered_map<int64_t, Chunk> chunks_;
	bool drawn_ = true;
	uint64_t revision_ = 1;
};

using RoadChunks = RoadChunksOf<NoExtra>;

} // namespace city
