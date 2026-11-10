// Every road of the map at its far level: one mesh a superchunk, made once and for good, with the same pieces and the same material as the
// detailed chunks but only their top faces. The detailed chunks stream round the camera; where one stands, its roads fold away here (`chunk_mask`,
// road.gdshader), so the two swap with no hole and no overlap and the streets never end before the buildings do. An edit makes its superchunk again,
// never the map.
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
	// The dirty superchunks as the frame's build units (frame_admission.h): nearest first in the frame's one order.
	void offer(const CityState &s, JobPool &pool, const godot::Vector3 &cam, std::vector<FrameAdmission::Unit> &out);
	void fold(const RoadChunks &detail); // the detailed chunks standing now: theirs fold away
	int waiting_supers() const { return int(dirty_.size()); }

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
	std::unordered_map<int64_t, Super> supers_;
	std::unordered_set<int64_t> dirty_;
	uint64_t folded_ = 0; // the detail's revision the masks were made from
	bool fresh_ = false; // a mesh came since: its instance has not had its mask
};

} // namespace city
