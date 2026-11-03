// Trees streamed around the camera: every 256 m chunk within TreeLook::view_m gets one MultiMesh per species and level
// of detail (near / far by distance), built a few chunks per frame and rebuilt when its roads or lots change.
// Material "prop" (tint parts take INSTANCE_CUSTOM.rgb).
#pragma once

#include "render/chunk_queue.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "render/trees/tree_place.h"

#include <unordered_map>

namespace city {

class TreeRenderer {
public:
	bool init(const RenderContext &ctx, const StreetLook &look, const FurnitureLook &furniture, godot::PackedStringArray &errors);
	void reset();
	void mark_dirty(int64_t key) { dirty_.insert(key); }
	void sync(const CityState &s, const godot::Vector3 &cam, BuildBudget &budget);
	int pending() const { return pending_; }

private:
	struct Chunk {
		std::vector<MultiMeshDraw> near, far;
	};
	void build(const CityState &s, int64_t key);
	godot::RID scenario_, material_;
	const StreetLook *look_ = nullptr;
	const FurnitureLook *furniture_ = nullptr;
	TreeLook trees_;
	TreeKit kit_;
	std::unordered_map<int64_t, Chunk> chunks_;
	std::unordered_set<int64_t> dirty_;
	int pending_ = 0;
};

} // namespace city
