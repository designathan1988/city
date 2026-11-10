// The roads at their far level: one mesh a superchunk, with the same pieces and the same material as the detailed chunks but only their top
// faces, streamed as far as the buildings' far level reaches (the streets never end before the buildings do) and let go past it, so what is
// held follows the view and not the size of the map. The detailed chunks stream nearer; where one stands, its roads fold away here
// (`chunk_mask`, road.gdshader), so the two swap with no hole and no overlap. An edit makes its superchunk again, never the map.
#pragma once

#include "render/chunk_queue.h"
#include "render/mesh_queue.h"
#include "render/render_context.h"
#include "render/roads/road_chunks.h"
#include "render/roads/road_look.h"
#include "render/rs_util.h"
#include "sim/state/city_view.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace city {

class FarRoadRenderer : public MeshQueue {
public:
	bool init(const RenderContext &ctx, const StreetLook &look, godot::PackedStringArray &errors);
	void reset();
	void mark(int64_t chunk); // the roads of this chunk changed
	// The dirty superchunks within `reach` of the eye as the frame's build units (frame_admission.h), nearest first in the frame's one
	// order; one past `reach` x `drop` is let go and stands dirty until the eye comes back.
	void offer(const CityState &s, JobPool &pool, const godot::Vector3 &cam, float reach, float drop, std::vector<FrameAdmission::Unit> &out);
	void fold(const RoadChunks &detail); // the detailed chunks standing now: theirs fold away
	int waiting_supers() const { return waiting_; }

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
	godot::RID scenario_, material_;
	std::unordered_map<int64_t, Super> supers_;
	std::unordered_set<int64_t> dirty_; // to make: changed, never made, or let go
	int waiting_ = 0; // of them, those within reach and not asked yet (the last offer): what the view still waits for
	uint64_t folded_ = 0; // the detail's revision the masks were made from
	bool fresh_ = false; // a mesh came since: its instance has not had its mask
};

} // namespace city
