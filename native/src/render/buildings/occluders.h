// The city as its own occluder (Roadcraft render/occlusion.ts): the walls that surely stand, of the chunks drawn in their detail, given to
// the engine's occlusion culling (RenderingServer occluder_*, Godot's Embree buffer, docs: tutorials/3d/occlusion_culling). Only rectangular
// volumes standing on the ground and at least occluder_min_storeys tall, of a body the facade kit draws whole (no design of its own, no
// block that is a void or cuts another, not cut open by the Interior layer): a wall that is not there would hide what is seen through
// it, so the buffer must never be wider than what stands. One occluder a chunk, gone with the chunk's near level: the engine rebuilds its scene in
// the background when an occluder comes or goes, so the count follows the near level, not the city.
#pragma once

#include "render/buildings/facade.h"

#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/rid.hpp>

namespace city {

struct OccluderWalls {
	godot::PackedVector3Array points;
	godot::PackedInt32Array triangles;
	void add(const b40::Placed &pl, int min_storeys);
};

// The occluder of one chunk, freed with it.
class ChunkOccluder {
public:
	ChunkOccluder() = default;
	ChunkOccluder(const ChunkOccluder &) = delete;
	ChunkOccluder &operator=(const ChunkOccluder &) = delete;
	~ChunkOccluder() { clear(); }
	void set(const godot::RID &scenario, const OccluderWalls &walls);
	void clear();

private:
	godot::RID occluder_, instance_;
};

} // namespace city
