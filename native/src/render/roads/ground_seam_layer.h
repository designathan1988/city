// The ground beside the roads, chunk by chunk on the road chunks' own streaming (ground_seam.h): one mesh a chunk in material
// "terrain_seam", over the terrain left whole, and the cut cells each chunk's triangulation refused (the probe reads them).
#pragma once

#include "core/job_pool.h"
#include "render/render_context.h"
#include "render/roads/ground_seam.h"
#include "render/roads/road_chunks.h"

#include <unordered_map>

namespace city {

class GroundSeamLayer : public RoadChunksOf<SeamResult> {
public:
	bool init(const RenderContext &ctx, const StreetLook &look, RoadShapes &shapes, godot::PackedStringArray &errors);
	void submit(const CityState &s, int64_t key, JobPool &pool, float priority);
	int refused() const; // the cut cells of the standing chunks no triangulation took

protected:
	void adopt_extra(Built &b) override { refused_[b.key] = b.extra.refused; }
	void drop_extra(int64_t key) override { refused_.erase(key); }
	void forget_extras() override { refused_.clear(); }

private:
	const StreetLook *look_ = nullptr;
	RoadShapes *shapes_ = nullptr;
	std::unordered_map<int64_t, int> refused_;
};

} // namespace city
