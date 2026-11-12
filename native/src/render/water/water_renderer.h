// The water that stands above the sea: lakes and the rivers the player draws, one mesh per 256 m chunk over the samples
// of Heightfield::water, each corner at the level of the water round it. The sea itself is the plane of
// project/render/city_world.gd. CPU cell preparation continues under RenderWorld shared credits; only wet cells draw.
#pragma once

#include "render/render_context.h"
#include "render/chunk_queue.h"
#include "render/mesh_builder.h"
#include "render/rs_util.h"
#include "sim/terrain/heightfield.h"

#include <cstdint>
#include <unordered_map>
#include <set>
#include <optional>

namespace city {

class WaterRenderer {
public:
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	void mark(int64_t key) { dirty_.insert(key); }
	void mark_all(const Heightfield &hf);
	void sync(const Heightfield &hf, BuildBudget &budget);

private:
	struct Draw {
		godot::RID inst, mesh;
	};
	struct Work {
		int64_t key = 0;
		uint64_t stamp = 0;
		int x0 = 0, x1 = 0, z1 = 0, gx = 0, gz = 0;
		MeshBuilder mesh;
	};
	void begin(const Heightfield &hf, int64_t key);
	uint64_t stamp(const Heightfield &hf, int64_t key) const;
	std::optional<Work> work_;
	godot::RID scenario_, material_;
	std::set<int64_t> dirty_;
	std::unordered_map<int64_t, Draw> chunks_;
};

} // namespace city
