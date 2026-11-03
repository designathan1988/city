// Street furniture chunk meshes: every prop and wire of a 256 m chunk (render/props/street_props.h) merged into one
// mesh drawn with the "prop" material (data/render/materials.json). Rebuilt with the road chunks.
#pragma once

#include "core/job_pool.h"
#include "render/mesh_builder.h"
#include "render/mesh_queue.h"
#include "render/props/prop_kit.h"
#include "render/props/street_props.h"
#include "render/render_context.h"
#include "render/rs_util.h"

#include <chrono>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace city {

class PropRenderer : public MeshQueue {
public:
	bool init(const RenderContext &ctx, const StreetLook &look, godot::PackedStringArray &errors);
	void reset();
	void submit(const CityState &s, int64_t key, JobPool &pool, float priority);
	void drop_chunk(int64_t key);
	const FurnitureLook &furniture() const { return furniture_; }

private:
	void build_cpu(const CityState &s, int64_t key, MeshBuilder &mb) const;
	void commit_one(BuiltMesh &b) override;
	godot::RID scenario_, material_;
	const StreetLook *look_ = nullptr;
	FurnitureLook furniture_;
	PropKit kit_;
	std::unordered_map<int64_t, MeshDraw> chunks_;
};

} // namespace city
