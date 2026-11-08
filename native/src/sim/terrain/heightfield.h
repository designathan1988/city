// Authoritative terrain: player-sculpted base heights + non-destructive modifiers (road corridors,
// intersection pads) composed into the final heights. Terrain3D only renders it (render/terrain_bridge).
#pragma once

#include "core/bytes.h"
#include "core/grid.h"
#include "sim/catalog/catalog.h"
#include "sim/state/change_log.h"
#include "sim/terrain/height_patch.h"
#include "sim/terrain/road_shape.h"

#include <unordered_map>
#include <vector>

namespace city {

class JobPool;

constexpr int TERRAIN_REGION_SIZE = 256; // samples per Terrain3D region side
constexpr int TERRAIN_MAX_REGIONS = 32; // Terrain3D limit per side
constexpr float HEIGHT_Q_OFFSET = 100.f, HEIGHT_Q_SCALE = 100.f; // 1 cm steps from -100 m
constexpr float QUANT_LIMIT = 65535.f; // a 16-bit height's top

struct TerrainModifier {
	bool alive = false;
	std::vector<float> pts; // x, target height, z triples; one point = round pad
	float flat = 0; // fully flattened half width
	float max_run = 0; // longest embankment / cut run
	float minx = 0, minz = 0, maxx = 0, maxz = 0;
};

// Roadcraft world/brush.ts BrushMode (the Relief cards and the landforms).
// The order is the Relief cards' "mode" in data/ui/drawers.json.
enum class BrushMode : int { RAISE = 0, LOWER, SMOOTH, FLATTEN, MESA, CANYON, ESCARPMENT, SUGARLOAF, RIVER, COUNT };

// One dab of a stroke (BrushStroke.dab): radius in metres, strength 0..1 (m/s to raise or lower, share/s to smooth or
// level), dt seconds held, h0 = the height under the stroke's first dab (level and landforms).
struct BrushDab {
	float x = 0, z = 0, radius = 0, strength = 0, dt = 0, h0 = 0, hardness = 0;
	BrushMode mode = BrushMode::RAISE;
};

// What a sample learns from a polyline: its distance to the nearest piece, the height there, and the side it lies on (1 left of
// the polyline's way, +u of a road's frame; 0 right).
struct NearSample {
	float d = BIG, surface = 0;
	int side = 0;
};

// A rectangle of height samples (inclusive).
struct SampleRect {
	int x0 = 0, z0 = 0, x1 = -1, z1 = -1;
	bool empty() const { return x0 > x1 || z0 > z1; }
};

class TerrainGen;

class Heightfield {
public:
	int N = 0; // samples per side; world x = (ix - N/2) * spacing
	float spacing = 0;
	int region_size = TERRAIN_REGION_SIZE;
	int regions = 0;
	float water_level = 0.f;
	uint32_t seed = 0;
	const Tuning *tune = nullptr;
	std::vector<uint16_t> base_q; // quantized 1 cm, -100..555 m
	std::vector<float> final_h; // base + modifiers (rendered)
	std::vector<TerrainModifier> mods;
	std::vector<int> free_mods;
	SpatialHash mod_hash;
	// The ground shaped to each road, by segment id: derived from the roads' heights, never journaled nor saved.
	std::vector<RoadShape> road_shapes;
	SpatialHash shape_hash;
	// World rectangles (x0, z0, x1, z1) where the natural ground or its water changed since the roads last read it.
	std::vector<float> ground_moved;
	ChangeLog *log = nullptr;

	std::vector<float> water; // water level per water sample (TerrainGen::NO_WATER where dry)
	// The generated level of every water sample an edit or a file changed, kept at its first change: what a save
	// writes and what Relief's "back to the original" restores, with no land generated again.
	std::unordered_map<uint32_t, float> water_original;
	void set_water(uint32_t i, float level) {
		water_original.emplace(i, water[i]);
		water[i] = level;
	}
	JobPool *jobs = nullptr; // the city's pool: filling and composing split over it
	int water_n = 0;
	float water_spacing_m = 0;

	// One Terrain3D region per map tile (tiles from data/catalog/maps.json, sizes from tuning "world").
	void init(int tiles, float tile_m, float spacing_m, uint32_t p_seed, ChangeLog *p_log, const Tuning *p_tune, JobPool *p_jobs);
	// Heights and water of a generated map, split over the pool.
	void fill(TerrainGen &gen, float water_spacing);
	float water_at(float x, float z) const; // nearest water sample's level
	float half_size() const { return N * spacing * 0.5f; }
	static uint16_t quant(float h) { return uint16_t(clampf((h + HEIGHT_Q_OFFSET) * HEIGHT_Q_SCALE, 0.f, QUANT_LIMIT)); }
	static float dequant(uint16_t q) { return q / HEIGHT_Q_SCALE - HEIGHT_Q_OFFSET; }
	int ix_of(float x) const { return int(std::floor(x / spacing + N * 0.5f)); }
	float x_of(int ix) const { return (ix - N * 0.5f) * spacing; }
	bool in_bounds(float x, float z, float margin = 0.f) const {
		float h = half_size() - margin;
		return x > -h && x < h && z > -h && z < h;
	}
	float height_at(float x, float z) const; // final heights (with road corridors and pads)
	// A copy of the final heights covering the box (plus the nodes its edges sample), for work done off the map lock.
	HeightPatch patch(float minx, float minz, float maxx, float maxz) const;
	float base_height_at(float x, float z) const; // natural ground, before modifiers
	// Water stands here: a water sample over the ground, or the ground under the sea.
	bool is_water(float x, float z, float clearance) const {
		float h = base_height_at(x, z) - clearance;
		return water_at(x, z) > h || water_level > h;
	}
	bool raycast(const V2 &o2, float oy, const V2 &d2, float dy, float max_dist, float &hx, float &hy, float &hz) const;
	SampleRect brush_rect(float cx, float cz, float radius) const;
	// Applies one dab to the base heights (the caller records the rectangle for undo first); true when any changed.
	bool brush(const BrushDab &d);
	// The level a river drawn from ground `h0` stands at.
	float river_level(float h0) const { return h0 - tune->terrain_river_freeboard_m; }
	// Fills with `level` the water samples of the circle whose ground lies under it; `index`, `before` get what changed.
	void flood(float x, float z, float radius, float level, std::vector<uint32_t> &index, std::vector<float> &before);
	// Writes base samples back (undo / redo / load) and recomposes around them.
	void write_samples(const std::vector<uint32_t> &idx, const std::vector<uint16_t> &values);
	// The ground under segment `id` shaped to it again (or no more), recomposed where it was and where it is.
	void put_road_shape(int id, const RoadShape &r);
	void drop_road_shape(int id);
	// Where the natural ground moved since the last call (the roads there read it again), and forgets it.
	std::vector<float> take_ground_moved();
	int add_modifier(const std::vector<float> &pts, float flat, float max_run);
	void remove_modifier(int id);
	void put_modifier(int id, const TerrainModifier &m); // undo / redo / load: the same id again
	std::vector<uint8_t> tile_dirty; // per region: base heights differ from the generated land (saved)
	void mark_tile(size_t sample) { tile_dirty[(sample / N / region_size) * regions + (sample % N) / region_size] = 1; }
	// The final heights of the rectangle composed again (compose.cpp); while a hold is open it waits for the release.
	void recompose(int x0, int z0, int x1, int z1);
	void recompose_world_rect(float minx, float minz, float maxx, float maxz);
	// A bulk edit (a city read, every road shaped again): the rectangles wait and are composed once, merged, when the last
	// hold is let go; nothing reads the final heights meanwhile. `compose` false drops them (a file that is only checked).
	void hold();
	void release(bool compose = true);
	void finish_load(); // rebuild final heights after base_q was loaded

private:
	float dab_value(const BrushDab &d, size_t i, float dist, int x, int z) const;
	float natural_reach(const BrushDab &d, float x, float z, float lobe_share, float salt_x, float salt_z) const;
	int held = 0;
	void compose_rect(const SampleRect &r); // its final heights, nothing logged: rectangles that share no sample may run at once
	std::vector<SampleRect> waiting;
	// Distance to the nearest piece of a polyline (x, height, z triples) for the samples of the box within `reach` of it.
	const std::vector<NearSample> &nearest(const std::vector<float> &pts, float reach, int ax0, int az0, int ax1, int az1) const;
	void apply_modifier(const TerrainModifier &m, int x0, int z0, int x1, int z1, std::vector<float> &best_w);
	void apply_road_shape(const RoadShape &r, int x0, int z0, int x1, int z1, std::vector<float> &best_w, std::vector<float> &best_d); // road_shape.cpp
};

// A held composition for the length of a scope (Heightfield::hold): composed when it ends, or dropped with `compose` false.
class ComposeHold {
public:
	explicit ComposeHold(Heightfield &t, bool compose = true) : t_(t), compose_(compose) { t_.hold(); }
	~ComposeHold() { t_.release(compose_); }
	ComposeHold(const ComposeHold &) = delete;
	ComposeHold &operator=(const ComposeHold &) = delete;

private:
	Heightfield &t_;
	bool compose_;
};

} // namespace city
