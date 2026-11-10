// The roads at their far level: one mesh a superchunk, with the same pieces and the same material as the detailed chunks but only their top
// faces. It follows the view as every other level does: a superchunk that holds roads is built when it comes within the buildings' reach of the
// eye and freed past it, so the streets end where the buildings end and what the GPU holds grows with the view, never with the map. The detailed
// chunks stream nearer the camera; where one stands, its roads fold away here (`chunk_mask`, road.gdshader), so the two swap with no hole and no
// overlap. An edit makes its superchunk again, never the map.
#pragma once

#include "render/chunk_queue.h"
#include "render/mesh_queue.h"
#include "render/render_context.h"
#include "render/roads/road_chunks.h"
#include "render/roads/road_look.h"
#include "render/roads/road_shapes.h"
#include "render/rs_util.h"
#include "sim/state/city_view.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace city {

class FarRoadRenderer : public MeshQueue {
public:
	bool init(const RenderContext &ctx, const StreetLook &look, RoadShapes &shapes, godot::PackedStringArray &errors);
	void reset();
	void mark(int64_t chunk); // the roads of this chunk changed
	// The superchunks within `reach` of the eye that are not drawn, or whose roads changed, as the frame's build units (frame_admission.h:
	// nearest first in the frame's one order); those past `reach` times `drop_k` are freed (no build and free at one border).
	void offer(const CityState &s, JobPool &pool, const godot::Vector3 &cam, float reach, float drop_k, std::vector<FrameAdmission::Unit> &out);
	void fold(const RoadChunks &detail); // the detailed chunks standing now: theirs fold away
	int waiting_supers() const { return waiting_; } // superchunks in reach offered this frame and not asked yet
	int drawn_supers() const { return int(supers_.size()); } // tests: the superchunks held now

protected:
	void commit_one(BuiltMesh &b) override;

private:
	struct Super {
		MeshDraw surface;
		int mask = 0, sent = -1;
	};
	void build_cpu(const CityView &s, int64_t key, MeshBuilder &mb) const;
	void send(Super &sp) const;

	const StreetLook *look_ = nullptr;
	RoadShapes *shapes_ = nullptr; // the road renderer's (road_shapes.h)
	godot::RID scenario_, material_;
	std::unordered_map<int64_t, Super> supers_; // the superchunks drawn now: those in reach
	std::unordered_set<int64_t> known_; // every superchunk that holds a road (marked since the map opened)
	std::unordered_set<int64_t> dirty_; // those whose roads changed since they were built
	int waiting_ = 0;
	uint64_t folded_ = 0; // the detail's revision the masks were made from
	bool fresh_ = false; // a mesh came since: its instance has not had its mask
};

} // namespace city
