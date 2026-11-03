// Building chunk meshes, Roadcraft "4.0" buildings (r40/render/buildings): every record of a 256 m chunk drawn in one
// material ("building", data/render/materials.json) at four levels switched by RenderingServer visibility ranges
// (tuning render.building_detail_end_m = D, building_mid_end_m, building_far_end_m; shares in buildings.json "lod"):
//   parts  0 .. D x parts_share        frames, railings, louvres, doors, AC units, fences (no shadows)
//   near   0 .. D x near_share         walls with real openings and reveals, panes, sills, bands, roofs, plant, lot
//   mid    near end .. mid_end         walls with shader-painted windows, the real roofs (parapets, cornices, plant)
//   far    mid_end .. far_end          the masses with painted windows, parapet rims and roof slabs
// A chunk is meshed only within far_end of the camera (RenderWorld streams it); near and mid levels lazily when the
// camera comes within their reach, freed far beyond.
#pragma once

#include "render/buildings/b40_data.h"
#include "render/buildings/building_bodies.h"
#include "render/chunk_queue.h"
#include "render/render_context.h"
#include "render/rs_util.h"
#include "sim/state/city_state.h"

#include <unordered_map>

namespace city {

class BuildingRenderer {
public:
	bool init(const RenderContext &ctx, godot::PackedStringArray &errors);
	void reset();
	void rebuild_chunk(const CityState &s, int64_t key);
	void drop_chunk(int64_t key); // frees every level of a chunk the camera left
	float far_end() const { return far_end_; }
	void set_ranges(const CityState &s);
	void update_view(const godot::Vector3 &cam, BuildBudget &budget);
	int pending() const { return pending_; } // lazy levels in reach not built yet

private:
	enum Level { LEVEL_NEAR, LEVEL_MID, LEVELS };
	struct Chunk {
		MeshDraw far, near, parts, mid;
		bool built[LEVELS] = {};
		godot::Vector3 centre;
	};
	void build(Chunk &ch, int64_t key, Level level);
	void drop(Chunk &ch, Level level);
	void ranges(Chunk &ch) const;
	float reach(Level level) const;
	b40::Data data_;
	BuildingBodies bodies_;
	godot::RID scenario_, material_;
	const CityState *state_ = nullptr;
	float parts_end_ = 0, near_end_ = 0, mid_end_ = 0, far_end_ = 0;
	int pending_ = 0;
	std::unordered_map<int64_t, Chunk> chunks_;
};

} // namespace city
