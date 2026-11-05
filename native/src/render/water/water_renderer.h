// The water that stands above the sea: lakes and the rivers the player draws, one mesh per 256 m chunk over the samples
// of Heightfield::water, each corner at the level of the water round it. The sea itself is the plane of
// project/render/city_world.gd. Chunks are built on the main thread, a few a frame: only those that hold water.
#pragma once

#include "render/render_context.h"
#include "render/rs_util.h"
#include "sim/terrain/heightfield.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace city {

class WaterRenderer {
public:
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	void mark(int64_t key) { dirty_.push_back(key); }
	void mark_all(const Heightfield &hf);
	void sync(const Heightfield &hf);

private:
	struct Draw {
		godot::RID inst, mesh;
	};
	void build(const Heightfield &hf, int64_t key);
	godot::RID scenario_, material_;
	std::vector<int64_t> dirty_;
	std::unordered_map<int64_t, Draw> chunks_;
};

} // namespace city
